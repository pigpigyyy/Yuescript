/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <chrono>
#include <memory>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "yuescript/yue_compiler.h"
#include "yuescript/yue_parser.h"

#ifndef YUE_NO_MACRO

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
} // extern "C"

// name of table stored in lua registry
#define YUE_MODULE "__yue_modules__"

#if LUA_VERSION_NUM > 501
#ifndef LUA_COMPAT_5_1
#ifndef lua_objlen
#define lua_objlen lua_rawlen
#endif // lua_objlen
#endif // LUA_COMPAT_5_1
#endif // LUA_VERSION_NUM

#endif // YUE_NO_MACRO

namespace yue {

#define BLOCK_START do {
#define BLOCK_END \
	} \
	while (false) \
		;
#define BREAK_IF(cond) \
	if (cond) break

#define _DEFER(code, line) std::shared_ptr<void> _defer_##line(nullptr, [&](auto) { code; })
#define DEFER(code) _DEFER(code, __LINE__)
#define YUEE(msg, node) throw CompileError( \
	"[File] "s + __FILE__ \
		+ ",\n[Func] "s + __FUNCTION__ \
		+ ",\n[Line] "s + std::to_string(__LINE__) \
		+ ",\n[Error] "s + msg, \
	node)

typedef std::list<std::string> str_list;

static std::unordered_set<std::string> Metamethods = {
	"add"s, "sub"s, "mul"s, "div"s, "mod"s,
	"pow"s, "unm"s, "concat"s, "len"s, "eq"s,
	"lt"s, "le"s, "index"s, "newindex"s, "call"s,
	"gc"s, "mode"s, "tostring"s, "metatable"s, // Lua 5.1
	"pairs"s, "ipairs"s, // Lua 5.2
	"name"s, "idiv"s, "band"s, "bor"s, "bxor"s,
	"bnot"s, "shl"s, "shr"s, // Lua 5.3 ipairs deprecated
	"close"s // Lua 5.4
};

const std::string_view version = "0.16.3"sv;
const std::string_view extension = "yue"sv;

class CompileError : public std::logic_error {
public:
	explicit CompileError(std::string_view msg, const input_range* range)
		: std::logic_error(std::string(msg))
		, line(range->m_begin.m_line)
		, col(range->m_begin.m_col) { }

	int line;
	int col;
};

CompileInfo::CompileInfo(
	std::string&& codes,
	std::optional<Error>&& error,
	std::unique_ptr<GlobalVars>&& globals,
	std::unique_ptr<Options>&& options,
	double parseTime,
	double compileTime)
	: codes(std::move(codes))
	, error(std::move(error))
	, globals(std::move(globals))
	, options(std::move(options))
	, parseTime(parseTime)
	, compileTime(compileTime) { }

CompileInfo::CompileInfo(CompileInfo&& other)
	: codes(std::move(other.codes))
	, error(std::move(other.error))
	, globals(std::move(other.globals))
	, options(std::move(other.options))
	, parseTime(other.parseTime)
	, compileTime(other.compileTime) { }

void CompileInfo::operator=(CompileInfo&& other) {
	codes = std::move(other.codes);
	error = std::move(other.error);
	globals = std::move(other.globals);
	options = std::move(other.options);
	parseTime = other.parseTime;
	compileTime = other.compileTime;
}

class YueCompilerImpl {
public:
#ifndef YUE_NO_MACRO
	YueCompilerImpl(lua_State* sharedState,
		const std::function<void(void*)>& luaOpen,
		bool sameModule)
		: L(sharedState)
		, _luaOpen(luaOpen) {
		BLOCK_START
		BREAK_IF(!sameModule);
		BREAK_IF(!L);
		_sameModule = true;
		int top = lua_gettop(L);
		DEFER(lua_settop(L, top));
		lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
		lua_rawget(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE], tb
		BREAK_IF(lua_istable(L, -1) == 0);
		int idx = static_cast<int>(lua_objlen(L, -1)); // idx = #tb, tb
		BREAK_IF(idx == 0);
		_useModule = true;
		BLOCK_END
	}

	~YueCompilerImpl() {
		if (L && _stateOwner) {
			lua_close(L);
			L = nullptr;
		}
	}
#endif // YUE_NO_MACRO

	CompileInfo compile(std::string_view codes, const YueConfig& config) {
		_config = config;
#ifndef YUE_NO_MACRO
		if (L) passOptions();
#endif // YUE_NO_MACRO
		double parseTime = 0.0;
		double compileTime = 0.0;
		if (config.profiling) {
			auto start = std::chrono::high_resolution_clock::now();
			_info = _parser.parse<File_t>(codes);
			auto stop = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = stop - start;
			parseTime = diff.count();
		} else {
			_info = _parser.parse<File_t>(codes);
		}
		std::unique_ptr<GlobalVars> globals;
		std::unique_ptr<Options> options;
		if (!config.options.empty()) {
			options = std::make_unique<Options>(config.options);
		}
		DEFER(clear());
		if (!_info.error) {
			try {
				auto block = _info.node.to<File_t>()->block.get();
				if (_info.exportMacro) {
					for (auto stmt_ : block->statements.objects()) {
						auto stmt = static_cast<Statement_t*>(stmt_);
						switch (stmt->content->getId()) {
							case id<MacroInPlace_t>():
							case id<Macro_t>():
								break;
							case id<Import_t>():
							case id<Export_t>():
								if (auto importNode = stmt->content.as<Import_t>()) {
									if (auto importAs = importNode->content.as<ImportAs_t>()) {
										if (importAs->target.is<ImportAllMacro_t>()) {
											break;
										} else if (auto tab = importAs->target.as<ImportTabLit_t>()) {
											bool macroImportingOnly = true;
											for (auto item : tab->items.objects()) {
												if (!ast_is<
														MacroName_t,
														MacroNamePair_t,
														ImportAllMacro_t>(item)) {
													macroImportingOnly = false;
												}
											}
											if (macroImportingOnly) break;
										}
									}
								} else if (auto exportNode = stmt->content.as<Export_t>()) {
									if (exportNode->target.is<Macro_t>()) break;
								}
							default:
								throw CompileError("macro exporting module only accepts macro definition, macro importing and macro expansion in place"sv, stmt);
								break;
						}
					}
				}
				str_list out;
				pushScope();
				_enableReturn.push(_info.moduleName.empty());
				_gotoScopes.push(0);
				_gotoScope = 1;
				_varArgs.push({true, false});
				if (_config.profiling) {
					auto start = std::chrono::high_resolution_clock::now();
					transformBlock(block, out,
						config.implicitReturnRoot ? ExpUsage::Return : ExpUsage::Common,
						nullptr, true);
					auto stop = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> diff = stop - start;
					compileTime = diff.count();
				} else {
					transformBlock(block, out,
						config.implicitReturnRoot ? ExpUsage::Return : ExpUsage::Common,
						nullptr, true);
				}
				popScope();
				if (!gotos.empty()) {
					for (const auto& gotoNode : gotos) {
						bool noLabel = true;
						BLOCK_START
						BREAK_IF(static_cast<int>(_labels.size()) <= gotoNode.scope);
						BREAK_IF(_labels[gotoNode.scope] == std::nullopt);
						const auto& scope = _labels[gotoNode.scope].value();
						auto it = scope.find(gotoNode.label);
						BREAK_IF(it == scope.end());
						BREAK_IF(gotoNode.level < it->second.level);
						noLabel = false;
						BLOCK_END
						if (noLabel) {
							throw CompileError("no visible label '"s + gotoNode.label + "' for <goto>"s, gotoNode.ptr->label);
						}
					}
				}
				if (config.lintGlobalVariable) {
					globals = std::make_unique<GlobalVars>();
					for (const auto& var : _globals) {
						int line, col;
						std::tie(line, col) = var.second;
						globals->push_back({var.first, line, col});
					}
					std::sort(globals->begin(), globals->end(), [](const GlobalVar& varA, const GlobalVar& varB) {
						if (varA.line < varB.line) {
							return true;
						} else if (varA.line == varB.line) {
							return varA.col < varB.col;
						} else {
							return false;
						}
					});
				}
#ifndef YUE_NO_MACRO
				if (L) {
					int top = lua_gettop(L);
					DEFER(lua_settop(L, top));
					if (!options) {
						options = std::make_unique<Options>();
					}
					pushYue("options"sv);
					lua_pushnil(L); // options startKey
					while (lua_next(L, -2) != 0) { // options key value
						size_t len = 0;
						auto pstr = lua_tolstring(L, -2, &len);
						std::string key{pstr, len};
						pstr = lua_tolstring(L, -1, &len);
						std::string value{pstr, len};
						(*options)[key] = value;
						lua_pop(L, 1); // options key
					}
				}
#endif // YUE_NO_MACRO
				return {std::move(out.back()), std::nullopt, std::move(globals), std::move(options), parseTime, compileTime};
			} catch (const CompileError& error) {
				auto displayMessage = _info.errorMessage(error.what(), error.line, error.col);
				return {
					std::string(),
					CompileInfo::Error{
						error.what(),
						error.line, error.col,
						displayMessage},
					std::move(globals),
					std::move(options),
					parseTime, compileTime};
			}
		} else {
			const auto& error = _info.error.value();
			if (!_info.codes) {
				return {
					std::string(),
					CompileInfo::Error{
						error.msg,
						error.line, error.col,
						""},
					std::move(globals),
					std::move(options),
					parseTime, compileTime};
			}
			auto displayMessage = _info.errorMessage(error.msg, error.line, error.col);
			return {
				std::string(),
				CompileInfo::Error{
					error.msg,
					error.line, error.col,
					displayMessage},
				std::move(globals),
				std::move(options),
				parseTime, compileTime};
		}
	}

	void clear() {
		_indentOffset = 0;
		_scopes.clear();
		_codeCache.clear();
		_buf.str("");
		_buf.clear();
		_joinBuf.str("");
		_joinBuf.clear();
		_globals.clear();
		_info = {};
		_varArgs = {};
		_withVars = {};
		_continueVars = {};
		_enableReturn = {};
#ifndef YUE_NO_MACRO
		if (_useModule) {
			_useModule = false;
			if (!_sameModule) {
				int top = lua_gettop(L);
				DEFER(lua_settop(L, top));
				lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
				lua_rawget(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE], tb
				int idx = static_cast<int>(lua_objlen(L, -1));
				lua_pushnil(L); // tb nil
				lua_rawseti(L, -2, idx); // tb[idx] = nil, tb
			}
		}
#endif // YUE_NO_MACRO
	}

private:
#ifndef YUE_NO_MACRO
	bool _stateOwner = false;
	bool _useModule = false;
	bool _sameModule = false;
	lua_State* L = nullptr;
	std::function<void(void*)> _luaOpen;
#endif // YUE_NO_MACRO
	YueConfig _config;
	YueParser _parser;
	ParseInfo _info;
	int _indentOffset = 0;
	struct VarArgState {
		bool hasVar;
		bool usedVar;
	};
	std::stack<VarArgState> _varArgs;
	std::stack<bool> _enableReturn;
	std::stack<bool> _enableBreakLoop;
	std::stack<std::string> _withVars;
	struct ContinueVar {
		std::string var;
		ast_ptr<false, ExpListAssign_t> condAssign;
	};
	std::stack<ContinueVar> _continueVars;
	std::list<std::unique_ptr<input>> _codeCache;
	std::unordered_map<std::string, std::pair<int, int>> _globals;
	std::ostringstream _buf;
	std::ostringstream _joinBuf;
	const std::string _newLine = "\n";
	int _gotoScope = 0;
	std::stack<int> _gotoScopes;
	struct LabelNode {
		int line;
		int level;
	};
	std::vector<std::optional<std::unordered_map<std::string, LabelNode>>> _labels;
	struct GotoNode {
		ast_ptr<true, Goto_t> ptr;
		std::string label;
		int scope;
		int level;
	};
	std::list<GotoNode> gotos;

	enum class LocalMode {
		None = 0,
		Capital = 1,
		Any = 2
	};
	enum class GlobalMode {
		None = 0,
		Capital = 1,
		Any = 2
	};
	enum class VarType {
		Local = 0,
		Const = 1,
		Global = 2
	};
	struct Scope {
		GlobalMode mode = GlobalMode::None;
		std::unique_ptr<std::unordered_map<std::string, VarType>> vars;
		std::unique_ptr<std::unordered_set<std::string>> allows;
		std::unique_ptr<std::unordered_set<std::string>> globals;
	};
	std::list<Scope> _scopes;
	static const std::string Empty;

	enum class MemType {
		Builtin,
		Common,
		Property
	};

	struct ClassMember {
		std::string item;
		MemType type;
		ast_node* node;
	};

	struct DestructItem {
		ast_ptr<true, Exp_t> target;
		std::string targetVar;
		ast_ptr<false, ChainValue_t> structure;
		ast_ptr<true, Exp_t> defVal;
	};

	struct Destructure {
		ast_ptr<true, ast_node> value;
		std::string valueVar;
		std::list<DestructItem> items;
		ast_ptr<false, ExpListAssign_t> inlineAssignment;
	};

	enum class ExpUsage {
		Return,
		Assignment,
		Common,
		Closure
	};

	void pushScope() {
		_scopes.emplace_back();
		_scopes.back().vars = std::make_unique<std::unordered_map<std::string, VarType>>();
	}

	void popScope() {
		_scopes.pop_back();
	}

	bool isDefined(const std::string& name) const {
		bool isDefined = false;
		int mode = int(std::isupper(name[0]) ? GlobalMode::Capital : GlobalMode::Any);
		const auto& current = _scopes.back();
		if (int(current.mode) >= mode) {
			if (!current.globals) {
				isDefined = true;
				current.vars->insert_or_assign(name, VarType::Global);
			}
		}
		decltype(_scopes.back().allows.get()) allows = nullptr;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			if (it->allows) allows = it->allows.get();
		}
		bool checkShadowScopeOnly = false;
		if (allows) {
			checkShadowScopeOnly = allows->find(name) == allows->end();
		}
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			if (vars->find(name) != vars->end()) {
				isDefined = true;
				break;
			}
			if (checkShadowScopeOnly && it->allows) break;
		}
		return isDefined;
	}

	bool isSolidDefined(const std::string& name) const {
		bool defined = false;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			auto vit = vars->find(name);
			if (vit != vars->end()) {
				defined = true;
				break;
			}
		}
		return defined;
	}

	bool isLocal(const std::string& name) const {
		bool local = false;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			auto vit = vars->find(name);
			if (vit != vars->end() && vit->second != VarType::Global) {
				local = true;
				break;
			}
		}
		return local;
	}

	bool isGlobal(const std::string& name) const {
		bool global = false;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			auto vit = vars->find(name);
			if (vit != vars->end() && vit->second == VarType::Global) {
				global = true;
				break;
			}
		}
		return global;
	}

	bool isConst(const std::string& name) const {
		bool isConst = false;
		decltype(_scopes.back().allows.get()) allows = nullptr;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			if (it->allows) allows = it->allows.get();
		}
		bool checkShadowScopeOnly = false;
		if (allows) {
			checkShadowScopeOnly = allows->find(name) == allows->end();
		}
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			auto vit = vars->find(name);
			if (vit != vars->end()) {
				isConst = (vit->second == VarType::Const);
				break;
			}
			if (checkShadowScopeOnly && it->allows) break;
		}
		return isConst;
	}

	void checkConst(const std::string& name, ast_node* x) const {
		if (isConst(name)) {
			throw CompileError("attempt to assign to const variable '"s + name + '\'', x);
		}
	}

	void markVarConst(const std::string& name) {
		auto& scope = _scopes.back();
		scope.vars->insert_or_assign(name, VarType::Const);
	}

	void markVarShadowed() {
		auto& scope = _scopes.back();
		scope.allows = std::make_unique<std::unordered_set<std::string>>();
	}

	void markVarsGlobal(GlobalMode mode) {
		auto& scope = _scopes.back();
		scope.mode = mode;
	}

	void addGlobalVar(const std::string& name, ast_node* x) {
		if (isLocal(name)) throw CompileError("can not declare a local variable to be global"sv, x);
		auto& scope = _scopes.back();
		if (!scope.globals) {
			scope.globals = std::make_unique<std::unordered_set<std::string>>();
		}
		scope.globals->insert(name);
		scope.vars->insert_or_assign(name, VarType::Global);
	}

	void addToAllowList(const std::string& name) {
		auto& scope = _scopes.back();
		scope.allows->insert(name);
	}

	void forceAddToScope(const std::string& name) {
		auto& scope = _scopes.back();
		scope.vars->insert_or_assign(name, VarType::Local);
	}

	Scope& currentScope() {
		return _scopes.back();
	}

	bool addToScope(const std::string& name) {
		bool defined = isDefined(name);
		if (!defined) {
			auto& scope = currentScope();
			scope.vars->insert_or_assign(name, VarType::Local);
		}
		return !defined;
	}

	std::string getUnusedName(std::string_view name) const {
		int index = 0;
		std::string newName;
		std::string nameStr(name);
		do {
			newName = nameStr + std::to_string(index);
			index++;
		} while (isSolidDefined(newName));
		return newName;
	}

	std::string getUnusedLabel(std::string_view label) const {
		int scopeIndex = _gotoScopes.top();
		if (static_cast<int>(_labels.size()) <= scopeIndex || _labels[scopeIndex] == std::nullopt) {
			return std::string(label) + '0';
		}
		auto& scope = _labels[scopeIndex].value();
		int index = 0;
		std::string newLabel;
		std::string labelStr(label);
		do {
			newLabel = labelStr + std::to_string(index);
			index++;
		} while (scope.find(newLabel) != scope.end());
		return newLabel;
	}

	std::string transformCondExp(Exp_t* cond, bool unless) {
		str_list tmp;
		if (unless) {
			if (auto value = singleValueFrom(cond)) {
				transformValue(value, tmp);
			} else {
				transformExp(cond, tmp, ExpUsage::Closure);
				tmp.back() = '(' + tmp.back() + ')';
			}
			return "not "s + tmp.back();
		} else {
			transformExp(cond, tmp, ExpUsage::Closure);
			return tmp.back();
		}
	}

	const std::string nll(ast_node* node) const {
		if (_config.reserveLineNumber) {
			return " -- "s + std::to_string(node->m_begin.m_line + _config.lineOffset) + _newLine;
		} else {
			return _newLine;
		}
	}

	const std::string nlr(ast_node* node) const {
		if (_config.reserveLineNumber) {
			return " -- "s + std::to_string(node->m_end.m_line + _config.lineOffset) + _newLine;
		} else {
			return _newLine;
		}
	}

	void incIndentOffset() {
		_indentOffset++;
	}

	void decIndentOffset() {
		_indentOffset--;
	}

	std::string indent() const {
		if (_config.useSpaceOverTab) {
			return std::string((_scopes.size() - 1 + _indentOffset) * 2, ' ');
		} else {
			return std::string(_scopes.size() - 1 + _indentOffset, '\t');
		}
	}

	std::string indent(int offset) const {
		if (_config.useSpaceOverTab) {
			return std::string((_scopes.size() - 1 + _indentOffset + offset) * 2, ' ');
		} else {
			return std::string(_scopes.size() - 1 + _indentOffset + offset, '\t');
		}
	}

	std::string clearBuf() {
		std::string str = _buf.str();
		_buf.str("");
		_buf.clear();
		return str;
	}

	std::string join(const str_list& items) {
		if (items.empty())
			return Empty;
		else if (items.size() == 1)
			return items.front();
		for (const auto& item : items) {
			_joinBuf << item;
		}
		auto result = _joinBuf.str();
		_joinBuf.str("");
		_joinBuf.clear();
		return result;
	}

	std::string join(const str_list& items, std::string_view sep) {
		if (items.empty())
			return Empty;
		else if (items.size() == 1)
			return items.front();
		std::string sepStr = std::string(sep);
		auto begin = ++items.begin();
		_joinBuf << items.front();
		for (auto it = begin; it != items.end(); ++it) {
			_joinBuf << sepStr << *it;
		}
		auto result = _joinBuf.str();
		_joinBuf.str("");
		_joinBuf.clear();
		return result;
	}

	UnaryExp_t* singleUnaryExpFrom(ast_node* item) const {
		Exp_t* exp = nullptr;
		switch (item->getId()) {
			case id<Exp_t>():
				exp = static_cast<Exp_t*>(item);
				break;
			case id<ExpList_t>(): {
				auto expList = static_cast<ExpList_t*>(item);
				if (expList->exprs.size() == 1) {
					exp = static_cast<Exp_t*>(expList->exprs.front());
				}
				break;
			}
			case id<ExpListLow_t>(): {
				auto expList = static_cast<ExpListLow_t*>(item);
				if (expList->exprs.size() == 1) {
					exp = static_cast<Exp_t*>(expList->exprs.front());
				}
				break;
			}
			case id<SwitchList_t>(): {
				auto expList = static_cast<SwitchList_t*>(item);
				if (expList->exprs.size() == 1) {
					exp = static_cast<Exp_t*>(expList->exprs.front());
				}
				break;
			}
			case id<UnaryExp_t>(): {
				auto unary = static_cast<UnaryExp_t*>(item);
				if (unary->expos.size() == 1) {
					return unary;
				}
				return nullptr;
			}
			default: break;
		}
		if (!exp) return nullptr;
		BLOCK_START
		BREAK_IF(exp->nilCoalesed);
		BREAK_IF(!exp->opValues.empty());
		BREAK_IF(exp->pipeExprs.size() != 1);
		auto unary = static_cast<UnaryExp_t*>(exp->pipeExprs.back());
		BREAK_IF(unary->expos.size() != 1);
		return unary;
		BLOCK_END
		return nullptr;
	}

	Value_t* singleValueFrom(ast_node* item) const {
		if (auto unary = singleUnaryExpFrom(item)) {
			if (unary->ops.empty()) {
				return static_cast<Value_t*>(unary->expos.back());
			}
		}
		return nullptr;
	}

	ast_ptr<false, Exp_t> newExp(SimpleValue_t* simpleValue, ast_node* x) {
		auto value = x->new_ptr<Value_t>();
		value->item.set(simpleValue);
		return newExp(value, x);
	}

	ast_ptr<false, Exp_t> newExp(String_t* string, ast_node* x) {
		auto value = x->new_ptr<Value_t>();
		value->item.set(string);
		return newExp(value, x);
	}

	ast_ptr<false, Exp_t> newExp(ChainValue_t* chainValue, ast_node* x) {
		auto value = x->new_ptr<Value_t>();
		value->item.set(chainValue);
		return newExp(value, x);
	}

	ast_ptr<false, Exp_t> newExp(Value_t* value, ast_node* x) {
		auto unary = x->new_ptr<UnaryExp_t>();
		unary->expos.push_back(value);
		auto exp = x->new_ptr<Exp_t>();
		exp->pipeExprs.push_back(unary);
		return exp;
	}

	ast_ptr<false, Exp_t> newExp(Value_t* left, BinaryOperator_t* op, Value_t* right, ast_node* x) {
		auto lunary = x->new_ptr<UnaryExp_t>();
		lunary->expos.push_back(left);
		auto opValue = x->new_ptr<ExpOpValue_t>();
		{
			auto runary = x->new_ptr<UnaryExp_t>();
			runary->expos.push_back(right);
			opValue->op.set(op);
			opValue->pipeExprs.push_back(runary);
		}
		auto exp = x->new_ptr<Exp_t>();
		exp->pipeExprs.push_back(lunary);
		exp->opValues.push_back(opValue);
		return exp;
	}

	ast_ptr<false, Exp_t> newExp(UnaryExp_t* unary, ast_node* x) {
		auto exp = x->new_ptr<Exp_t>();
		exp->pipeExprs.push_back(unary);
		return exp;
	}

	SimpleValue_t* simpleSingleValueFrom(ast_node* node) const {
		auto value = singleValueFrom(node);
		if (value && value->item.is<SimpleValue_t>()) {
			return static_cast<SimpleValue_t*>(value->item.get());
		}
		return nullptr;
	}

	Statement_t* lastStatementFrom(ast_node* body) const {
		switch (body->getId()) {
			case id<Block_t>():
				return lastStatementFrom(static_cast<Block_t*>(body));
			case id<Statement_t>(): {
				return static_cast<Statement_t*>(body);
			}
			default: YUEE("AST node mismatch", body); break;
		}
		return nullptr;
	}

	Statement_t* lastStatementFrom(const node_container& stmts) const {
		if (!stmts.empty()) {
			auto it = stmts.end();
			--it;
			while (!static_cast<Statement_t*>(*it)->content && it != stmts.begin()) {
				--it;
			}
			return static_cast<Statement_t*>(*it);
		}
		return nullptr;
	}

	Statement_t* lastStatementFrom(Body_t* body) const {
		if (auto stmt = body->content.as<Statement_t>()) {
			return stmt;
		} else {
			const auto& stmts = body->content.to<Block_t>()->statements.objects();
			return lastStatementFrom(stmts);
		}
	}

	Statement_t* lastStatementFrom(Block_t* block) const {
		const auto& stmts = block->statements.objects();
		return lastStatementFrom(stmts);
	}

	Exp_t* lastExpFromAssign(ast_node* action) {
		switch (action->getId()) {
			case id<Update_t>(): {
				auto update = static_cast<Update_t*>(action);
				return update->value;
			}
			case id<Assign_t>(): {
				auto assign = static_cast<Assign_t*>(action);
				return ast_cast<Exp_t>(assign->values.back());
			}
		}
		return nullptr;
	}

	Exp_t* lastExpFromStatement(Statement_t* stmt) {
		if (!stmt->content) return nullptr;
		switch (stmt->content->getId()) {
			case id<ExpListAssign_t>(): {
				auto expListAssign = static_cast<ExpListAssign_t*>(stmt->content.get());
				if (auto action = expListAssign->action.get()) {
					return lastExpFromAssign(action);
				} else {
					return static_cast<Exp_t*>(expListAssign->expList->exprs.back());
				}
			}
			case id<Export_t>(): {
				auto exportNode = static_cast<Export_t*>(stmt->content.get());
				if (auto action = exportNode->assign.get()) {
					return lastExpFromAssign(action);
				} else {
					switch (exportNode->target->getId()) {
						case id<Exp_t>(): return exportNode->target.to<Exp_t>();
						case id<ExpList_t>(): return static_cast<Exp_t*>(exportNode->target.to<ExpList_t>()->exprs.back());
					}
				}
				return nullptr;
			}
			case id<Local_t>(): {
				if (auto localValues = static_cast<Local_t*>(stmt->content.get())->item.as<LocalValues_t>()) {
					if (auto expList = localValues->valueList.as<ExpListLow_t>()) {
						return static_cast<Exp_t*>(expList->exprs.back());
					}
				}
				return nullptr;
			}
			case id<Global_t>(): {
				if (auto globalValues = static_cast<Global_t*>(stmt->content.get())->item.as<GlobalValues_t>()) {
					if (auto expList = globalValues->valueList.as<ExpListLow_t>()) {
						return static_cast<Exp_t*>(expList->exprs.back());
					}
				}
				return nullptr;
			}
		}
		return nullptr;
	}

	template <class T>
	ast_ptr<false, T> toAst(std::string_view codes, ast_node* parent) {
		auto res = _parser.parse<T>(std::string(codes));
		res.node->traverse([&](ast_node* node) {
			node->m_begin.m_line = parent->m_begin.m_line;
			node->m_begin.m_col = parent->m_begin.m_col;
			node->m_end.m_line = parent->m_end.m_line;
			node->m_end.m_col = parent->m_end.m_col;
			return traversal::Continue;
		});
		_codeCache.push_back(std::move(res.codes));
		return ast_ptr<false, T>(res.node.template to<T>());
	}

	bool isChainValueCall(ChainValue_t* chainValue) const {
		return ast_is<InvokeArgs_t, Invoke_t>(chainValue->items.back());
	}

	enum class ChainType {
		Common,
		EndWithColon,
		EndWithEOP,
		HasEOP,
		HasKeyword,
		Macro,
		Metatable,
		MetaFieldInvocation
	};

	ChainType specialChainValue(ChainValue_t* chainValue) const {
		if (isMacroChain(chainValue)) {
			return ChainType::Macro;
		}
		if (ast_is<ColonChainItem_t>(chainValue->items.back())) {
			return ChainType::EndWithColon;
		}
		if (ast_is<ExistentialOp_t>(chainValue->items.back())) {
			return ChainType::EndWithEOP;
		}
		if (auto dot = ast_cast<DotChainItem_t>(chainValue->items.back())) {
			if (dot->name.is<Metatable_t>()) {
				return ChainType::Metatable;
			}
		}
		ChainType type = ChainType::Common;
		for (auto item : chainValue->items.objects()) {
			if (auto colonChain = ast_cast<ColonChainItem_t>(item)) {
				if (ast_is<LuaKeyword_t>(colonChain->name)) {
					type = ChainType::HasKeyword;
				} else if (auto meta = colonChain->name.as<Metamethod_t>(); meta && !meta->item.is<Name_t>()) {
					return ChainType::MetaFieldInvocation;
				}
			} else if (ast_is<ExistentialOp_t>(item)) {
				return ChainType::HasEOP;
			}
		}
		return type;
	}

	std::string singleVariableFrom(ChainValue_t* chainValue) {
		BLOCK_START
		BREAK_IF(!chainValue);
		BREAK_IF(chainValue->items.size() != 1);
		auto callable = ast_cast<Callable_t>(chainValue->items.front());
		BREAK_IF(!callable);
		ast_node* var = callable->item.as<Variable_t>();
		if (!var) {
			if (auto self = callable->item.as<SelfItem_t>()) {
				var = self->name.as<Self_t>();
			}
		}
		BREAK_IF(!var);
		str_list tmp;
		transformCallable(callable, tmp);
		return tmp.back();
		BLOCK_END
		return Empty;
	}

	std::string singleVariableFrom(ast_node* expList, bool accessing) {
		if (!ast_is<Exp_t, ExpList_t>(expList)) return Empty;
		BLOCK_START
		auto value = singleValueFrom(expList);
		BREAK_IF(!value);
		auto chainValue = value->item.as<ChainValue_t>();
		BREAK_IF(!chainValue);
		BREAK_IF(chainValue->items.size() != 1);
		auto callable = ast_cast<Callable_t>(chainValue->items.front());
		BREAK_IF(!callable || !(callable->item.is<Variable_t>() || callable->getByPath<SelfItem_t, Self_t>()));
		str_list tmp;
		if (accessing) {
			transformCallable(callable, tmp);
		} else {
			bool lintGlobal = _config.lintGlobalVariable;
			_config.lintGlobalVariable = false;
			transformCallable(callable, tmp);
			_config.lintGlobalVariable = lintGlobal;
		}
		return tmp.back();
		BLOCK_END
		return Empty;
	}

	Variable_t* variableFrom(Exp_t* exp) {
		BLOCK_START
		auto value = singleValueFrom(exp);
		BREAK_IF(!value);
		auto chainValue = value->item.as<ChainValue_t>();
		BREAK_IF(!chainValue);
		BREAK_IF(chainValue->items.size() != 1);
		auto callable = ast_cast<Callable_t>(chainValue->items.front());
		BREAK_IF(!callable);
		return callable->item.as<Variable_t>();
		BLOCK_END
		return nullptr;
	}

	bool isAssignable(const node_container& chainItems) {
		if (chainItems.size() == 1) {
			auto firstItem = chainItems.back();
			if (auto callable = ast_cast<Callable_t>(firstItem)) {
				switch (callable->item->getId()) {
					case id<Variable_t>():
						checkConst(_parser.toString(callable->item.get()), callable->item.get());
						return true;
					case id<SelfItem_t>():
						return true;
				}
			} else
				switch (firstItem->getId()) {
					case id<DotChainItem_t>():
					case id<Exp_t>():
						return true;
				}
		} else {
			if (std::find_if(chainItems.begin(), chainItems.end(), [](ast_node* node) {
					return ast_is<ExistentialOp_t>(node);
				})
				!= chainItems.end()) {
				return false;
			}
			auto lastItem = chainItems.back();
			switch (lastItem->getId()) {
				case id<DotChainItem_t>():
				case id<Exp_t>():
				case id<TableAppendingOp_t>():
					return true;
			}
		}
		return false;
	}

	bool isAssignable(Exp_t* exp) {
		if (auto value = singleValueFrom(exp)) {
			auto item = value->item.get();
			switch (item->getId()) {
				case id<SimpleTable_t>():
					return true;
				case id<SimpleValue_t>(): {
					auto simpleValue = static_cast<SimpleValue_t*>(item);
					if (simpleValue->value.is<TableLit_t>()) {
						return true;
					}
					return false;
				}
				case id<ChainValue_t>(): {
					auto chainValue = static_cast<ChainValue_t*>(item);
					return isAssignable(chainValue->items.objects());
				}
			}
		}
		return false;
	}

	bool isAssignable(Assignable_t* assignable) {
		if (auto assignableChain = ast_cast<AssignableChain_t>(assignable->item)) {
			return isAssignable(assignableChain->items.objects());
		} else if (auto variable = assignable->item.as<Variable_t>()) {
			checkConst(_parser.toString(variable), variable);
		}
		return true;
	}

	void checkAssignable(ExpList_t* expList) {
		for (auto exp_ : expList->exprs.objects()) {
			Exp_t* exp = static_cast<Exp_t*>(exp_);
			if (!isAssignable(exp)) {
				throw CompileError("left hand expression is not assignable"sv, exp);
			}
		}
	}

	bool isPureBackcall(Exp_t* exp) const {
		return exp->opValues.empty() && exp->pipeExprs.size() > 1;
	}

	bool isPureNilCoalesed(Exp_t* exp) const {
		return exp->nilCoalesed && exp->opValues.empty();
	}

	bool isMacroChain(ChainValue_t* chainValue) const {
		const auto& chainList = chainValue->items.objects();
		auto callable = ast_cast<Callable_t>(chainList.front());
		if (callable && callable->item.is<MacroName_t>()) {
			return true;
		}
		return false;
	}

	void pushFunctionScope() {
		_enableReturn.push(true);
		_enableBreakLoop.push(false);
		_gotoScopes.push(_gotoScope);
		_gotoScope++;
	}

	void popFunctionScope() {
		_enableReturn.pop();
		_enableBreakLoop.pop();
		_gotoScopes.pop();
	}

	void pushAnonVarArg() {
		if (!_varArgs.empty() && _varArgs.top().hasVar) {
			_varArgs.push({true, false});
		} else {
			_varArgs.push({false, false});
		}
	}

	void popAnonVarArg() {
		_varArgs.pop();
	}

	std::string anonFuncStart() const {
		return !_varArgs.empty() && _varArgs.top().hasVar && _varArgs.top().usedVar ? "(function(...)"s : "(function()"s;
	}

	std::string anonFuncEnd() const {
		return !_varArgs.empty() && _varArgs.top().usedVar ? "end)(...)"s : "end)()"s;
	}

	std::string globalVar(std::string_view var, ast_node* x) {
		std::string str(var);
		if (_config.lintGlobalVariable) {
			if (!isDefined(str)) {
				if (_globals.find(str) == _globals.end()) {
					_globals[str] = {x->m_begin.m_line, x->m_begin.m_col};
				}
			}
		}
		return str;
	}

	void checkMetamethod(const std::string& name, ast_node* x) {
		if (Metamethods.find(name) == Metamethods.end()) {
			throw CompileError("invalid metamethod name"sv, x);
		}
		int target = getLuaTarget(x);
		switch (target) {
			case 501: goto metamethod51;
			case 502: goto metamethod52;
			case 503: {
				if (name == "ipairs"sv) {
					throw CompileError("metamethod is deprecated since Lua 5.3"sv, x);
				}
				goto metamethod53;
			}
			case 504: {
				if (name == "ipairs"sv) {
					throw CompileError("metamethod is not supported since Lua 5.4"sv, x);
				}
				goto metamethod54;
			}
		}
	metamethod51:
		if (name == "pairs"sv || name == "ipairs"sv) {
			throw CompileError("metamethod is not supported until Lua 5.2"sv, x);
		}
	metamethod52:
		if (name == "name"sv || name == "idiv"sv || name == "band"sv || name == "bor"sv || name == "bxor"sv || name == "bnot"sv || name == "shl"sv || name == "shr"sv) {
			throw CompileError("metamethod is not supported until Lua 5.3"sv, x);
		}
	metamethod53:
		if (name == "close"sv) {
			throw CompileError("metamethod is not supported until Lua 5.4"sv, x);
		}
	metamethod54:
		return;
	}

	void transformStatement(Statement_t* statement, str_list& out) {
		auto x = statement;
		if (_config.reserveComment && !x->comments.empty()) {
			for (ast_node* node : x->comments.objects()) {
				switch (node->getId()) {
					case id<YueLineComment_t>(): {
						auto comment = ast_cast<YueLineComment_t>(node);
						out.push_back(indent() + "--"s + _parser.toString(comment) + '\n');
						break;
					}
					case id<YueMultilineComment_t>(): {
						auto comment = ast_cast<YueMultilineComment_t>(node);
						out.push_back(indent() + _parser.toString(comment) + '\n');
						break;
					}
				}
			}
		}
		if (statement->appendix) {
			if (auto assignment = assignmentFrom(statement)) {
				auto preDefine = getPreDefineLine(assignment);
				if (!preDefine.empty()) out.push_back(preDefine);
			} else if (auto local = statement->content.as<Local_t>()) {
				if (!local->defined) {
					local->defined = true;
					transformLocalDef(local, out);
				}
			} else if (auto attrib = statement->content.as<LocalAttrib_t>()) {
				auto appendix = statement->appendix.get();
				switch (appendix->item->getId()) {
					case id<IfLine_t>(): {
						auto if_line = static_cast<IfLine_t*>(appendix->item.get());
						auto ifNode = x->new_ptr<If_t>();
						ifNode->type.set(if_line->type);
						ifNode->nodes.push_back(if_line->condition);

						auto expList = x->new_ptr<ExpList_t>();
						for (auto val : attrib->assign->values.objects()) {
							switch (val->getId()) {
								case id<If_t>():
								case id<Switch_t>():
								case id<With_t>(): {
									auto simpleValue = x->new_ptr<SimpleValue_t>();
									simpleValue->value.set(val);
									auto exp = newExp(simpleValue, x);
									expList->exprs.push_back(exp);
									break;
								}
								case id<TableBlock_t>(): {
									auto tableBlock = static_cast<TableBlock_t*>(val);
									auto tabLit = x->new_ptr<TableLit_t>();
									tabLit->values.dup(tableBlock->values);
									auto simpleValue = x->new_ptr<SimpleValue_t>();
									simpleValue->value.set(tabLit);
									auto exp = newExp(simpleValue, x);
									expList->exprs.push_back(exp);
									break;
								}
								case id<Exp_t>(): {
									expList->exprs.push_back(val);
									break;
								}
								default: YUEE("AST node mismatch", val); break;
							}
						}
						auto expListAssign = x->new_ptr<ExpListAssign_t>();
						expListAssign->expList.set(expList);
						auto stmt = x->new_ptr<Statement_t>();
						stmt->content.set(expListAssign);
						ifNode->nodes.push_back(stmt);

						statement->appendix.set(nullptr);
						attrib->assign->values.clear();
						attrib->assign->values.push_back(ifNode);
						transformStatement(statement, out);
						return;
					}
					case id<WhileLine_t>(): {
						throw CompileError("while-loop line decorator is not supported here"sv, appendix->item.get());
						break;
					}
					case id<CompInner_t>(): {
						throw CompileError("for-loop line decorator is not supported here"sv, appendix->item.get());
						break;
					}
					default: YUEE("AST node mismatch", appendix->item.get()); break;
				}
			} else if (!statement->appendix->item.is<IfLine_t>()) {
				auto appendix = statement->appendix->item.get();
				switch (statement->content->getId()) {
					case id<Return_t>():
						throw CompileError("loop line decorator can not be used in a return statement"sv, appendix);
						break;
					case id<BreakLoop_t>():
						throw CompileError("loop line decorator can not be used in a break-loop statement"sv, appendix);
						break;
				}
			}
			auto appendix = statement->appendix.get();
			switch (appendix->item->getId()) {
				case id<IfLine_t>(): {
					auto if_line = static_cast<IfLine_t*>(appendix->item.get());
					auto ifNode = x->new_ptr<If_t>();
					ifNode->type.set(if_line->type);
					ifNode->nodes.push_back(if_line->condition);

					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					ifNode->nodes.push_back(stmt);

					statement->appendix.set(nullptr);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(ifNode);
					auto exp = newExp(simpleValue, x);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					statement->content.set(expListAssign);
					break;
				}
				case id<WhileLine_t>(): {
					auto while_line = static_cast<WhileLine_t*>(appendix->item.get());
					auto whileNode = x->new_ptr<While_t>();
					whileNode->type.set(while_line->type);
					whileNode->condition.set(while_line->condition);

					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					whileNode->body.set(stmt);

					statement->appendix.set(nullptr);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(whileNode);
					auto exp = newExp(simpleValue, x);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					statement->content.set(expListAssign);
					break;
				}
				case id<CompInner_t>(): {
					auto compInner = appendix->item.to<CompInner_t>();
					auto comp = x->new_ptr<Comprehension_t>();
					comp->forLoop.set(compInner);
					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					comp->value.set(stmt);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(comp);
					auto exp = newExp(simpleValue, x);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					statement->content.set(expListAssign);
					statement->appendix.set(nullptr);
					break;
				}
				default: YUEE("AST node mismatch", appendix->item.get()); break;
			}
		}
		auto content = statement->content.get();
		if (!content) {
			out.push_back(Empty);
			return;
		}
		switch (content->getId()) {
			case id<Import_t>(): transformImport(static_cast<Import_t*>(content), out); break;
			case id<While_t>(): transformWhile(static_cast<While_t*>(content), out); break;
			case id<Repeat_t>(): transformRepeat(static_cast<Repeat_t*>(content), out); break;
			case id<For_t>(): transformFor(static_cast<For_t*>(content), out); break;
			case id<ForEach_t>(): transformForEach(static_cast<ForEach_t*>(content), out); break;
			case id<Return_t>(): transformReturn(static_cast<Return_t*>(content), out); break;
			case id<Local_t>(): transformLocal(static_cast<Local_t*>(content), out); break;
			case id<Global_t>(): transformGlobal(static_cast<Global_t*>(content), out); break;
			case id<Export_t>(): transformExport(static_cast<Export_t*>(content), out); break;
			case id<Macro_t>(): transformMacro(static_cast<Macro_t*>(content), out, false); break;
			case id<MacroInPlace_t>(): transformMacroInPlace(static_cast<MacroInPlace_t*>(content)); break;
			case id<BreakLoop_t>(): transformBreakLoop(static_cast<BreakLoop_t*>(content), out); break;
			case id<Label_t>(): transformLabel(static_cast<Label_t*>(content), out); break;
			case id<Goto_t>(): transformGoto(static_cast<Goto_t*>(content), out); break;
			case id<ShortTabAppending_t>(): transformShortTabAppending(static_cast<ShortTabAppending_t*>(content), out); break;
			case id<LocalAttrib_t>(): transformLocalAttrib(static_cast<LocalAttrib_t*>(content), out); break;
			case id<PipeBody_t>(): throw CompileError("pipe chain must be following a value"sv, x); break;
			case id<ExpListAssign_t>(): {
				auto expListAssign = static_cast<ExpListAssign_t*>(content);
				if (expListAssign->action) {
					transformAssignment(expListAssign, out);
				} else {
					auto expList = expListAssign->expList.get();
					if (expList->exprs.objects().empty()) {
						out.push_back(Empty);
						break;
					}
					if (auto singleValue = singleValueFrom(expList)) {
						if (auto simpleValue = singleValue->item.as<SimpleValue_t>()) {
							auto value = simpleValue->value.get();
							bool specialSingleValue = true;
							switch (value->getId()) {
								case id<If_t>(): transformIf(static_cast<If_t*>(value), out, ExpUsage::Common); break;
								case id<ClassDecl_t>(): transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Common); break;
								case id<Switch_t>(): transformSwitch(static_cast<Switch_t*>(value), out, ExpUsage::Common); break;
								case id<With_t>(): transformWith(static_cast<With_t*>(value), out); break;
								case id<ForEach_t>(): transformForEach(static_cast<ForEach_t*>(value), out); break;
								case id<For_t>(): transformFor(static_cast<For_t*>(value), out); break;
								case id<While_t>(): transformWhile(static_cast<While_t*>(value), out); break;
								case id<Do_t>(): transformDo(static_cast<Do_t*>(value), out, ExpUsage::Common); break;
								case id<Try_t>(): transformTry(static_cast<Try_t*>(value), out, ExpUsage::Common); break;
								case id<Comprehension_t>(): transformCompCommon(static_cast<Comprehension_t*>(value), out); break;
								default: specialSingleValue = false; break;
							}
							if (specialSingleValue) {
								break;
							}
						}
						if (auto chainValue = singleValue->item.as<ChainValue_t>()) {
							if (isChainValueCall(chainValue) || isMacroChain(chainValue)) {
								transformChainValue(chainValue, out, ExpUsage::Common);
								break;
							}
						}
					} else if (expList->exprs.size() == 1) {
						auto exp = static_cast<Exp_t*>(expList->exprs.back());
						if (isPureBackcall(exp)) {
							transformExp(exp, out, ExpUsage::Common);
							break;
						}
					}
					throw CompileError("unexpected expression"sv, expList);
				}
				break;
			}
			case id<ChainAssign_t>(): transformChainAssign(static_cast<ChainAssign_t*>(content), out); break;
			default: YUEE("AST node mismatch", content); break;
		}
		if (statement->needSep && !out.empty() && !out.back().empty()) {
			auto index = std::string::npos;
			if (_config.reserveLineNumber) {
				index = out.back().rfind(" -- "sv);
			} else {
				index = out.back().find_last_not_of('\n');
				if (index != std::string::npos) index++;
			}
			if (index != std::string::npos) {
				auto ending = out.back().substr(0, index);
				auto ind = ending.find_last_of(" \t\n"sv);
				if (ind != std::string::npos) {
					ending = ending.substr(ind + 1);
				}
				if (LuaKeywords.find(ending) == LuaKeywords.end()) {
					out.back().insert(index, ";"sv);
				}
			}
		}
	}

	str_list getAssignVars(ExpListAssign_t* assignment) {
		str_list vars;
		bool lintGlobal = _config.lintGlobalVariable;
		_config.lintGlobalVariable = false;
		if (!assignment->action.is<Assign_t>()) return vars;
		for (auto exp : assignment->expList->exprs.objects()) {
			auto var = singleVariableFrom(exp, true);
			vars.push_back(var.empty() ? Empty : var);
		}
		_config.lintGlobalVariable = lintGlobal;
		return vars;
	}

	str_list getAssignVars(With_t* with) {
		str_list vars;
		bool lintGlobal = _config.lintGlobalVariable;
		_config.lintGlobalVariable = false;
		for (auto exp : with->valueList->exprs.objects()) {
			auto var = singleVariableFrom(exp, true);
			vars.push_back(var.empty() ? Empty : var);
		}
		_config.lintGlobalVariable = lintGlobal;
		return vars;
	}

	enum class DefOp {
		Get,
		Check,
		Mark
	};

	str_list transformAssignDefs(ExpList_t* expList, DefOp op) {
		str_list defs;
		for (auto exp_ : expList->exprs.objects()) {
			auto exp = static_cast<Exp_t*>(exp_);
			if (auto value = singleValueFrom(exp)) {
				if (auto chain = value->item.as<ChainValue_t>()) {
					BLOCK_START
					BREAK_IF(chain->items.size() != 1);
					auto callable = ast_cast<Callable_t>(chain->items.front());
					BREAK_IF(!callable);
					std::string name;
					if (auto var = callable->item.as<Variable_t>()) {
						name = _parser.toString(var);
					} else if (auto self = callable->item.as<SelfItem_t>()) {
						if (self->name.is<Self_t>()) name = "self"sv;
					}
					BREAK_IF(name.empty());
					switch (op) {
						case DefOp::Mark:
							if (addToScope(name)) defs.push_back(name);
							break;
						case DefOp::Check:
							if (!isDefined(name)) defs.push_back(name);
							break;
						case DefOp::Get:
							defs.push_back(name);
							break;
					}
					BLOCK_END
				}
			} else {
				throw CompileError("left hand expression is not assignable"sv, exp);
			}
		}
		return defs;
	}

	std::string toLocalDecl(const str_list& defs) {
		if (defs.empty()) return Empty;
		return indent() + "local "s + join(defs, ", "sv);
	}

	std::string getDestrucureDefine(ExpListAssign_t* assignment) {
		auto info = extractDestructureInfo(assignment, true, false);
		if (info.assignment) {
			_buf << getPreDefineLine(info.assignment);
		}
		if (!info.destructures.empty()) {
			for (const auto& destruct : info.destructures) {
				str_list defs;
				for (const auto& item : destruct.items) {
					if (!item.targetVar.empty()) {
						if (addToScope(item.targetVar)) {
							defs.push_back(item.targetVar);
						}
					}
				}
				if (!defs.empty()) _buf << indent() << "local "sv << join(defs, ", "sv);
			}
		}
		return clearBuf();
	}

	std::string getPreDefine(ExpListAssign_t* assignment) {
		auto preDefine = getDestrucureDefine(assignment);
		if (preDefine.empty()) {
			preDefine = toLocalDecl(transformAssignDefs(assignment->expList, DefOp::Mark));
		}
		return preDefine;
	}

	std::string getPreDefineLine(ExpListAssign_t* assignment) {
		auto preDefine = getPreDefine(assignment);
		if (!preDefine.empty()) preDefine += nll(assignment);
		return preDefine;
	}

	ExpList_t* expListFrom(Statement_t* statement) {
		if (auto expListAssign = statement->content.as<ExpListAssign_t>()) {
			if (!expListAssign->action) {
				return expListAssign->expList.get();
			}
		}
		return nullptr;
	}

	ExpListAssign_t* assignmentFrom(Statement_t* statement) {
		if (auto expListAssign = statement->content.as<ExpListAssign_t>()) {
			if (expListAssign->action) {
				return expListAssign;
			}
		}
		return nullptr;
	}

	ast_ptr<false, ExpListAssign_t> assignmentFrom(Exp_t* target, ast_node* value, ast_node* x) {
		auto assignment = x->new_ptr<ExpListAssign_t>();
		auto assignList = x->new_ptr<ExpList_t>();
		assignList->exprs.push_back(target);
		assignment->expList.set(assignList);
		auto assign = x->new_ptr<Assign_t>();
		assign->values.push_back(value);
		assignment->action.set(assign);
		return assignment;
	}

	void markDestructureConst(ExpListAssign_t* assignment) {
		auto info = extractDestructureInfo(assignment, true, false);
		for (auto& destruct : info.destructures) {
			for (auto& item : destruct.items) {
				if (item.targetVar.empty()) {
					throw CompileError("can only declare variable as const"sv, item.target);
				}
				markVarConst(item.targetVar);
			}
		}
	}

	void transformAssignment(ExpListAssign_t* assignment, str_list& out, bool optionalDestruct = false) {
		checkAssignable(assignment->expList);
		BLOCK_START
		auto assign = ast_cast<Assign_t>(assignment->action);
		BREAK_IF(!assign);
		auto x = assignment;
		const auto& exprs = x->expList->exprs.objects();
		const auto& values = assign->values.objects();
		if (exprs.size() < values.size()) {
			auto num = exprs.size();
			if (num > 1) {
				_buf << "no more than "sv << num << " right values expected, got "sv << values.size();
			} else {
				_buf << "only one right value expected, got "sv << values.size();
			}
			throw CompileError(clearBuf(), values.front());
		}
		bool checkValuesLater = false;
		if (exprs.size() > values.size()) {
			BLOCK_START
			switch (values.back()->getId()) {
				case id<If_t>():
				case id<Switch_t>():
					checkValuesLater = true;
					break;
			}
			BREAK_IF(checkValuesLater);
			auto value = singleValueFrom(values.back());
			if (!value) {
				_buf << exprs.size() << " right values expected, got "sv << values.size();
				throw CompileError(clearBuf(), values.front());
			}
			if (auto val = value->item.as<SimpleValue_t>()) {
				switch (val->value->getId()) {
					case id<If_t>():
					case id<Switch_t>():
					case id<Do_t>():
					case id<Try_t>():
						checkValuesLater = true;
						break;
				}
				BREAK_IF(checkValuesLater);
			}
			auto chainValue = value->item.as<ChainValue_t>();
			if (!chainValue || !ast_is<Invoke_t, InvokeArgs_t>(chainValue->items.back())) {
				_buf << exprs.size() << " right values expected, got "sv << values.size();
				throw CompileError(clearBuf(), values.front());
			}
			auto newAssign = assign->new_ptr<Assign_t>();
			newAssign->values.dup(assign->values);
			auto i = exprs.begin();
			auto j = values.begin();
			auto je = --values.end();
			while (j != je) {
				++i;
				++j;
			}
			bool holdItem = false;
			for (auto it = i; it != exprs.end(); ++it) {
				BLOCK_START
				auto value = singleValueFrom(*it);
				BREAK_IF(!value);
				if (value->item.is<SimpleTable_t>() || value->getByPath<SimpleValue_t, TableLit_t>()) {
					holdItem = true;
					break;
				}
				auto chainValue = value->item.as<ChainValue_t>();
				BREAK_IF(!chainValue);
				str_list temp;
				if (auto dot = ast_cast<DotChainItem_t>(chainValue->items.back())) {
					BREAK_IF(!dot->name.is<Metatable_t>());
					holdItem = true;
				} else if (ast_is<TableAppendingOp_t>(chainValue->items.back())) {
					holdItem = true;
				}
				BLOCK_END
				if (holdItem) {
					break;
				}
			}
			BREAK_IF(!holdItem);
			pushScope();
			std::list<ast_ptr<false, Exp_t>> extraExprs;
			for (; i != exprs.end(); ++i) {
				auto var = getUnusedName("_obj_"sv);
				addToScope(var);
				extraExprs.push_back(toAst<Exp_t>(var, *i));
			}
			popScope();
			ast_ptr<true, ast_node> funcCall = values.back();
			assign->values.pop_back();
			auto preAssignment = funcCall->new_ptr<ExpListAssign_t>();
			auto preAssign = funcCall->new_ptr<Assign_t>();
			preAssign->values.push_back(funcCall);
			preAssignment->action.set(preAssign);
			auto preExplist = funcCall->new_ptr<ExpList_t>();
			for (const auto& item : extraExprs) {
				preExplist->exprs.push_back(item.get());
				assign->values.push_back(item.get());
			}
			preAssignment->expList.set(preExplist);
			str_list temp;
			temp.push_back(getPreDefineLine(assignment));
			temp.push_back(indent() + "do"s + nll(assignment));
			pushScope();
			transformAssignmentCommon(preAssignment, temp);
			transformAssignment(assignment, temp);
			popScope();
			temp.push_back(indent() + "end"s + nll(assignment));
			out.push_back(join(temp));
			return;
			BLOCK_END
		}
		if (!checkValuesLater) {
			auto vit = values.begin();
			for (auto it = exprs.begin(); it != exprs.end(); ++it) {
				BLOCK_START
				auto value = singleValueFrom(*it);
				BREAK_IF(!value);
				auto chainValue = value->item.as<ChainValue_t>();
				BREAK_IF(!chainValue);
				str_list temp;
				if (auto dot = ast_cast<DotChainItem_t>(chainValue->items.back())) {
					BREAK_IF(!dot->name.is<Metatable_t>());
					str_list args;
					auto tmpChain = chainValue->new_ptr<ChainValue_t>();
					tmpChain->items.dup(chainValue->items);
					tmpChain->items.pop_back();
					if (tmpChain->items.empty()) {
						if (_withVars.empty()) {
							throw CompileError("short dot/colon syntax must be called within a with block"sv, x);
						} else {
							args.push_back(_withVars.top());
						}
					} else {
						transformExp(newExp(tmpChain, tmpChain), args, ExpUsage::Closure);
					}
					if (vit == values.end()) {
						throw CompileError("right value missing"sv, values.front());
					}
					transformAssignItem(*vit, args);
					_buf << indent() << globalVar("setmetatable"sv, x) << '(' << join(args, ", "sv) << ')' << nll(x);
					temp.push_back(clearBuf());
				} else if (ast_is<TableAppendingOp_t>(chainValue->items.back())) {
					auto tmpChain = chainValue->new_ptr<ChainValue_t>();
					tmpChain->items.dup(chainValue->items);
					tmpChain->items.pop_back();
					if (tmpChain->items.empty()) {
						if (_withVars.empty()) {
							throw CompileError("short table appending must be called within a with block"sv, x);
						} else {
							tmpChain->items.push_back(toAst<Callable_t>(_withVars.top(), chainValue));
						}
					}
					auto varName = singleVariableFrom(tmpChain);
					bool isScoped = false;
					if (varName.empty() || !isLocal(varName)) {
						isScoped = true;
						temp.push_back(indent() + "do"s + nll(x));
						pushScope();
						auto objVar = getUnusedName("_obj_"sv);
						auto newAssignment = x->new_ptr<ExpListAssign_t>();
						newAssignment->expList.set(toAst<ExpList_t>(objVar, x));
						auto assign = x->new_ptr<Assign_t>();
						assign->values.push_back(newExp(tmpChain, tmpChain));
						newAssignment->action.set(assign);
						transformAssignment(newAssignment, temp);
						varName = objVar;
					}
					auto newAssignment = x->new_ptr<ExpListAssign_t>();
					newAssignment->expList.set(toAst<ExpList_t>(varName + "[#"s + varName + "+1]"s, x));
					auto assign = x->new_ptr<Assign_t>();
					if (vit == values.end()) {
						throw CompileError("right value missing"sv, values.front());
					}
					assign->values.push_back(*vit);
					newAssignment->action.set(assign);
					transformAssignment(newAssignment, temp);
					if (isScoped) {
						popScope();
						temp.push_back(indent() + "end"s + nlr(x));
					}
				} else
					break;
				auto newExpList = x->new_ptr<ExpList_t>();
				auto newAssign = x->new_ptr<Assign_t>();
				auto newAssignment = x->new_ptr<ExpListAssign_t>();
				newAssignment->expList.set(newExpList);
				newAssignment->action.set(newAssign);
				for (auto exp : exprs) {
					if (exp != *it) newExpList->exprs.push_back(exp);
				}
				for (auto value : values) {
					if (value != *vit) newAssign->values.push_back(value);
				}
				if (newExpList->exprs.empty() && newAssign->values.empty()) {
					out.push_back(join(temp));
					return;
				}
				if (newExpList->exprs.size() < newAssign->values.size()) {
					auto exp = toAst<Exp_t>("_"sv, x);
					while (newExpList->exprs.size() < newAssign->values.size()) {
						newExpList->exprs.push_back(exp);
					}
				}
				transformAssignment(newAssignment, temp);
				out.push_back(join(temp));
				return;
				BLOCK_END
				if (vit != values.end()) ++vit;
			}
		}
		BREAK_IF(assign->values.objects().size() != 1);
		auto value = assign->values.objects().back();
		if (ast_is<Exp_t>(value)) {
			if (auto val = simpleSingleValueFrom(value)) {
				value = val->value.get();
			}
		}
		switch (value->getId()) {
			case id<If_t>(): {
				auto ifNode = static_cast<If_t*>(value);
				auto assignList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformIf(ifNode, out, ExpUsage::Assignment, assignList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<Switch_t>(): {
				auto switchNode = static_cast<Switch_t*>(value);
				auto assignList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformSwitch(switchNode, out, ExpUsage::Assignment, assignList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<With_t>(): {
				auto withNode = static_cast<With_t*>(value);
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformWith(withNode, out, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<Do_t>(): {
				auto expList = assignment->expList.get();
				auto doNode = static_cast<Do_t*>(value);
				std::string preDefine = getPreDefineLine(assignment);
				transformDo(doNode, out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<Comprehension_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<TblComprehension_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<For_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformForInPlace(static_cast<For_t*>(value), out, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<ForEach_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformForEachInPlace(static_cast<ForEach_t*>(value), out, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<ClassDecl_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<While_t>(): {
				auto expList = assignment->expList.get();
				std::string preDefine = getPreDefineLine(assignment);
				transformWhileInPlace(static_cast<While_t*>(value), out, expList);
				out.back().insert(0, preDefine);
				return;
			}
			case id<TableLit_t>(): {
				auto tableLit = static_cast<TableLit_t*>(value);
				if (hasSpreadExp(tableLit->values.objects())) {
					auto expList = assignment->expList.get();
					std::string preDefine = getPreDefineLine(assignment);
					transformSpreadTable(tableLit->values.objects(), out, ExpUsage::Assignment, expList);
					out.back().insert(0, preDefine);
					return;
				}
			}
			case id<TableBlock_t>(): {
				auto tableBlock = static_cast<TableBlock_t*>(value);
				if (hasSpreadExp(tableBlock->values.objects())) {
					auto expList = assignment->expList.get();
					std::string preDefine = getPreDefineLine(assignment);
					transformSpreadTable(tableBlock->values.objects(), out, ExpUsage::Assignment, expList);
					out.back().insert(0, preDefine);
					return;
				}
			}
		}
		auto exp = ast_cast<Exp_t>(value);
		BREAK_IF(!exp);
		if (isPureBackcall(exp)) {
			auto expList = assignment->expList.get();
			transformExp(exp, out, ExpUsage::Assignment, expList);
			return;
		} else if (isPureNilCoalesed(exp)) {
			auto expList = assignment->expList.get();
			transformNilCoalesedExp(exp, out, ExpUsage::Assignment, expList);
			return;
		}
		auto singleVal = singleValueFrom(exp);
		BREAK_IF(!singleVal);
		if (auto chainValue = singleVal->item.as<ChainValue_t>()) {
			auto type = specialChainValue(chainValue);
			auto expList = assignment->expList.get();
			switch (type) {
				case ChainType::HasEOP:
				case ChainType::EndWithColon:
				case ChainType::MetaFieldInvocation: {
					std::string preDefine = getPreDefineLine(assignment);
					transformChainValue(chainValue, out, ExpUsage::Assignment, expList, false, optionalDestruct);
					out.back().insert(0, preDefine);
					return;
				}
				case ChainType::HasKeyword:
				case ChainType::Macro:
					transformChainValue(chainValue, out, ExpUsage::Assignment, expList);
					return;
				case ChainType::Common:
				case ChainType::EndWithEOP:
				case ChainType::Metatable:
					break;
			}
		}
		BLOCK_END
		auto info = extractDestructureInfo(assignment, false, optionalDestruct);
		if (info.destructures.empty()) {
			transformAssignmentCommon(assignment, out);
		} else {
			auto x = assignment;
			str_list temp;
			if (info.extraScope) {
				str_list defs;
				for (auto& destruct : info.destructures) {
					for (auto& item : destruct.items) {
						if (!item.targetVar.empty()) {
							if (!isDefined(item.targetVar)) {
								defs.push_back(item.targetVar);
							}
						}
					}
				}
				if (!defs.empty()) {
					for (const auto& def : defs) {
						checkConst(def, x);
						addToScope(def);
					}
					temp.push_back(indent() + "local "s + join(defs, ", "sv) + nll(x));
				}
				temp.push_back(indent() + "do"s + nll(x));
				pushScope();
			}
			if (info.assignment) {
				transformAssignmentCommon(info.assignment, temp);
			}
			for (auto& destruct : info.destructures) {
				std::list<std::pair<ast_ptr<true, Exp_t>, ast_ptr<true, Exp_t>>> leftPairs;
				bool extraScope = false;
				if (!destruct.inlineAssignment && destruct.items.size() == 1) {
					auto& pair = destruct.items.front();
					if (pair.targetVar.empty() && pair.defVal) {
						extraScope = true;
						auto objVar = getUnusedName("_tmp_"sv);
						auto objExp = toAst<Exp_t>(objVar, pair.target);
						leftPairs.push_back({pair.target, objExp.get()});
						pair.target.set(objExp);
						pair.targetVar = objVar;
					} else if (auto val = singleValueFrom(destruct.value); val->item.is<ChainValue_t>()) {
						auto chainValue = static_cast<ChainValue_t*>(val->item.get());
						auto newChain = val->item->new_ptr<ChainValue_t>();
						newChain->items.dup(chainValue->items);
						if (pair.structure) {
							newChain->items.dup(pair.structure->items);
						}
						auto newAssignment = assignmentFrom(pair.target, newExp(newChain, val->item), x);
						transformAssignment(newAssignment, temp, optionalDestruct);
						if (pair.defVal) {
							bool isNil = false;
							if (auto v1 = singleValueFrom(pair.defVal)) {
								if (auto v2 = v1->item.as<SimpleValue_t>()) {
									if (auto v3 = v2->value.as<ConstValue_t>()) {
										isNil = _parser.toString(v3) == "nil"sv;
									}
								}
							}
							if (!isNil) {
								auto stmt = toAst<Statement_t>(pair.targetVar + "=nil if "s + pair.targetVar + "==nil"s, pair.defVal);
								auto defAssign = stmt->content.as<ExpListAssign_t>();
								auto assign = defAssign->action.as<Assign_t>();
								assign->values.clear();
								assign->values.push_back(pair.defVal);
								transformStatement(stmt, temp);
							}
						}
						continue;
					}
					if (extraScope) {
						temp.push_back(indent() + "do"s + nll(x));
						pushScope();
					}
					if (!pair.targetVar.empty()) {
						checkConst(pair.targetVar, x);
						if (addToScope(pair.targetVar)) {
							_buf << indent() << "local "sv << pair.targetVar << nll(x);
							temp.push_back(clearBuf());
						}
					}
					bool isLocalValue = isLocal(destruct.valueVar);
					std::string objVar;
					if (isLocalValue) {
						objVar = destruct.valueVar;
					} else {
						temp.push_back(indent() + "do"s + nll(x));
						pushScope();
						objVar = getUnusedName("_obj_"sv);
						auto newAssignment = assignmentFrom(toAst<Exp_t>(objVar, x), destruct.value, x);
						transformAssignment(newAssignment, temp);
					}
					auto chain = pair.target->new_ptr<ChainValue_t>();
					chain->items.push_back(toAst<Callable_t>(objVar, chain));
					chain->items.dup(pair.structure->items);
					auto valueExp = newExp(chain, pair.target);
					auto newAssignment = assignmentFrom(pair.target, valueExp, x);
					transformAssignment(newAssignment, temp, optionalDestruct);
					if (!isLocalValue) {
						popScope();
						_buf << indent() << "end"sv << nlr(x);
						temp.push_back(clearBuf());
					}
				} else {
					str_list defs;
					std::list<ast_ptr<false, ChainValue_t>> values;
					std::list<Exp_t*> names;
					pushScope();
					for (auto& item : destruct.items) {
						if (!item.targetVar.empty()) {
							if (!isDefined(item.targetVar)) {
								defs.push_back(item.targetVar);
							}
						} else if (item.defVal) {
							extraScope = true;
							auto objVar = getUnusedName("_tmp_"sv);
							addToScope(objVar);
							auto objExp = toAst<Exp_t>(objVar, item.target);
							leftPairs.push_back({item.target, objExp.get()});
							item.target.set(objExp);
							item.targetVar = objVar;
						}
						names.push_back(item.target);
						values.push_back(item.structure);
					}
					popScope();
					if (_parser.match<Name_t>(destruct.valueVar) && isLocal(destruct.valueVar)) {
						auto callable = toAst<Callable_t>(destruct.valueVar, destruct.value);
						for (auto& v : values) {
							v->items.push_front(callable);
						}
						if (extraScope) {
							if (!defs.empty()) {
								for (const auto& def : defs) {
									checkConst(def, x);
									addToScope(def);
								}
								temp.push_back(indent() + "local "s + join(defs, ", "sv) + nll(x));
							}
							temp.push_back(indent() + "do"s + nll(x));
							pushScope();
						}
					} else {
						if (!defs.empty()) {
							for (const auto& def : defs) {
								checkConst(def, x);
								addToScope(def);
							}
							temp.push_back(indent() + "local "s + join(defs, ", "sv) + nll(x));
						}
						extraScope = true;
						temp.push_back(indent() + "do"s + nll(x));
						pushScope();
						auto valVar = getUnusedName("_obj_"sv);
						auto targetVar = toAst<Exp_t>(valVar, destruct.value);
						auto newAssignment = assignmentFrom(targetVar, destruct.value, destruct.value);
						transformAssignment(newAssignment, temp);
						auto callable = singleValueFrom(targetVar)->item.to<ChainValue_t>()->items.front();
						for (auto& v : values) {
							v->items.push_front(callable);
						}
					}
					if (destruct.inlineAssignment) {
						if (!extraScope) {
							extraScope = true;
							temp.push_back(indent() + "do"s + nll(x));
							pushScope();
						}
						transformAssignment(destruct.inlineAssignment, temp);
					}
					if (optionalDestruct) {
						while (!names.empty()) {
							auto name = names.front();
							names.pop_front();
							auto value = values.front().get();
							auto valueList = value->new_ptr<ExpList_t>();
							valueList->exprs.push_back(newExp(value, value));
							values.pop_front();
							auto newAssignment = x->new_ptr<ExpListAssign_t>();
							auto assignList = x->new_ptr<ExpList_t>();
							assignList->exprs.push_back(name);
							newAssignment->expList.set(assignList);
							auto assign = x->new_ptr<Assign_t>();
							assign->values.dup(valueList->exprs);
							newAssignment->action.set(assign);
							transformAssignment(newAssignment, temp, true);
						}
					} else {
						auto valueList = x->new_ptr<ExpList_t>();
						for (const auto& v : values) {
							valueList->exprs.push_back(newExp(v, v));
						}
						auto newAssignment = x->new_ptr<ExpListAssign_t>();
						auto assignList = x->new_ptr<ExpList_t>();
						for (auto name : names) {
							assignList->exprs.push_back(name);
						}
						newAssignment->expList.set(assignList);
						auto assign = x->new_ptr<Assign_t>();
						assign->values.dup(valueList->exprs);
						newAssignment->action.set(assign);
						transformAssignment(newAssignment, temp);
					}
				}
				for (const auto& item : destruct.items) {
					if (item.defVal) {
						bool isNil = false;
						if (auto v1 = singleValueFrom(item.defVal)) {
							if (auto v2 = v1->item.as<SimpleValue_t>()) {
								if (auto v3 = v2->value.as<ConstValue_t>()) {
									isNil = _parser.toString(v3) == "nil"sv;
								}
							}
						}
						if (!isNil) {
							auto stmt = toAst<Statement_t>(item.targetVar + "=nil if "s + item.targetVar + "==nil", item.defVal);
							auto defAssign = stmt->content.as<ExpListAssign_t>();
							auto assign = defAssign->action.as<Assign_t>();
							assign->values.clear();
							assign->values.push_back(item.defVal);
							transformStatement(stmt, temp);
						}
					}
				}
				for (const auto& item : leftPairs) {
					auto newAssignment = assignmentFrom(item.first, item.second, x);
					transformAssignment(newAssignment, temp);
				}
				if (extraScope) {
					popScope();
					_buf << indent() << "end"sv << nlr(x);
					temp.push_back(clearBuf());
				}
			}
			if (info.extraScope) {
				popScope();
				temp.push_back(indent() + "end"s + nlr(x));
			}
			out.push_back(join(temp));
		}
	}

	void transformAssignItem(ast_node* value, str_list& out) {
		switch (value->getId()) {
			case id<With_t>(): transformWithClosure(static_cast<With_t*>(value), out); break;
			case id<If_t>(): transformIf(static_cast<If_t*>(value), out, ExpUsage::Closure); break;
			case id<Switch_t>(): transformSwitch(static_cast<Switch_t*>(value), out, ExpUsage::Closure); break;
			case id<TableBlock_t>(): transformTableBlock(static_cast<TableBlock_t*>(value), out); break;
			case id<Exp_t>(): transformExp(static_cast<Exp_t*>(value), out, ExpUsage::Closure); break;
			default: YUEE("AST node mismatch", value); break;
		}
	}

	std::list<DestructItem> destructFromExp(ast_node* node, bool optional) {
		const node_container* tableItems = nullptr;
		ast_ptr<false, ExistentialOp_t> sep = optional ? node->new_ptr<ExistentialOp_t>() : nullptr;
		switch (node->getId()) {
			case id<Exp_t>(): {
				auto item = singleValueFrom(node)->item.get();
				if (!item) throw CompileError("invalid destructure value"sv, node);
				auto tbA = item->getByPath<TableLit_t>();
				if (tbA) {
					tableItems = &tbA->values.objects();
				} else {
					auto tbB = ast_cast<SimpleTable_t>(item);
					if (tbB) tableItems = &tbB->pairs.objects();
				}
				break;
			}
			case id<TableBlock_t>(): {
				auto table = ast_cast<TableBlock_t>(node);
				tableItems = &table->values.objects();
				break;
			}
			case id<TableBlockIndent_t>(): {
				auto table = ast_cast<TableBlockIndent_t>(node);
				tableItems = &table->values.objects();
				break;
			}
			case id<TableLit_t>(): {
				auto table = ast_cast<TableLit_t>(node);
				tableItems = &table->values.objects();
				break;
			}
			case id<SimpleTable_t>(): {
				auto table = ast_cast<SimpleTable_t>(node);
				tableItems = &table->pairs.objects();
				break;
			}
			default: YUEE("AST node mismatch", node); break;
		}
		if (!tableItems) throw CompileError("invalid destructure value"sv, node);
		std::list<DestructItem> pairs;
		int index = 0;
		auto subMetaDestruct = node->new_ptr<TableLit_t>();
		for (auto pair : *tableItems) {
			switch (pair->getId()) {
				case id<Exp_t>():
				case id<NormalDef_t>(): {
					Exp_t* defVal = nullptr;
					if (auto nd = ast_cast<NormalDef_t>(pair)) {
						pair = nd->item.get();
						defVal = nd->defVal.get();
					}
					++index;
					if (!isAssignable(static_cast<Exp_t*>(pair))) {
						throw CompileError("can't destructure value"sv, pair);
					}
					auto value = singleValueFrom(pair);
					auto item = value->item.get();
					if (ast_is<SimpleTable_t>(item) || item->getByPath<TableLit_t>()) {
						auto subPairs = destructFromExp(pair, optional);
						if (!subPairs.empty()) {
							if (defVal) {
								throw CompileError("default value is not supported here"sv, defVal);
							}
						}
						auto indexItem = toAst<Exp_t>(std::to_string(index), value);
						for (auto& p : subPairs) {
							if (sep) p.structure->items.push_front(sep);
							p.structure->items.push_front(indexItem);
							pairs.push_back(p);
						}
					} else {
						auto exp = static_cast<Exp_t*>(pair);
						auto varName = singleVariableFrom(exp, false);
						if (varName == "_"sv) break;
						auto chain = exp->new_ptr<ChainValue_t>();
						auto indexItem = toAst<Exp_t>(std::to_string(index), exp);
						chain->items.push_back(indexItem);
						pairs.push_back({exp,
							varName,
							chain,
							defVal});
					}
					break;
				}
				case id<VariablePair_t>():
				case id<VariablePairDef_t>(): {
					Exp_t* defVal = nullptr;
					if (auto vpd = ast_cast<VariablePairDef_t>(pair)) {
						pair = vpd->pair.get();
						defVal = vpd->defVal.get();
					}
					auto vp = static_cast<VariablePair_t*>(pair);
					auto name = _parser.toString(vp->name);
					auto chain = toAst<ChainValue_t>('.' + name, vp->name);
					pairs.push_back({toAst<Exp_t>(name, vp).get(),
						name,
						chain,
						defVal});
					break;
				}
				case id<NormalPair_t>():
				case id<NormalPairDef_t>(): {
					Exp_t* defVal = nullptr;
					if (auto npd = ast_cast<NormalPairDef_t>(pair)) {
						pair = npd->pair.get();
						defVal = npd->defVal.get();
					}
					auto np = static_cast<NormalPair_t*>(pair);
					ast_ptr<true, ast_node> keyIndex;
					if (np->key) {
						if (auto key = np->key->getByPath<Name_t>()) {
							auto keyNameStr = _parser.toString(key);
							if (LuaKeywords.find(keyNameStr) != LuaKeywords.end()) {
								keyIndex = toAst<Exp_t>('"' + keyNameStr + '"', key).get();
							} else {
								keyIndex = toAst<DotChainItem_t>('.' + keyNameStr, key).get();
							}
						} else if (auto key = np->key->getByPath<SelfItem_t>()) {
							auto callable = np->new_ptr<Callable_t>();
							callable->item.set(key);
							auto chainValue = np->new_ptr<ChainValue_t>();
							chainValue->items.push_back(callable);
							keyIndex = newExp(chainValue, np).get();
						} else if (auto key = np->key.as<Exp_t>()) {
							keyIndex = key;
						} else if (auto key = np->key.as<String_t>()) {
							keyIndex = newExp(key, np->key).get();
						} else {
							throw CompileError("unsupported key for destructuring"sv, np);
						}
					}
					if (auto exp = np->value.as<Exp_t>()) {
						if (!isAssignable(exp)) throw CompileError("can't do destructure value"sv, exp);
						auto item = singleValueFrom(exp)->item.get();
						if (ast_is<SimpleTable_t>(item) || item->getByPath<TableLit_t>()) {
							auto subPairs = destructFromExp(exp, optional);
							if (!subPairs.empty()) {
								if (defVal) {
									throw CompileError("default value is not supported here"sv, defVal);
								}
							}
							for (auto& p : subPairs) {
								if (keyIndex) {
									if (sep) p.structure->items.push_front(sep);
									p.structure->items.push_front(keyIndex);
								}
								pairs.push_back(p);
							}
						} else {
							auto chain = exp->new_ptr<ChainValue_t>();
							if (keyIndex) chain->items.push_back(keyIndex);
							auto varName = singleVariableFrom(exp, false);
							pairs.push_back({exp,
								varName,
								chain,
								defVal});
						}
						break;
					}
					if (np->value.is<TableBlock_t>()) {
						auto subPairs = destructFromExp(np->value, optional);
						if (!subPairs.empty()) {
							if (defVal) {
								throw CompileError("default value is not supported here"sv, defVal);
							}
						}
						for (auto& p : subPairs) {
							if (keyIndex) {
								if (sep) p.structure->items.push_front(sep);
								p.structure->items.push_front(keyIndex);
							}
							pairs.push_back(p);
						}
					}
					break;
				}
				case id<TableBlockIndent_t>(): {
					auto tb = static_cast<TableBlockIndent_t*>(pair);
					++index;
					auto subPairs = destructFromExp(tb, optional);
					auto indexItem = toAst<Exp_t>(std::to_string(index), tb);
					for (auto& p : subPairs) {
						if (sep) p.structure->items.push_front(sep);
						p.structure->items.push_front(indexItem);
						pairs.push_back(p);
					}
					break;
				}
				case id<MetaVariablePair_t>():
				case id<MetaVariablePairDef_t>(): {
					Exp_t* defVal = nullptr;
					if (auto mvpd = ast_cast<MetaVariablePairDef_t>(pair)) {
						pair = mvpd->pair.get();
						defVal = mvpd->defVal.get();
					}
					auto mp = static_cast<MetaVariablePair_t*>(pair);
					auto name = _parser.toString(mp->name);
					checkMetamethod(name, mp->name);
					_buf << "__"sv << name << ':' << name;
					auto newPairDef = toAst<NormalPairDef_t>(clearBuf(), pair);
					newPairDef->defVal.set(defVal);
					subMetaDestruct->values.push_back(newPairDef);
					break;
				}
				case id<MetaNormalPair_t>():
				case id<MetaNormalPairDef_t>(): {
					Exp_t* defVal = nullptr;
					if (auto mnpd = ast_cast<MetaNormalPairDef_t>(pair)) {
						pair = mnpd->pair.get();
						defVal = mnpd->defVal.get();
					}
					auto mp = static_cast<MetaNormalPair_t*>(pair);
					auto newPair = pair->new_ptr<NormalPair_t>();
					if (mp->key) {
						switch (mp->key->getId()) {
							case id<Name_t>(): {
								auto key = _parser.toString(mp->key);
								checkMetamethod(key, mp->key);
								_buf << "__"sv << key;
								auto newKey = toAst<KeyName_t>(clearBuf(), mp->key);
								newPair->key.set(newKey);
								break;
							}
							case id<String_t>():
							case id<Exp_t>():
								newPair->key.set(mp->key);
								break;
							default: YUEE("AST node mismatch", mp->key); break;
						}
					}
					newPair->value.set(mp->value);
					auto newPairDef = mp->new_ptr<NormalPairDef_t>();
					newPairDef->pair.set(newPair);
					newPairDef->defVal.set(defVal);
					subMetaDestruct->values.push_back(newPairDef);
					break;
				}
				default: YUEE("AST node mismatch", pair); break;
			}
		}
		if (!subMetaDestruct->values.empty()) {
			auto simpleValue = subMetaDestruct->new_ptr<SimpleValue_t>();
			simpleValue->value.set(subMetaDestruct);
			auto subPairs = destructFromExp(newExp(simpleValue, subMetaDestruct), optional);
			auto mt = simpleValue->new_ptr<Metatable_t>();
			auto dot = mt->new_ptr<DotChainItem_t>();
			dot->name.set(mt);
			for (const auto& p : subPairs) {
				if (!p.structure->items.empty()) {
					if (sep) p.structure->items.push_front(sep);
				}
				p.structure->items.push_front(dot);
				pairs.push_back(p);
			}
		}
		return pairs;
	}

	struct DestructureInfo {
		std::list<Destructure> destructures;
		ast_ptr<false, ExpListAssign_t> assignment;
		bool extraScope = false;
	};

	DestructureInfo extractDestructureInfo(ExpListAssign_t* assignment, bool varDefOnly, bool optional) {
		auto x = assignment;
		bool extraScope = false;
		std::list<Destructure> destructs;
		if (!assignment->action.is<Assign_t>()) return {destructs, nullptr};
		auto exprs = assignment->expList->exprs.objects();
		auto values = assignment->action.to<Assign_t>()->values.objects();
		size_t size = std::max(exprs.size(), values.size());
		ast_ptr<false, Exp_t> nil;
		if (values.size() < size) {
			nil = toAst<Exp_t>("nil"sv, x);
			while (values.size() < size) values.emplace_back(nil);
		}
		using iter = node_container::iterator;
		std::vector<std::pair<iter, iter>> destructPairs;
		ast_list<false, ast_node> valueItems;
		str_list temp;
		pushScope();
		for (auto i = exprs.begin(), j = values.begin(); i != exprs.end(); ++i, ++j) {
			auto expr = *i;
			auto value = singleValueFrom(expr);
			if (!value) {
				throw CompileError("invalid destructure"sv, expr);
			}
			ast_node* destructNode = value->getByPath<SimpleValue_t, TableLit_t>();
			if (destructNode || (destructNode = value->item.as<SimpleTable_t>())) {
				if (*j != nil) {
					if (auto ssVal = simpleSingleValueFrom(*j)) {
						switch (ssVal->value->getId()) {
							case id<ConstValue_t>():
								throw CompileError("can not destructure a constant"sv, ssVal->value);
								break;
							case id<Num_t>():
								throw CompileError("can not destructure a number"sv, ssVal->value);
								break;
							case id<FunLit_t>():
								throw CompileError("can not destructure a function"sv, ssVal->value);
								break;
						}
					}
				}
				destructPairs.push_back({i, j});
				auto subDestruct = destructNode->new_ptr<TableLit_t>();
				auto subMetaDestruct = destructNode->new_ptr<TableLit_t>();
				const node_container* dlist = nullptr;
				switch (destructNode->getId()) {
					case id<TableLit_t>():
						dlist = &static_cast<TableLit_t*>(destructNode)->values.objects();
						break;
					case id<SimpleTable_t>():
						dlist = &static_cast<SimpleTable_t*>(destructNode)->pairs.objects();
						break;
					default: YUEE("AST node mismatch", destructNode); break;
				}
				for (auto item : *dlist) {
					switch (item->getId()) {
						case id<MetaVariablePairDef_t>(): {
							auto mvp = static_cast<MetaVariablePairDef_t*>(item);
							auto mp = mvp->pair.get();
							auto name = _parser.toString(mp->name);
							checkMetamethod(name, mp->name);
							_buf << "__"sv << name << ':' << name;
							auto newPairDef = toAst<NormalPairDef_t>(clearBuf(), item);
							newPairDef->defVal.set(mvp->defVal);
							subMetaDestruct->values.push_back(newPairDef);
							break;
						}
						case id<MetaNormalPairDef_t>(): {
							auto mnp = static_cast<MetaNormalPairDef_t*>(item);
							auto mp = mnp->pair.get();
							auto newPair = item->new_ptr<NormalPair_t>();
							if (mp->key) {
								switch (mp->key->getId()) {
									case id<Name_t>(): {
										auto key = _parser.toString(mp->key);
										checkMetamethod(key, mp->key);
										_buf << "__"sv << key;
										auto newKey = toAst<KeyName_t>(clearBuf(), mp->key);
										newPair->key.set(newKey);
										break;
									}
									case id<String_t>(): {
										newPair->key.set(mp->key);
										break;
									}
									case id<Exp_t>():
										newPair->key.set(mp->key);
										break;
									default: YUEE("AST node mismatch", mp->key); break;
								}
							}
							newPair->value.set(mp->value);
							auto newPairDef = item->new_ptr<NormalPairDef_t>();
							newPairDef->pair.set(newPair);
							newPairDef->defVal.set(mnp->defVal);
							subMetaDestruct->values.push_back(newPairDef);
							break;
						}
						case id<MetaVariablePair_t>(): {
							auto mp = static_cast<MetaVariablePair_t*>(item);
							auto name = _parser.toString(mp->name);
							checkMetamethod(name, mp->name);
							_buf << "__"sv << name << ':' << name;
							auto newPairDef = toAst<NormalPairDef_t>(clearBuf(), item);
							subMetaDestruct->values.push_back(newPairDef);
							break;
						}
						case id<MetaNormalPair_t>(): {
							auto mp = static_cast<MetaNormalPair_t*>(item);
							auto newPair = item->new_ptr<NormalPair_t>();
							if (mp->key) {
								switch (mp->key->getId()) {
									case id<Name_t>(): {
										auto key = _parser.toString(mp->key);
										checkMetamethod(key, mp->key);
										_buf << "__"sv << key;
										auto newKey = toAst<KeyName_t>(clearBuf(), mp->key);
										newPair->key.set(newKey);
										break;
									}
									case id<Exp_t>():
										newPair->key.set(mp->key);
										break;
									case id<String_t>(): {
										auto value = mp->new_ptr<Value_t>();
										value->item.set(mp->key);
										newPair->key.set(newExp(value, mp));
										break;
									}
									default: YUEE("AST node mismatch", mp->key); break;
								}
							}
							newPair->value.set(mp->value);
							auto newPairDef = item->new_ptr<NormalPairDef_t>();
							newPairDef->pair.set(newPair);
							subMetaDestruct->values.push_back(newPairDef);
							break;
						}
						case id<VariablePair_t>(): {
							auto pair = static_cast<VariablePair_t*>(item);
							auto newPairDef = item->new_ptr<VariablePairDef_t>();
							newPairDef->pair.set(pair);
							subDestruct->values.push_back(newPairDef);
							break;
						}
						case id<NormalPair_t>(): {
							auto pair = static_cast<NormalPair_t*>(item);
							auto newPairDef = item->new_ptr<NormalPairDef_t>();
							newPairDef->pair.set(pair);
							subDestruct->values.push_back(newPairDef);
							break;
						}
						default:
							subDestruct->values.push_back(item);
							break;
					}
				}
				valueItems.push_back(*j);
				if (!varDefOnly && !subDestruct->values.empty() && !subMetaDestruct->values.empty()) {
					auto var = singleVariableFrom(*j, false);
					if (var.empty() || !isLocal(var)) {
						auto objVar = getUnusedName("_obj_"sv);
						addToScope(objVar);
						valueItems.pop_back();
						valueItems.push_back(toAst<Exp_t>(objVar, *j));
						exprs.push_back(valueItems.back());
						values.push_back(*j);
						extraScope = true;
					}
				}
				TableLit_t* tabs[] = {subDestruct.get(), subMetaDestruct.get()};
				for (auto tab : tabs) {
					if (!tab->values.empty()) {
						auto& destruct = destructs.emplace_back();
						if (!varDefOnly) {
							destruct.value = valueItems.back();
							destruct.valueVar = singleVariableFrom(destruct.value, false);
						}
						auto simpleValue = tab->new_ptr<SimpleValue_t>();
						simpleValue->value.set(tab);
						auto pairs = destructFromExp(newExp(simpleValue, expr), optional);
						if (pairs.empty()) {
							throw CompileError("expect items to be destructured"sv, tab);
						}
						destruct.items = std::move(pairs);
						if (!varDefOnly) {
							if (*j == nil) {
								for (auto& item : destruct.items) {
									item.structure = nullptr;
								}
							} else if (tab == subMetaDestruct.get()) {
								auto p = destruct.value.get();
								auto chainValue = toAst<ChainValue_t>("getmetatable()", p);
								static_cast<Invoke_t*>(chainValue->items.back())->args.push_back(destruct.value);
								auto exp = newExp(chainValue, p);
								destruct.value.set(exp);
								destruct.valueVar.clear();
							} else if (destruct.items.size() == 1 && !singleValueFrom(*j)) {
								auto p = destruct.value.get();
								auto parens = p->new_ptr<Parens_t>();
								parens->expr.set(p);
								auto callable = p->new_ptr<Callable_t>();
								callable->item.set(parens);
								auto chainValue = p->new_ptr<ChainValue_t>();
								chainValue->items.push_back(callable);
								auto exp = newExp(chainValue, p);
								destruct.value.set(exp);
								destruct.valueVar.clear();
							}
						}
					}
				}
			}
		}
		for (const auto& p : destructPairs) {
			exprs.erase(p.first);
			values.erase(p.second);
		}
		ast_ptr<false, ExpListAssign_t> newAssignment;
		if (!destructPairs.empty() && !exprs.empty()) {
			auto x = assignment;
			auto expList = x->new_ptr<ExpList_t>();
			auto newAssign = x->new_ptr<ExpListAssign_t>();
			newAssign->expList.set(expList);
			for (auto expr : exprs) expList->exprs.push_back(expr);
			auto assign = x->new_ptr<Assign_t>();
			for (auto value : values) assign->values.push_back(value);
			newAssign->action.set(assign);
			newAssignment = newAssign;
		}
		if (!varDefOnly) {
			for (auto& des : destructs) {
				for (const auto& item : des.items) {
					for (auto node : item.structure->items.objects()) {
						if (auto exp = ast_cast<Exp_t>(node)) {
							if (auto value = simpleSingleValueFrom(node)) {
								if (ast_is<Num_t, ConstValue_t>(value->value)) {
									continue;
								}
							}
							if (auto value = singleValueFrom(exp); !value || !value->item.is<String_t>()) {
								auto var = singleVariableFrom(exp, false);
								if (var.empty()) {
									if (!des.inlineAssignment) {
										des.inlineAssignment = x->new_ptr<ExpListAssign_t>();
										auto expList = x->new_ptr<ExpList_t>();
										des.inlineAssignment->expList.set(expList);
										auto assign = x->new_ptr<Assign_t>();
										des.inlineAssignment->action.set(assign);
									}
									auto assignList = des.inlineAssignment->expList.get();
									auto assign = des.inlineAssignment->action.to<Assign_t>();
									auto tmpVar = getUnusedName("_tmp_"sv);
									forceAddToScope(tmpVar);
									auto tmpExp = toAst<Exp_t>(tmpVar, exp);
									assignList->exprs.push_back(tmpExp);
									auto vExp = exp->new_ptr<Exp_t>();
									vExp->pipeExprs.dup(exp->pipeExprs);
									vExp->opValues.dup(exp->opValues);
									vExp->nilCoalesed.set(exp->nilCoalesed);
									exp->pipeExprs.clear();
									exp->pipeExprs.dup(tmpExp->pipeExprs);
									exp->opValues.clear();
									exp->opValues.dup(tmpExp->opValues);
									exp->nilCoalesed = tmpExp->nilCoalesed;
									assign->values.push_back(vExp);
								}
							}
						}
					}
				}
			}
		}
		popScope();
		return {std::move(destructs), newAssignment, extraScope};
	}

	void transformAssignmentCommon(ExpListAssign_t* assignment, str_list& out) {
		auto x = assignment;
		str_list temp;
		auto expList = assignment->expList.get();
		auto action = assignment->action.get();
		switch (action->getId()) {
			case id<Update_t>(): {
				if (expList->exprs.size() > 1) throw CompileError("can not apply update to multiple values"sv, expList);
				auto update = static_cast<Update_t*>(action);
				auto leftExp = static_cast<Exp_t*>(expList->exprs.objects().front());
				auto leftValue = singleValueFrom(leftExp);
				if (!leftValue) throw CompileError("left hand expression is not assignable"sv, leftExp);
				auto chain = leftValue->item.as<ChainValue_t>();
				if (!chain) throw CompileError("left hand expression is not assignable"sv, leftValue);
				BLOCK_START {
					auto dot = ast_cast<DotChainItem_t>(chain->items.back());
					if (dot && dot->name.is<Metatable_t>()) {
						throw CompileError("can not apply update to a metatable"sv, leftExp);
					}
					BREAK_IF(chain->items.size() < 2);
					if (chain->items.size() == 2) {
						if (auto callable = ast_cast<Callable_t>(chain->items.front())) {
							ast_node* var = callable->item.as<Variable_t>();
							if (auto self = callable->item.as<SelfItem_t>()) {
								var = self->name.as<Self_t>();
							}
							BREAK_IF(var && isLocal(_parser.toString(var)));
						}
					}
					auto tmpChain = x->new_ptr<ChainValue_t>();
					ast_ptr<false, ast_node> ptr(chain->items.back());
					for (auto item : chain->items.objects()) {
						if (item != ptr) {
							tmpChain->items.push_back(item);
						}
					}
					auto exp = newExp(tmpChain, x);
					auto objVar = getUnusedName("_obj_"sv);
					auto newAssignment = x->new_ptr<ExpListAssign_t>();
					newAssignment->expList.set(toAst<ExpList_t>(objVar, x));
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					newAssignment->action.set(assign);
					transformAssignment(newAssignment, temp);
					chain->items.clear();
					chain->items.push_back(toAst<Callable_t>(objVar, x));
					chain->items.push_back(ptr);
				}
				BLOCK_END
				auto tmpChain = x->new_ptr<ChainValue_t>();
				for (auto item : chain->items.objects()) {
					bool itemAdded = false;
					BLOCK_START
					auto exp = ast_cast<Exp_t>(item);
					BREAK_IF(!exp);
					auto var = singleVariableFrom(exp, true);
					BREAK_IF(!var.empty());
					auto upVar = getUnusedName("_update_"sv);
					auto newAssignment = x->new_ptr<ExpListAssign_t>();
					newAssignment->expList.set(toAst<ExpList_t>(upVar, x));
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					newAssignment->action.set(assign);
					transformAssignment(newAssignment, temp);
					tmpChain->items.push_back(toAst<Exp_t>(upVar, x));
					itemAdded = true;
					BLOCK_END
					if (!itemAdded) tmpChain->items.push_back(item);
				}
				chain->items.clear();
				chain->items.dup(tmpChain->items);
				auto op = _parser.toString(update->op);
				checkOperatorAvailable(op, update->op);
				if (op == "??"sv) {
					auto defs = getPreDefineLine(assignment);
					temp.push_back(defs);
					auto rightExp = x->new_ptr<Exp_t>();
					rightExp->pipeExprs.dup(leftExp->pipeExprs);
					rightExp->opValues.dup(leftExp->opValues);
					rightExp->nilCoalesed.set(update->value);
					transformNilCoalesedExp(rightExp, temp, ExpUsage::Assignment, assignment->expList, true);
					out.push_back(join(temp));
					return;
				}
				auto defs = getPreDefine(assignment);
				transformValue(leftValue, temp);
				auto left = std::move(temp.back());
				temp.pop_back();
				transformExp(update->value, temp, ExpUsage::Closure);
				auto right = std::move(temp.back());
				temp.pop_back();
				if (!singleValueFrom(update->value)) {
					right = '(' + right + ')';
				}
				_buf << join(temp);
				if (!defs.empty())
					_buf << defs;
				else
					_buf << indent() << left;
				_buf << " = "sv << left << ' ' << op << ' ' << right << nll(assignment);
				out.push_back(clearBuf());
				break;
			}
			case id<Assign_t>(): {
				auto assign = static_cast<Assign_t*>(action);
				auto defs = transformAssignDefs(expList, DefOp::Check);
				bool oneLined = defs.size() == expList->exprs.objects().size();
				for (auto val : assign->values.objects()) {
					if (auto value = singleValueFrom(val)) {
						if (auto spValue = value->item.as<SimpleValue_t>()) {
							if (spValue->value.is<FunLit_t>()) {
								oneLined = false;
								break;
							}
						}
					}
				}
				if (oneLined) {
					for (auto value : assign->values.objects()) {
						transformAssignItem(value, temp);
					}
					std::string preDefine = toLocalDecl(defs);
					for (const auto& def : defs) {
						addToScope(def);
					}
					if (preDefine.empty()) {
						transformExpList(expList, temp);
						std::string left = std::move(temp.back());
						temp.pop_back();
						out.push_back(indent() + left + " = "s + join(temp, ", "sv) + nll(assignment));
					} else {
						out.push_back(preDefine + " = "s + join(temp, ", "sv) + nll(assignment));
					}
				} else {
					std::string preDefine = toLocalDecl(defs);
					for (const auto& def : defs) {
						addToScope(def);
					}
					transformExpList(expList, temp);
					std::string left = std::move(temp.back());
					temp.pop_back();
					for (auto value : assign->values.objects()) {
						transformAssignItem(value, temp);
					}
					out.push_back((preDefine.empty() ? Empty : preDefine + nll(assignment)) + indent() + left + " = "s + join(temp, ", "sv) + nll(assignment));
				}
				break;
			}
			default: YUEE("AST node mismatch", action); break;
		}
	}

	void transformCond(const node_container& nodes, str_list& out, ExpUsage usage, bool unless, ExpList_t* assignList) {
		std::vector<ast_ptr<false, ast_node>> ns;
		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
			ns.push_back(*it);
			if (auto cond = ast_cast<IfCond_t>(*it)) {
				if (*it != nodes.front() && cond->condition.is<Assignment_t>()) {
					auto x = *it;
					auto newIf = x->new_ptr<If_t>();
					newIf->type.set(toAst<IfType_t>("if"sv, x));
					for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
						newIf->nodes.push_back(*j);
					}
					ns.clear();
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(newIf);
					auto exp = newExp(simpleValue, x);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(expListAssign);
					ns.push_back(stmt.get());
				}
			}
		}
		if (nodes.size() != ns.size()) {
			auto x = ns.back();
			auto newIf = x->new_ptr<If_t>();
			newIf->type.set(toAst<IfType_t>("if"sv, x));
			for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
				newIf->nodes.push_back(*j);
			}
			transformCond(newIf->nodes.objects(), out, usage, unless, assignList);
			return;
		}
		str_list temp;
		std::string* funcStart = nullptr;
		if (usage == ExpUsage::Closure) {
			pushFunctionScope();
			pushAnonVarArg();
			funcStart = &temp.emplace_back();
			pushScope();
		}
		std::list<std::pair<IfCond_t*, ast_node*>> ifCondPairs;
		ifCondPairs.emplace_back();
		for (auto node : nodes) {
			switch (node->getId()) {
				case id<IfCond_t>():
					ifCondPairs.back().first = static_cast<IfCond_t*>(node);
					break;
				case id<Block_t>():
				case id<Statement_t>():
					ifCondPairs.back().second = node;
					ifCondPairs.emplace_back();
					break;
				default: YUEE("AST node mismatch", node); break;
			}
		}
		auto asmt = ifCondPairs.front().first->condition.as<Assignment_t>();
		bool storingValue = false;
		ast_ptr<false, ExpListAssign_t> extraAssignment;
		if (asmt) {
			ast_ptr<false, ast_node> exp = asmt->expList->exprs.front();
			auto x = exp;
			auto var = singleVariableFrom(exp, false);
			if (var.empty() || isGlobal(var)) {
				storingValue = true;
				auto desVar = getUnusedName("_des_"sv);
				if (asmt->assign->values.objects().size() == 1) {
					auto var = singleVariableFrom(asmt->assign->values.objects().front(), true);
					if (!var.empty() && isLocal(var)) {
						desVar = var;
						storingValue = false;
					}
				}
				if (storingValue) {
					if (usage != ExpUsage::Closure) {
						temp.push_back(indent() + "do"s + nll(asmt));
						pushScope();
					}
					asmt->expList->exprs.pop_front();
					auto expList = toAst<ExpList_t>(desVar, x);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					for (auto expr : asmt->expList->exprs.objects()) {
						expList->exprs.push_back(expr);
					}
					assignment->expList.set(expList);
					assignment->action.set(asmt->assign);
					transformAssignment(assignment, temp);
				}
				{
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto assignOne = x->new_ptr<Assign_t>();
					auto valExp = toAst<Exp_t>(desVar, x);
					assignOne->values.push_back(valExp);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(expList);
					assignment->action.set(assignOne);
					extraAssignment.set(assignment);
					ifCondPairs.front().first->condition.set(valExp);
				}
			} else {
				if (!isDefined(var)) {
					storingValue = true;
					if (usage != ExpUsage::Closure) {
						temp.push_back(indent() + "do"s + nll(asmt));
						pushScope();
					}
				}
				auto expList = x->new_ptr<ExpList_t>();
				expList->exprs.push_back(exp);
				asmt->expList->exprs.pop_front();
				for (auto expr : asmt->expList->exprs.objects()) {
					expList->exprs.push_back(expr);
				}
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(expList);
				assignment->action.set(asmt->assign);
				transformAssignment(assignment, temp);
				ifCondPairs.front().first->condition.set(exp);
			}
		}
		for (const auto& pair : ifCondPairs) {
			if (pair.first) {
				str_list tmp;
				auto condition = pair.first->condition.to<Exp_t>();
				auto condStr = transformCondExp(condition, unless);
				if (unless) unless = false;
				_buf << indent();
				if (pair != ifCondPairs.front()) {
					_buf << "else"sv;
				}
				_buf << "if "sv << condStr << " then"sv << nll(condition);
				temp.push_back(clearBuf());
			}
			if (pair.second) {
				if (!pair.first) {
					temp.push_back(indent() + "else"s + nll(pair.second));
				}
				pushScope();
				if (pair == ifCondPairs.front() && extraAssignment) {
					transformAssignment(extraAssignment, temp);
				}
				transform_plain_body(pair.second, temp, usage, assignList);
				popScope();
			}
			if (!pair.first) {
				temp.push_back(indent() + "end"s + nll(nodes.front()));
				break;
			}
		}
		if (storingValue && usage != ExpUsage::Closure) {
			popScope();
			temp.push_back(indent() + "end"s + nlr(nodes.front()));
		}
		if (usage == ExpUsage::Closure) {
			popScope();
			*funcStart = anonFuncStart() + nll(nodes.front());
			temp.push_back(indent() + anonFuncEnd());
			popAnonVarArg();
			popFunctionScope();
		}
		out.push_back(join(temp));
	}

	void transformIf(If_t* ifNode, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		bool unless = _parser.toString(ifNode->type) == "unless"sv;
		transformCond(ifNode->nodes.objects(), out, usage, unless, assignList);
	}

	void transformExpList(ExpList_t* expList, str_list& out) {
		str_list temp;
		for (auto exp : expList->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp, ExpUsage::Closure);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformExpListLow(ExpListLow_t* expListLow, str_list& out) {
		str_list temp;
		for (auto exp : expListLow->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp, ExpUsage::Closure);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transform_pipe_exp(const node_container& values, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		if (values.size() == 1 && usage == ExpUsage::Closure) {
			transform_unary_exp(static_cast<UnaryExp_t*>(values.front()), out);
		} else {
			auto x = values.front();
			auto arg = newExp(static_cast<UnaryExp_t*>(x), x);
			auto begin = values.begin();
			begin++;
			for (auto it = begin; it != values.end(); ++it) {
				auto unary = static_cast<UnaryExp_t*>(*it);
				auto value = static_cast<Value_t*>(singleUnaryExpFrom(unary) ? unary->expos.back() : nullptr);
				if (values.back() == *it && !unary->ops.empty() && usage == ExpUsage::Common) {
					throw CompileError("unexpected expression"sv, x);
				}
				if (!value) throw CompileError("pipe operator must be followed by chain value"sv, *it);
				if (auto chainValue = value->item.as<ChainValue_t>()) {
					if (isChainValueCall(chainValue)) {
						auto last = chainValue->items.back();
						_ast_list* args = nullptr;
						if (auto invoke = ast_cast<InvokeArgs_t>(last)) {
							args = &invoke->args;
						} else {
							args = &(ast_to<Invoke_t>(last)->args);
						}
						bool findPlaceHolder = false;
						for (auto a : args->objects()) {
							auto name = singleVariableFrom(a, false);
							if (name == "_"sv) {
								if (!findPlaceHolder) {
									args->swap(a, arg);
									findPlaceHolder = true;
								} else {
									throw CompileError("pipe placeholder can be used only in one place"sv, a);
								}
							}
						}
						if (!findPlaceHolder) {
							args->push_front(arg);
						}
					} else {
						auto invoke = x->new_ptr<Invoke_t>();
						invoke->args.push_front(arg);
						chainValue->items.push_back(invoke);
					}
					arg.set(newExp(unary, x));
				} else {
					throw CompileError("pipe operator must be followed by chain value"sv, value);
				}
			}
			switch (usage) {
				case ExpUsage::Assignment: {
					auto assignment = x->new_ptr<ExpListAssign_t>();
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(arg);
					assignment->action.set(assign);
					assignment->expList.set(assignList);
					transformAssignment(assignment, out);
					return;
				}
				case ExpUsage::Common: {
					auto value = singleValueFrom(arg);
					if (value && value->item.is<ChainValue_t>()) {
						transformChainValue(value->item.to<ChainValue_t>(), out, ExpUsage::Common);
					} else {
						transformExp(arg, out, ExpUsage::Closure);
						out.back().insert(0, indent());
						out.back().append(nlr(x));
					}
					return;
				}
				case ExpUsage::Return: {
					auto ret = x->new_ptr<Return_t>();
					auto expListLow = x->new_ptr<ExpListLow_t>();
					expListLow->exprs.push_back(arg);
					ret->valueList.set(expListLow);
					transformReturn(ret, out);
					return;
				}
				case ExpUsage::Closure: {
					transformExp(arg, out, ExpUsage::Closure);
					return;
				}
				default: YUEE("invalid expression usage", x); return;
			}
		}
	}

	void transformExp(Exp_t* exp, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		if (exp->opValues.empty() && !exp->nilCoalesed) {
			transform_pipe_exp(exp->pipeExprs.objects(), out, usage, assignList);
			return;
		}
		if (usage != ExpUsage::Closure) {
			YUEE("invalid expression usage", exp);
		}
		if (exp->nilCoalesed) {
			transformNilCoalesedExp(exp, out, ExpUsage::Closure);
			return;
		}
		str_list temp;
		transform_pipe_exp(exp->pipeExprs.objects(), temp, ExpUsage::Closure);
		for (auto _opValue : exp->opValues.objects()) {
			auto opValue = static_cast<ExpOpValue_t*>(_opValue);
			transformBinaryOperator(opValue->op, temp);
			transform_pipe_exp(opValue->pipeExprs.objects(), temp, ExpUsage::Closure);
		}
		out.push_back(join(temp, " "sv));
	}

	void transformNilCoalesedExp(Exp_t* exp, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr, bool nilBranchOnly = false) {
		auto x = exp;
		str_list temp;
		std::string prefix;
		auto left = exp->new_ptr<Exp_t>();
		if (exp->opValues.empty()) {
			left->pipeExprs.dup(exp->pipeExprs);
		} else {
			if (usage != ExpUsage::Closure) {
				YUEE("invalid expression usage", exp);
			}
			transform_pipe_exp(exp->pipeExprs.objects(), temp, ExpUsage::Closure);
			auto last = exp->opValues.objects().back();
			for (auto _opValue : exp->opValues.objects()) {
				auto opValue = static_cast<ExpOpValue_t*>(_opValue);
				transformBinaryOperator(opValue->op, temp);
				if (opValue == last) {
					left->pipeExprs.dup(opValue->pipeExprs);
				} else {
					transform_pipe_exp(opValue->pipeExprs.objects(), temp, ExpUsage::Closure);
				}
			}
			prefix = join(temp, " "sv) + ' ';
			temp.clear();
			temp.push_back(prefix);
		}
		std::string* funcStart = nullptr;
		if (usage == ExpUsage::Closure) {
			pushFunctionScope();
			pushAnonVarArg();
			funcStart = &temp.emplace_back();
			pushScope();
		}
		auto objVar = singleVariableFrom(left, true);
		auto prepareValue = [&](bool forAssignment = false) {
			if (objVar.empty() || !isLocal(objVar)) {
				if (forAssignment) {
					temp.push_back(indent() + "do"s + nll(x));
					pushScope();
				}
				objVar = getUnusedName("_exp_"sv);
				auto expList = toAst<ExpList_t>(objVar, x);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(left);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(expList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				return forAssignment;
			}
			return false;
		};
		switch (usage) {
			case ExpUsage::Common: YUEE("AST node mismatch", x); return;
			case ExpUsage::Return:
			case ExpUsage::Closure: {
				prepareValue();
				_buf << indent() << "if "sv << objVar << " ~= nil then"sv << nll(x);
				_buf << indent(1) << "return "s << objVar << nll(x);
				_buf << indent() << "else"s << nll(x);
				temp.push_back(clearBuf());
				auto ret = x->new_ptr<Return_t>();
				auto retList = x->new_ptr<ExpListLow_t>();
				retList->exprs.push_back(exp->nilCoalesed);
				ret->valueList.set(retList);
				incIndentOffset();
				transformReturn(ret, temp);
				decIndentOffset();
				temp.push_back(indent() + "end"s + nll(x));
				if (usage == ExpUsage::Closure) {
					popScope();
					*funcStart = anonFuncStart() + nll(x);
					temp.push_back(indent() + anonFuncEnd());
					popAnonVarArg();
					popFunctionScope();
				}
				break;
			}
			case ExpUsage::Assignment: {
				auto assign = x->new_ptr<Assign_t>();
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				bool extraScope = false;
				if (!nilBranchOnly) {
					assign->values.push_back(exp);
					temp.push_back(getPreDefineLine(assignment));
					extraScope = prepareValue(true);
					_buf << indent() << "if "sv << objVar << " ~= nil then"sv << nll(x);
					temp.push_back(clearBuf());
					pushScope();
					assign->values.clear();
					assign->values.push_back(toAst<Exp_t>(objVar, x));
					transformAssignment(assignment, temp);
					popScope();
					temp.push_back(indent() + "else"s + nll(x));
					assign->values.clear();
					assign->values.push_back(exp->nilCoalesed);
				} else {
					assign->values.clear();
					assign->values.push_back(exp->nilCoalesed);
					temp.push_back(getPreDefineLine(assignment));
					transformExp(left, temp, ExpUsage::Closure);
					_buf << indent() << "if "sv << temp.back() << " == nil then"sv << nll(x);
					temp.pop_back();
					temp.push_back(clearBuf());
				}
				pushScope();
				transformAssignment(assignment, temp);
				popScope();
				temp.push_back(indent() + "end"s + nlr(x));
				if (extraScope) {
					popScope();
					temp.push_back(indent() + "end"s + nlr(x));
				}
				break;
			}
		}
		out.push_back(join(temp));
	}

	void transformValue(Value_t* value, str_list& out) {
		auto item = value->item.get();
		switch (item->getId()) {
			case id<SimpleValue_t>(): transformSimpleValue(static_cast<SimpleValue_t*>(item), out); break;
			case id<SimpleTable_t>(): transform_simple_table(static_cast<SimpleTable_t*>(item), out); break;
			case id<ChainValue_t>(): transformChainValue(static_cast<ChainValue_t*>(item), out, ExpUsage::Closure); break;
			case id<String_t>(): transformString(static_cast<String_t*>(item), out); break;
			default: YUEE("AST node mismatch", value); break;
		}
	}

	void transformCallable(Callable_t* callable, str_list& out, const ast_sel<false, Invoke_t, InvokeArgs_t>& invoke = {}) {
		auto item = callable->item.get();
		switch (item->getId()) {
			case id<Variable_t>(): {
				transformVariable(static_cast<Variable_t*>(item), out);
				if (_config.lintGlobalVariable && !isLocal(out.back())) {
					if (_globals.find(out.back()) == _globals.end()) {
						_globals[out.back()] = {item->m_begin.m_line, item->m_begin.m_col};
					}
				}
				break;
			}
			case id<SelfItem_t>(): {
				transformSelfName(static_cast<SelfItem_t*>(item), out, invoke);
				globalVar("self"sv, item);
				break;
			}
			case id<Parens_t>(): transformParens(static_cast<Parens_t*>(item), out); break;
			default: YUEE("AST node mismatch", item); break;
		}
	}

	void transformParens(Parens_t* parans, str_list& out) {
		str_list temp;
		transformExp(parans->expr, temp, ExpUsage::Closure);
		out.push_back('(' + temp.front() + ')');
	}

	void transformSimpleValue(SimpleValue_t* simpleValue, str_list& out) {
		auto value = simpleValue->value.get();
		switch (value->getId()) {
			case id<ConstValue_t>(): transform_const_value(static_cast<ConstValue_t*>(value), out); break;
			case id<If_t>(): transformIf(static_cast<If_t*>(value), out, ExpUsage::Closure); break;
			case id<Switch_t>(): transformSwitch(static_cast<Switch_t*>(value), out, ExpUsage::Closure); break;
			case id<With_t>(): transformWithClosure(static_cast<With_t*>(value), out); break;
			case id<ClassDecl_t>(): transformClassDeclClosure(static_cast<ClassDecl_t*>(value), out); break;
			case id<ForEach_t>(): transformForEachClosure(static_cast<ForEach_t*>(value), out); break;
			case id<For_t>(): transformForClosure(static_cast<For_t*>(value), out); break;
			case id<While_t>(): transformWhileClosure(static_cast<While_t*>(value), out); break;
			case id<Do_t>(): transformDo(static_cast<Do_t*>(value), out, ExpUsage::Closure); break;
			case id<Try_t>(): transformTry(static_cast<Try_t*>(value), out, ExpUsage::Closure); break;
			case id<UnaryValue_t>(): transform_unary_value(static_cast<UnaryValue_t*>(value), out); break;
			case id<TblComprehension_t>(): transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Closure); break;
			case id<TableLit_t>(): transformTableLit(static_cast<TableLit_t*>(value), out); break;
			case id<Comprehension_t>(): transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Closure); break;
			case id<FunLit_t>(): transformFunLit(static_cast<FunLit_t*>(value), out); break;
			case id<Num_t>(): transformNum(static_cast<Num_t*>(value), out); break;
			case id<VarArg_t>(): transformVarArg(static_cast<VarArg_t*>(value), out); break;
			default: YUEE("AST node mismatch", value); break;
		}
	}

	void transformFunLit(FunLit_t* funLit, str_list& out) {
		pushFunctionScope();
		_varArgs.push({false, false});
		bool isFatArrow = _parser.toString(funLit->arrow) == "=>"sv;
		pushScope();
		if (isFatArrow) {
			forceAddToScope("self"s);
		}
		str_list temp;
		if (auto argsDef = funLit->argsDef.get()) {
			transformFnArgsDef(argsDef, temp);
			if (funLit->body) {
				transformBody(funLit->body, temp, ExpUsage::Return);
			} else {
				temp.push_back(Empty);
			}
			auto it = temp.begin();
			auto& args = *it;
			auto& initArgs = *(++it);
			auto& bodyCodes = *(++it);
			_buf << "function("sv;
			if (isFatArrow) {
				_buf << "self"sv;
				if (!args.empty()) _buf << ", "sv;
			}
			_buf << args << ')';
			if (!initArgs.empty() || !bodyCodes.empty()) {
				_buf << nlr(argsDef) << initArgs << bodyCodes;
				popScope();
				_buf << indent() << "end"sv;
			} else {
				popScope();
				_buf << " end"sv;
			}
		} else {
			if (funLit->body) {
				transformBody(funLit->body, temp, ExpUsage::Return);
			} else {
				temp.push_back(Empty);
			}
			auto& bodyCodes = temp.back();
			_buf << "function("sv << (isFatArrow ? "self"s : Empty) << ')';
			if (!bodyCodes.empty()) {
				_buf << nll(funLit) << bodyCodes;
				popScope();
				_buf << indent() << "end"sv;
			} else {
				popScope();
				_buf << " end"sv;
			}
		}
		out.push_back(clearBuf());
		popFunctionScope();
		_varArgs.pop();
	}

	void transformBody(Body_t* body, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = body;
		if (auto stmt = body->content.as<Statement_t>()) {
			auto block = x->new_ptr<Block_t>();
			block->statements.push_back(stmt);
			transformBlock(block, out, usage, assignList);
		} else {
			transformBlock(body->content.to<Block_t>(), out, usage, assignList);
		}
	}

	void transformBlock(Block_t* block, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr, bool isRoot = false) {
		if (!block) {
			out.push_back(Empty);
			return;
		}
		const auto& nodes = block->statements.objects();
		LocalMode mode = LocalMode::None;
		Local_t *any = nullptr, *capital = nullptr;
		for (auto it = nodes.begin(); it != nodes.end(); ++it) {
			auto node = *it;
			auto stmt = static_cast<Statement_t*>(node);
			if (auto pipeBody = stmt->content.as<PipeBody_t>()) {
				auto x = stmt;
				bool cond = false;
				BLOCK_START
				BREAK_IF(it == nodes.begin());
				auto last = it;
				--last;
				auto lst = static_cast<Statement_t*>(*last);
				if (lst->appendix) {
					throw CompileError("statement decorator must be placed at the end of pipe chain"sv, lst->appendix.get());
				}
				lst->appendix.set(stmt->appendix);
				stmt->appendix.set(nullptr);
				lst->needSep.set(stmt->needSep);
				stmt->needSep.set(nullptr);
				auto exp = lastExpFromStatement(lst);
				BREAK_IF(!exp);
				for (auto val : pipeBody->values.objects()) {
					exp->pipeExprs.push_back(val);
				}
				cond = true;
				BLOCK_END
				if (!cond) throw CompileError("pipe chain must be following a value"sv, x);
				stmt->content.set(nullptr);
				auto next = it;
				++next;
				BLOCK_START
				BREAK_IF(next == nodes.end());
				BREAK_IF(!static_cast<Statement_t*>(*next)->content.as<PipeBody_t>());
				throw CompileError("indent mismatch in pipe chain"sv, *next);
				BLOCK_END
			} else if (auto backcall = stmt->content.as<Backcall_t>()) {
				auto x = *nodes.begin();
				auto newBlock = x->new_ptr<Block_t>();
				if (it != nodes.begin()) {
					for (auto i = nodes.begin(); i != it; ++i) {
						newBlock->statements.push_back(*i);
					}
				}
				x = backcall;
				ast_ptr<false, Exp_t> arg;
				{
					auto block = x->new_ptr<Block_t>();
					auto next = it;
					++next;
					if (next != nodes.end()) {
						for (auto i = next; i != nodes.end(); ++i) {
							block->statements.push_back(*i);
						}
					}
					auto body = x->new_ptr<Body_t>();
					body->content.set(block);
					auto funLit = x->new_ptr<FunLit_t>();
					funLit->argsDef.set(backcall->argsDef);
					auto arrow = _parser.toString(backcall->arrow);
					funLit->arrow.set(toAst<FnArrow_t>(arrow == "<-"sv ? "->"sv : "=>"sv, x));
					funLit->body.set(body);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(funLit);
					arg = newExp(simpleValue, x);
				}
				if (isChainValueCall(backcall->value)) {
					auto last = backcall->value->items.back();
					_ast_list* args = nullptr;
					if (auto invoke = ast_cast<InvokeArgs_t>(last)) {
						args = &invoke->args;
					} else {
						args = &(ast_to<Invoke_t>(last)->args);
					}
					bool findPlaceHolder = false;
					for (auto a : args->objects()) {
						auto name = singleVariableFrom(a, false);
						if (name == "_"sv) {
							if (!findPlaceHolder) {
								args->swap(a, arg);
								findPlaceHolder = true;
							} else {
								throw CompileError("backcall placeholder can be used only in one place"sv, a);
							}
						}
					}
					if (!findPlaceHolder) {
						args->push_back(arg);
					}
				} else {
					auto invoke = x->new_ptr<Invoke_t>();
					invoke->args.push_back(arg);
					backcall->value->items.push_back(invoke);
				}
				auto newStmt = x->new_ptr<Statement_t>();
				{
					auto chainValue = backcall->value.get();
					auto exp = newExp(chainValue, x);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					newStmt->content.set(expListAssign);
					newStmt->appendix.set(stmt->appendix);
					newBlock->statements.push_back(newStmt);
				}
				transformBlock(newBlock, out, usage, assignList, isRoot);
				return;
			}
			if (auto local = stmt->content.as<Local_t>()) {
				if (!local->collected) {
					local->collected = true;
					switch (local->item->getId()) {
						case id<LocalFlag_t>(): {
							auto flag = static_cast<LocalFlag_t*>(local->item.get());
							LocalMode newMode = _parser.toString(flag) == "*"sv ? LocalMode::Any : LocalMode::Capital;
							if (int(newMode) > int(mode)) {
								mode = newMode;
							}
							if (mode == LocalMode::Any) {
								if (!any) any = local;
								if (!capital) capital = local;
							} else {
								if (!capital) capital = local;
							}
							break;
						}
						case id<LocalValues_t>(): {
							auto values = local->item.to<LocalValues_t>();
							for (auto name : values->nameList->names.objects()) {
								local->forceDecls.push_back(_parser.toString(name));
							}
							break;
						}
						default: YUEE("AST node mismatch", local->item); break;
					}
				}
			} else if (mode != LocalMode::None) {
				ClassDecl_t* classDecl = nullptr;
				ast_ptr<false, ExpListAssign_t> assignment;
				if (auto exportNode = stmt->content.as<Export_t>()) {
					if (exportNode->assign) {
						assignment = stmt->new_ptr<ExpListAssign_t>();
						assignment->expList.set(exportNode->target);
						assignment->action.set(exportNode->assign);
					}
				}
				if (!assignment) assignment = assignmentFrom(stmt);
				if (assignment) {
					auto vars = getAssignVars(assignment);
					for (const auto& var : vars) {
						if (var.empty()) continue;
						if (std::isupper(var[0]) && capital) {
							capital->decls.push_back(var);
						} else if (any) {
							any->decls.push_back(var);
						}
					}
					auto info = extractDestructureInfo(assignment, true, false);
					if (!info.destructures.empty()) {
						for (const auto& destruct : info.destructures)
							for (const auto& item : destruct.items)
								if (!item.targetVar.empty()) {
									if (std::isupper(item.targetVar[0]) && capital) {
										capital->decls.push_back(item.targetVar);
									} else if (any) {
										any->decls.push_back(item.targetVar);
									}
								}
					}
					if (info.assignment) {
						auto defs = transformAssignDefs(info.assignment->expList, DefOp::Get);
						for (const auto& def : defs) {
							if (std::isupper(def[0]) && capital) {
								capital->decls.push_back(def);
							} else if (any) {
								any->decls.push_back(def);
							}
						}
					}
					BLOCK_START
					auto assign = assignment->action.as<Assign_t>();
					BREAK_IF(!assign);
					BREAK_IF(assign->values.objects().size() != 1);
					auto exp = ast_cast<Exp_t>(assign->values.objects().front());
					BREAK_IF(!exp);
					auto value = singleValueFrom(exp);
					BREAK_IF(!value);
					classDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
					BLOCK_END
				} else if (auto expList = expListFrom(stmt)) {
					auto value = singleValueFrom(expList);
					classDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
				}
				if (classDecl) {
					if (auto variable = classDecl->name->item.as<Variable_t>()) {
						auto className = _parser.toString(variable);
						if (!className.empty()) {
							if (std::isupper(className[0]) && capital) {
								capital->decls.push_back(className);
							} else if (any) {
								any->decls.push_back(className);
							}
						}
					}
				}
			}
		}
		if (isRoot && !_info.moduleName.empty() && !_info.exportMacro) {
			block->statements.push_front(toAst<Statement_t>(_info.moduleName + (_info.exportDefault ? "=nil"s : "={}"s), block));
		}
		switch (usage) {
			case ExpUsage::Closure:
			case ExpUsage::Return: {
				BLOCK_START
				BREAK_IF(isRoot && !_info.moduleName.empty());
				auto last = lastStatementFrom(nodes);
				BREAK_IF(!last);
				auto x = last;
				auto expList = expListFrom(last);
				BREAK_IF(!expList);
				BREAK_IF(last->appendix && !last->appendix->item.is<IfLine_t>());
				auto expListLow = x->new_ptr<ExpListLow_t>();
				expListLow->exprs.dup(expList->exprs);
				auto returnNode = x->new_ptr<Return_t>();
				returnNode->valueList.set(expListLow);
				returnNode->allowBlockMacroReturn = true;
				last->content.set(returnNode);
				last->needSep.set(nullptr);
				auto bLast = ++nodes.rbegin();
				if (bLast != nodes.rend()) {
					bool isMacro = false;
					BLOCK_START
					BREAK_IF(expListLow->exprs.size() != 1);
					auto exp = static_cast<Exp_t*>(expListLow->exprs.back());
					BREAK_IF(!exp->opValues.empty());
					auto chainValue = exp->getByPath<UnaryExp_t, Value_t, ChainValue_t>();
					BREAK_IF(!chainValue);
					isMacro = isMacroChain(chainValue);
					BLOCK_END
					if (!isMacro) {
						ast_to<Statement_t>(*bLast)->needSep.set(nullptr);
					}
				}
				BLOCK_END
				break;
			}
			case ExpUsage::Assignment: {
				auto last = lastStatementFrom(block);
				if (!last) throw CompileError("block is not assignable"sv, block);
				if (last->appendix) {
					auto appendix = last->appendix->item.get();
					switch (appendix->getId()) {
						case id<WhileLine_t>():
							throw CompileError("while-loop line decorator is not supported here"sv, appendix);
							break;
						case id<CompFor_t>():
							throw CompileError("for-loop line decorator is not supported here"sv, appendix);
							break;
					}
				}
				bool lastAssignable = (expListFrom(last) || ast_is<For_t, ForEach_t, While_t>(last->content));
				if (lastAssignable) {
					auto x = last;
					auto newAssignment = x->new_ptr<ExpListAssign_t>();
					newAssignment->expList.set(assignList);
					auto assign = x->new_ptr<Assign_t>();
					if (auto valueList = last->content.as<ExpListAssign_t>()) {
						assign->values.dup(valueList->expList->exprs);
					} else {
						auto simpleValue = x->new_ptr<SimpleValue_t>();
						simpleValue->value.set(last->content);
						auto exp = newExp(simpleValue, x);
						assign->values.push_back(exp);
					}
					newAssignment->action.set(assign);
					last->content.set(newAssignment);
					last->needSep.set(nullptr);
					auto bLast = ++nodes.rbegin();
					if (bLast != nodes.rend()) {
						static_cast<Statement_t*>(*bLast)->needSep.set(nullptr);
					}
				} else if (!last->content.is<BreakLoop_t>()) {
					throw CompileError("expecting assignable statement or break loop"sv, last);
				}
				break;
			}
			default: break;
		}
		if (!nodes.empty()) {
			str_list temp;
			for (auto node : nodes) {
				transformStatement(static_cast<Statement_t*>(node), temp);
			}
			out.push_back(join(temp));
		} else {
			out.push_back(Empty);
		}
		if (isRoot && !_info.moduleName.empty() && !_info.exportMacro) {
			out.back().append(indent() + "return "s + _info.moduleName + nlr(block));
		}
	}

	std::optional<std::string> getOption(std::string_view key) {
#ifndef YUE_NO_MACRO
		if (L) {
			int top = lua_gettop(L);
			DEFER(lua_settop(L, top));
			pushYue("options"sv); // options
			lua_pushlstring(L, &key.front(), key.size());
			lua_gettable(L, -2);
			if (lua_isstring(L, -1) != 0) {
				size_t size = 0;
				const char* str = lua_tolstring(L, -1, &size);
				return std::string(str, size);
			}
		}
#endif // YUE_NO_MACRO
		auto it = _config.options.find(std::string(key));
		if (it != _config.options.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	int getLuaTarget(ast_node* x) {
		if (auto target = getOption("target")) {
			if (target.value() == "5.1"sv) {
				return 501;
			} else if (target.value() == "5.2"sv) {
				return 502;
			} else if (target.value() == "5.3"sv) {
				return 503;
			} else if (target.value() == "5.4"sv) {
				return 504;
			} else {
				throw CompileError("get invalid Lua target \""s + target.value() + "\", should be 5.1, 5.2, 5.3 or 5.4"s, x);
			}
		}
#ifndef YUE_NO_MACRO
		return LUA_VERSION_NUM;
#else
		return 504;
#endif // YUE_NO_MACRO
	}

#ifndef YUE_NO_MACRO
	void passOptions() {
		if (!_config.options.empty()) {
			pushYue("options"sv); // options
			for (const auto& option : _config.options) {
				lua_pushlstring(L, option.second.c_str(), option.second.size());
				lua_setfield(L, -2, option.first.c_str());
			}
			lua_pop(L, 1);
		}
	}

	void pushCurrentModule() {
		if (_useModule) {
			lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
			lua_rawget(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE], tb
			int idx = static_cast<int>(lua_objlen(L, -1)); // idx = #tb, tb
			lua_rawgeti(L, -1, idx); // tb[idx], tb cur
			lua_remove(L, -2); // cur
			return;
		}
		_useModule = true;
		if (!L) {
			L = luaL_newstate();
			int top = lua_gettop(L);
			DEFER(lua_settop(L, top));
			if (_luaOpen) {
				_luaOpen(static_cast<void*>(L));
			}
			passOptions();
			auto it = _config.options.find("path"s);
			if (it != _config.options.end()) {
				lua_getglobal(L, "package");
				auto path = it->second + ';';
				lua_pushlstring(L, path.c_str(), path.size());
				lua_getfield(L, -2, "path");
				lua_concat(L, 2);
				lua_setfield(L, -2, "path");
			}
			_stateOwner = true;
		}
		lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
		lua_rawget(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE], tb
		if (lua_isnil(L, -1) != 0) { // tb == nil
			lua_pop(L, 1);
			lua_newtable(L); // tb
			lua_pushliteral(L, YUE_MODULE); // tb YUE_MODULE
			lua_pushvalue(L, -2); // tb YUE_MODULE tb
			lua_rawset(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE] = tb, tb
		} // tb
		int idx = static_cast<int>(lua_objlen(L, -1)); // idx = #tb, tb
		lua_newtable(L); // tb cur
		lua_pushvalue(L, -1); // tb cur cur
		lua_rawseti(L, -3, idx + 1); // tb[idx + 1] = cur, tb cur
		lua_remove(L, -2); // cur
	}

	void pushYue(std::string_view name) {
		lua_getglobal(L, "package"); // package
		lua_getfield(L, -1, "loaded"); // package loaded
		lua_getfield(L, -1, "yue"); // package loaded yue
		lua_pushlstring(L, &name.front(), name.size()); // package loaded yue name
		lua_gettable(L, -2); // loaded[name], package loaded yue item
		lua_insert(L, -4); // item package loaded yue
		lua_pop(L, 3); // item
	}

	bool isModuleLoaded(std::string_view name) {
		int top = lua_gettop(L);
		DEFER(lua_settop(L, top));
		lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
		lua_rawget(L, LUA_REGISTRYINDEX); // modules
		lua_pushlstring(L, &name.front(), name.size());
		lua_rawget(L, -2); // modules module
		if (lua_isnil(L, -1) != 0) {
			return false;
		}
		return true;
	}

	void pushModuleTable(std::string_view name) {
		lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
		lua_rawget(L, LUA_REGISTRYINDEX); // modules
		lua_pushlstring(L, &name.front(), name.size());
		lua_rawget(L, -2); // modules module
		if (lua_isnil(L, -1) != 0) {
			lua_pop(L, 1);
			lua_newtable(L); // modules module
			lua_pushlstring(L, &name.front(), name.size());
			lua_pushvalue(L, -2); // modules module name module
			lua_rawset(L, -4); // modules[name] = module, modules module
		}
		lua_remove(L, -2); // module
	}

	void pushOptions(int lineOffset) {
		lua_newtable(L);
		lua_pushliteral(L, "lint_global");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);
		lua_pushliteral(L, "implicit_return_root");
		lua_pushboolean(L, 1);
		lua_rawset(L, -3);
		lua_pushliteral(L, "reserve_line_number");
		lua_pushboolean(L, 1);
		lua_rawset(L, -3);
		lua_pushliteral(L, "same_module");
		lua_pushboolean(L, 1);
		lua_rawset(L, -3);
		lua_pushliteral(L, "line_offset");
		lua_pushinteger(L, lineOffset);
		lua_rawset(L, -3);
	}

	void transformMacro(Macro_t* macro, str_list& out, bool exporting) {
		if (_scopes.size() > 1) {
			throw CompileError("can not define macro outside the root block"sv, macro);
		}
		auto macroName = _parser.toString(macro->name);
		auto argsDef = macro->macroLit->argsDef.get();
		str_list newArgs;
		if (argsDef) {
			for (auto def_ : argsDef->definitions.objects()) {
				auto def = static_cast<FnArgDef_t*>(def_);
				if (def->name.is<SelfItem_t>()) {
					throw CompileError("self name is not supported for macro function argument"sv, def->name);
				} else {
					std::string defVal;
					if (def->defaultValue) {
						defVal = _parser.toString(def->defaultValue);
						Utils::trim(defVal);
						defVal.insert(0, "=[==========["sv);
						defVal.append("]==========]"sv);
					}
					newArgs.emplace_back(_parser.toString(def->name) + defVal);
				}
			}
			if (argsDef->varArg) {
				newArgs.emplace_back(_parser.toString(argsDef->varArg));
			}
		}
		_buf << "("sv << join(newArgs, ","sv) << ")->"sv;
		_buf << _parser.toString(macro->macroLit->body);
		auto macroCodes = clearBuf();
		_buf << "=(macro "sv << macroName << ")";
		auto chunkName = clearBuf();
		pushCurrentModule(); // cur
		int top = lua_gettop(L) - 1;
		DEFER(lua_settop(L, top));
		pushYue("loadstring"sv); // cur loadstring
		lua_pushlstring(L, macroCodes.c_str(), macroCodes.size()); // cur loadstring codes
		lua_pushlstring(L, chunkName.c_str(), chunkName.size()); // cur loadstring codes chunk
		pushOptions(macro->m_begin.m_line - 1); // cur loadstring codes chunk options
		if (lua_pcall(L, 3, 2, 0) != 0) { // loadstring(codes,chunk,options), cur f err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to load macro codes\n"s + err, macro->macroLit);
		} // cur f err
		if (lua_isnil(L, -2) != 0) { // f == nil, cur f err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to load macro codes, at (macro "s + macroName + "): "s + err, macro->macroLit);
		}
		lua_pop(L, 1); // cur f
		pushYue("pcall"sv); // cur f pcall
		lua_insert(L, -2); // cur pcall f
		if (lua_pcall(L, 1, 2, 0) != 0) { // f(), cur success macro
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to generate macro function\n"s + err, macro->macroLit);
		} // cur success res
		if (lua_toboolean(L, -2) == 0) {
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to generate macro function\n"s + err, macro->macroLit);
		} // cur true macro
		lua_remove(L, -2); // cur macro
		if (exporting && _config.exporting && !_config.module.empty()) {
			pushModuleTable(_config.module); // cur macro module
			lua_pushlstring(L, macroName.c_str(), macroName.size()); // cur macro module name
			lua_pushvalue(L, -3); // cur macro module name macro
			lua_rawset(L, -3); // cur macro module
			lua_pop(L, 1);
		} // cur macro
		lua_pushlstring(L, macroName.c_str(), macroName.size()); // cur macro name
		lua_insert(L, -2); // cur name macro
		lua_rawset(L, -3); // cur[name] = macro, cur
		out.push_back(Empty);
	}
#else
	void transformMacro(Macro_t* macro, str_list&, bool) {
		throw CompileError("macro feature not supported"sv, macro));
	}
#endif // YUE_NO_MACRO

	void transformReturn(Return_t* returnNode, str_list& out) {
		if (!_enableReturn.top()) {
			ast_node* target = returnNode->valueList.get();
			if (!target) target = returnNode;
			throw CompileError("can not mix use of return and export statements in module scope"sv, target);
		}
		if (auto valueList = returnNode->valueList.as<ExpListLow_t>()) {
			if (valueList->exprs.size() == 1) {
				auto exp = static_cast<Exp_t*>(valueList->exprs.back());
				if (isPureBackcall(exp)) {
					transformExp(exp, out, ExpUsage::Return);
					return;
				} else if (isPureNilCoalesed(exp)) {
					transformNilCoalesedExp(exp, out, ExpUsage::Return);
					return;
				}
			}
			if (auto singleValue = singleValueFrom(valueList)) {
				if (auto simpleValue = singleValue->item.as<SimpleValue_t>()) {
					auto value = simpleValue->value.get();
					switch (value->getId()) {
						case id<Comprehension_t>():
							transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Return);
							return;
						case id<TblComprehension_t>():
							transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Return);
							return;
						case id<With_t>():
							transformWith(static_cast<With_t*>(value), out, nullptr, true);
							return;
						case id<ClassDecl_t>():
							transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Return);
							return;
						case id<Do_t>():
							transformDo(static_cast<Do_t*>(value), out, ExpUsage::Return);
							return;
						case id<Switch_t>():
							transformSwitch(static_cast<Switch_t*>(value), out, ExpUsage::Return);
							return;
						case id<While_t>():
							transformWhileInPlace(static_cast<While_t*>(value), out);
							return;
						case id<For_t>():
							transformForInPlace(static_cast<For_t*>(value), out);
							return;
						case id<ForEach_t>():
							transformForEachInPlace(static_cast<ForEach_t*>(value), out);
							return;
						case id<If_t>():
							transformIf(static_cast<If_t*>(value), out, ExpUsage::Return);
							return;
						case id<TableLit_t>(): {
							auto tableLit = static_cast<TableLit_t*>(value);
							if (hasSpreadExp(tableLit->values.objects())) {
								transformSpreadTable(tableLit->values.objects(), out, ExpUsage::Return);
								return;
							}
						}
					}
				} else if (auto chainValue = singleValue->item.as<ChainValue_t>()) {
					if (specialChainValue(chainValue) != ChainType::Common) {
						transformChainValue(chainValue, out, ExpUsage::Return, nullptr, returnNode->allowBlockMacroReturn);
						return;
					}
				}
				transformValue(singleValue, out);
				out.back() = indent() + "return "s + out.back() + nlr(returnNode);
				return;
			} else {
				str_list temp;
				transformExpListLow(valueList, temp);
				out.push_back(indent() + "return "s + temp.back() + nlr(returnNode));
			}
		} else if (auto tableBlock = returnNode->valueList.as<TableBlock_t>()) {
			const auto& values = tableBlock->values.objects();
			if (hasSpreadExp(values)) {
				transformSpreadTable(values, out, ExpUsage::Return);
			} else {
				transformTable(values, out);
				out.back() = indent() + "return "s + out.back() + nlr(returnNode);
			}
		} else {
			out.push_back(indent() + "return"s + nll(returnNode));
		}
	}

	void transformFnArgsDef(FnArgsDef_t* argsDef, str_list& out) {
		if (!argsDef->defList) {
			out.push_back(Empty);
			out.push_back(Empty);
		} else {
			transformFnArgDefList(argsDef->defList, out);
		}
		if (argsDef->shadowOption) {
			transform_outer_var_shadow(argsDef->shadowOption);
		}
	}

	void transform_outer_var_shadow(OuterVarShadow_t* shadow) {
		markVarShadowed();
		if (shadow->varList) {
			for (auto name : shadow->varList->names.objects()) {
				addToAllowList(_parser.toString(name));
			}
		}
	}

	void transformFnArgDefList(FnArgDefList_t* argDefList, str_list& out) {
		auto x = argDefList;
		struct ArgItem {
			bool checkExistence = false;
			std::string name;
			std::string assignSelf;
		};
		std::list<ArgItem> argItems;
		str_list temp;
		std::string varNames;
		bool assignSelf = false;
		for (auto _def : argDefList->definitions.objects()) {
			auto def = static_cast<FnArgDef_t*>(_def);
			auto& arg = argItems.emplace_back();
			switch (def->name->getId()) {
				case id<Variable_t>(): arg.name = _parser.toString(def->name); break;
				case id<SelfItem_t>(): {
					assignSelf = true;
					if (def->op) {
						if (def->defaultValue) {
							throw CompileError("argument with default value should not check for existence"sv, def->op);
						}
						arg.checkExistence = true;
					}
					auto selfName = static_cast<SelfItem_t*>(def->name.get());
					switch (selfName->name->getId()) {
						case id<SelfClassName_t>(): {
							auto clsName = static_cast<SelfClassName_t*>(selfName->name.get());
							arg.name = _parser.toString(clsName->name);
							arg.assignSelf = _parser.toString(clsName);
							break;
						}
						case id<SelfName_t>(): {
							auto sfName = static_cast<SelfName_t*>(selfName->name.get());
							arg.name = _parser.toString(sfName->name);
							arg.assignSelf = _parser.toString(sfName);
							break;
						}
						case id<Self_t>():
							arg.name = "self"sv;
							if (def->op) throw CompileError("can only check existence for assigning self field"sv, selfName->name);
							break;
						default:
							throw CompileError("invald self expression here"sv, selfName->name);
							break;
					}
					break;
				}
				default: YUEE("AST node mismatch", def->name.get()); break;
			}
			forceAddToScope(arg.name);
			if (def->defaultValue) {
				pushScope();
				auto expList = toAst<ExpList_t>(arg.name, x);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(def->defaultValue.get());
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(expList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				popScope();
				_buf << indent() << "if "sv << arg.name << " == nil then"sv << nll(def);
				_buf << temp.back();
				_buf << indent() << "end"sv << nll(def);
				temp.back() = clearBuf();
			}
			if (varNames.empty())
				varNames = arg.name;
			else
				varNames.append(", "s + arg.name);
		}
		if (argDefList->varArg) {
			auto& arg = argItems.emplace_back();
			arg.name = "..."sv;
			if (varNames.empty())
				varNames = arg.name;
			else
				varNames.append(", "s + arg.name);
			_varArgs.top().hasVar = true;
		}
		if (assignSelf) {
			for (const auto& item : argItems) {
				if (item.assignSelf.empty()) continue;
				if (item.checkExistence) {
					auto stmt = toAst<Statement_t>(item.assignSelf + " = "s + item.name + " if "s + item.name + '?', x);
					transformStatement(stmt, temp);
				} else {
					auto assignment = toAst<ExpListAssign_t>(item.assignSelf + " = "s + item.name, x);
					transformAssignment(assignment, temp);
				}
			}
		}
		out.push_back(varNames);
		out.push_back(join(temp));
	}

	void transformSelfName(SelfItem_t* selfName, str_list& out, const ast_sel<false, Invoke_t, InvokeArgs_t>& invoke = {}) {
		auto x = selfName;
		auto name = selfName->name.get();
		switch (name->getId()) {
			case id<SelfClassName_t>(): {
				auto clsName = static_cast<SelfClassName_t*>(name);
				auto nameStr = _parser.toString(clsName->name);
				if (LuaKeywords.find(nameStr) != LuaKeywords.end()) {
					out.push_back("self.__class[\""s + nameStr + "\"]"s);
					if (invoke) {
						if (auto invokePtr = invoke.as<Invoke_t>()) {
							invokePtr->args.push_front(toAst<Exp_t>("self.__class"sv, x));
						} else {
							auto invokeArgsPtr = invoke.as<InvokeArgs_t>();
							invokeArgsPtr->args.push_front(toAst<Exp_t>("self.__class"sv, x));
						}
					}
				} else {
					out.push_back("self.__class"s + (invoke ? ':' : '.') + nameStr);
				}
				break;
			}
			case id<SelfClass_t>():
				out.push_back("self.__class"s);
				break;
			case id<SelfName_t>(): {
				auto sfName = static_cast<SelfClassName_t*>(name);
				auto nameStr = _parser.toString(sfName->name);
				if (LuaKeywords.find(nameStr) != LuaKeywords.end()) {
					out.push_back("self[\""s + nameStr + "\"]"s);
					if (invoke) {
						if (auto invokePtr = invoke.as<Invoke_t>()) {
							invokePtr->args.push_front(toAst<Exp_t>("self"sv, x));
						} else {
							auto invokeArgsPtr = invoke.as<InvokeArgs_t>();
							invokeArgsPtr->args.push_front(toAst<Exp_t>("self"sv, x));
						}
					}
				} else {
					out.push_back("self"s + (invoke ? ':' : '.') + nameStr);
				}
				break;
			}
			case id<Self_t>():
				out.push_back("self"s);
				break;
			default: YUEE("AST node mismatch", name); break;
		}
	}

	bool transformChainEndWithEOP(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList) {
		auto x = chainList.front();
		if (ast_is<ExistentialOp_t>(chainList.back())) {
			auto parens = x->new_ptr<Parens_t>();
			{
				auto chainValue = x->new_ptr<ChainValue_t>();
				for (auto item : chainList) {
					chainValue->items.push_back(item);
				}
				chainValue->items.pop_back();
				auto value = x->new_ptr<Value_t>();
				value->item.set(chainValue);
				auto exp = newExp(value, toAst<BinaryOperator_t>("!="sv, x), toAst<Value_t>("nil"sv, x), x);
				parens->expr.set(exp);
			}
			switch (usage) {
				case ExpUsage::Assignment: {
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(parens);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto exp = newExp(chainValue, x);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(assignList);
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					assignment->action.set(assign);
					transformAssignment(assignment, out);
					break;
				}
				case ExpUsage::Return:
					transformParens(parens, out);
					out.back().insert(0, indent() + "return "s);
					out.back().append(nlr(x));
					break;
				default:
					transformParens(parens, out);
					break;
			}
			return true;
		}
		return false;
	}

	bool transformChainWithEOP(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList, bool optionalDestruct) {
		auto opIt = std::find_if(chainList.begin(), chainList.end(), [](ast_node* node) { return ast_is<ExistentialOp_t>(node); });
		if (opIt != chainList.end()) {
			auto x = chainList.front();
			str_list temp;
			std::string* funcStart = nullptr;
			if (usage == ExpUsage::Closure) {
				pushFunctionScope();
				pushAnonVarArg();
				funcStart = &temp.emplace_back();
				pushScope();
			}
			auto partOne = x->new_ptr<ChainValue_t>();
			for (auto it = chainList.begin(); it != opIt; ++it) {
				partOne->items.push_back(*it);
			}
			BLOCK_START
			auto back = ast_cast<Callable_t>(partOne->items.back());
			BREAK_IF(!back);
			auto selfName = ast_cast<SelfItem_t>(back->item);
			BREAK_IF(!selfName);
			if (auto sname = ast_cast<SelfName_t>(selfName->name)) {
				auto colonItem = x->new_ptr<ColonChainItem_t>();
				colonItem->name.set(sname->name);
				partOne->items.pop_back();
				partOne->items.push_back(toAst<Callable_t>("@"sv, x));
				partOne->items.push_back(colonItem);
				break;
			}
			if (auto cname = ast_cast<SelfClassName_t>(selfName->name)) {
				auto colonItem = x->new_ptr<ColonChainItem_t>();
				colonItem->name.set(cname->name);
				partOne->items.pop_back();
				partOne->items.push_back(toAst<Callable_t>("@@"sv, x));
				partOne->items.push_back(colonItem);
				break;
			}
			BLOCK_END
			auto objVar = singleVariableFrom(partOne);
			bool isScoped = false;
			if (objVar.empty() || !isLocal(objVar)) {
				switch (usage) {
					case ExpUsage::Common:
					case ExpUsage::Assignment:
						isScoped = true;
						break;
					default: break;
				}
				if (isScoped) {
					temp.push_back(indent() + "do"s + nll(x));
					pushScope();
				}
				objVar = getUnusedName("_obj_"sv);
				if (auto colonItem = ast_cast<ColonChainItem_t>(partOne->items.back())) {
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.dup(partOne->items);
					chainValue->items.pop_back();
					if (chainValue->items.empty()) {
						if (_withVars.empty()) {
							throw CompileError("short dot/colon syntax must be called within a with block"sv, x);
						}
						chainValue->items.push_back(toAst<Callable_t>(_withVars.top(), x));
					}
					auto newObj = singleVariableFrom(chainValue);
					if (!newObj.empty()) {
						objVar = newObj;
					} else {
						auto exp = newExp(chainValue, x);
						auto assign = x->new_ptr<Assign_t>();
						assign->values.push_back(exp);
						auto expListAssign = x->new_ptr<ExpListAssign_t>();
						expListAssign->expList.set(toAst<ExpList_t>(objVar, x));
						expListAssign->action.set(assign);
						transformAssignment(expListAssign, temp);
					}
					auto dotItem = x->new_ptr<DotChainItem_t>();
					auto name = colonItem->name.get();
					if (auto keyword = ast_cast<LuaKeyword_t>(name)) {
						name = keyword->name.get();
					}
					dotItem->name.set(name);
					partOne->items.clear();
					partOne->items.push_back(toAst<Callable_t>(objVar, x));
					partOne->items.push_back(dotItem);
					auto it = opIt;
					++it;
					if (it != chainList.end() && ast_is<Invoke_t, InvokeArgs_t>(*it)) {
						if (auto invoke = ast_cast<Invoke_t>(*it)) {
							invoke->args.push_front(toAst<Exp_t>(objVar, x));
						} else {
							auto invokeArgs = static_cast<InvokeArgs_t*>(*it);
							invokeArgs->args.push_front(toAst<Exp_t>(objVar, x));
						}
					}
					objVar = getUnusedName("_obj_"sv);
				}
				auto exp = newExp(partOne, x);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(exp);
				auto expListAssign = x->new_ptr<ExpListAssign_t>();
				expListAssign->expList.set(toAst<ExpList_t>(objVar, x));
				expListAssign->action.set(assign);
				transformAssignment(expListAssign, temp);
			}
			if (optionalDestruct) {
				auto typeVar = getUnusedName("_type_"sv);
				_buf << typeVar << "=type "sv << objVar;
				auto typeAssign = toAst<ExpListAssign_t>(clearBuf(), partOne);
				transformAssignment(typeAssign, temp);
				_buf << indent() << "if \"table\" == " << typeVar << " or \"userdata\" == "sv << typeVar << " then"sv << nll(x);
			} else {
				_buf << indent() << "if "sv << objVar << " ~= nil then"sv << nll(x);
			}
			temp.push_back(clearBuf());
			pushScope();
			auto partTwo = x->new_ptr<ChainValue_t>();
			partTwo->items.push_back(toAst<Callable_t>(objVar, x));
			for (auto it = ++opIt; it != chainList.end(); ++it) {
				partTwo->items.push_back(*it);
			}
			switch (usage) {
				case ExpUsage::Common:
					transformChainValue(partTwo, temp, ExpUsage::Common);
					break;
				case ExpUsage::Assignment: {
					auto exp = newExp(partTwo, x);
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(assignList);
					assignment->action.set(assign);
					transformAssignment(assignment, temp, optionalDestruct);
					break;
				}
				case ExpUsage::Return:
				case ExpUsage::Closure: {
					auto exp = newExp(partTwo, x);
					auto ret = x->new_ptr<Return_t>();
					auto expListLow = x->new_ptr<ExpListLow_t>();
					expListLow->exprs.push_back(exp);
					ret->valueList.set(expListLow);
					transformReturn(ret, temp);
					break;
				}
			}
			popScope();
			temp.push_back(indent() + "end"s + nlr(x));
			switch (usage) {
				case ExpUsage::Return:
					temp.push_back(indent() + "return nil"s + nlr(x));
					break;
				case ExpUsage::Closure:
					temp.push_back(indent() + "return nil"s + nlr(x));
					popScope();
					*funcStart = anonFuncStart() + nll(x);
					temp.push_back(indent() + anonFuncEnd());
					popAnonVarArg();
					popFunctionScope();
					break;
				default:
					break;
			}
			if (isScoped) {
				popScope();
				temp.push_back(indent() + "end"s + nlr(x));
			}
			out.push_back(join(temp));
			return true;
		}
		return false;
	}

	bool transformChainEndWithColonItem(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList) {
		if (ast_is<ColonChainItem_t>(chainList.back())) {
			auto x = chainList.front();
			str_list temp;
			std::string* funcStart = nullptr;
			switch (usage) {
				case ExpUsage::Assignment:
					temp.push_back(indent() + "do"s + nll(x));
					pushScope();
					break;
				case ExpUsage::Closure:
					pushFunctionScope();
					pushAnonVarArg();
					funcStart = &temp.emplace_back();
					pushScope();
					break;
				default:
					break;
			}
			auto baseChain = x->new_ptr<ChainValue_t>();
			switch (chainList.front()->getId()) {
				case id<DotChainItem_t>():
				case id<ColonChainItem_t>():
				case id<Exp_t>():
					if (_withVars.empty()) {
						throw CompileError("short dot/colon syntax must be called within a with block"sv, chainList.front());
					} else {
						baseChain->items.push_back(toAst<Callable_t>(_withVars.top(), x));
					}
					break;
			}
			auto end = --chainList.end();
			for (auto it = chainList.begin(); it != end; ++it) {
				baseChain->items.push_back(*it);
			}
			auto colonChainItem = static_cast<ColonChainItem_t*>(chainList.back());
			auto funcName = _parser.toString(colonChainItem->name);
			auto baseVar = getUnusedName("_base_"sv);
			auto fnVar = getUnusedName("_fn_"sv);
			{
				auto exp = newExp(baseChain, x);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(exp);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(baseVar, x));
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
			{
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(baseVar + "." + funcName, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(fnVar, x));
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
			auto funLit = toAst<Exp_t>(fnVar + " and (...)-> "s + fnVar + ' ' + baseVar + ", ..."s, x);
			switch (usage) {
				case ExpUsage::Closure:
				case ExpUsage::Return: {
					auto returnNode = x->new_ptr<Return_t>();
					auto expListLow = x->new_ptr<ExpListLow_t>();
					expListLow->exprs.push_back(funLit);
					returnNode->valueList.set(expListLow);
					transformReturn(returnNode, temp);
					break;
				}
				case ExpUsage::Assignment: {
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(funLit);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(assignList);
					assignment->action.set(assign);
					transformAssignment(assignment, temp);
					break;
				}
				default:
					break;
			}
			switch (usage) {
				case ExpUsage::Assignment:
					popScope();
					temp.push_back(indent() + "end"s + nlr(x));
					break;
				case ExpUsage::Closure:
					popScope();
					*funcStart = anonFuncStart() + nll(x);
					temp.push_back(indent() + anonFuncEnd());
					popAnonVarArg();
					popFunctionScope();
					break;
				default:
					break;
			}
			out.push_back(join(temp));
			return true;
		}
		return false;
	}

	bool transformChainWithMetatable(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList) {
		auto opIt = std::find_if(chainList.begin(), chainList.end(), [](ast_node* node) {
			if (auto colonChain = ast_cast<ColonChainItem_t>(node)) {
				if (ast_is<Metamethod_t>(colonChain->name)) {
					return true;
				}
			} else if (auto dotChain = ast_cast<DotChainItem_t>(node)) {
				if (ast_is<Metatable_t, Metamethod_t>(dotChain->name)) {
					return true;
				}
			}
			return false;
		});
		if (opIt == chainList.end()) return false;
		auto x = chainList.front();
		auto chain = x->new_ptr<ChainValue_t>();
		if (opIt == chainList.begin() && ast_is<ColonChainItem_t, DotChainItem_t>(x)) {
			if (_withVars.empty()) {
				throw CompileError("short dot/colon syntax must be called within a with block"sv, x);
			} else {
				chain->items.push_back(toAst<Callable_t>(_withVars.top(), x));
			}
		}
		for (auto it = chainList.begin(); it != opIt; ++it) {
			chain->items.push_back(*it);
		}
		{
			auto exp = newExp(chain, x);
			chain = toAst<ChainValue_t>("getmetatable()"sv, x);
			ast_to<Invoke_t>(chain->items.back())->args.push_back(exp);
		}
		switch ((*opIt)->getId()) {
			case id<ColonChainItem_t>(): {
				auto colon = static_cast<ColonChainItem_t*>(*opIt);
				auto meta = colon->name.to<Metamethod_t>();
				switch (meta->item->getId()) {
					case id<Name_t>(): {
						auto newColon = toAst<ColonChainItem_t>("\\__"s + _parser.toString(meta->item), x);
						chain->items.push_back(newColon);
						break;
					}
					case id<String_t>():
					case id<Exp_t>(): {
						str_list temp;
						std::string* funcStart = nullptr;
						if (usage == ExpUsage::Closure) {
							pushFunctionScope();
							pushAnonVarArg();
							funcStart = &temp.emplace_back();
							pushScope();
						} else if (usage != ExpUsage::Return) {
							temp.push_back(indent() + "do"s + nll(x));
							pushScope();
						}
						auto var = getUnusedName("_obj_"sv);
						auto target = toAst<Exp_t>(var, x);
						{
							auto assignment = assignmentFrom(target, newExp(chain, x), x);
							transformAssignment(assignment, temp);
						}
						auto newChain = toAst<ChainValue_t>(var, x);
						if (auto str = meta->item.as<String_t>()) {
							newChain->items.push_back(newExp(str, x));
						} else {
							newChain->items.push_back(meta->item);
						}
						auto newChainExp = newExp(newChain, x);
						for (auto it = ++opIt; it != chainList.end(); ++it) {
							newChain->items.push_back(*it);
						}
						auto last = newChain->items.back();
						if (ast_is<Invoke_t, InvokeArgs_t>(last)) {
							if (auto invoke = ast_cast<InvokeArgs_t>(last)) {
								invoke->args.push_front(target);
							} else {
								ast_to<Invoke_t>(last)->args.push_front(target);
							}
						}
						switch (usage) {
							case ExpUsage::Closure: {
								auto returnNode = x->new_ptr<Return_t>();
								auto values = x->new_ptr<ExpListLow_t>();
								values->exprs.push_back(newChainExp);
								returnNode->valueList.set(values);
								transformReturn(returnNode, temp);
								popScope();
								*funcStart = anonFuncStart() + nll(x);
								temp.push_back(indent() + anonFuncEnd());
								popAnonVarArg();
								popFunctionScope();
								break;
							}
							case ExpUsage::Return: {
								auto returnNode = x->new_ptr<Return_t>();
								auto values = x->new_ptr<ExpListLow_t>();
								values->exprs.push_back(newChainExp);
								returnNode->valueList.set(values);
								transformReturn(returnNode, temp);
								break;
							}
							case ExpUsage::Assignment: {
								auto assignment = x->new_ptr<ExpListAssign_t>();
								assignment->expList.set(assignList);
								auto assign = x->new_ptr<Assign_t>();
								assign->values.push_back(newChainExp);
								assignment->action.set(assign);
								transformAssignment(assignment, temp);
								popScope();
								temp.push_back(indent() + "end"s + nlr(x));
								break;
							}
							case ExpUsage::Common:
								transformExp(newChainExp, temp, usage);
								popScope();
								temp.push_back(indent() + "end"s + nlr(x));
								break;
						}
						out.push_back(join(temp));
						return true;
					}
					default: YUEE("AST node mismatch", meta->item); break;
				}
				break;
			}
			case id<DotChainItem_t>(): {
				auto dot = static_cast<DotChainItem_t*>(*opIt);
				if (dot->name.is<Metatable_t>()) break;
				auto meta = dot->name.to<Metamethod_t>();
				switch (meta->item->getId()) {
					case id<Name_t>(): {
						auto newDot = toAst<DotChainItem_t>(".__"s + _parser.toString(meta->item), x);
						chain->items.push_back(newDot);
						break;
					}
					case id<Exp_t>():
						chain->items.push_back(meta->item);
						break;
					case id<String_t>(): {
						auto str = static_cast<String_t*>(meta->item.get());
						chain->items.push_back(newExp(str, x));
						break;
					}
					default: YUEE("AST node mismatch", meta->item); break;
				}
				break;
			}
		}
		for (auto it = ++opIt; it != chainList.end(); ++it) {
			chain->items.push_back(*it);
		}
		transformChainValue(chain, out, usage, assignList);
		return true;
	}

	void transformChainList(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = chainList.front();
		str_list temp;
		switch (x->getId()) {
			case id<DotChainItem_t>():
			case id<ColonChainItem_t>():
			case id<Exp_t>():
				if (_withVars.empty()) {
					throw CompileError("short dot/colon and indexing syntax must be called within a with block"sv, x);
				} else {
					temp.push_back(_withVars.top());
				}
				break;
		}
		for (auto it = chainList.begin(); it != chainList.end(); ++it) {
			auto item = *it;
			switch (item->getId()) {
				case id<Invoke_t>():
					transformInvoke(static_cast<Invoke_t*>(item), temp);
					break;
				case id<DotChainItem_t>():
					transformDotChainItem(static_cast<DotChainItem_t*>(item), temp);
					break;
				case id<ColonChainItem_t>(): {
					auto colonItem = static_cast<ColonChainItem_t*>(item);
					auto current = it;
					auto next = current;
					++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					if (current != chainList.begin()) {
						--current;
						if (!ast_is<ExistentialOp_t>(*current)) {
							++current;
						}
					}
					if (ast_is<ExistentialOp_t>(followItem)) {
						++next;
						followItem = next != chainList.end() ? *next : nullptr;
						--next;
					}
					if (!ast_is<Invoke_t, InvokeArgs_t>(followItem)) {
						throw CompileError("colon chain item must be followed by invoke arguments"sv, colonItem);
					}
					if (colonItem->name.is<LuaKeyword_t>()) {
						std::string callVar;
						auto block = x->new_ptr<Block_t>();
						{
							auto chainValue = x->new_ptr<ChainValue_t>();
							switch (chainList.front()->getId()) {
								case id<DotChainItem_t>():
								case id<ColonChainItem_t>():
									chainValue->items.push_back(toAst<Callable_t>(_withVars.top(), x));
									break;
							}
							for (auto i = chainList.begin(); i != current; ++i) {
								chainValue->items.push_back(*i);
							}
							auto exp = newExp(chainValue, x);
							callVar = singleVariableFrom(exp, true);
							if (callVar.empty()) {
								callVar = getUnusedName("_call_"s);
								auto assignment = x->new_ptr<ExpListAssign_t>();
								assignment->expList.set(toAst<ExpList_t>(callVar, x));
								auto assign = x->new_ptr<Assign_t>();
								assign->values.push_back(exp);
								assignment->action.set(assign);
								auto stmt = x->new_ptr<Statement_t>();
								stmt->content.set(assignment);
								block->statements.push_back(stmt);
							}
						}
						ast_ptr<false, Exp_t> nexp;
						{
							auto name = _parser.toString(colonItem->name);
							auto chainValue = x->new_ptr<ChainValue_t>();
							chainValue->items.push_back(toAst<Callable_t>(callVar, x));
							if (ast_is<ExistentialOp_t>(*current)) {
								chainValue->items.push_back(x->new_ptr<ExistentialOp_t>());
							}
							chainValue->items.push_back(toAst<Exp_t>('\"' + name + '\"', x));
							if (auto invoke = ast_cast<Invoke_t>(followItem)) {
								invoke->args.push_front(toAst<Exp_t>(callVar, x));
							} else {
								auto invokeArgs = static_cast<InvokeArgs_t*>(followItem);
								invokeArgs->args.push_front(toAst<Exp_t>(callVar, x));
							}
							for (auto i = next; i != chainList.end(); ++i) {
								chainValue->items.push_back(*i);
							}
							nexp = newExp(chainValue, x);
							auto expList = x->new_ptr<ExpList_t>();
							expList->exprs.push_back(nexp);
							auto expListAssign = x->new_ptr<ExpListAssign_t>();
							expListAssign->expList.set(expList);
							auto stmt = x->new_ptr<Statement_t>();
							stmt->content.set(expListAssign);
							block->statements.push_back(stmt);
						}
						switch (usage) {
							case ExpUsage::Common:
							case ExpUsage::Return:
								transformBlock(block, out, usage);
								return;
							case ExpUsage::Assignment: {
								transformBlock(block, out, ExpUsage::Assignment, assignList);
								return;
							}
							default:
								break;
						}
						if (block->statements.size() == 1) {
							transformExp(nexp, out, usage, assignList);
						} else {
							auto body = x->new_ptr<Body_t>();
							body->content.set(block);
							auto funLit = toAst<FunLit_t>("->"sv, x);
							funLit->body.set(body);
							auto simpleValue = x->new_ptr<SimpleValue_t>();
							simpleValue->value.set(funLit);
							auto exp = newExp(simpleValue, x);
							auto paren = x->new_ptr<Parens_t>();
							paren->expr.set(exp);
							auto callable = x->new_ptr<Callable_t>();
							callable->item.set(paren);
							auto chainValue = x->new_ptr<ChainValue_t>();
							chainValue->items.push_back(callable);
							auto invoke = x->new_ptr<Invoke_t>();
							chainValue->items.push_back(invoke);
							transformChainValue(chainValue, out, ExpUsage::Closure);
						}
						return;
					}
					transformColonChainItem(colonItem, temp);
					break;
				}
				case id<Slice_t>():
					transformSlice(static_cast<Slice_t*>(item), temp);
					break;
				case id<Callable_t>(): {
					auto next = it;
					++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					ast_sel<false, Invoke_t, InvokeArgs_t> invoke;
					if (ast_is<Invoke_t, InvokeArgs_t>(followItem)) {
						invoke.set(followItem);
					}
					transformCallable(static_cast<Callable_t*>(item), temp, invoke);
					break;
				}
				case id<String_t>():
					transformString(static_cast<String_t*>(item), temp);
					temp.back() = '(' + temp.back() + ')';
					break;
				case id<Exp_t>():
					transformExp(static_cast<Exp_t*>(item), temp, ExpUsage::Closure);
					temp.back() = (temp.back().front() == '[' ? "[ "s : "["s) + temp.back() + ']';
					break;
				case id<InvokeArgs_t>(): transformInvokeArgs(static_cast<InvokeArgs_t*>(item), temp); break;
				case id<TableAppendingOp_t>():
					transform_table_appending_op(static_cast<TableAppendingOp_t*>(item), temp);
					break;
				default: YUEE("AST node mismatch", item); break;
			}
		}
		switch (usage) {
			case ExpUsage::Common:
				out.push_back(indent() + join(temp) + nll(x));
				break;
			case ExpUsage::Return:
				out.push_back(indent() + "return "s + join(temp) + nll(x));
				break;
			case ExpUsage::Assignment: YUEE("invalid expression usage", x); break;
			default:
				out.push_back(join(temp));
				break;
		}
	}

	void transformMacroInPlace(MacroInPlace_t* macroInPlace) {
#ifdef YUE_NO_MACRO
		throw CompileError("macro feature not supported"sv, macroInPlace));
#else // YUE_NO_MACRO
		auto x = macroInPlace;
		pushCurrentModule(); // cur
		int top = lua_gettop(L) - 1;
		DEFER(lua_settop(L, top));
		lua_pop(L, 1); // empty
		auto fcodes = _parser.toString(macroInPlace).substr(1);
		Utils::trim(fcodes);
		pushYue("loadstring"sv); // loadstring
		lua_pushlstring(L, fcodes.c_str(), fcodes.size()); // loadstring codes
		lua_pushliteral(L, "=(macro in-place)"); // loadstring codes chunk
		pushOptions(macroInPlace->m_begin.m_line - 1); // loadstring codes chunk options
		if (lua_pcall(L, 3, 2, 0) != 0) { // loadstring(codes,chunk,options), f err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to load macro codes\n"s + err, x);
		} // f err
		if (lua_isnil(L, -2) != 0) { // f == nil, f err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to load macro codes, at (macro in-place): "s + err, x);
		}
		lua_pop(L, 1); // f
		pushYue("pcall"sv); // f pcall
		lua_insert(L, -2); // pcall f
		if (lua_pcall(L, 1, 2, 0) != 0) { // f(), success macroFunc
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to generate macro function\n"s + err, x);
		} // success res
		if (lua_toboolean(L, -2) == 0) {
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to generate macro function\n"s + err, x);
		} // true macroFunc
		lua_remove(L, -2); // macroFunc
		pushYue("pcall"sv); // macroFunc pcall
		lua_insert(L, -2); // pcall macroFunc
		bool success = lua_pcall(L, 1, 2, 0) == 0;
		if (!success) { // err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to expand macro: "s + err, x);
		} // success err
		if (lua_toboolean(L, -2) == 0) {
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to expand macro: "s + err, x);
		}
#endif // YUE_NO_MACRO
	}

#ifndef YUE_NO_MACRO
	std::string expandBuiltinMacro(const std::string& name, ast_node* x) {
		if (name == "LINE"sv) {
			return std::to_string(x->m_begin.m_line + _config.lineOffset);
		}
		if (name == "FILE"sv) {
			return _config.module.empty() ? "\"yuescript\""s : '"' + _config.module + '"';
		}
		return Empty;
	}

	std::tuple<std::string, std::string, str_list> expandMacroStr(ChainValue_t* chainValue) {
		const auto& chainList = chainValue->items.objects();
		auto x = ast_to<Callable_t>(chainList.front())->item.to<MacroName_t>();
		auto macroName = _parser.toString(x->name);
		if (!_useModule) {
			auto code = expandBuiltinMacro(macroName, x);
			if (!code.empty()) return {Empty, code, {}};
			throw CompileError("can not resolve macro"sv, x);
		}
		pushCurrentModule(); // cur
		int top = lua_gettop(L) - 1;
		DEFER(lua_settop(L, top));
		lua_pushlstring(L, macroName.c_str(), macroName.size()); // cur macroName
		lua_rawget(L, -2); // cur[macroName], cur macroFunc
		if (lua_isfunction(L, -1) == 0) {
			auto code = expandBuiltinMacro(macroName, x);
			if (!code.empty()) return {Empty, code, {}};
			throw CompileError("can not resolve macro"sv, x);
		} // cur macroFunc
		pushYue("pcall"sv); // cur macroFunc pcall
		lua_insert(L, -2); // cur pcall macroFunc
		const node_container* args = nullptr;
		if (chainList.size() > 1) {
			auto item = *(++chainList.begin());
			if (auto invoke = ast_cast<Invoke_t>(item)) {
				args = &invoke->args.objects();
			} else if (auto invoke = ast_cast<InvokeArgs_t>(item)) {
				args = &invoke->args.objects();
			}
		}
		if (args) {
			for (auto arg : *args) {
				std::string str;
				bool rawString = false;
				if (auto lstr = ast_cast<LuaString_t>(arg)) {
					str = _parser.toString(lstr->content);
					rawString = true;
				} else {
					BLOCK_START
					auto exp = ast_cast<Exp_t>(arg);
					BREAK_IF(!exp);
					auto value = singleValueFrom(exp);
					BREAK_IF(!value);
					auto lstr = value->getByPath<String_t, LuaString_t>();
					BREAK_IF(!lstr);
					str = _parser.toString(lstr->content);
					rawString = true;
					BLOCK_END
				}
				if (!rawString && str.empty()) {
					// check whether arg is reassembled
					// do some workaround for pipe expression
					if (ast_is<Exp_t>(arg)) {
						auto exp = static_cast<Exp_t*>(arg);
						BLOCK_START
						BREAK_IF(!exp->opValues.empty());
						auto chainValue = exp->getByPath<UnaryExp_t, Value_t, ChainValue_t>();
						BREAK_IF(!chainValue);
						BREAK_IF(!isMacroChain(chainValue));
						str = std::get<1>(expandMacroStr(chainValue));
						BLOCK_END
						if (str.empty() && arg->m_begin.m_it == arg->m_end.m_it) {
							// exp is reassembled due to pipe expressions
							// in transform stage, toString(exp) won't be able
							// to convert its whole text content
							str = _parser.toString(exp->pipeExprs.front());
						}
					}
				}
				if (!rawString && str.empty()) {
					str = _parser.toString(arg);
				}
				Utils::trim(str);
				Utils::replace(str, "\r\n"sv, "\n"sv);
				lua_pushlstring(L, str.c_str(), str.size());
			} // cur pcall macroFunc args...
		}
		bool success = lua_pcall(L, (args ? static_cast<int>(args->size()) : 0) + 1, 2, 0) == 0;
		if (!success) { // cur err
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to expand macro: "s + err, x);
		} // cur success res
		if (lua_toboolean(L, -2) == 0) {
			std::string err = lua_tostring(L, -1);
			throw CompileError("failed to expand macro: "s + err, x);
		}
		lua_remove(L, -2); // cur res
		if (lua_isstring(L, -1) == 0 && lua_istable(L, -1) == 0) {
			throw CompileError("macro function must return string or table"sv, x);
		} // cur res
		std::string codes;
		std::string type;
		str_list localVars;
		if (lua_istable(L, -1) != 0) {
			lua_getfield(L, -1, "code"); // cur res code
			if (lua_isstring(L, -1) != 0) {
				codes = lua_tostring(L, -1);
			} else {
				throw CompileError("macro table must contain field \"code\" of string"sv, x);
			}
			lua_pop(L, 1); // cur res
			lua_getfield(L, -1, "type"); // cur res type
			if (lua_isstring(L, -1) != 0) {
				type = lua_tostring(L, -1);
			}
			if (type != "lua"sv && type != "text"sv) {
				throw CompileError("macro table must contain field \"type\" of value \"lua\" or \"text\""sv, x);
			}
			lua_pop(L, 1); // cur res
			lua_getfield(L, -1, "locals"); // cur res locals
			if (lua_istable(L, -1) != 0) {
				for (int i = 0; i < static_cast<int>(lua_objlen(L, -1)); i++) {
					lua_rawgeti(L, -1, i + 1); // cur res locals item
					size_t len = 0;
					if (lua_isstring(L, -1) == 0) {
						throw CompileError("macro table field \"locals\" must be a table of strings"sv, x);
					}
					auto name = lua_tolstring(L, -1, &len);
					if (_parser.match<Variable_t>({name, len})) {
						localVars.push_back(std::string(name, len));
					} else {
						throw CompileError("macro table field \"locals\" must contain names for local variables, got \""s + std::string(name, len) + '"', x);
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1); // cur res
		} else {
			codes = lua_tostring(L, -1);
		}
		Utils::trim(codes);
		Utils::replace(codes, "\r\n"sv, "\n"sv);
		return {type, codes, std::move(localVars)};
	}

	std::tuple<ast_ptr<false, ast_node>, std::unique_ptr<input>, std::string, str_list> expandMacro(ChainValue_t* chainValue, ExpUsage usage, bool allowBlockMacroReturn) {
		auto x = ast_to<Callable_t>(chainValue->items.front())->item.to<MacroName_t>();
		const auto& chainList = chainValue->items.objects();
		std::string type, codes;
		str_list localVars;
		std::tie(type, codes, localVars) = expandMacroStr(chainValue);
		bool isBlock = (usage == ExpUsage::Common) && (chainList.size() < 2 || (chainList.size() == 2 && ast_is<Invoke_t, InvokeArgs_t>(chainList.back())));
		ParseInfo info;
		if (type == "lua"sv) {
			if (!isBlock) {
				throw CompileError("lua macro can only be placed where block macro is allowed"sv, x);
			}
			auto macroChunk = "=(macro "s + _parser.toString(x->name) + ')';
			int top = lua_gettop(L);
			DEFER(lua_settop(L, top));
			if (luaL_loadbuffer(L, codes.c_str(), codes.size(), macroChunk.c_str()) != 0) {
				std::string err = lua_tostring(L, -1);
				throw CompileError(err, x);
			}
			if (!codes.empty()) {
				if (_config.reserveLineNumber) {
					codes.insert(0, nll(chainValue).substr(1));
				}
				codes.append(nlr(chainValue));
			}
			return {nullptr, nullptr, std::move(codes), std::move(localVars)};
		} else if (type == "text"sv) {
			if (!isBlock) {
				throw CompileError("text macro can only be placed where block macro is allowed"sv, x);
			}
			if (!codes.empty()) {
				codes.append(_newLine);
			}
			return {nullptr, nullptr, std::move(codes), std::move(localVars)};
		} else {
			if (!codes.empty()) {
				if (isBlock) {
					info = _parser.parse<BlockEnd_t>(codes);
					if (info.error) {
						throw CompileError("failed to expand macro as block: "s + info.error.value().msg, x);
					}
				} else {
					info = _parser.parse<Exp_t>(codes);
					if (!info.node && allowBlockMacroReturn) {
						info = _parser.parse<BlockEnd_t>(codes);
						if (info.error) {
							throw CompileError("failed to expand macro as expr or block: "s + info.error.value().msg, x);
						}
						isBlock = true;
					} else if (info.error) {
						throw CompileError("failed to expand macro as expr: "s + info.error.value().msg, x);
					}
				}
				int line = x->m_begin.m_line;
				int col = x->m_begin.m_col;
				info.node->traverse([&](ast_node* node) {
					node->m_begin.m_line = line;
					node->m_end.m_line = line;
					node->m_begin.m_col = col;
					node->m_end.m_col = col;
					return traversal::Continue;
				});
				if (!isBlock) {
					ast_ptr<false, Exp_t> exp;
					exp.set(info.node);
					if (!exp->opValues.empty() || (chainList.size() > 2 || (chainList.size() == 2 && !ast_is<Invoke_t, InvokeArgs_t>(chainList.back())))) {
						auto paren = x->new_ptr<Parens_t>();
						paren->expr.set(exp);
						auto callable = x->new_ptr<Callable_t>();
						callable->item.set(paren);
						auto newChain = x->new_ptr<ChainValue_t>();
						newChain->items.push_back(callable);
						auto it = chainList.begin();
						it++;
						if (chainList.size() > 1 && ast_is<Invoke_t, InvokeArgs_t>(*it)) it++;
						for (; it != chainList.end(); ++it) {
							newChain->items.push_back(*it);
						}
						exp = newExp(newChain, x);
					}
					if (usage == ExpUsage::Common) {
						auto expList = x->new_ptr<ExpList_t>();
						expList->exprs.push_back(exp);
						auto exps = x->new_ptr<ExpListAssign_t>();
						exps->expList.set(expList);
						auto stmt = x->new_ptr<Statement_t>();
						stmt->content.set(exps);
						auto block = x->new_ptr<Block_t>();
						block->statements.push_back(stmt);
						info.node.set(block);
					} else {
						info.node.set(exp);
					}
				}
				if (auto blockEnd = info.node.as<BlockEnd_t>()) {
					auto block = blockEnd->block.get();
					info.node.set(block);
					for (auto stmt_ : block->statements.objects()) {
						auto stmt = static_cast<Statement_t*>(stmt_);
						if (auto global = stmt->content.as<Global_t>()) {
							if (global->item.is<GlobalOp_t>()) {
								throw CompileError("can not insert global statement with wildcard operator from macro"sv, x);
							}
						} else if (auto local = stmt->content.as<Local_t>()) {
							if (local->item.is<LocalFlag_t>()) {
								throw CompileError("can not insert local statement with wildcard operator from macro"sv, x);
							}
						}
					}
				}
				return {info.node, std::move(info.codes), Empty, std::move(localVars)};
			} else {
				if (!isBlock) throw CompileError("failed to expand empty macro as expr"sv, x);
				return {x->new_ptr<Block_t>().get(), std::move(info.codes), Empty, std::move(localVars)};
			}
		}
	}
#endif // YUE_NO_MACRO

	void transformChainValue(ChainValue_t* chainValue, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr, bool allowBlockMacroReturn = false, bool optionalDestruct = false) {
		if (isMacroChain(chainValue)) {
#ifndef YUE_NO_MACRO
			ast_ptr<false, ast_node> node;
			std::unique_ptr<input> codes;
			std::string luaCodes;
			str_list localVars;
			std::tie(node, codes, luaCodes, localVars) = expandMacro(chainValue, usage, allowBlockMacroReturn);
			if (!node) {
				out.push_back(luaCodes);
				if (!localVars.empty()) {
					for (const auto& var : localVars) {
						addToScope(var);
					}
				}
				return;
			}
			if (usage == ExpUsage::Common || (usage == ExpUsage::Return && node.is<Block_t>())) {
				transformBlock(node.to<Block_t>(), out, usage, assignList);
			} else {
				auto x = chainValue;
				switch (usage) {
					case ExpUsage::Assignment: {
						auto assign = x->new_ptr<Assign_t>();
						assign->values.push_back(node);
						auto assignment = x->new_ptr<ExpListAssign_t>();
						assignment->expList.set(assignList);
						assignment->action.set(assign);
						transformAssignment(assignment, out);
						break;
					}
					case ExpUsage::Return: {
						auto expListLow = x->new_ptr<ExpListLow_t>();
						expListLow->exprs.push_back(node);
						auto returnNode = x->new_ptr<Return_t>();
						returnNode->valueList.set(expListLow);
						transformReturn(returnNode, out);
						break;
					}
					default:
						transformExp(node.to<Exp_t>(), out, usage);
						break;
				}
			}
			return;
#else
			(void)allowBlockMacroReturn;
			throw CompileError("macro feature not supported"sv, chainValue));
#endif // YUE_NO_MACRO
		}
		const auto& chainList = chainValue->items.objects();
		if (transformChainEndWithEOP(chainList, out, usage, assignList)) {
			return;
		}
		if (transformChainWithEOP(chainList, out, usage, assignList, optionalDestruct)) {
			return;
		}
		if (transformChainWithMetatable(chainList, out, usage, assignList)) {
			return;
		}
		if (transformChainEndWithColonItem(chainList, out, usage, assignList)) {
			return;
		}
		transformChainList(chainList, out, usage, assignList);
	}

	void transformAssignableChain(AssignableChain_t* chain, str_list& out) {
		transformChainList(chain->items.objects(), out, ExpUsage::Closure);
	}

	void transformDotChainItem(DotChainItem_t* dotChainItem, str_list& out) {
		auto name = _parser.toString(dotChainItem->name);
		if (LuaKeywords.find(name) != LuaKeywords.end()) {
			out.push_back("[\""s + name + "\"]"s);
		} else {
			out.push_back('.' + name);
		}
	}

	void transformColonChainItem(ColonChainItem_t* colonChainItem, str_list& out) {
		auto name = _parser.toString(colonChainItem->name);
		out.push_back((colonChainItem->switchToDot ? '.' : ':') + name);
	}

	void transformSlice(Slice_t* slice, str_list&) {
		throw CompileError("slice syntax not supported here"sv, slice);
	}

	void transform_table_appending_op(TableAppendingOp_t* op, str_list&) {
		throw CompileError("table appending syntax not supported here"sv, op);
	}

	void transformInvoke(Invoke_t* invoke, str_list& out) {
		str_list temp;
		for (auto arg : invoke->args.objects()) {
			switch (arg->getId()) {
				case id<Exp_t>(): transformExp(static_cast<Exp_t*>(arg), temp, ExpUsage::Closure); break;
				case id<SingleString_t>(): transformSingleString(static_cast<SingleString_t*>(arg), temp); break;
				case id<DoubleString_t>(): transformDoubleString(static_cast<DoubleString_t*>(arg), temp); break;
				case id<LuaString_t>(): transformLuaString(static_cast<LuaString_t*>(arg), temp); break;
				case id<TableLit_t>(): transformTableLit(static_cast<TableLit_t*>(arg), temp); break;
				default: YUEE("AST node mismatch", arg); break;
			}
		}
		out.push_back('(' + join(temp, ", "sv) + ')');
	}

	void transform_unary_value(UnaryValue_t* unary_value, str_list& out) {
		str_list temp;
		for (auto _op : unary_value->ops.objects()) {
			std::string op = _parser.toString(_op);
			if (op == "~"sv && getLuaTarget(_op) < 503) {
				throw CompileError("bitwise operator is not available when not targeting Lua version 5.3 or higher"sv, _op);
			}
			temp.push_back(op == "not"sv ? op + ' ' : op);
		}
		transformValue(unary_value->value, temp);
		out.push_back(join(temp));
	}

	void transform_unary_exp(UnaryExp_t* unary_exp, str_list& out) {
		if (unary_exp->ops.empty() && unary_exp->expos.size() == 1) {
			transformValue(static_cast<Value_t*>(unary_exp->expos.back()), out);
			return;
		}
		std::string unary_op;
		for (auto _op : unary_exp->ops.objects()) {
			std::string op = _parser.toString(_op);
			if (op == "~"sv && getLuaTarget(_op) < 503) {
				throw CompileError("bitwise operator is not available when not targeting Lua version 5.3 or higher"sv, _op);
			}
			unary_op.append(op == "not"sv ? op + ' ' : op);
		}
		str_list temp;
		for (auto _value : unary_exp->expos.objects()) {
			auto value = static_cast<Value_t*>(_value);
			transformValue(value, temp);
		}
		out.push_back(unary_op + join(temp, " ^ "sv));
	}

	void transformVariable(Variable_t* name, str_list& out) {
		out.push_back(_parser.toString(name));
	}

	void transformNum(Num_t* num, str_list& out) {
		std::string numStr = _parser.toString(num);
		numStr.erase(std::remove(numStr.begin(), numStr.end(), '_'), numStr.end());
		out.push_back(numStr);
	}

	void transformVarArg(VarArg_t* varArg, str_list& out) {
		if (_varArgs.empty() || !_varArgs.top().hasVar) {
			throw CompileError("cannot use '...' outside a vararg function near '...'"sv, varArg);
		}
		_varArgs.top().usedVar = true;
		out.push_back("..."s);
	}

	bool hasSpreadExp(const node_container& items) {
		for (auto item : items) {
			if (ast_is<SpreadExp_t>(item)) return true;
		}
		return false;
	}

	void transformSpreadTable(const node_container& values, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = values.front();
		switch (usage) {
			case ExpUsage::Closure:
				pushFunctionScope();
				pushAnonVarArg();
				pushScope();
				break;
			case ExpUsage::Assignment:
				pushScope();
				break;
			default:
				break;
		}
		str_list temp;
		std::string tableVar = getUnusedName("_tab_"sv);
		forceAddToScope(tableVar);
		auto it = values.begin();
		if (ast_is<SpreadExp_t>(*it)) {
			temp.push_back(indent() + "local "s + tableVar + " = { }"s + nll(x));
		} else {
			auto initialTab = x->new_ptr<TableLit_t>();
			while (it != values.end() && !ast_is<SpreadExp_t>(*it)) {
				initialTab->values.push_back(*it);
				++it;
			}
			transformTable(initialTab->values.objects(), temp);
			temp.back() = indent() + "local "s + tableVar + " = "s + temp.back() + nll(*it);
		}
		for (; it != values.end(); ++it) {
			auto item = *it;
			switch (item->getId()) {
				case id<SpreadExp_t>(): {
					auto spread = static_cast<SpreadExp_t*>(item);
					std::string indexVar = getUnusedName("_idx_"sv);
					std::string keyVar = getUnusedName("_key_"sv);
					std::string valueVar = getUnusedName("_value_"sv);
					auto objVar = singleVariableFrom(spread->exp, true);
					if (objVar.empty()) {
						objVar = getUnusedName("_obj_");
						auto assignment = toAst<ExpListAssign_t>(objVar + "=nil"s, item);
						auto assign = assignment->action.to<Assign_t>();
						assign->values.clear();
						assign->values.push_back(spread->exp);
						transformAssignment(assignment, temp);
					}
					forceAddToScope(indexVar);
					temp.push_back(indent() + "local "s + indexVar + " = 1"s + nll(item));
					_buf << "for "sv << keyVar << ',' << valueVar << " in pairs "sv << objVar
						 << "\n\tif "sv << indexVar << "=="sv << keyVar
						 << "\n\t\t"sv << tableVar << "[]="sv << valueVar
						 << "\n\t\t"sv << indexVar << "+=1"sv
						 << "\n\telse "sv << tableVar << '[' << keyVar << "]="sv << valueVar;
					auto forEach = toAst<ForEach_t>(clearBuf(), item);
					transformForEach(forEach, temp);
					break;
				}
				case id<VariablePair_t>():
				case id<VariablePairDef_t>(): {
					if (auto pair = ast_cast<VariablePairDef_t>(item)) {
						if (pair->defVal) {
							throw CompileError("invalid default value here"sv, pair->defVal);
						}
						item = pair->pair.get();
					}
					auto variablePair = static_cast<VariablePair_t*>(item);
					auto nameStr = _parser.toString(variablePair->name);
					auto assignment = toAst<ExpListAssign_t>(tableVar + '.' + nameStr + '=' + nameStr, item);
					transformAssignment(assignment, temp);
					break;
				}
				case id<NormalPair_t>():
				case id<NormalPairDef_t>(): {
					if (auto pair = ast_cast<NormalPairDef_t>(item)) {
						if (pair->defVal) {
							throw CompileError("invalid default value here"sv, pair->defVal);
						}
						item = pair->pair.get();
					}
					auto normalPair = static_cast<NormalPair_t*>(item);
					auto assignment = toAst<ExpListAssign_t>(tableVar + "=nil"s, item);
					auto chainValue = singleValueFrom(ast_to<Exp_t>(assignment->expList->exprs.front()))->item.to<ChainValue_t>();
					auto key = normalPair->key.get();
					switch (key->getId()) {
						case id<KeyName_t>(): {
							auto keyName = static_cast<KeyName_t*>(key);
							ast_ptr<false, ast_node> chainItem;
							if (auto name = keyName->name.as<Name_t>()) {
								auto dotItem = x->new_ptr<DotChainItem_t>();
								dotItem->name.set(name);
								chainItem = dotItem.get();
							} else {
								auto selfName = keyName->name.to<SelfItem_t>();
								auto callable = x->new_ptr<Callable_t>();
								callable->item.set(selfName);
								auto chainValue = x->new_ptr<ChainValue_t>();
								chainValue->items.push_back(callable);
								auto exp = newExp(chainValue, key);
								chainItem = exp.get();
							}
							chainValue->items.push_back(chainItem);
							break;
						}
						case id<Exp_t>():
							chainValue->items.push_back(key);
							break;
						case id<DoubleString_t>():
						case id<SingleString_t>():
						case id<LuaString_t>(): {
							auto strNode = x->new_ptr<String_t>();
							strNode->str.set(key);
							chainValue->items.push_back(strNode);
							break;
						}
						default: YUEE("AST node mismatch", key); break;
					}
					auto assign = assignment->action.to<Assign_t>();
					assign->values.clear();
					assign->values.push_back(normalPair->value);
					transformAssignment(assignment, temp);
					break;
				}
				case id<Exp_t>():
				case id<NormalDef_t>(): {
					auto current = item;
					if (auto pair = ast_cast<NormalDef_t>(item)) {
						if (pair->defVal) {
							throw CompileError("invalid default value here"sv, pair->defVal);
						}
						item = pair->item.get();
					}
					bool lastVarArg = false;
					BLOCK_START
					BREAK_IF(current != values.back());
					auto value = singleValueFrom(item);
					BREAK_IF(!value);
					auto simpleValue = value->item.as<SimpleValue_t>();
					BREAK_IF(!simpleValue);
					BREAK_IF(!simpleValue->value.is<VarArg_t>());
					auto indexVar = getUnusedName("_index_");
					_buf << "for "sv << indexVar << "=1,select '#',...\n\t"sv << tableVar << "[]= select "sv << indexVar << ",..."sv;
					transformFor(toAst<For_t>(clearBuf(), item), temp);
					lastVarArg = true;
					BLOCK_END
					if (!lastVarArg) {
						auto assignment = toAst<ExpListAssign_t>(tableVar + "[]=nil"s, item);
						auto assign = assignment->action.to<Assign_t>();
						assign->values.clear();
						assign->values.push_back(item);
						transformAssignment(assignment, temp);
					}
					break;
				}
				case id<TableBlockIndent_t>(): {
					auto tbIndent = static_cast<TableBlockIndent_t*>(item);
					auto tableBlock = item->new_ptr<TableBlock_t>();
					tableBlock->values.dup(tbIndent->values);
					auto assignment = toAst<ExpListAssign_t>(tableVar + "[]=nil"s, item);
					auto assign = assignment->action.to<Assign_t>();
					assign->values.clear();
					assign->values.push_back(tableBlock);
					transformAssignment(assignment, temp);
					break;
				}
				case id<TableBlock_t>(): {
					auto assignment = toAst<ExpListAssign_t>(tableVar + "[]=nil"s, item);
					auto assign = assignment->action.to<Assign_t>();
					assign->values.clear();
					assign->values.push_back(item);
					transformAssignment(assignment, temp);
					break;
				}
				case id<MetaVariablePair_t>():
				case id<MetaVariablePairDef_t>(): {
					if (auto pair = ast_cast<MetaVariablePairDef_t>(item)) {
						if (pair->defVal) {
							throw CompileError("invalid default value here"sv, pair->defVal);
						}
						item = pair->pair.get();
					}
					auto metaVarPair = static_cast<MetaVariablePair_t*>(item);
					auto nameStr = _parser.toString(metaVarPair->name);
					auto assignment = toAst<ExpListAssign_t>(tableVar + ".<"s + nameStr + ">="s + nameStr, item);
					transformAssignment(assignment, temp);
					break;
				}
				case id<MetaNormalPair_t>():
				case id<MetaNormalPairDef_t>(): {
					if (auto pair = ast_cast<MetaNormalPairDef_t>(item)) {
						if (pair->defVal) {
							throw CompileError("invalid default value here"sv, pair->defVal);
						}
						item = pair->pair.get();
					}
					auto metaNormalPair = static_cast<MetaNormalPair_t*>(item);
					auto assignment = toAst<ExpListAssign_t>(tableVar + "=nil"s, item);
					auto chainValue = singleValueFrom(ast_to<Exp_t>(assignment->expList->exprs.front()))->item.to<ChainValue_t>();
					auto key = metaNormalPair->key.get();
					switch (key->getId()) {
						case id<Name_t>(): {
							auto dotItem = x->new_ptr<DotChainItem_t>();
							dotItem->name.set(key);
							chainValue->items.push_back(dotItem);
							break;
						}
						case id<Exp_t>(): {
							auto mt = key->new_ptr<Metatable_t>();
							auto dot = mt->new_ptr<DotChainItem_t>();
							dot->name.set(mt);
							chainValue->items.push_back(dot);
							chainValue->items.push_back(key);
							break;
						}
						case id<String_t>():
							chainValue->items.push_back(key);
							break;
						default: YUEE("AST node mismatch", key); break;
					}
					auto assign = assignment->action.to<Assign_t>();
					assign->values.clear();
					assign->values.push_back(metaNormalPair->value);
					transformAssignment(assignment, temp);
					break;
				}
				default: YUEE("AST node mismatch", item); break;
			}
		}
		switch (usage) {
			case ExpUsage::Common:
				break;
			case ExpUsage::Closure: {
				out.push_back(join(temp));
				out.back().append(indent() + "return "s + tableVar + nlr(x));
				popScope();
				out.back().insert(0, anonFuncStart() + nll(x));
				out.back().append(indent() + anonFuncEnd());
				popAnonVarArg();
				popFunctionScope();
				break;
			}
			case ExpUsage::Assignment: {
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(tableVar, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				popScope();
				out.push_back(join(temp));
				out.back() = indent() + "do"s + nll(x) + out.back() + indent() + "end"s + nlr(x);
				break;
			}
			case ExpUsage::Return:
				out.push_back(join(temp));
				out.back().append(indent() + "return "s + tableVar + nlr(x));
				break;
			default:
				break;
		}
	}

	void transformTable(const node_container& values, str_list& out) {
		if (values.empty()) {
			out.push_back("{ }"s);
			return;
		}
		auto x = values.front();
		str_list temp;
		incIndentOffset();
		auto metatable = x->new_ptr<SimpleTable_t>();
		ast_sel<false, Exp_t, TableBlock_t> metatableItem;
		for (auto value : values) {
			auto item = value;
			switch (item->getId()) {
				case id<VariablePairDef_t>(): {
					auto pair = static_cast<VariablePairDef_t*>(item);
					if (pair->defVal) {
						throw CompileError("invalid default value"sv, pair->defVal);
					}
					item = pair->pair.get();
					break;
				}
				case id<NormalPairDef_t>(): {
					auto pair = static_cast<NormalPairDef_t*>(item);
					if (pair->defVal) {
						throw CompileError("invalid default value"sv, pair->defVal);
					}
					item = pair->pair.get();
					break;
				}
				case id<MetaVariablePairDef_t>(): {
					auto pair = static_cast<MetaVariablePairDef_t*>(item);
					if (pair->defVal) {
						throw CompileError("invalid default value"sv, pair->defVal);
					}
					item = pair->pair.get();
					break;
				}
				case id<MetaNormalPairDef_t>(): {
					auto pair = static_cast<MetaNormalPairDef_t*>(item);
					if (pair->defVal) {
						throw CompileError("invalid default value"sv, pair->defVal);
					}
					item = pair->pair.get();
					break;
				}
				case id<NormalDef_t>(): {
					auto pair = static_cast<NormalDef_t*>(item);
					if (pair->defVal) {
						throw CompileError("invalid default value"sv, pair->defVal);
					}
					item = pair->item.get();
					break;
				}
			}
			bool isMetamethod = false;
			switch (item->getId()) {
				case id<Exp_t>(): transformExp(static_cast<Exp_t*>(item), temp, ExpUsage::Closure); break;
				case id<VariablePair_t>(): transform_variable_pair(static_cast<VariablePair_t*>(item), temp); break;
				case id<NormalPair_t>(): transform_normal_pair(static_cast<NormalPair_t*>(item), temp, false); break;
				case id<TableBlockIndent_t>(): transformTableBlockIndent(static_cast<TableBlockIndent_t*>(item), temp); break;
				case id<TableBlock_t>(): transformTableBlock(static_cast<TableBlock_t*>(item), temp); break;
				case id<MetaVariablePair_t>(): {
					isMetamethod = true;
					auto mp = static_cast<MetaVariablePair_t*>(item);
					if (metatableItem) {
						throw CompileError("too many metatable declarations"sv, mp->name);
					}
					auto name = _parser.toString(mp->name);
					checkMetamethod(name, mp->name);
					_buf << "__"sv << name << ':' << name;
					auto newPair = toAst<NormalPair_t>(clearBuf(), item);
					metatable->pairs.push_back(newPair);
					break;
				}
				case id<MetaNormalPair_t>(): {
					isMetamethod = true;
					auto mp = static_cast<MetaNormalPair_t*>(item);
					auto newPair = item->new_ptr<NormalPair_t>();
					if (mp->key) {
						if (metatableItem) {
							throw CompileError("too many metatable declarations"sv, mp->key);
						}
						switch (mp->key->getId()) {
							case id<Name_t>(): {
								auto key = _parser.toString(mp->key);
								checkMetamethod(key, mp->key);
								_buf << "__"sv << key;
								auto newKey = toAst<KeyName_t>(clearBuf(), mp->key);
								newPair->key.set(newKey);
								break;
							}
							case id<Exp_t>():
								newPair->key.set(mp->key);
								break;
							case id<String_t>(): {
								auto str = static_cast<String_t*>(mp->key.get());
								newPair->key.set(newExp(str, mp));
								break;
							}
							default: YUEE("AST node mismatch", mp->key); break;
						}
						newPair->value.set(mp->value);
						metatable->pairs.push_back(newPair);
					} else {
						if (!metatable->pairs.empty()) {
							throw CompileError("too many metatable declarations"sv, mp->value);
						}
						metatableItem.set(mp->value);
					}
					break;
				}
				default: YUEE("AST node mismatch", item); break;
			}
			if (!isMetamethod) {
				temp.back() = indent() + (value == values.back() ? temp.back() : temp.back() + ',') + nll(value);
			}
		}
		if (metatable->pairs.empty() && !metatableItem) {
			out.push_back('{' + nll(x) + join(temp));
			decIndentOffset();
			out.back() += (indent() + '}');
		} else {
			auto tabStr = globalVar("setmetatable"sv, x);
			tabStr += '(';
			if (temp.empty()) {
				decIndentOffset();
				tabStr += "{ }"sv;
			} else {
				tabStr += ('{' + nll(x) + join(temp));
				decIndentOffset();
				tabStr += (indent() + '}');
			}
			tabStr += ", "sv;
			str_list tmp;
			if (!metatable->pairs.empty()) {
				transform_simple_table(metatable, tmp);
			} else
				switch (metatableItem->getId()) {
					case id<Exp_t>():
						transformExp(static_cast<Exp_t*>(metatableItem.get()), tmp, ExpUsage::Closure);
						break;
					case id<TableBlock_t>():
						transformTableBlock(static_cast<TableBlock_t*>(metatableItem.get()), tmp);
						break;
				}
			tabStr += tmp.back();
			tabStr += ')';
			out.push_back(tabStr);
		}
	}

	void transformTableLit(TableLit_t* table, str_list& out) {
		const auto& values = table->values.objects();
		if (hasSpreadExp(values)) {
			transformSpreadTable(values, out, ExpUsage::Closure);
		} else {
			transformTable(values, out);
		}
	}

	void transformCompCommon(Comprehension_t* comp, str_list& out) {
		str_list temp;
		auto x = comp;
		auto compInner = comp->forLoop.get();
		for (auto item : compInner->items.objects()) {
			switch (item->getId()) {
				case id<CompForEach_t>():
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case id<CompFor_t>():
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case id<Exp_t>():
					transformExp(static_cast<Exp_t*>(item), temp, ExpUsage::Closure);
					temp.back() = indent() + "if "s + temp.back() + " then"s + nll(item);
					pushScope();
					break;
				default: YUEE("AST node mismatch", item); break;
			}
		}
		if (auto stmt = comp->value.as<Statement_t>()) {
			transformStatement(stmt, temp);
		} else if (auto exp = comp->value.as<Exp_t>()) {
			auto expList = x->new_ptr<ExpList_t>();
			expList->exprs.push_back(exp);
			auto expListAssign = x->new_ptr<ExpListAssign_t>();
			expListAssign->expList.set(expList);
			auto statement = x->new_ptr<Statement_t>();
			statement->content.set(expListAssign);
			transformStatement(statement, temp);
		}
		auto value = std::move(temp.back());
		temp.pop_back();
		_buf << join(temp) << value;
		for (size_t i = 0; i < compInner->items.objects().size(); ++i) {
			popScope();
			_buf << indent() << "end"sv << nll(comp);
		}
		out.push_back(clearBuf());
	}

	void transformComprehension(Comprehension_t* comp, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = comp;
		switch (usage) {
			case ExpUsage::Closure:
				pushFunctionScope();
				pushAnonVarArg();
				pushScope();
				break;
			case ExpUsage::Assignment:
				pushScope();
				break;
			default:
				break;
		}
		str_list temp;
		std::string accumVar = getUnusedName("_accum_"sv);
		std::string lenVar = getUnusedName("_len_"sv);
		addToScope(accumVar);
		addToScope(lenVar);
		auto compInner = comp->forLoop.get();
		for (auto item : compInner->items.objects()) {
			switch (item->getId()) {
				case id<CompForEach_t>():
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case id<CompFor_t>():
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case id<Exp_t>():
					transformExp(static_cast<Exp_t*>(item), temp, ExpUsage::Closure);
					temp.back() = indent() + "if "s + temp.back() + " then"s + nll(item);
					pushScope();
					break;
				default: YUEE("AST node mismatch", item); break;
			}
		}
		{
			auto assignLeft = toAst<ExpList_t>(accumVar + '[' + lenVar + ']', x);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(comp->value);
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignLeft);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		}
		auto assignStr = std::move(temp.back());
		temp.pop_back();
		for (size_t i = 0; i < compInner->items.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << accumVar << " = { }"sv << nll(comp);
		_buf << indent() << "local "sv << lenVar << " = 1"sv << nll(comp);
		_buf << join(temp);
		_buf << assignStr;
		_buf << indent(int(temp.size())) << lenVar << " = "sv << lenVar << " + 1"sv << nll(comp);
		for (int ind = int(temp.size()) - 1; ind > -1; --ind) {
			_buf << indent(ind) << "end"sv << nll(comp);
		}
		switch (usage) {
			case ExpUsage::Common:
				break;
			case ExpUsage::Closure: {
				out.push_back(clearBuf());
				out.back().append(indent() + "return "s + accumVar + nlr(comp));
				popScope();
				out.back().insert(0, anonFuncStart() + nll(comp));
				out.back().append(indent() + anonFuncEnd());
				popAnonVarArg();
				popFunctionScope();
				break;
			}
			case ExpUsage::Assignment: {
				out.push_back(clearBuf());
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(accumVar, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				popScope();
				out.back() = indent() + "do"s + nll(comp) + out.back() + temp.back() + indent() + "end"s + nlr(comp);
				break;
			}
			case ExpUsage::Return:
				out.push_back(clearBuf());
				out.back().append(indent() + "return "s + accumVar + nlr(comp));
				break;
			default:
				break;
		}
	}

	bool transformForEachHead(AssignableNameList_t* nameList, ast_node* loopTarget, str_list& out, bool isStatement = false) {
		auto x = nameList;
		str_list temp;
		str_list vars;
		str_list varBefore, varAfter;
		bool extraScope = false;
		std::list<std::pair<ast_node*, ast_ptr<false, ast_node>>> destructPairs;
		for (auto _item : nameList->items.objects()) {
			auto item = static_cast<NameOrDestructure_t*>(_item)->item.get();
			switch (item->getId()) {
				case id<Variable_t>():
					transformVariable(static_cast<Variable_t*>(item), vars);
					varAfter.push_back(vars.back());
					break;
				case id<TableLit_t>(): {
					auto desVar = getUnusedName("_des_"sv);
					destructPairs.emplace_back(item, toAst<Exp_t>(desVar, x));
					vars.push_back(desVar);
					varAfter.push_back(desVar);
					break;
				}
				default: YUEE("AST node mismatch", item); break;
			}
		}
		switch (loopTarget->getId()) {
			case id<StarExp_t>(): {
				auto star_exp = static_cast<StarExp_t*>(loopTarget);
				auto listVar = singleVariableFrom(star_exp->value, true);
				if (!isLocal(listVar)) listVar.clear();
				auto indexVar = getUnusedName("_index_"sv);
				varAfter.push_back(indexVar);
				auto value = singleValueFrom(star_exp->value);
				if (!value) throw CompileError("invalid star syntax"sv, star_exp);
				bool endWithSlice = false;
				BLOCK_START
				auto chainValue = value->item.as<ChainValue_t>();
				BREAK_IF(!chainValue);
				auto chainList = chainValue->items.objects();
				auto slice = ast_cast<Slice_t>(chainList.back());
				BREAK_IF(!slice);
				endWithSlice = true;
				if (listVar.empty() && chainList.size() == 2) {
					if (auto var = chainList.front()->getByPath<Variable_t>()) {
						listVar = _parser.toString(var);
						if (!isLocal(listVar)) listVar.clear();
					}
				}
				chainList.pop_back();
				auto chain = x->new_ptr<ChainValue_t>();
				for (auto item : chainList) {
					chain->items.push_back(item);
				}
				std::string startValue("1"sv);
				if (auto exp = slice->startValue.as<Exp_t>()) {
					transformExp(exp, temp, ExpUsage::Closure);
					startValue = std::move(temp.back());
					temp.pop_back();
				}
				std::string stopValue;
				if (auto exp = slice->stopValue.as<Exp_t>()) {
					transformExp(exp, temp, ExpUsage::Closure);
					stopValue = std::move(temp.back());
					temp.pop_back();
				}
				std::string stepValue;
				if (auto exp = slice->stepValue.as<Exp_t>()) {
					transformExp(exp, temp, ExpUsage::Closure);
					stepValue = std::move(temp.back());
					temp.pop_back();
				}
				if (listVar.empty()) {
					std::string prefix;
					if (isStatement) {
						extraScope = true;
						prefix = indent() + "do"s + nll(x);
						pushScope();
					}
					listVar = getUnusedName("_list_"sv);
					varBefore.push_back(listVar);
					transformChainValue(chain, temp, ExpUsage::Closure);
					_buf << prefix << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
				}
				std::string maxVar;
				if (!stopValue.empty()) {
					std::string prefix;
					if (isStatement && !extraScope) {
						extraScope = true;
						prefix = indent() + "do"s + nll(x);
						pushScope();
					}
					maxVar = getUnusedName("_max_"sv);
					varBefore.push_back(maxVar);
					_buf << prefix << indent() << "local "sv << maxVar << " = "sv << stopValue << nll(nameList);
				}
				_buf << indent() << "for "sv << indexVar << " = "sv;
				_buf << startValue << ", "sv;
				if (stopValue.empty()) {
					_buf << "#"sv << listVar;
				} else {
					_buf << maxVar << " < 0 and #"sv << listVar << " + "sv << maxVar << " or "sv << maxVar;
				}
				if (!stepValue.empty()) {
					_buf << ", "sv << stepValue;
				}
				_buf << " do"sv << nlr(loopTarget);
				_buf << indent(1) << "local "sv << join(vars, ", "sv) << " = "sv << listVar << "["sv << indexVar << "]"sv << nll(nameList);
				out.push_back(clearBuf());
				BLOCK_END
				bool newListVal = false;
				if (listVar.empty()) {
					newListVal = true;
					listVar = getUnusedName("_list_"sv);
					varBefore.push_back(listVar);
				}
				if (!endWithSlice) {
					transformExp(star_exp->value, temp, ExpUsage::Closure);
					if (newListVal) _buf << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
					_buf << indent() << "for "sv << indexVar << " = 1, #"sv << listVar << " do"sv << nlr(loopTarget);
					_buf << indent(1) << "local "sv << join(vars, ", "sv) << " = "sv << listVar << "["sv << indexVar << "]"sv << nll(nameList);
					out.push_back(clearBuf());
				}
				break;
			}
			case id<Exp_t>():
				transformExp(static_cast<Exp_t*>(loopTarget), temp, ExpUsage::Closure);
				_buf << indent() << "for "sv << join(vars, ", "sv) << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			case id<ExpList_t>():
				transformExpList(static_cast<ExpList_t*>(loopTarget), temp);
				_buf << indent() << "for "sv << join(vars, ", "sv) << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			default: YUEE("AST node mismatch", loopTarget); break;
		}
		for (auto& var : varBefore) addToScope(var);
		pushScope();
		for (const auto& var : vars) forceAddToScope(var);
		for (const auto& var : varAfter) addToScope(var);
		if (!destructPairs.empty()) {
			temp.clear();
			for (auto& pair : destructPairs) {
				auto sValue = x->new_ptr<SimpleValue_t>();
				sValue->value.set(pair.first);
				auto exp = newExp(sValue, x);
				auto expList = x->new_ptr<ExpList_t>();
				expList->exprs.push_back(exp);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(pair.second);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(expList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
			out.back().append(join(temp));
		}
		return extraScope;
	}

	void transformCompForEach(CompForEach_t* comp, str_list& out) {
		transformForEachHead(comp->nameList, comp->loopValue, out);
	}

	void transformInvokeArgs(InvokeArgs_t* invokeArgs, str_list& out) {
		if (invokeArgs->args.size() > 1) {
			/* merge all the key-value pairs into one table
			 from arguments in the end */
			auto lastArg = invokeArgs->args.back();
			_ast_list* lastTable = nullptr;
			if (auto tableBlock = ast_cast<TableBlock_t>(lastArg)) {
				lastTable = &tableBlock->values;
			} else if (auto value = singleValueFrom(lastArg)) {
				if (auto simpleTable = ast_cast<SimpleTable_t>(value->item)) {
					lastTable = &simpleTable->pairs;
				}
			}
			if (lastTable) {
				ast_ptr<false, ast_node> ref(lastArg);
				invokeArgs->args.pop_back();
				while (!invokeArgs->args.empty()) {
					if (Value_t* value = singleValueFrom(invokeArgs->args.back())) {
						if (auto tb = value->item.as<SimpleTable_t>()) {
							const auto& ps = tb->pairs.objects();
							for (auto it = ps.rbegin(); it != ps.rend(); ++it) {
								lastTable->push_front(*it);
							}
							invokeArgs->args.pop_back();
							continue;
						}
					}
					break;
				}
				invokeArgs->args.push_back(lastArg);
			}
		}
		str_list temp;
		for (auto arg : invokeArgs->args.objects()) {
			switch (arg->getId()) {
				case id<Exp_t>(): transformExp(static_cast<Exp_t*>(arg), temp, ExpUsage::Closure); break;
				case id<TableBlock_t>(): transformTableBlock(static_cast<TableBlock_t*>(arg), temp); break;
				default: YUEE("AST node mismatch", arg); break;
			}
		}
		out.push_back('(' + join(temp, ", "sv) + ')');
	}

	void transformForHead(Variable_t* var, Exp_t* startVal, Exp_t* stopVal, ForStepValue_t* stepVal, str_list& out) {
		str_list temp;
		std::string varName = _parser.toString(var);
		transformExp(startVal, temp, ExpUsage::Closure);
		transformExp(stopVal, temp, ExpUsage::Closure);
		if (stepVal) {
			transformExp(stepVal->value, temp, ExpUsage::Closure);
		} else {
			temp.emplace_back();
		}
		auto it = temp.begin();
		const auto& start = *it;
		const auto& stop = *(++it);
		const auto& step = *(++it);
		_buf << indent() << "for "sv << varName << " = "sv << start << ", "sv << stop << (step.empty() ? Empty : ", "s + step) << " do"sv << nll(var);
		pushScope();
		forceAddToScope(varName);
		out.push_back(clearBuf());
	}

	void transformForHead(For_t* forNode, str_list& out) {
		transformForHead(forNode->varName, forNode->startValue, forNode->stopValue, forNode->stepValue, out);
	}

	void transform_plain_body(ast_node* body, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		switch (body->getId()) {
			case id<Block_t>():
				transformBlock(static_cast<Block_t*>(body), out, usage, assignList);
				break;
			case id<Statement_t>(): {
				auto newBlock = body->new_ptr<Block_t>();
				newBlock->statements.push_back(body);
				transformBlock(newBlock, out, usage, assignList);
				break;
			}
			default: YUEE("AST node mismatch", body); break;
		}
	}

	bool hasContinueStatement(ast_node* body) {
		return traversal::Stop == body->traverse([&](ast_node* node) {
			if (auto stmt = ast_cast<Statement_t>(node)) {
				if (stmt->content.is<BreakLoop_t>()) {
					return _parser.toString(stmt->content) == "continue"sv ? traversal::Stop : traversal::Return;
				} else if (auto expList = expListFrom(stmt)) {
					BLOCK_START
					auto value = singleValueFrom(expList);
					BREAK_IF(!value);
					auto simpleValue = value->item.as<SimpleValue_t>();
					BREAK_IF(!simpleValue);
					auto sVal = simpleValue->value.get();
					switch (sVal->getId()) {
						case id<With_t>(): {
							auto withNode = static_cast<With_t*>(sVal);
							if (hasContinueStatement(withNode->body)) {
								return traversal::Stop;
							}
							break;
						}
						case id<Do_t>(): {
							auto doNode = static_cast<Do_t*>(sVal);
							if (hasContinueStatement(doNode->body)) {
								return traversal::Stop;
							}
							break;
						}
						case id<If_t>(): {
							auto ifNode = static_cast<If_t*>(sVal);
							for (auto n : ifNode->nodes.objects()) {
								if (hasContinueStatement(n)) {
									return traversal::Stop;
								}
							}
							break;
						}
						case id<Switch_t>(): {
							auto switchNode = static_cast<Switch_t*>(sVal);
							for (auto branch : switchNode->branches.objects()) {
								if (hasContinueStatement(static_cast<SwitchCase_t*>(branch)->body)) {
									return traversal::Stop;
								}
							}
							if (switchNode->lastBranch) {
								if (hasContinueStatement(switchNode->lastBranch)) {
									return traversal::Stop;
								}
							}
							break;
						}
					}
					BLOCK_END
				}
			} else {
				switch (node->getId()) {
					case id<Body_t>():
					case id<Block_t>():
						return traversal::Continue;
				}
			}
			return traversal::Return;
		});
	}

	void addDoToLastLineReturn(ast_node* body) {
		if (auto block = ast_cast<Block_t>(body); body && !block->statements.empty()) {
			auto last = static_cast<Statement_t*>(block->statements.back());
			if (last->content.is<Return_t>()) {
				auto doNode = last->new_ptr<Do_t>();
				auto newBody = last->new_ptr<Body_t>();
				auto newStmt = last->new_ptr<Statement_t>();
				newStmt->content.set(last->content);
				newBody->content.set(newStmt);
				doNode->body.set(newBody);
				auto simpleValue = last->new_ptr<SimpleValue_t>();
				simpleValue->value.set(doNode);
				auto expList = last->new_ptr<ExpList_t>();
				expList->exprs.push_back(newExp(simpleValue, last));
				auto expListAssign = last->new_ptr<ExpListAssign_t>();
				expListAssign->expList.set(expList);
				last->content.set(expListAssign);
			}
		}
	}

	void transformLoopBody(ast_node* body, str_list& out, const std::string& appendContent, ExpUsage usage, ExpList_t* assignList = nullptr) {
		str_list temp;
		bool extraDo = false;
		bool withContinue = hasContinueStatement(body);
		int target = getLuaTarget(body);
		std::string extraLabel;
		if (withContinue) {
			if (target < 502) {
				if (auto block = ast_cast<Block_t>(body)) {
					if (!block->statements.empty()) {
						auto stmt = static_cast<Statement_t*>(block->statements.back());
						if (auto breakLoop = ast_cast<BreakLoop_t>(stmt->content)) {
							extraDo = _parser.toString(breakLoop) == "break"sv;
						}
					}
				}
				auto continueVar = getUnusedName("_continue_"sv);
				addToScope(continueVar);
				_continueVars.push({continueVar, nullptr});
				_buf << indent() << "local "sv << continueVar << " = false"sv << nll(body);
				_buf << indent() << "repeat"sv << nll(body);
				pushScope();
				if (extraDo) {
					_buf << indent() << "do"sv << nll(body);
					pushScope();
				}
				temp.push_back(clearBuf());
			} else {
				auto continueLabel = getUnusedLabel("_continue_"sv);
				_continueVars.push({continueLabel, nullptr});
				transformLabel(toAst<Label_t>("::"s + _continueVars.top().var + "::"s, body), temp);
				extraLabel = temp.back();
				temp.pop_back();
			}
			addDoToLastLineReturn(body);
		}
		_enableBreakLoop.push(true);
		transform_plain_body(body, temp, usage, assignList);
		_enableBreakLoop.pop();
		if (withContinue) {
			if (target < 502) {
				if (extraDo) {
					popScope();
					_buf << indent() << "end"sv << nll(body);
				}
				if (!appendContent.empty()) {
					_buf << indent() << appendContent;
				}
				_buf << indent() << _continueVars.top().var << " = true"sv << nll(body);
				popScope();
				_buf << indent() << "until true"sv << nlr(body);
				_buf << indent() << "if not "sv << _continueVars.top().var << " then"sv << nlr(body);
				_buf << indent(1) << "break"sv << nlr(body);
				_buf << indent() << "end"sv << nlr(body);
				temp.push_back(clearBuf());
				_continueVars.pop();
			} else {
				if (!appendContent.empty()) {
					temp.push_back(indent() + appendContent);
				}
				temp.push_back(extraLabel);
				_continueVars.pop();
			}
		} else if (!appendContent.empty()) {
			temp.back().append(indent() + appendContent);
		}
		out.push_back(join(temp));
	}

	std::string transformRepeatBody(Repeat_t* repeatNode, str_list& out) {
		str_list temp;
		bool extraDo = false;
		auto body = repeatNode->body->content.get();
		bool withContinue = hasContinueStatement(body);
		std::string conditionVar;
		std::string extraLabel;
		ast_ptr<false, ExpListAssign_t> condAssign;
		int target = getLuaTarget(repeatNode);
		if (withContinue) {
			if (target < 502) {
				if (auto block = ast_cast<Block_t>(body)) {
					if (!block->statements.empty()) {
						auto stmt = static_cast<Statement_t*>(block->statements.back());
						if (auto breakLoop = ast_cast<BreakLoop_t>(stmt->content)) {
							extraDo = _parser.toString(breakLoop) == "break"sv;
						}
					}
				}
				conditionVar = getUnusedName("_cond_");
				forceAddToScope(conditionVar);
				auto continueVar = getUnusedName("_continue_"sv);
				forceAddToScope(continueVar);
				{
					auto assignment = toAst<ExpListAssign_t>(conditionVar + "=nil"s, repeatNode->condition);
					auto assign = assignment->action.to<Assign_t>();
					assign->values.clear();
					assign->values.push_back(repeatNode->condition);
					_continueVars.push({continueVar, assignment.get()});
				}
				_buf << indent() << "local "sv << conditionVar << " = false"sv << nll(body);
				_buf << indent() << "local "sv << continueVar << " = false"sv << nll(body);
				_buf << indent() << "repeat"sv << nll(body);
				pushScope();
				if (extraDo) {
					_buf << indent() << "do"sv << nll(body);
					pushScope();
				}
				temp.push_back(clearBuf());
			} else {
				auto continueLabel = getUnusedLabel("_continue_"sv);
				_continueVars.push({continueLabel, nullptr});
				transformLabel(toAst<Label_t>("::"s + _continueVars.top().var + "::"s, body), temp);
				extraLabel = temp.back();
				temp.pop_back();
			}
			addDoToLastLineReturn(body);
		}
		_enableBreakLoop.push(true);
		transform_plain_body(body, temp, ExpUsage::Common);
		_enableBreakLoop.pop();
		if (withContinue) {
			if (target < 502) {
				transformAssignment(_continueVars.top().condAssign, temp);
				if (extraDo) {
					popScope();
					_buf << indent() << "end"sv << nll(body);
				}
				_buf << indent() << _continueVars.top().var << " = true"sv << nll(body);
				popScope();
				_buf << indent() << "until true"sv << nlr(body);
				_buf << indent() << "if not "sv << _continueVars.top().var << " then"sv << nlr(body);
				_buf << indent(1) << "break"sv << nlr(body);
				_buf << indent() << "end"sv << nlr(body);
				temp.push_back(clearBuf());
				_continueVars.pop();
			} else {
				temp.push_back(extraLabel);
				_continueVars.pop();
			}
		}
		out.push_back(join(temp));
		return conditionVar;
	}

	void transformFor(For_t* forNode, str_list& out) {
		str_list temp;
		transformForHead(forNode, temp);
		transformLoopBody(forNode->body, temp, Empty, ExpUsage::Common);
		popScope();
		out.push_back(join(temp) + indent() + "end"s + nlr(forNode));
	}

	std::string transformForInner(For_t* forNode, str_list& out) {
		auto x = forNode;
		std::string accum = getUnusedName("_accum_"sv);
		addToScope(accum);
		std::string len = getUnusedName("_len_"sv);
		addToScope(len);
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forNode);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forNode);
		out.push_back(clearBuf());
		transformForHead(forNode, out);
		auto expList = toAst<ExpList_t>(accum + '[' + len + ']', x);
		auto lenLine = len + " = "s + len + " + 1"s + nlr(forNode->body);
		transformLoopBody(forNode->body, out, lenLine, ExpUsage::Assignment, expList);
		popScope();
		out.push_back(indent() + "end"s + nlr(forNode));
		return accum;
	}

	void transformForClosure(For_t* forNode, str_list& out) {
		str_list temp;
		pushFunctionScope();
		pushAnonVarArg();
		std::string& funcStart = temp.emplace_back();
		pushScope();
		auto accum = transformForInner(forNode, temp);
		temp.push_back(indent() + "return "s + accum + nlr(forNode));
		popScope();
		funcStart = anonFuncStart() + nll(forNode);
		temp.push_back(indent() + anonFuncEnd());
		popAnonVarArg();
		popFunctionScope();
		out.push_back(join(temp));
	}

	void transformForInPlace(For_t* forNode, str_list& out, ExpList_t* assignExpList = nullptr) {
		auto x = forNode;
		str_list temp;
		if (assignExpList) {
			_buf << indent() << "do"sv << nll(forNode);
			pushScope();
			auto accum = transformForInner(forNode, temp);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accum, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignExpList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
			popScope();
			temp.push_back(indent() + "end"s + nlr(forNode));
		} else {
			auto accum = transformForInner(forNode, temp);
			auto returnNode = x->new_ptr<Return_t>();
			auto expListLow = toAst<ExpListLow_t>(accum, x);
			returnNode->valueList.set(expListLow);
			transformReturn(returnNode, temp);
		}
		out.push_back(join(temp));
	}

	void checkOperatorAvailable(const std::string& op, ast_node* node) {
		if (op == "&"sv || op == "~"sv || op == "|"sv || op == ">>"sv || op == "<<"sv) {
			if (getLuaTarget(node) < 503) {
				throw CompileError("bitwise operator is not available when not targeting Lua version 5.3 or higher"sv, node);
			}
		} else if (op == "//"sv) {
			if (getLuaTarget(node) < 503) {
				throw CompileError("floor division is not available when not targeting Lua version 5.3 or higher"sv, node);
			}
		}
	}

	void transformBinaryOperator(BinaryOperator_t* node, str_list& out) {
		auto op = _parser.toString(node);
		checkOperatorAvailable(op, node);
		out.push_back(op == "!="sv ? "~="s : op);
	}

	void transformForEach(ForEach_t* forEach, str_list& out) {
		str_list temp;
		bool extraScope = transformForEachHead(forEach->nameList, forEach->loopValue, temp, true);
		transformLoopBody(forEach->body, temp, Empty, ExpUsage::Common);
		popScope();
		out.push_back(temp.front() + temp.back() + indent() + "end"s + nlr(forEach));
		if (extraScope) {
			popScope();
			out.back().append(indent() + "end"s + nlr(forEach));
		}
	}

	std::string transformForEachInner(ForEach_t* forEach, str_list& out) {
		auto x = forEach;
		std::string accum = getUnusedName("_accum_"sv);
		addToScope(accum);
		std::string len = getUnusedName("_len_"sv);
		addToScope(len);
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forEach);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forEach);
		out.push_back(clearBuf());
		transformForEachHead(forEach->nameList, forEach->loopValue, out);
		auto expList = toAst<ExpList_t>(accum + '[' + len + ']', x);
		auto lenLine = len + " = "s + len + " + 1"s + nlr(forEach->body);
		transformLoopBody(forEach->body, out, lenLine, ExpUsage::Assignment, expList);
		popScope();
		out.push_back(indent() + "end"s + nlr(forEach));
		return accum;
	}

	void transformForEachClosure(ForEach_t* forEach, str_list& out) {
		str_list temp;
		pushFunctionScope();
		pushAnonVarArg();
		std::string& funcStart = temp.emplace_back();
		pushScope();
		auto accum = transformForEachInner(forEach, temp);
		temp.push_back(indent() + "return "s + accum + nlr(forEach));
		popScope();
		funcStart = anonFuncStart() + nll(forEach);
		temp.push_back(indent() + anonFuncEnd());
		popAnonVarArg();
		popFunctionScope();
		out.push_back(join(temp));
	}

	void transformForEachInPlace(ForEach_t* forEach, str_list& out, ExpList_t* assignExpList = nullptr) {
		auto x = forEach;
		str_list temp;
		if (assignExpList) {
			_buf << indent() << "do"sv << nll(forEach);
			pushScope();
			auto accum = transformForEachInner(forEach, temp);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accum, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignExpList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
			popScope();
			temp.push_back(indent() + "end"s + nlr(forEach));
		} else {
			auto accum = transformForEachInner(forEach, temp);
			auto returnNode = x->new_ptr<Return_t>();
			auto expListLow = toAst<ExpListLow_t>(accum, x);
			returnNode->valueList.set(expListLow);
			transformReturn(returnNode, temp);
		}
		out.push_back(join(temp));
	}

	void transform_variable_pair(VariablePair_t* pair, str_list& out) {
		auto name = _parser.toString(pair->name);
		if (_config.lintGlobalVariable && !isLocal(name)) {
			if (_globals.find(name) == _globals.end()) {
				_globals[name] = {pair->name->m_begin.m_line, pair->name->m_begin.m_col};
			}
		}
		out.push_back(name + " = "s + name);
	}

	void transform_normal_pair(NormalPair_t* pair, str_list& out, bool assignClass) {
		auto key = pair->key.get();
		str_list temp;
		switch (key->getId()) {
			case id<KeyName_t>(): {
				auto keyName = static_cast<KeyName_t*>(key);
				transformKeyName(keyName, temp);
				if (keyName->name.is<SelfItem_t>() && !assignClass) {
					temp.back() = '[' + temp.back() + ']';
				}
				break;
			}
			case id<Exp_t>():
				transformExp(static_cast<Exp_t*>(key), temp, ExpUsage::Closure);
				temp.back() = (temp.back().front() == '[' ? "[ "s : "["s) + temp.back() + ']';
				break;
			case id<String_t>(): {
				auto strNode = static_cast<String_t*>(key);
				auto str = strNode->str.get();
				switch (str->getId()) {
					case id<DoubleString_t>():
						transformDoubleString(static_cast<DoubleString_t*>(str), temp);
						temp.back() = '[' + temp.back() + ']';
						break;
					case id<SingleString_t>():
						transformSingleString(static_cast<SingleString_t*>(str), temp);
						temp.back() = '[' + temp.back() + ']';
						break;
					case id<LuaString_t>():
						transformLuaString(static_cast<LuaString_t*>(str), temp);
						temp.back() = "[ "s + temp.back() + ']';
						break;
					default: YUEE("AST node mismatch", str); break;
				}
				break;
			}
			default: YUEE("AST node mismatch", key); break;
		}
		auto value = pair->value.get();
		switch (value->getId()) {
			case id<Exp_t>(): transformExp(static_cast<Exp_t*>(value), temp, ExpUsage::Closure); break;
			case id<TableBlock_t>(): transformTableBlock(static_cast<TableBlock_t*>(value), temp); break;
			default: YUEE("AST node mismatch", value); break;
		}
		out.push_back(temp.front() + " = "s + temp.back());
	}

	void transformKeyName(KeyName_t* keyName, str_list& out) {
		auto name = keyName->name.get();
		switch (name->getId()) {
			case id<SelfItem_t>():
				transformSelfName(static_cast<SelfItem_t*>(name), out);
				break;
			case id<Name_t>(): {
				auto nameStr = _parser.toString(name);
				if (LuaKeywords.find(nameStr) != LuaKeywords.end()) {
					out.push_back("[\""s + nameStr + "\"]"s);
				} else {
					out.push_back(nameStr);
				}
				break;
			}
			default: YUEE("AST node mismatch", name); break;
		}
	}

	void transformLuaString(LuaString_t* luaString, str_list& out) {
		auto content = _parser.toString(luaString->content);
		Utils::replace(content, "\r\n"sv, "\n");
		out.push_back(_parser.toString(luaString->open) + content + _parser.toString(luaString->close));
	}

	void transformSingleString(SingleString_t* singleString, str_list& out) {
		auto str = _parser.toString(singleString);
		Utils::replace(str, "\r\n"sv, "\n");
		Utils::replace(str, "\n"sv, "\\n"sv);
		out.push_back(str);
	}

	void transformDoubleString(DoubleString_t* doubleString, str_list& out) {
		str_list temp;
		for (auto _seg : doubleString->segments.objects()) {
			auto seg = static_cast<DoubleStringContent_t*>(_seg);
			auto content = seg->content.get();
			switch (content->getId()) {
				case id<DoubleStringInner_t>(): {
					auto str = _parser.toString(content);
					Utils::replace(str, "\r\n"sv, "\n");
					Utils::replace(str, "\n"sv, "\\n"sv);
					temp.push_back('\"' + str + '\"');
					break;
				}
				case id<Exp_t>(): {
					transformExp(static_cast<Exp_t*>(content), temp, ExpUsage::Closure);
					temp.back() = globalVar("tostring"sv, content) + '(' + temp.back() + ')';
					break;
				}
				default: YUEE("AST node mismatch", content); break;
			}
		}
		out.push_back(temp.empty() ? "\"\""s : join(temp, " .. "sv));
	}

	void transformString(String_t* string, str_list& out) {
		auto str = string->str.get();
		switch (str->getId()) {
			case id<SingleString_t>(): transformSingleString(static_cast<SingleString_t*>(str), out); break;
			case id<DoubleString_t>(): transformDoubleString(static_cast<DoubleString_t*>(str), out); break;
			case id<LuaString_t>(): transformLuaString(static_cast<LuaString_t*>(str), out); break;
			default: YUEE("AST node mismatch", str); break;
		}
	}

	std::pair<std::string, bool> defineClassVariable(Assignable_t* assignable) {
		if (auto variable = assignable->item.as<Variable_t>()) {
			auto name = _parser.toString(variable);
			if (addToScope(name)) {
				return {name, true};
			} else {
				return {name, false};
			}
		}
		return {Empty, false};
	}

	void transformClassDeclClosure(ClassDecl_t* classDecl, str_list& out) {
		str_list temp;
		pushFunctionScope();
		pushAnonVarArg();
		std::string& funcStart = temp.emplace_back();
		pushScope();
		transformClassDecl(classDecl, temp, ExpUsage::Return);
		popScope();
		funcStart = anonFuncStart() + nll(classDecl);
		temp.push_back(indent() + anonFuncEnd());
		popAnonVarArg();
		popFunctionScope();
		out.push_back(join(temp));
	}

	void transformClassDecl(ClassDecl_t* classDecl, str_list& out, ExpUsage usage, ExpList_t* expList = nullptr) {
		str_list temp;
		auto x = classDecl;
		auto body = classDecl->body.get();
		auto assignable = classDecl->name.get();
		auto extend = classDecl->extend.get();
		std::string className;
		std::string assignItem;
		if (assignable) {
			if (!isAssignable(assignable)) {
				throw CompileError("left hand expression is not assignable"sv, assignable);
			}
			bool newDefined = false;
			std::tie(className, newDefined) = defineClassVariable(assignable);
			if (newDefined) {
				temp.push_back(indent() + "local "s + className + nll(classDecl));
			}
			if (className.empty()) {
				if (auto chain = ast_cast<AssignableChain_t>(assignable->item)) {
					if (auto dotChain = ast_cast<DotChainItem_t>(chain->items.back())) {
						className = '\"' + _parser.toString(dotChain->name) + '\"';
					} else if (auto index = ast_cast<Exp_t>(chain->items.back())) {
						if (auto name = index->getByPath<UnaryExp_t, Value_t, String_t>()) {
							transformString(name, temp);
							className = std::move(temp.back());
							temp.pop_back();
						}
					}
				}
			} else {
				className = '\"' + className + '\"';
			}
			pushScope();
			transformAssignable(assignable, temp);
			popScope();
			assignItem = std::move(temp.back());
			temp.pop_back();
		} else if (expList) {
			auto name = singleVariableFrom(expList, true);
			if (!name.empty()) {
				className = '\"' + name + '\"';
			}
		}
		temp.push_back(indent() + "do"s + nll(classDecl));
		pushScope();
		auto classVar = getUnusedName("_class_"sv);
		addToScope(classVar);
		temp.push_back(indent() + "local "s + classVar + nll(classDecl));
		if (body) {
			str_list varDefs;
			for (auto item : body->contents.objects()) {
				if (auto statement = ast_cast<Statement_t>(item)) {
					ClassDecl_t* clsDecl = nullptr;
					if (auto assignment = assignmentFrom(statement)) {
						auto names = transformAssignDefs(assignment->expList.get(), DefOp::Mark);
						varDefs.insert(varDefs.end(), names.begin(), names.end());
						auto info = extractDestructureInfo(assignment, true, false);
						if (!info.destructures.empty()) {
							for (const auto& destruct : info.destructures)
								for (const auto& item : destruct.items)
									if (!item.targetVar.empty() && addToScope(item.targetVar))
										varDefs.push_back(item.targetVar);
						}
						BLOCK_START
						auto assign = assignment->action.as<Assign_t>();
						BREAK_IF(!assign);
						BREAK_IF(assign->values.objects().size() != 1);
						auto exp = ast_cast<Exp_t>(assign->values.objects().front());
						BREAK_IF(!exp);
						auto value = singleValueFrom(exp);
						BREAK_IF(!value);
						clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
						BLOCK_END
					} else if (auto expList = expListFrom(statement)) {
						auto value = singleValueFrom(expList);
						clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
					}
					if (clsDecl) {
						std::string clsName;
						bool newDefined = false;
						std::tie(clsName, newDefined) = defineClassVariable(clsDecl->name);
						if (newDefined) varDefs.push_back(clsName);
					}
				}
			}
			if (!varDefs.empty()) {
				temp.push_back(indent() + "local "s + join(varDefs, ", "sv) + nll(body));
			}
		}
		std::string parent, parentVar;
		if (extend) {
			parentVar = getUnusedName("_parent_"sv);
			addToScope(parentVar);
			transformExp(extend, temp, ExpUsage::Closure);
			parent = std::move(temp.back());
			temp.pop_back();
			temp.push_back(indent() + "local "s + parentVar + " = "s + parent + nll(classDecl));
		}
		auto baseVar = getUnusedName("_base_"sv);
		addToScope(baseVar);
		temp.push_back(indent() + "local "s + baseVar + " = "s);
		str_list builtins;
		str_list commons;
		str_list statements;
		if (body) {
			std::list<ClassMember> members;
			for (auto content : classDecl->body->contents.objects()) {
				switch (content->getId()) {
					case id<ClassMemberList_t>(): {
						size_t inc = transform_class_member_list(static_cast<ClassMemberList_t*>(content), members, classVar);
						auto it = members.end();
						for (size_t i = 0; i < inc; ++i, --it)
							;
						for (; it != members.end(); ++it) {
							auto& member = *it;
							if (member.type == MemType::Property) {
								statements.push_back(indent() + member.item + nll(content));
							} else {
								member.item = indent(1) + member.item;
							}
						}
						break;
					}
					case id<Statement_t>():
						transformStatement(static_cast<Statement_t*>(content), statements);
						break;
					default: YUEE("AST node mismatch", content); break;
				}
			}
			for (auto& member : members) {
				switch (member.type) {
					case MemType::Common:
						commons.push_back((commons.empty() ? Empty : ',' + nll(member.node)) + member.item);
						break;
					case MemType::Builtin:
						builtins.push_back((builtins.empty() ? Empty : ',' + nll(member.node)) + member.item);
						break;
					default: break;
				}
			}
			if (!commons.empty()) {
				temp.back() += '{' + nll(body);
				temp.push_back(join(commons) + nll(body));
				temp.push_back(indent() + '}' + nll(body));
			} else {
				temp.back() += "{ }"s + nll(body);
			}
		} else {
			temp.back() += "{ }"s + nll(classDecl);
		}
		if (classDecl->mixes) {
			auto item = getUnusedName("_item_"sv);
			auto cls = getUnusedName("_cls_"sv);
			auto mixin = getUnusedName("_mixin_"sv);
			auto key = getUnusedName("_key_"sv);
			auto val = getUnusedName("_val_"sv);
			auto mixins = _parser.toString(classDecl->mixes);
			_buf << "for "sv << item << " in *{"sv << mixins << "}\n"sv;
			_buf << '\t' << cls << ',' << mixin << '=' << item << ".__base?,"sv << item << ".__base or "sv << item << '\n';
			_buf << "\tfor "sv << key << ',' << val << " in pairs "sv << mixin << '\n';
			_buf << "\t\t"sv << baseVar << '[' << key << "]="sv << val << " if "sv << baseVar << '[' << key << "]==nil and (not "sv << cls << " or not "sv << key << "\\match \"^__\")"sv;
			transformBlock(toAst<Block_t>(clearBuf(), x), temp, ExpUsage::Common);
		}
		if (!parentVar.empty()) {
			auto key = getUnusedName("_key_"sv);
			auto val = getUnusedName("_val_"sv);
			_buf << "for "sv << key << ',' << val << " in pairs "sv << parentVar << ".__base"sv << '\n'
				 << '\t' << baseVar << '[' << key << "]="sv << val << " if "sv << baseVar << '[' << key << "]==nil and "sv << key << "\\match(\"^__\") and not ("sv << key << "==\"__index\" and "sv << val << "=="sv << parentVar << ".__base)"sv;
			transformBlock(toAst<Block_t>(clearBuf(), x), temp, ExpUsage::Common);
		}
		transformAssignment(toAst<ExpListAssign_t>(baseVar + ".__index ?\?= "s + baseVar, classDecl), temp);
		str_list tmp;
		if (usage == ExpUsage::Assignment) {
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(classVar, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			transformAssignment(assignment, tmp);
		}
		if (extend) {
			_buf << indent() << "setmetatable("sv << baseVar << ", "sv << parentVar << ".__base)"sv << nll(classDecl);
		}
		_buf << indent() << classVar << " = "sv << globalVar("setmetatable"sv, classDecl) << "({"sv << nll(classDecl);
		if (!builtins.empty()) {
			_buf << join(builtins) << ',' << nll(classDecl);
		} else {
			if (extend) {
				_buf << indent(1) << "__init = function(self, ...)"sv << nll(classDecl);
				_buf << indent(2) << "return "sv << classVar << ".__parent.__init(self, ...)"sv << nll(classDecl);
				_buf << indent(1) << "end,"sv << nll(classDecl);
			} else {
				_buf << indent(1) << "__init = function() end,"sv << nll(classDecl);
			}
		}
		_buf << indent(1) << "__base = "sv << baseVar;
		if (!className.empty()) {
			_buf << ","sv << nll(classDecl);
			_buf << indent(1) << "__name = "sv << className;
		}
		if (extend) {
			_buf << ","sv << nll(classDecl);
			_buf << indent(1) << "__parent = "sv << parentVar;
		}
		_buf << nll(classDecl);
		_buf << indent() << "}, {"sv << nll(classDecl);
		if (extend) {
			_buf << indent(1) << "__index = function(cls, name)"sv << nll(classDecl);
			_buf << indent(2) << "local val = rawget("sv << baseVar << ", name)"sv << nll(classDecl);
			_buf << indent(2) << "if val == nil then"sv << nll(classDecl);
			_buf << indent(3) << "local parent = rawget(cls, \"__parent\")"sv << nll(classDecl);
			_buf << indent(3) << "if parent then"sv << nll(classDecl);
			_buf << indent(4) << "return parent[name]"sv << nll(classDecl);
			_buf << indent(3) << "end"sv << nll(classDecl);
			_buf << indent(2) << "else"sv << nll(classDecl);
			_buf << indent(3) << "return val"sv << nll(classDecl);
			_buf << indent(2) << "end"sv << nll(classDecl);
			_buf << indent(1) << "end,"sv << nll(classDecl);
		} else {
			_buf << indent(1) << "__index = "sv << baseVar << ","sv << nll(classDecl);
		}
		_buf << indent(1) << "__call = function(cls, ...)"sv << nll(classDecl);
		pushScope();
		auto selfVar = getUnusedName("_self_"sv);
		addToScope(selfVar);
		_buf << indent(1) << "local "sv << selfVar << " = setmetatable({ }, "sv << baseVar << ")"sv << nll(classDecl);
		_buf << indent(1) << "cls.__init("sv << selfVar << ", ...)"sv << nll(classDecl);
		_buf << indent(1) << "return "sv << selfVar << nll(classDecl);
		popScope();
		_buf << indent(1) << "end"sv << nll(classDecl);
		_buf << indent() << "})"sv << nll(classDecl);
		_buf << indent() << baseVar << ".__class = "sv << classVar << nll(classDecl);
		if (!statements.empty()) {
			_buf << indent() << "local self = "sv << classVar << ';' << nll(classDecl);
		}
		_buf << join(statements);
		if (extend) {
			_buf << indent() << "if "sv << parentVar << ".__inherited then"sv << nll(classDecl);
			_buf << indent(1) << parentVar << ".__inherited("sv << parentVar << ", "sv << classVar << ")"sv << nll(classDecl);
			_buf << indent() << "end"sv << nll(classDecl);
		}
		if (!assignItem.empty()) {
			_buf << indent() << assignItem << " = "sv << classVar << nll(classDecl);
		}
		switch (usage) {
			case ExpUsage::Return: {
				_buf << indent() << "return "sv << classVar << nlr(classDecl);
				break;
			}
			case ExpUsage::Assignment: {
				_buf << tmp.back();
				break;
			}
			default: break;
		}
		temp.push_back(clearBuf());
		popScope();
		temp.push_back(indent() + "end"s + nlr(classDecl));
		out.push_back(join(temp));
	}

	size_t transform_class_member_list(ClassMemberList_t* class_member_list, std::list<ClassMember>& out, const std::string& classVar) {
		str_list temp;
		size_t count = 0;
		for (auto keyValue : class_member_list->values.objects()) {
			MemType type = MemType::Common;
			ast_ptr<false, ast_node> ref;
			switch (keyValue->getId()) {
				case id<MetaVariablePair_t>(): {
					auto mtPair = static_cast<MetaVariablePair_t*>(keyValue);
					auto nameStr = _parser.toString(mtPair->name);
					checkMetamethod(nameStr, mtPair->name);
					ref.set(toAst<NormalPair_t>("__"s + nameStr + ':' + nameStr, keyValue));
					keyValue = ref.get();
					break;
				}
				case id<MetaNormalPair_t>(): {
					auto mtPair = static_cast<MetaNormalPair_t*>(keyValue);
					auto normal_pair = keyValue->new_ptr<NormalPair_t>();
					if (auto name = mtPair->key.as<Name_t>()) {
						auto nameStr = _parser.toString(name);
						checkMetamethod(nameStr, name);
						normal_pair->key.set(toAst<KeyName_t>("__"s + nameStr, keyValue));
					} else if (auto str = mtPair->key.as<String_t>()) {
						normal_pair->key.set(newExp(str, str));
					} else {
						normal_pair->key.set(mtPair->key);
					}
					normal_pair->value.set(mtPair->value);
					ref.set(normal_pair);
					keyValue = ref.get();
					break;
				}
			}
			BLOCK_START
			auto normal_pair = ast_cast<NormalPair_t>(keyValue);
			BREAK_IF(!normal_pair);
			auto keyName = normal_pair->key.as<KeyName_t>();
			BREAK_IF(!keyName);
			std::string newSuperCall;
			auto selfItem = keyName->name.as<SelfItem_t>();
			if (selfItem) {
				type = MemType::Property;
				auto name = ast_cast<SelfName_t>(selfItem->name);
				if (!name) throw CompileError("invalid class poperty name"sv, selfItem->name);
				newSuperCall = classVar + ".__parent."s + _parser.toString(name->name);
			} else {
				auto x = keyName;
				auto nameNode = keyName->name.as<Name_t>();
				if (!nameNode) break;
				auto name = _parser.toString(nameNode);
				if (name == "new"sv) {
					type = MemType::Builtin;
					keyName->name.set(toAst<Name_t>("__init"sv, x));
					newSuperCall = classVar + ".__parent.__init"s;
				} else {
					newSuperCall = classVar + ".__parent.__base."s + name;
				}
			}
			normal_pair->value->traverse([&](ast_node* node) {
				if (node->getId() == id<ClassDecl_t>()) return traversal::Return;
				if (auto chainValue = ast_cast<ChainValue_t>(node)) {
					if (auto callable = ast_cast<Callable_t>(chainValue->items.front())) {
						auto var = callable->item.get();
						if (_parser.toString(var) == "super"sv) {
							auto insertSelfToArguments = [&](ast_node* item) {
								auto x = item;
								switch (item->getId()) {
									case id<InvokeArgs_t>(): {
										auto invoke = static_cast<InvokeArgs_t*>(item);
										invoke->args.push_front(toAst<Exp_t>("self"sv, x));
										return true;
									}
									case id<Invoke_t>(): {
										auto invoke = static_cast<Invoke_t*>(item);
										invoke->args.push_front(toAst<Exp_t>("self"sv, x));
										return true;
									}
									default:
										return false;
								}
							};
							const auto& chainList = chainValue->items.objects();
							if (chainList.size() >= 2) {
								auto it = chainList.begin();
								auto secondItem = *(++it);
								if (!insertSelfToArguments(secondItem)) {
									if (auto colonChainItem = ast_cast<ColonChainItem_t>(secondItem)) {
										if (chainList.size() > 2 && insertSelfToArguments(*(++it))) {
											colonChainItem->switchToDot = true;
										}
									}
									newSuperCall = classVar + ".__parent"s;
								}
							} else {
								newSuperCall = classVar + ".__parent"s;
							}
							auto newChain = toAst<ChainValue_t>(newSuperCall, chainValue);
							chainValue->items.pop_front();
							const auto& items = newChain->items.objects();
							for (auto it = items.rbegin(); it != items.rend(); ++it) {
								chainValue->items.push_front(*it);
							}
						}
					}
				}
				return traversal::Continue;
			});
			BLOCK_END
			pushScope();
			if (type == MemType::Property) {
				decIndentOffset();
			}
			switch (keyValue->getId()) {
				case id<VariablePair_t>():
					transform_variable_pair(static_cast<VariablePair_t*>(keyValue), temp);
					break;
				case id<NormalPair_t>():
					transform_normal_pair(static_cast<NormalPair_t*>(keyValue), temp, true);
					break;
				default: YUEE("AST node mismatch", keyValue); break;
			}
			if (type == MemType::Property) {
				incIndentOffset();
			}
			popScope();
			out.push_back({temp.back(), type, keyValue});
			temp.clear();
			++count;
		}
		return count;
	}

	void transformAssignable(Assignable_t* assignable, str_list& out) {
		auto item = assignable->item.get();
		switch (item->getId()) {
			case id<AssignableChain_t>(): transformAssignableChain(static_cast<AssignableChain_t*>(item), out); break;
			case id<Variable_t>(): transformVariable(static_cast<Variable_t*>(item), out); break;
			case id<SelfItem_t>(): transformSelfName(static_cast<SelfItem_t*>(item), out); break;
			default: YUEE("AST node mismatch", item); break;
		}
	}

	void transformWithClosure(With_t* with, str_list& out) {
		str_list temp;
		pushFunctionScope();
		pushAnonVarArg();
		std::string& funcStart = temp.emplace_back();
		pushScope();
		transformWith(with, temp, nullptr, true);
		popScope();
		funcStart = anonFuncStart() + nll(with);
		temp.push_back(indent() + anonFuncEnd());
		popAnonVarArg();
		popFunctionScope();
		out.push_back(join(temp));
	}

	void transformWith(With_t* with, str_list& out, ExpList_t* assignList = nullptr, bool returnValue = false) {
		auto x = with;
		str_list temp;
		std::string withVar;
		bool scoped = false;
		if (with->assigns) {
			auto vars = getAssignVars(with);
			if (vars.front().empty() || isGlobal(vars.front())) {
				if (with->assigns->values.objects().size() == 1) {
					auto var = singleVariableFrom(with->assigns->values.objects().front(), true);
					if (!var.empty() && isLocal(var)) {
						withVar = var;
					}
				}
				if (withVar.empty()) {
					withVar = getUnusedName("_with_"sv);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(toAst<ExpList_t>(withVar, x));
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(with->assigns->values.objects().front());
					assignment->action.set(assign);
					if (!returnValue) {
						scoped = true;
						temp.push_back(indent() + "do"s + nll(with));
						pushScope();
					}
					transformAssignment(assignment, temp);
				}
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(with->valueList);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(withVar, x));
				bool skipFirst = true;
				for (auto value : with->assigns->values.objects()) {
					if (skipFirst) {
						skipFirst = false;
						continue;
					}
					assign->values.push_back(value);
				}
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			} else {
				withVar = vars.front();
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(with->valueList);
				assignment->action.set(with->assigns);
				if (!returnValue) {
					scoped = true;
					temp.push_back(indent() + "do"s + nll(with));
					pushScope();
				}
				transformAssignment(assignment, temp);
			}
		} else {
			withVar = singleVariableFrom(with->valueList, true);
			if (withVar.empty() || !isLocal(withVar)) {
				withVar = getUnusedName("_with_"sv);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(withVar, x));
				auto assign = x->new_ptr<Assign_t>();
				assign->values.dup(with->valueList->exprs);
				assignment->action.set(assign);
				if (!returnValue) {
					scoped = true;
					temp.push_back(indent() + "do"s + nll(with));
					pushScope();
				}
				transformAssignment(assignment, temp);
			}
		}
		if (!with->eop && !scoped && !returnValue) {
			pushScope();
			scoped = traversal::Stop == with->body->traverse([&](ast_node* node) {
				if (auto statement = ast_cast<Statement_t>(node)) {
					ClassDecl_t* clsDecl = nullptr;
					if (auto assignment = assignmentFrom(statement)) {
						auto names = transformAssignDefs(assignment->expList.get(), DefOp::Get);
						if (!names.empty()) {
							return traversal::Stop;
						}
						auto info = extractDestructureInfo(assignment, true, false);
						if (!info.destructures.empty()) {
							for (const auto& destruct : info.destructures)
								for (const auto& item : destruct.items)
									if (!item.targetVar.empty() && !isDefined(item.targetVar))
										return traversal::Stop;
						}
						BLOCK_START
						auto assign = assignment->action.as<Assign_t>();
						BREAK_IF(!assign);
						BREAK_IF(assign->values.objects().size() != 1);
						auto exp = ast_cast<Exp_t>(assign->values.objects().front());
						BREAK_IF(!exp);
						if (auto value = singleValueFrom(exp)) {
							clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
						}
						BLOCK_END
					} else if (auto expList = expListFrom(statement)) {
						auto value = singleValueFrom(expList);
						clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
					}
					if (clsDecl) {
						auto variable = clsDecl->name.as<Variable_t>();
						if (!isDefined(_parser.toString(variable))) return traversal::Stop;
					}
					return traversal::Return;
				}
				return traversal::Continue;
			});
			popScope();
			if (scoped) {
				temp.push_back(indent() + "do"s + nll(with));
				pushScope();
			}
		}
		_withVars.push(withVar);
		if (with->eop) {
			auto ifNode = x->new_ptr<If_t>();
			ifNode->type.set(toAst<IfType_t>("if"sv, x));
			ifNode->nodes.push_back(toAst<IfCond_t>(withVar + "~=nil"s, x));
			ifNode->nodes.push_back(with->body);
			transformIf(ifNode, temp, ExpUsage::Common);
		} else {
			transform_plain_body(with->body, temp, ExpUsage::Common);
		}
		_withVars.pop();
		if (assignList) {
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignList);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(withVar, x));
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		}
		if (returnValue) {
			auto last = lastStatementFrom(with->body);
			if (last && !last->content.is<Return_t>()) {
				temp.push_back(indent() + "return "s + withVar + nll(with));
			}
		}
		if (scoped) {
			popScope();
			temp.push_back(indent() + "end"s + nll(with));
		}
		out.push_back(join(temp));
	}

	void transform_const_value(ConstValue_t* const_value, str_list& out) {
		out.push_back(_parser.toString(const_value));
	}

	void transformGlobal(Global_t* global, str_list& out) {
		auto x = global;
		auto item = global->item.get();
		switch (item->getId()) {
			case id<ClassDecl_t>(): {
				auto classDecl = static_cast<ClassDecl_t*>(item);
				if (classDecl->name && classDecl->name->item->getId() == id<Variable_t>()) {
					markVarsGlobal(GlobalMode::Any);
					addGlobalVar(_parser.toString(classDecl->name->item), classDecl->name->item);
				}
				transformClassDecl(classDecl, out, ExpUsage::Common);
				break;
			}
			case id<GlobalOp_t>():
				if (_parser.toString(item) == "*"sv) {
					markVarsGlobal(GlobalMode::Any);
				} else {
					markVarsGlobal(GlobalMode::Capital);
				}
				break;
			case id<GlobalValues_t>(): {
				markVarsGlobal(GlobalMode::Any);
				auto values = global->item.to<GlobalValues_t>();
				if (values->valueList) {
					auto expList = x->new_ptr<ExpList_t>();
					for (auto name : values->nameList->names.objects()) {
						addGlobalVar(_parser.toString(name), name);
						auto callable = x->new_ptr<Callable_t>();
						callable->item.set(name);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						auto exp = newExp(chainValue, x);
						expList->exprs.push_back(exp);
					}
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(expList);
					auto assign = x->new_ptr<Assign_t>();
					if (auto expListLow = values->valueList.as<ExpListLow_t>()) {
						assign->values.dup(expListLow->exprs);
					} else {
						auto tableBlock = values->valueList.to<TableBlock_t>();
						assign->values.push_back(tableBlock);
					}
					assignment->action.set(assign);
					transformAssignment(assignment, out);
				} else {
					for (auto name : values->nameList->names.objects()) {
						addGlobalVar(_parser.toString(name), name);
					}
				}
				break;
			}
			default: YUEE("AST node mismatch", item); break;
		}
	}

	void transformExport(Export_t* exportNode, str_list& out) {
		auto x = exportNode;
		if (_scopes.size() > 1) {
			throw CompileError("can not do module export outside the root block"sv, exportNode);
		}
		if (exportNode->assign) {
			auto expList = exportNode->target.to<ExpList_t>();
			if (expList->exprs.size() != exportNode->assign->values.size()) {
				throw CompileError("left and right expressions must be matched in export statement"sv, x);
			}
			for (auto _exp : expList->exprs.objects()) {
				auto exp = static_cast<Exp_t*>(_exp);
				if (!variableFrom(exp) && !exp->getByPath<UnaryExp_t, Value_t, SimpleValue_t, TableLit_t>() && !exp->getByPath<UnaryExp_t, Value_t, SimpleTable_t>()) {
					throw CompileError("left hand expressions must be variables in export statement"sv, x);
				}
			}
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(exportNode->assign);
			transformAssignment(assignment, out);
			str_list names = transformAssignDefs(expList, DefOp::Get);
			auto info = extractDestructureInfo(assignment, true, false);
			if (!info.destructures.empty()) {
				for (const auto& destruct : info.destructures)
					for (const auto& item : destruct.items)
						if (!item.targetVar.empty())
							names.push_back(item.targetVar);
			}
			if (_info.exportDefault) {
				out.back().append(indent() + _info.moduleName + " = "s + names.back() + nlr(exportNode));
			} else {
				str_list lefts, rights;
				for (const auto& name : names) {
					lefts.push_back(_info.moduleName + "[\""s + name + "\"]"s);
					rights.push_back(name);
				}
				out.back().append(indent() + join(lefts, ", "sv) + " = "s + join(rights, ", "sv) + nlr(exportNode));
			}
		} else {
			if (auto macro = exportNode->target.as<Macro_t>()) {
				transformMacro(macro, out, true);
			} else if (_info.exportDefault) {
				auto exp = exportNode->target.to<Exp_t>();
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(_info.moduleName, x));
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(exp);
				assignment->action.set(assign);
				transformAssignment(assignment, out);
			} else {
				str_list temp;
				auto expList = exportNode->target.to<ExpList_t>();
				auto assignment = x->new_ptr<ExpListAssign_t>();
				auto assignList = toAst<ExpList_t>(_info.moduleName + "[#"s + _info.moduleName + "+1]"s, x);
				assignment->expList.set(assignList);
				for (auto exp : expList->exprs.objects()) {
					if (auto classDecl = exp->getByPath<UnaryExp_t, Value_t, SimpleValue_t, ClassDecl_t>()) {
						if (classDecl->name && classDecl->name->item->getId() == id<Variable_t>()) {
							transformClassDecl(classDecl, temp, ExpUsage::Common);
							auto name = _parser.toString(classDecl->name->item);
							assignment->expList.set(toAst<ExpList_t>(_info.moduleName + "[\""s + name + "\"]"s, x));
							auto assign = x->new_ptr<Assign_t>();
							assign->values.push_back(toAst<Exp_t>(name, x));
							assignment->action.set(assign);
							transformAssignment(assignment, temp);
							assignment->expList.set(assignList);
							continue;
						}
					}
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					assignment->action.set(assign);
					transformAssignment(assignment, temp);
				}
				out.push_back(join(temp));
			}
		}
	}

	void transform_simple_table(SimpleTable_t* table, str_list& out) {
		transformTable(table->pairs.objects(), out);
	}

	void transformTblComprehension(TblComprehension_t* comp, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		switch (usage) {
			case ExpUsage::Closure:
				pushFunctionScope();
				pushAnonVarArg();
				pushScope();
				break;
			case ExpUsage::Assignment:
				pushScope();
				break;
			default:
				break;
		}
		auto x = comp;
		str_list kv;
		std::string tbl = getUnusedName("_tbl_"sv);
		addToScope(tbl);
		str_list temp;
		auto compInner = comp->forLoop.get();
		for (auto item : compInner->items.objects()) {
			switch (item->getId()) {
				case id<CompForEach_t>():
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case id<CompFor_t>():
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case id<Exp_t>():
					transformExp(static_cast<Exp_t*>(item), temp, ExpUsage::Closure);
					temp.back() = indent() + "if "s + temp.back() + " then"s + nll(item);
					pushScope();
					break;
				default: YUEE("AST node mismatch", item); break;
			}
		}
		transformExp(comp->key, kv, ExpUsage::Closure);
		if (comp->value) {
			transformExp(comp->value->value, kv, ExpUsage::Closure);
		}
		for (size_t i = 0; i < compInner->items.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << tbl << " = { }"sv << nll(comp);
		_buf << join(temp);
		pushScope();
		if (!comp->value) {
			auto keyVar = getUnusedName("_key_"sv);
			auto valVar = getUnusedName("_val_"sv);
			_buf << indent(int(temp.size()) - 1) << "local "sv << keyVar << ", "sv << valVar << " = "sv << kv.front() << nll(comp);
			kv.front() = keyVar;
			kv.push_back(valVar);
		}
		_buf << indent(int(temp.size()) - 1) << tbl << "["sv << kv.front() << "] = "sv << kv.back() << nll(comp);
		for (int ind = int(temp.size()) - 2; ind > -1; --ind) {
			_buf << indent(ind) << "end"sv << nll(comp);
		}
		popScope();
		_buf << indent() << "end"sv << nll(comp);
		switch (usage) {
			case ExpUsage::Closure:
				out.push_back(clearBuf() + indent() + "return "s + tbl + nlr(comp));
				popScope();
				out.back().insert(0, anonFuncStart() + nll(comp));
				out.back().append(indent() + anonFuncEnd());
				popAnonVarArg();
				popFunctionScope();
				break;
			case ExpUsage::Assignment: {
				out.push_back(clearBuf());
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(tbl, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				out.back().append(temp.back());
				popScope();
				out.back().insert(0, indent() + "do"s + nll(comp));
				out.back().append(indent() + "end"s + nlr(comp));
				break;
			}
			case ExpUsage::Return:
				out.push_back(clearBuf() + indent() + "return "s + tbl + nlr(comp));
				break;
			default:
				break;
		}
	}

	void transformCompFor(CompFor_t* comp, str_list& out) {
		transformForHead(comp->varName, comp->startValue, comp->stopValue, comp->stepValue, out);
	}

	void transformTableBlockIndent(TableBlockIndent_t* table, str_list& out) {
		transformTable(table->values.objects(), out);
	}

	void transformTableBlock(TableBlock_t* table, str_list& out) {
		const auto& values = table->values.objects();
		if (hasSpreadExp(values)) {
			transformSpreadTable(values, out, ExpUsage::Closure);
		} else {
			transformTable(values, out);
		}
	}

	void transformDo(Do_t* doNode, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		str_list temp;
		std::string* funcStart = nullptr;
		if (usage == ExpUsage::Closure) {
			pushFunctionScope();
			pushAnonVarArg();
			funcStart = &temp.emplace_back();
		} else {
			temp.push_back(indent() + "do"s + nll(doNode));
		}
		pushScope();
		transformBody(doNode->body, temp, usage, assignList);
		popScope();
		if (usage == ExpUsage::Closure) {
			*funcStart = anonFuncStart() + nll(doNode);
			temp.push_back(indent() + anonFuncEnd());
			popAnonVarArg();
			popFunctionScope();
		} else {
			temp.push_back(indent() + "end"s + nlr(doNode));
		}
		out.push_back(join(temp));
	}

	void transformTry(Try_t* tryNode, str_list& out, ExpUsage usage) {
		auto x = tryNode;
		ast_ptr<true, Exp_t> errHandler;
		if (tryNode->catchBlock) {
			auto errHandleStr = "("s + _parser.toString(tryNode->catchBlock->err) + ")->"s;
			errHandler.set(toAst<Exp_t>(errHandleStr, x->func));
			auto funLit = simpleSingleValueFrom(errHandler)->value.to<FunLit_t>();
			auto body = x->new_ptr<Body_t>();
			body->content.set(tryNode->catchBlock->body);
			funLit->body.set(body);
		}
		if (auto tryBlock = tryNode->func.as<Block_t>()) {
			auto tryExp = toAst<Exp_t>("->"sv, x);
			auto funLit = simpleSingleValueFrom(tryExp)->value.to<FunLit_t>();
			auto body = x->new_ptr<Body_t>();
			body->content.set(tryBlock);
			funLit->body.set(body);
			if (errHandler) {
				auto xpcall = toAst<ChainValue_t>("xpcall()", x);
				auto invoke = ast_to<Invoke_t>(xpcall->items.back());
				invoke->args.push_back(tryExp);
				invoke->args.push_back(errHandler);
				transformChainValue(xpcall, out, ExpUsage::Closure);
			} else {
				auto pcall = toAst<ChainValue_t>("pcall()", x);
				auto invoke = ast_to<Invoke_t>(pcall->items.back());
				invoke->args.push_back(tryExp);
				transformChainValue(pcall, out, ExpUsage::Closure);
			}
			if (usage == ExpUsage::Common) {
				out.back().append(nlr(x));
			}
			return;
		} else if (auto value = singleValueFrom(tryNode->func)) {
			BLOCK_START
			auto chainValue = value->item.as<ChainValue_t>();
			BREAK_IF(!chainValue);
			BREAK_IF(!isChainValueCall(chainValue));
			ast_ptr<true, ast_node> last = chainValue->items.back();
			chainValue->items.pop_back();
			_ast_list* args = nullptr;
			if (auto invoke = ast_cast<InvokeArgs_t>(last)) {
				args = &invoke->args;
			} else {
				args = &(ast_to<Invoke_t>(last)->args);
			}
			if (errHandler) {
				auto xpcall = toAst<ChainValue_t>("xpcall()", x);
				auto invoke = ast_to<Invoke_t>(xpcall->items.back());
				invoke->args.push_back(tryNode->func);
				invoke->args.push_back(errHandler);
				for (auto arg : args->objects()) {
					invoke->args.push_back(arg);
				}
				transformChainValue(xpcall, out, ExpUsage::Closure);
			} else {
				auto pcall = toAst<ChainValue_t>("pcall()", x);
				auto invoke = ast_to<Invoke_t>(pcall->items.back());
				invoke->args.push_back(tryNode->func);
				for (auto arg : args->objects()) {
					invoke->args.push_back(arg);
				}
				transformChainValue(pcall, out, ExpUsage::Closure);
			}
			if (usage == ExpUsage::Common) {
				out.back().append(nlr(x));
			}
			return;
			BLOCK_END
		}
		if (errHandler) {
			auto xpcall = toAst<ChainValue_t>("xpcall()", x);
			auto invoke = ast_to<Invoke_t>(xpcall->items.back());
			invoke->args.push_back(tryNode->func);
			invoke->args.push_back(errHandler);
			transformChainValue(xpcall, out, ExpUsage::Closure);
		} else {
			auto pcall = toAst<ChainValue_t>("pcall()", x);
			auto invoke = ast_to<Invoke_t>(pcall->items.back());
			invoke->args.push_back(tryNode->func);
			transformChainValue(pcall, out, ExpUsage::Closure);
		}
		if (usage == ExpUsage::Common) {
			out.back().append(nlr(x));
		}
	}

	void transformImportFrom(ImportFrom_t* import, str_list& out) {
		str_list temp;
		auto x = import;
		auto objVar = singleVariableFrom(import->exp, true);
		ast_ptr<false, ExpListAssign_t> objAssign;
		if (objVar.empty()) {
			objVar = getUnusedName("_obj_"sv);
			auto expList = toAst<ExpList_t>(objVar, x);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(import->exp);
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			objAssign.set(assignment);
		}
		auto expList = x->new_ptr<ExpList_t>();
		auto assign = x->new_ptr<Assign_t>();
		for (auto name : import->names.objects()) {
			switch (name->getId()) {
				case id<Variable_t>(): {
					auto var = static_cast<Variable_t*>(name);
					{
						auto callable = toAst<Callable_t>(objVar, x);
						auto dotChainItem = x->new_ptr<DotChainItem_t>();
						dotChainItem->name.set(var->name);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						chainValue->items.push_back(dotChainItem);
						auto exp = newExp(chainValue, x);
						assign->values.push_back(exp);
					}
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(var);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto exp = newExp(chainValue, x);
					expList->exprs.push_back(exp);
					break;
				}
				case id<ColonImportName_t>(): {
					auto var = static_cast<ColonImportName_t*>(name)->name.get();
					{
						auto nameNode = var->name.get();
						auto callable = toAst<Callable_t>(objVar, x);
						auto colonChain = x->new_ptr<ColonChainItem_t>();
						colonChain->name.set(nameNode);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						chainValue->items.push_back(colonChain);
						auto exp = newExp(chainValue, x);
						assign->values.push_back(exp);
					}
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(var);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto exp = newExp(chainValue, x);
					expList->exprs.push_back(exp);
					break;
				}
				default: YUEE("AST node mismatch", name); break;
			}
		}
		if (objAssign) {
			auto preDef = toLocalDecl(transformAssignDefs(expList, DefOp::Mark));
			if (!preDef.empty()) {
				temp.push_back(preDef + nll(import));
			}
			temp.push_back(indent() + "do"s + nll(import));
			pushScope();
			transformAssignment(objAssign, temp);
		}
		auto assignment = x->new_ptr<ExpListAssign_t>();
		assignment->expList.set(expList);
		assignment->action.set(assign);
		transformAssignment(assignment, temp);
		if (objAssign) {
			popScope();
			temp.push_back(indent() + "end"s + nlr(import));
		}
		out.push_back(join(temp));
		auto vars = getAssignVars(assignment);
		for (const auto& var : vars) {
			markVarConst(var);
		}
	}

	std::string moduleNameFrom(ImportLiteral_t* literal) {
		auto name = _parser.toString(literal->inners.back());
		Utils::replace(name, "-"sv, "_"sv);
		Utils::replace(name, " "sv, "_"sv);
		return name;
	}

	void transformImportAs(ImportAs_t* import, str_list& out) {
		ast_node* x = import;
		if (!import->target) {
			auto name = moduleNameFrom(import->literal);
			import->target.set(toAst<Variable_t>(name, x));
		}
		if (ast_is<ImportAllMacro_t, ImportTabLit_t>(import->target)) {
			x = import->target.get();
			bool importAllMacro = import->target.is<ImportAllMacro_t>();
			std::list<std::pair<std::string, std::string>> macroPairs;
			auto newTab = x->new_ptr<ImportTabLit_t>();
			if (auto tabLit = import->target.as<ImportTabLit_t>()) {
				for (auto item : tabLit->items.objects()) {
					switch (item->getId()) {
#ifdef YUE_NO_MACRO
						case id<MacroName_t>():
						case id<MacroNamePair_t>():
						case id<ImportAllMacro_t>(): {
							throw CompileError("macro feature not supported"sv, item));
							break;
						}
#else // YUE_NO_MACRO
						case id<MacroName_t>(): {
							auto macroName = static_cast<MacroName_t*>(item);
							auto name = _parser.toString(macroName->name);
							macroPairs.emplace_back(name, name);
							break;
						}
						case id<MacroNamePair_t>(): {
							auto pair = static_cast<MacroNamePair_t*>(item);
							macroPairs.emplace_back(_parser.toString(pair->key->name), _parser.toString(pair->value->name));
							break;
						}
						case id<ImportAllMacro_t>():
							if (importAllMacro) throw CompileError("import all macro symbol duplicated"sv, item);
							importAllMacro = true;
							break;
#endif // YUE_NO_MACRO
						case id<VariablePair_t>():
						case id<NormalPair_t>():
						case id<MetaVariablePair_t>():
						case id<MetaNormalPair_t>():
						case id<Exp_t>():
							newTab->items.push_back(item);
							break;
						default: YUEE("AST node mismatch", item); break;
					}
				}
			}
#ifndef YUE_NO_MACRO
			if (importAllMacro || !macroPairs.empty()) {
				auto moduleName = _parser.toString(import->literal);
				Utils::replace(moduleName, "'"sv, ""sv);
				Utils::replace(moduleName, "\""sv, ""sv);
				Utils::trim(moduleName);
				pushCurrentModule(); // cur
				int top = lua_gettop(L) - 1; // Lua state may be setup by pushCurrentModule()
				DEFER(lua_settop(L, top));
				pushYue("find_modulepath"sv); // cur find_modulepath
				lua_pushlstring(L, moduleName.c_str(), moduleName.size()); // cur find_modulepath moduleName
				if (lua_pcall(L, 1, 2, 0) != 0) {
					std::string err = lua_tostring(L, -1);
					throw CompileError("failed to resolve module path\n"s + err, x);
				}
				if (lua_isnil(L, -2) != 0) {
					str_list files;
					if (lua_istable(L, -1) != 0) {
						int size = static_cast<int>(lua_objlen(L, -1));
						for (int i = 0; i < size; i++) {
							lua_rawgeti(L, -1, i + 1);
							files.push_back("no file \""s + lua_tostring(L, -1) + "\""s);
							lua_pop(L, 1);
						}
					}
					throw CompileError("module '"s + moduleName + "\' not found:\n\t"s + join(files, "\n\t"sv), x);
				}
				lua_pop(L, 1);
				std::string moduleFullName = lua_tostring(L, -1);
				lua_pop(L, 1); // cur
				if (!isModuleLoaded(moduleFullName)) {
					pushYue("read_file"sv); // cur read_file
					lua_pushlstring(L, moduleFullName.c_str(), moduleFullName.size()); // cur load_text moduleFullName
					if (lua_pcall(L, 1, 1, 0) != 0) {
						std::string err = lua_tostring(L, -1);
						throw CompileError("failed to read module file\n"s + err, x);
					} // cur text
					if (lua_isnil(L, -1) != 0) {
						throw CompileError("failed to get module text"sv, x);
					} // cur text
					std::string text = lua_tostring(L, -1);
					auto compiler = YueCompilerImpl(L, _luaOpen, false);
					YueConfig config;
					config.lineOffset = 0;
					config.lintGlobalVariable = false;
					config.reserveLineNumber = false;
					config.implicitReturnRoot = _config.implicitReturnRoot;
					config.module = moduleFullName;
					config.exporting = true;
					auto result = compiler.compile(text, config);
					if (result.error) {
						throw CompileError("failed to compile module '"s + moduleName + "\': "s + result.error.value().msg, x);
					}
					lua_pop(L, 1); // cur
				}
				pushModuleTable(moduleFullName); // cur mod
				if (importAllMacro) {
					lua_pushnil(L); // cur mod startKey
					while (lua_next(L, -2) != 0) { // cur mod key value
						lua_pushvalue(L, -2); // cur mod key value key
						lua_insert(L, -2); // cur mod key key value
						lua_rawset(L, -5); // cur[key] = value, cur mod key
					}
				}
				for (const auto& pair : macroPairs) {
					lua_getfield(L, -1, pair.first.c_str()); // mod[first], cur mod val
					lua_setfield(L, -3, pair.second.c_str()); // cur[second] = val, cur mod
				}
			}
#else // YUE_NO_MACRO
			if (importAllMacro) {
				throw CompileError("macro feature not supported"sv, import->target));
			}
#endif // YUE_NO_MACRO
			if (newTab->items.empty()) {
				out.push_back(Empty);
				return;
			} else {
				import->target.set(newTab);
			}
		}
		auto target = import->target.get();
		x = target;
		auto value = x->new_ptr<Value_t>();
		if (auto var = ast_cast<Variable_t>(target)) {
			auto callable = x->new_ptr<Callable_t>();
			callable->item.set(var);
			auto chainValue = x->new_ptr<ChainValue_t>();
			chainValue->items.push_back(callable);
			value->item.set(chainValue);
		} else if (auto tabLit = ast_cast<ImportTabLit_t>(target)) {
			auto simpleValue = x->new_ptr<SimpleValue_t>();
			auto tableLit = x->new_ptr<TableLit_t>();
			for (auto pair : tabLit->items.objects()) {
				switch (pair->getId()) {
					case id<VariablePair_t>(): {
						auto pairDef = pair->new_ptr<VariablePairDef_t>();
						pairDef->pair.set(pair);
						tableLit->values.push_back(pairDef);
						break;
					}
					case id<NormalPair_t>(): {
						auto pairDef = pair->new_ptr<NormalPairDef_t>();
						pairDef->pair.set(pair);
						tableLit->values.push_back(pairDef);
						break;
					}
					case id<Exp_t>(): {
						auto pairDef = pair->new_ptr<NormalDef_t>();
						pairDef->item.set(pair);
						tableLit->values.push_back(pairDef);
						break;
					}
					case id<MetaVariablePair_t>(): {
						auto pairDef = pair->new_ptr<MetaVariablePairDef_t>();
						pairDef->pair.set(pair);
						tableLit->values.push_back(pairDef);
						break;
					}
					case id<MetaNormalPair_t>(): {
						auto pairDef = pair->new_ptr<MetaNormalPairDef_t>();
						pairDef->pair.set(pair);
						tableLit->values.push_back(pairDef);
						break;
					}
					default: YUEE("AST node mismatch", pair); break;
				}
			}
			simpleValue->value.set(tableLit);
			value->item.set(simpleValue);
		} else {
			return;
		}
		auto exp = newExp(value, x);
		auto assignList = x->new_ptr<ExpList_t>();
		assignList->exprs.push_back(exp);
		auto assign = x->new_ptr<Assign_t>();
		assign->values.push_back(toAst<Exp_t>("require "s + _parser.toString(import->literal), x));
		auto assignment = x->new_ptr<ExpListAssign_t>();
		assignment->expList.set(assignList);
		assignment->action.set(assign);
		transformAssignment(assignment, out);
		if (auto var = ast_cast<Variable_t>(target)) {
			auto moduleName = _parser.toString(var);
			markVarConst(moduleName);
		} else {
			markDestructureConst(assignment);
		}
	}

	void transformImport(Import_t* import, str_list& out) {
		auto content = import->content.get();
		switch (content->getId()) {
			case id<ImportAs_t>():
				transformImportAs(static_cast<ImportAs_t*>(content), out);
				break;
			case id<ImportFrom_t>():
				transformImportFrom(static_cast<ImportFrom_t*>(content), out);
				break;
			default: YUEE("AST node mismatch", content); break;
		}
	}

	void transformWhileInPlace(While_t* whileNode, str_list& out, ExpList_t* expList = nullptr) {
		auto x = whileNode;
		str_list temp;
		if (expList) {
			temp.push_back(indent() + "do"s + nll(whileNode));
			pushScope();
		}
		auto accumVar = getUnusedName("_accum_"sv);
		addToScope(accumVar);
		auto lenVar = getUnusedName("_len_"sv);
		addToScope(lenVar);
		temp.push_back(indent() + "local "s + accumVar + " = { }"s + nll(whileNode));
		temp.push_back(indent() + "local "s + lenVar + " = 1"s + nll(whileNode));
		bool isUntil = _parser.toString(whileNode->type) == "until"sv;
		auto condStr = transformCondExp(whileNode->condition, isUntil);
		temp.push_back(indent() + "while "s + condStr + " do"s + nll(whileNode));
		pushScope();
		auto assignLeft = toAst<ExpList_t>(accumVar + '[' + lenVar + ']', x);
		auto lenLine = lenVar + " = "s + lenVar + " + 1"s + nlr(whileNode);
		transformLoopBody(whileNode->body, temp, lenLine, ExpUsage::Assignment, assignLeft);
		popScope();
		temp.push_back(indent() + "end"s + nlr(whileNode));
		if (expList) {
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accumVar, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
			popScope();
		} else {
			temp.push_back(indent() + "return "s + accumVar + nlr(whileNode));
		}
		if (expList) {
			temp.push_back(indent() + "end"s + nlr(whileNode));
		}
		out.push_back(join(temp));
	}

	void transformWhileClosure(While_t* whileNode, str_list& out) {
		auto x = whileNode;
		str_list temp;
		pushFunctionScope();
		pushAnonVarArg();
		std::string& funcStart = temp.emplace_back();
		pushScope();
		auto accumVar = getUnusedName("_accum_"sv);
		addToScope(accumVar);
		auto lenVar = getUnusedName("_len_"sv);
		addToScope(lenVar);
		temp.push_back(indent() + "local "s + accumVar + " = { }"s + nll(whileNode));
		temp.push_back(indent() + "local "s + lenVar + " = 1"s + nll(whileNode));
		bool isUntil = _parser.toString(whileNode->type) == "until"sv;
		auto condStr = transformCondExp(whileNode->condition, isUntil);
		temp.push_back(indent() + "while "s + condStr + " do"s + nll(whileNode));
		pushScope();
		auto assignLeft = toAst<ExpList_t>(accumVar + '[' + lenVar + ']', x);
		auto lenLine = lenVar + " = "s + lenVar + " + 1"s + nlr(whileNode);
		transformLoopBody(whileNode->body, temp, lenLine, ExpUsage::Assignment, assignLeft);
		popScope();
		temp.push_back(indent() + "end"s + nlr(whileNode));
		temp.push_back(indent() + "return "s + accumVar + nlr(whileNode));
		popScope();
		funcStart = anonFuncStart() + nll(whileNode);
		temp.push_back(indent() + anonFuncEnd());
		popAnonVarArg();
		popFunctionScope();
		out.push_back(join(temp));
	}

	void transformWhile(While_t* whileNode, str_list& out) {
		str_list temp;
		pushScope();
		bool isUntil = _parser.toString(whileNode->type) == "until"sv;
		auto condStr = transformCondExp(whileNode->condition, isUntil);
		transformLoopBody(whileNode->body, temp, Empty, ExpUsage::Common);
		popScope();
		_buf << indent() << "while "sv << condStr << " do"sv << nll(whileNode);
		_buf << temp.back();
		_buf << indent() << "end"sv << nlr(whileNode);
		out.push_back(clearBuf());
	}

	void transformRepeat(Repeat_t* repeat, str_list& out) {
		str_list temp;
		pushScope();
		auto condVar = transformRepeatBody(repeat, temp);
		if (condVar.empty()) {
			transformExp(repeat->condition, temp, ExpUsage::Closure);
		} else {
			temp.push_back(condVar);
		}
		popScope();
		_buf << indent() << "repeat"sv << nll(repeat);
		_buf << temp.front();
		_buf << indent() << "until "sv << temp.back() << nlr(repeat);
		out.push_back(clearBuf());
	}

	void transformSwitch(Switch_t* switchNode, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = switchNode;
		str_list temp;
		std::string* funcStart = nullptr;
		if (usage == ExpUsage::Closure) {
			pushFunctionScope();
			pushAnonVarArg();
			funcStart = &temp.emplace_back();
			pushScope();
		}
		bool extraScope = false;
		auto objVar = singleVariableFrom(switchNode->target, true);
		if (objVar.empty() || !isLocal(objVar)) {
			if (usage == ExpUsage::Common || usage == ExpUsage::Assignment) {
				extraScope = true;
				temp.push_back(indent() + "do"s + nll(x));
				pushScope();
			}
			objVar = getUnusedName("_exp_"sv);
			auto expList = toAst<ExpList_t>(objVar, x);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(switchNode->target);
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		}
		const auto& branches = switchNode->branches.objects();
		int addScope = 0;
		bool firstBranch = true;
		std::string tabCheckVar;
		for (auto branch_ : branches) {
			auto branch = static_cast<SwitchCase_t*>(branch_);
			if (auto value = singleValueFrom(branch->valueList);
				value && (value->item.is<SimpleTable_t>() || value->getByPath<SimpleValue_t, TableLit_t>())) {
				if (!firstBranch) {
					temp.push_back(indent() + "else"s + nll(branch));
					pushScope();
					addScope++;
				}
				if (tabCheckVar.empty()) {
					if (!extraScope) {
						temp.push_back(indent() + "do"s + nll(branch));
						pushScope();
						extraScope = true;
					}
					auto typeVar = getUnusedName("_type_");
					forceAddToScope(typeVar);
					tabCheckVar = getUnusedName("_tab_");
					forceAddToScope(tabCheckVar);
					temp.push_back(indent() + "local "s + typeVar + " = "s + globalVar("type", branch) + '(' + objVar + ')' + nll(branch));
					temp.push_back(indent() + "local "s + tabCheckVar + " = \"table\" == "s + typeVar + " or \"userdata\" == "s + typeVar + nll(branch));
				}
				std::string matchVar;
				bool lastBranch = branches.back() == branch_ && !switchNode->lastBranch;
				if (!lastBranch) {
					matchVar = getUnusedName("_match_");
					forceAddToScope(matchVar);
					temp.push_back(indent() + "local "s + matchVar + " = false"s + nll(branch));
				}
				temp.back().append(indent() + "if "s + tabCheckVar + " then"s + nll(branch));
				pushScope();
				auto assignment = assignmentFrom(static_cast<Exp_t*>(branch->valueList->exprs.front()), toAst<Exp_t>(objVar, branch), branch);
				auto info = extractDestructureInfo(assignment, true, false);
				transformAssignment(assignment, temp, true);
				str_list conds;
				for (const auto& destruct : info.destructures) {
					for (const auto& item : destruct.items) {
						if (!item.defVal) {
							transformExp(item.target, conds, ExpUsage::Closure);
							conds.back().append(" ~= nil"s);
						}
					}
				}
				if (!conds.empty()) {
					temp.push_back(indent() + "if "s + join(conds, " and "sv) + " then"s + nll(branch));
					pushScope();
				}
				if (!lastBranch) {
					temp.push_back(indent() + matchVar + " = true"s + nll(branch));
				}
				transform_plain_body(branch->body, temp, usage, assignList);
				if (!conds.empty()) {
					popScope();
					temp.push_back(indent() + "end"s + nll(branch));
				}
				if (!lastBranch) {
					popScope();
					temp.push_back(indent() + "end"s + nll(branch));
					temp.push_back(indent() + "if not "s + matchVar + " then"s + nll(branch));
					pushScope();
					addScope++;
				} else {
					popScope();
				}
				firstBranch = true;
			} else {
				temp.push_back(indent() + (firstBranch ? "if"s : "elseif"s));
				firstBranch = false;
				str_list tmp;
				const auto& exprs = branch->valueList->exprs.objects();
				for (auto exp_ : exprs) {
					auto exp = static_cast<Exp_t*>(exp_);
					transformExp(exp, tmp, ExpUsage::Closure);
					if (!singleValueFrom(exp)) {
						tmp.back() = '(' + tmp.back() + ')';
					}
					temp.back().append(' ' + tmp.back() + " == "s + (exp == exprs.back() ? objVar : objVar + " or"s));
				}
				temp.back().append(" then"s + nll(branch));
				pushScope();
				transform_plain_body(branch->body, temp, usage, assignList);
				popScope();
			}
		}
		if (switchNode->lastBranch) {
			if (!firstBranch) {
				temp.push_back(indent() + "else"s + nll(switchNode->lastBranch));
				pushScope();
			} else {
				addScope--;
			}
			transform_plain_body(switchNode->lastBranch, temp, usage, assignList);
			popScope();
		}
		while (addScope > 0) {
			addScope--;
			temp.push_back(indent() + "end"s + nlr(switchNode));
			popScope();
		}
		temp.push_back(indent() + "end"s + nlr(switchNode));
		if (usage == ExpUsage::Closure) {
			popFunctionScope();
			popScope();
			*funcStart = anonFuncStart() + nll(switchNode);
			temp.push_back(indent() + anonFuncEnd());
			popAnonVarArg();
		}
		if (extraScope) {
			popScope();
			temp.push_back(indent() + "end"s + nlr(switchNode));
		}
		out.push_back(join(temp));
	}

	void transformLocalDef(Local_t* local, str_list& out) {
		if (!local->forceDecls.empty() || !local->decls.empty()) {
			str_list defs;
			for (const auto& decl : local->forceDecls) {
				forceAddToScope(decl);
				defs.push_back(decl);
			}
			for (const auto& decl : local->decls) {
				if (addToScope(decl)) {
					defs.push_back(decl);
				}
			}
			auto preDefine = toLocalDecl(defs);
			if (!preDefine.empty()) {
				out.push_back(preDefine + nll(local));
			}
		}
	}

	void transformLocal(Local_t* local, str_list& out) {
		str_list temp;
		if (!local->defined) {
			local->defined = true;
			transformLocalDef(local, temp);
		}
		switch (local->item->getId()) {
			case id<LocalValues_t>(): {
				auto values = static_cast<LocalValues_t*>(local->item.get());
				if (values->valueList) {
					auto x = local;
					auto expList = x->new_ptr<ExpList_t>();
					for (auto name : values->nameList->names.objects()) {
						auto callable = x->new_ptr<Callable_t>();
						callable->item.set(name);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						auto exp = newExp(chainValue, x);
						expList->exprs.push_back(exp);
					}
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(expList);
					auto assign = x->new_ptr<Assign_t>();
					if (auto expListLow = values->valueList.as<ExpListLow_t>()) {
						assign->values.dup(expListLow->exprs);
					} else {
						auto tableBlock = values->valueList.to<TableBlock_t>();
						assign->values.push_back(tableBlock);
					}
					assignment->action.set(assign);
					transformAssignment(assignment, temp);
				}
				break;
			}
			case id<LocalFlag_t>(): break;
			default: YUEE("AST node mismatch", local->item); break;
		}
		out.push_back(join(temp));
	}

	void transformLocalAttrib(LocalAttrib_t* localAttrib, str_list& out) {
		auto x = localAttrib;
		if (x->leftList.size() < x->assign->values.size()) {
			auto num = x->leftList.size();
			if (num > 1) {
				_buf << "no more than "sv << num << " right values expected, got "sv << x->assign->values.size();
			} else {
				_buf << "only one right value expected, got "sv << x->assign->values.size();
			}
			throw CompileError(clearBuf(), x->assign->values.front());
		}
		auto listA = x->new_ptr<NameList_t>();
		auto assignA = x->new_ptr<Assign_t>();
		auto listB = x->new_ptr<ExpList_t>();
		auto assignB = x->new_ptr<Assign_t>();
		auto i = x->leftList.objects().begin();
		auto ie = x->leftList.objects().end();
		auto j = x->assign->values.objects().begin();
		auto je = x->assign->values.objects().end();
		while (i != ie) {
			if (ast_is<Variable_t>(*i)) {
				listA->names.push_back(*i);
				if (j != je) assignA->values.push_back(*j);
			} else {
				auto item = *i;
				auto value = item->new_ptr<Value_t>();
				switch (item->getId()) {
					case id<SimpleTable_t>():
						value->item.set(item);
						break;
					case id<TableLit_t>(): {
						auto simpleValue = item->new_ptr<SimpleValue_t>();
						simpleValue->value.set(item);
						value->item.set(simpleValue);
						break;
					}
					default: YUEE("AST node mismatch", item); break;
				}
				auto exp = newExp(value, item);
				listB->exprs.push_back(exp);
				if (j != je) assignB->values.push_back(*j);
			}
			++i;
			if (j != je) ++j;
		}
		if (!listA->names.empty()) {
			str_list vars;
			for (auto name : listA->names.objects()) {
				auto var = _parser.toString(name);
				forceAddToScope(var);
				vars.push_back(var);
			}
			if (getLuaTarget(x) >= 504) {
				std::string attrib;
				if (localAttrib->attrib.is<ConstAttrib_t>()) {
					attrib = " <const>"s;
				} else if (localAttrib->attrib.is<CloseAttrib_t>()) {
					attrib = " <close>"s;
				} else {
					YUEE("AST node mismatch", localAttrib->attrib);
				}
				for (auto& var : vars) {
					markVarConst(var);
					var.append(attrib);
				}
			} else {
				if (localAttrib->attrib.is<CloseAttrib_t>()) {
					throw CompileError("close attribute is not available when not targeting Lua version 5.4 or higher"sv, x);
				}
				for (auto& var : vars) {
					markVarConst(var);
				}
			}
			str_list temp;
			for (auto item : assignA->values.objects()) {
				transformAssignItem(item, temp);
			}
			out.push_back(indent() + "local "s + join(vars, ", "sv) + " = "s + join(temp, ", "sv) + nll(x));
		}
		if (!listB->exprs.empty()) {
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(listB);
			assignment->action.set(assignB);
			transformAssignment(assignment, out);
			markDestructureConst(assignment);
		}
	}

	void transformBreakLoop(BreakLoop_t* breakLoop, str_list& out) {
		auto keyword = _parser.toString(breakLoop);
		if (_enableBreakLoop.empty() || !_enableBreakLoop.top()) {
			throw CompileError(keyword + " is not inside a loop"s, breakLoop);
		}
		if (keyword == "break"sv) {
			out.push_back(indent() + keyword + nll(breakLoop));
			return;
		}
		if (_continueVars.empty()) throw CompileError("continue is not inside a loop"sv, breakLoop);
		str_list temp;
		auto& item = _continueVars.top();
		if (item.condAssign) {
			transformAssignment(item.condAssign, temp);
		}
		if (getLuaTarget(breakLoop) < 502) {
			if (!temp.empty()) {
				_buf << temp.back();
			}
			_buf << indent() << item.var << " = true"sv << nll(breakLoop);
			_buf << indent() << "break"sv << nll(breakLoop);
			out.push_back(clearBuf());
		} else {
			transformGoto(toAst<Goto_t>("goto "s + item.var, breakLoop), temp);
			out.push_back(join(temp));
		}
	}

	void transformLabel(Label_t* label, str_list& out) {
		if (getLuaTarget(label) < 502) {
			throw CompileError("label statement is not available when not targeting Lua version 5.2 or higher"sv, label);
		}
		auto labelStr = _parser.toString(label->label);
		int currentScope = _gotoScopes.top();
		if (static_cast<int>(_labels.size()) <= currentScope) {
			_labels.resize(currentScope + 1, std::nullopt);
			_labels[currentScope] = std::unordered_map<std::string, LabelNode>();
		}
		if (!_labels[currentScope]) {
			_labels[currentScope] = std::unordered_map<std::string, LabelNode>();
		}
		auto& scope = _labels[currentScope].value();
		if (auto it = scope.find(labelStr); it != scope.end()) {
			throw CompileError("label '"s + labelStr + "' already defined at line "s + std::to_string(it->second.line), label);
		}
		scope[labelStr] = {label->m_begin.m_line, static_cast<int>(_scopes.size())};
		out.push_back(indent() + "::"s + labelStr + "::"s + nll(label));
	}

	void transformGoto(Goto_t* gotoNode, str_list& out) {
		if (getLuaTarget(gotoNode) < 502) {
			throw CompileError("goto statement is not available when not targeting Lua version 5.2 or higher"sv, gotoNode);
		}
		auto labelStr = _parser.toString(gotoNode->label);
		gotos.push_back({gotoNode, labelStr, _gotoScopes.top(), static_cast<int>(_scopes.size())});
		out.push_back(indent() + "goto "s + labelStr + nll(gotoNode));
	}

	void transformShortTabAppending(ShortTabAppending_t* tab, str_list& out) {
		if (_withVars.empty()) {
			throw CompileError("short table appending syntax must be called within a with block"sv, tab);
		}
		auto assignment = toAst<ExpListAssign_t>(_withVars.top() + "[]=nil"s, tab);
		assignment->action.set(tab->assign);
		transformAssignment(assignment, out);
	}

	void transformChainAssign(ChainAssign_t* chainAssign, str_list& out) {
		auto x = chainAssign;
		auto value = chainAssign->assign->values.front();
		if (chainAssign->assign->values.size() != 1) {
			throw CompileError("only one right value expected"sv, value);
		}
		str_list temp;
		bool constVal = false;
		if (auto simpleVal = simpleSingleValueFrom(value)) {
			constVal = ast_is<ConstValue_t, Num_t>(simpleVal->value);
		}
		bool localVal = false;
		if (auto var = singleVariableFrom(value, false); isLocal(var)) {
			localVal = true;
		}
		if (!constVal && !localVal) {
			for (auto exp : chainAssign->exprs.objects()) {
				std::string var = singleVariableFrom(exp, false);
				if (!var.empty()) {
					str_list temp;
					transformAssignment(assignmentFrom(static_cast<Exp_t*>(exp), value, exp), temp);
					auto newChainAssign = x->new_ptr<ChainAssign_t>();
					auto newAssign = x->new_ptr<Assign_t>();
					newAssign->values.push_back(exp);
					newChainAssign->assign.set(newAssign);
					for (auto e : chainAssign->exprs.objects()) {
						if (e != exp) {
							newChainAssign->exprs.push_back(e);
						}
					}
					transformChainAssign(newChainAssign, temp);
					out.push_back(join(temp));
					return;
				}
			}
		}
		if (constVal || localVal) {
			for (auto exp : chainAssign->exprs.objects()) {
				transformAssignment(assignmentFrom(static_cast<Exp_t*>(exp), value, exp), temp);
			}
			out.push_back(join(temp));
			return;
		}
		auto valName = getUnusedName("_tmp_");
		auto newValue = toAst<Exp_t>(valName, value);
		ast_list<false, ExpListAssign_t> assignments;
		for (auto exp : chainAssign->exprs.objects()) {
			auto assignment = assignmentFrom(static_cast<Exp_t*>(exp), newValue, exp);
			assignments.push_back(assignment.get());
			temp.push_back(getPreDefineLine(assignment));
		}
		assignments.push_front(assignmentFrom(newValue, value, value));
		temp.push_back(indent() + "do"s + nll(x));
		pushScope();
		for (auto item : assignments.objects()) {
			transformAssignment(static_cast<ExpListAssign_t*>(item), temp);
		}
		popScope();
		temp.push_back(indent() + "end"s + nll(x));
		out.push_back(join(temp));
	}
};

const std::string YueCompilerImpl::Empty;

YueCompiler::YueCompiler(void* sharedState,
	const std::function<void(void*)>& luaOpen,
	bool sameModule)
	:
#ifndef YUE_NO_MACRO
	_compiler(std::make_unique<YueCompilerImpl>(static_cast<lua_State*>(sharedState), luaOpen, sameModule)) {
}
#else
	_compiler(std::make_unique<YueCompilerImpl>()) {
	(void)sharedState;
	(void)luaOpen;
	(void)sameModule;
}
#endif // YUE_NO_MACRO

YueCompiler::~YueCompiler() { }

CompileInfo YueCompiler::compile(std::string_view codes, const YueConfig& config) {
	return _compiler->compile(codes, config);
}

void YueCompiler::clear(void* luaState) {
	auto L = static_cast<lua_State*>(luaState);
	lua_pushliteral(L, YUE_MODULE); // YUE_MODULE
	lua_pushnil(L);
	lua_rawset(L, LUA_REGISTRYINDEX); // reg[YUE_MODULE] = nil
}

} // namespace yue
