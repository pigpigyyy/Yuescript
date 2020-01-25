/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <vector>
#include <numeric>
#include <memory>
#include <sstream>
#include <string_view>
using namespace std::string_view_literals;
#include "MoonP/parser.hpp"
#include "MoonP/moon_ast.h"
#include "MoonP/moon_compiler.h"

namespace MoonP {

#define BLOCK_START do {
#define BLOCK_END } while (false);
#define BREAK_IF(cond) if (cond) break

typedef std::list<std::string> str_list;

inline std::string s(std::string_view sv) {
	return std::string(sv);
}

const char* moonScriptVersion() {
	return "0.5.0";
}

class MoonCompliler {
public:
	std::pair<std::string,std::string> complile(const std::string& codes, const MoonConfig& config) {
		_config = config;
		try {
			_input = _converter.from_bytes(codes);
		} catch (const std::range_error&) {
			return {Empty, "Invalid text encoding."};
		}
		error_list el;
		State st;
		ast_ptr<false, File_t> root;
		try {
			root = parse<File_t>(_input, File, el, &st);
		} catch (const std::logic_error& error) {
			clear();
			return {Empty, error.what()};
		}
		if (root) {
			try {
				str_list out;
				pushScope();
				transformBlock(root->block, out, config.implicitReturnRoot);
				popScope();
				return {std::move(out.back()), Empty};
			} catch (const std::logic_error& error) {
				clear();
				return {Empty, error.what()};
			}
		} else {
			clearBuf();
			for (error_list::iterator it = el.begin(); it != el.end(); ++it) {
				const error& err = *it;
				_buf << debugInfo("Syntax error."sv, &err);
			}
			std::pair<std::string,std::string> result{Empty, clearBuf()};
			clear();
			return result;
		}
	}

	const std::unordered_map<std::string,std::pair<int,int>>& getGlobals() const {
		return _globals;
	}

	void clear() {
		_indentOffset = 0;
		_scopes.clear();
		_codeCache.clear();
		std::stack<std::string> emptyWith;
		_withVars.swap(emptyWith);
		std::stack<std::string> emptyContinue;
		_continueVars.swap(emptyContinue);
		_buf.str("");
		_buf.clear();
		_joinBuf.str("");
		_joinBuf.clear();
		_globals.clear();
		_input.clear();
	}
private:
	MoonConfig _config;
	int _indentOffset = 0;
	Converter _converter;
	input _input;
	std::list<input> _codeCache;
	std::stack<std::string> _withVars;
	std::stack<std::string> _continueVars;
	std::unordered_map<std::string,std::pair<int,int>> _globals;
	std::ostringstream _buf;
	std::ostringstream _joinBuf;
	std::string _newLine = "\n";
	enum class LocalMode {
		None = 0,
		Capital = 1,
		Any = 2
	};
	enum class ExportMode {
		None = 0,
		Capital = 1,
		Any = 2
	};
	struct Scope {
		ExportMode mode = ExportMode::None;
		std::unique_ptr<std::unordered_set<std::string>> vars;
		std::unique_ptr<std::unordered_set<std::string>> allows;
		std::unique_ptr<std::unordered_set<std::string>> exports;
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
		bool isVariable = false;
		std::string name;
		std::string structure;
	};

	struct Destructure {
		std::string value;
		std::list<DestructItem> items;
	};

	enum class ExpUsage {
		Return,
		Assignment,
		Common,
		Closure
	};

	void pushScope() {
		_scopes.emplace_back();
		_scopes.back().vars = std::make_unique<std::unordered_set<std::string>>();
	}

	void popScope() {
		_scopes.pop_back();
	}

	bool isDefined(const std::string& name) {
		bool isDefined = false;
		int mode = int(std::isupper(name[0]) ? ExportMode::Capital : ExportMode::Any);
		const auto& current = _scopes.back();
		if (int(current.mode) >= mode) {
			if (current.exports) {
				if (current.exports->find(name) != current.exports->end()) {
					isDefined = true;
					current.vars->insert(name);
				}
			} else {
				isDefined = true;
				current.vars->insert(name);
			}
		}
		decltype(_scopes.back().allows.get()) allows = nullptr;
		for (auto it  = _scopes.rbegin(); it != _scopes.rend(); ++it) {
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

	bool isSolidDefined(const std::string& name) {
		bool isDefined = false;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			if (vars->find(name) != vars->end()) {
				isDefined = true;
				break;
			}
		}
		return isDefined;
	}

	void markVarShadowed() {
		auto& scope = _scopes.back();
		scope.allows = std::make_unique<std::unordered_set<std::string>>();
	}

	void markVarExported(ExportMode mode, bool specified) {
		auto& scope = _scopes.back();
		scope.mode = mode;
		if (specified && !scope.exports) {
			scope.exports = std::make_unique<std::unordered_set<std::string>>();
		}
	}

	void addExportedVar(const std::string& name) {
		auto& scope = _scopes.back();
		scope.exports->insert(name);
	}

	void addToAllowList(const std::string& name) {
		auto& scope = _scopes.back();
		scope.allows->insert(name);
	}

	void forceAddToScope(const std::string& name) {
		auto& scope = _scopes.back();
		scope.vars->insert(name);
	}

	Scope& currentScope() {
		return _scopes.back();
	}

	bool addToScope(const std::string& name) {
		bool defined = isDefined(name);
		if (!defined) {
			auto& scope = currentScope();
			scope.vars->insert(name);
		}
		return !defined;
	}

	std::string getUnusedName(std::string_view name) {
		int index = 0;
		std::string newName;
		do {
			newName = s(name) + std::to_string(index);
			index++;
		} while (isSolidDefined(newName));
		return newName;
	}

	const std::string nll(ast_node* node) {
		if (_config.reserveLineNumber) {
			return s(" -- "sv) + std::to_string(node->m_begin.m_line) + _newLine;
		} else {
			return _newLine;
		}
	}

	const std::string nlr(ast_node* node) {
		if (_config.reserveLineNumber) {
			return s(" -- "sv) + std::to_string(node->m_end.m_line) + _newLine;
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

	std::string indent() {
		return std::string(_scopes.size() - 1 + _indentOffset, '\t');
	}

	std::string indent(int offset) {
		return std::string(_scopes.size() - 1 + _indentOffset + offset, '\t');
	}

	std::string clearBuf() {
		std::string str = _buf.str();
		_buf.str("");
		_buf.clear();
		return str;
	}

	std::string join(const str_list& items) {
		if (items.empty()) return Empty;
		else if (items.size() == 1) return items.front();
		for (const auto& item : items) {
			_joinBuf << item;
		}
		auto result = _joinBuf.str();
		_joinBuf.str("");
		_joinBuf.clear();
		return result;
	}

	std::string join(const str_list& items, std::string_view sep) {
		if (items.empty()) return Empty;
		else if (items.size() == 1) return items.front();
		std::string sepStr = s(sep);
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

	std::string toString(ast_node* node) {
		return _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
	}

	std::string toString(input::iterator begin, input::iterator end) {
		return _converter.to_bytes(std::wstring(begin, end));
	}

	Value_t* singleValueFrom(ast_node* item) {
		Exp_t* exp = nullptr;
		switch (item->getId()) {
			case "Exp"_id:
				exp = static_cast<Exp_t*>(item);
				break;
			case "ExpList"_id: {
				auto expList = static_cast<ExpList_t*>(item);
				if (expList->exprs.size() == 1) {
					exp = static_cast<Exp_t*>(expList->exprs.front());
				}
				break;
			}
			case "ExpListLow"_id: {
				auto expList = static_cast<ExpListLow_t*>(item);
				if (expList->exprs.size() == 1) {
					exp = static_cast<Exp_t*>(expList->exprs.front());
				}
				break;
			}
		}
		if (!exp) return nullptr;
		if (exp->opValues.empty()) {
			return exp->value.get();
		}
		return nullptr;
	}

	SimpleValue_t* simpleSingleValueFrom(ast_node* expList) {
		auto value = singleValueFrom(expList);
		if (value && value->item.is<SimpleValue_t>()) {
			return static_cast<SimpleValue_t*>(value->item.get());
		}
		return nullptr;
	}

	Value_t* firstValueFrom(ast_node* item) {
		Exp_t* exp = nullptr;
		if (auto expList = ast_cast<ExpList_t>(item)) {
			if (!expList->exprs.empty()) {
				exp = static_cast<Exp_t*>(expList->exprs.front());
			}
		} else {
			exp = ast_cast<Exp_t>(item);
		}
		return exp->value.get();
	}

	Statement_t* lastStatementFrom(Body_t* body) {
		if (auto stmt = body->content.as<Statement_t>()) {
			return stmt;
		} else {
			auto node = body->content.to<Block_t>()->statements.objects().back();
			return static_cast<Statement_t*>(node);
		}
	}

	Statement_t* lastStatementFrom(Block_t* block) {
		auto node = block->statements.objects().back();
		return static_cast<Statement_t*>(node);
	}

	template <class T>
	ast_ptr<false, T> toAst(std::string_view codes, rule& r, ast_node* parent) {
		_codeCache.push_back(_converter.from_bytes(s(codes)));
		error_list el;
		State st;
		auto ptr = parse<T>(_codeCache.back(), r, el, &st);
		ptr->traverse([&](ast_node* node) {
			node->m_begin.m_line = parent->m_begin.m_line;
			node->m_end.m_line = parent->m_begin.m_line;
			return traversal::Continue;
		});
		return ptr;
	}

	bool matchAst(rule& r, std::string_view codes) {
		error_list el;
		State st;
		input i = _converter.from_bytes(s(codes));
		auto rEnd = rule(r >> eof());
		ast_ptr<false, ast_node> result(_parse(i, rEnd, el, &st));
		return result;
	}

	bool isChainValueCall(ChainValue_t* chainValue) {
		return ast_is<InvokeArgs_t, Invoke_t>(chainValue->items.back());
	}

	enum class ChainType {
		Common,
		EndWithColon,
		EndWithEOP,
		HasEOP,
		HasKeyword
	};

	ChainType specialChainValue(ChainValue_t* chainValue) {
		if (ast_is<ColonChainItem_t>(chainValue->items.back())) {
			return ChainType::EndWithColon;
		}
		if (ast_is<existential_op_t>(chainValue->items.back())) {
			return ChainType::EndWithEOP;
		}
		ChainType type = ChainType::Common;
		for (auto item : chainValue->items.objects()) {
			if (auto colonChain = ast_cast<ColonChainItem_t>(item)) {
				if (ast_is<LuaKeyword_t>(colonChain->name)) {
					type = ChainType::HasKeyword;
				}
			} else if (ast_is<existential_op_t>(item)) {
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
			if (auto self = callable->item.as<SelfName_t>()) {
				var = self->name.as<self_t>();
			}
		}
		BREAK_IF(!var);
		str_list tmp;
		transformCallable(callable, tmp);
		return tmp.back();
		BLOCK_END
		return Empty;
	}

	std::string singleVariableFrom(ast_node* expList) {
		if (!ast_is<Exp_t, ExpList_t>(expList)) return Empty;
		BLOCK_START
		auto value = singleValueFrom(expList);
		BREAK_IF(!value);
		auto chainValue = value->getByPath<ChainValue_t>();
		BREAK_IF(!chainValue);
		BREAK_IF(chainValue->items.size() != 1);
		auto callable = ast_cast<Callable_t>(chainValue->items.front());
		BREAK_IF(!callable || !(callable->item.is<Variable_t>() || callable->getByPath<SelfName_t,self_t>()));
		str_list tmp;
		transformCallable(callable, tmp);
		return tmp.back();
		BLOCK_END
		return Empty;
	}

	bool isAssignable(const node_container& chainItems) {
		if (chainItems.size() == 1) {
			 auto firstItem = chainItems.back();
			 if (auto callable = ast_cast<Callable_t>(firstItem)) {
				 switch (callable->item->getId()) {
					 case "Variable"_id:
					 case "SelfName"_id:
						 return true;
				 }
			 } else if (firstItem->getId() == "DotChainItem"_id) {
				 return true;
			 }
		 } else {
			auto lastItem = chainItems.back();
			switch (lastItem->getId()) {
				case "DotChainItem"_id:
				case "Exp"_id:
					return true;
			}
		}
		return false;
	}

	bool isAssignable(Exp_t* exp) {
		if (auto value = singleValueFrom(exp)) {
			auto item = value->item.get();
			switch (item->getId()) {
				case "simple_table"_id:
					return true;
				case "SimpleValue"_id: {
					auto simpleValue = static_cast<SimpleValue_t*>(item);
					if (simpleValue->value.is<TableLit_t>()) {
						return true;
					}
					return false;
				}
				case "ChainValue"_id: {
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
		}
		return true;
	}

	void checkAssignable(ExpList_t* expList) {
		for (auto exp_ : expList->exprs.objects()) {
			Exp_t* exp = static_cast<Exp_t*>(exp_);
			if (!isAssignable(exp)) {
				throw std::logic_error(debugInfo("Left hand expression is not assignable."sv, exp));
			}
		}
	}

	std::string debugInfo(std::string_view msg, const input_range* loc) {
		const int ASCII = 255;
		int length = loc->m_begin.m_line;
		auto begin = _input.begin();
		auto end = _input.end();
		int count = 0;
		for (auto it = _input.begin(); it != _input.end(); ++it) {
			if (*it == '\n') {
				if (count + 1 == length) {
					end = it;
					break;
				} else {
					begin = it + 1;
				}
				count++;
			}
		}
		auto line = _converter.to_bytes(std::wstring(begin, end));
		int oldCol = loc->m_begin.m_col;
		int col = std::max(0, oldCol - 1);
		auto it = begin;
		for (int i = 0; i < oldCol; ++i) {
			if (*it > ASCII) {
				++col;
			}
			++it;
		}
		replace(line, "\t"sv, " "sv);
		std::ostringstream buf;
		buf << loc->m_begin.m_line << ": "sv << msg <<
			'\n' << line << '\n' << std::string(col, ' ') << "^"sv;
		return buf.str();
	}

	void transformStatement(Statement_t* statement, str_list& out) {
		auto x = statement;
		if (statement->appendix) {
			if (auto assignment = assignmentFrom(statement)) {
				auto preDefine = getPredefine(assignment);
				if (!preDefine.empty()) out.push_back(preDefine + nll(statement));
			}
			auto appendix = statement->appendix.get();
			switch (appendix->item->getId()) {
				case "if_else_line"_id: {
					auto if_else_line = appendix->item.to<if_else_line_t>();
					auto ifNode = x->new_ptr<If_t>();

					auto ifCond = x->new_ptr<IfCond_t>();
					ifCond->condition.set(if_else_line->condition);
					ifNode->nodes.push_back(ifCond);

					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					auto body = x->new_ptr<Body_t>();
					body->content.set(stmt);
					ifNode->nodes.push_back(body);

					if (!ast_is<default_value_t>(if_else_line->elseExpr)) {
						auto expList = x->new_ptr<ExpList_t>();
						expList->exprs.push_back(if_else_line->elseExpr);
						auto expListAssign = x->new_ptr<ExpListAssign_t>();
						expListAssign->expList.set(expList);
						auto stmt = x->new_ptr<Statement_t>();
						stmt->content.set(expListAssign);
						auto body = x->new_ptr<Body_t>();
						body->content.set(stmt);
						ifNode->nodes.push_back(body);
					}

					statement->appendix.set(nullptr);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(ifNode);
					auto value = x->new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					statement->content.set(expListAssign);
					break;
				}
				case "unless_line"_id: {
					auto unless_line = appendix->item.to<unless_line_t>();
					auto unless = x->new_ptr<Unless_t>();

					auto ifCond = x->new_ptr<IfCond_t>();
					ifCond->condition.set(unless_line->condition);
					unless->nodes.push_back(ifCond);

					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					auto body = x->new_ptr<Body_t>();
					body->content.set(stmt);
					unless->nodes.push_back(body);

					statement->appendix.set(nullptr);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(unless);
					auto value = x->new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto exprList = x->new_ptr<ExpList_t>();
					exprList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(exprList);
					statement->content.set(expListAssign);
					break;
				}
				case "CompInner"_id: {
					auto compInner = appendix->item.to<CompInner_t>();
					auto comp = x->new_ptr<Comprehension_t>();
					comp->forLoop.set(compInner);
					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					comp->value.set(stmt);
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(comp);
					auto value = x->new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					statement->content.set(expListAssign);
					statement->appendix.set(nullptr);
					break;
				}
				default: break;
			}
		}
		auto content = statement->content.get();
		if (!content) {
			out.push_back(Empty);
			return;
		}
		switch (content->getId()) {
			case "Import"_id: transformImport(static_cast<Import_t*>(content), out); break;
			case "While"_id: transformWhile(static_cast<While_t*>(content), out); break;
			case "For"_id: transformFor(static_cast<For_t*>(content), out); break;
			case "ForEach"_id: transformForEach(static_cast<ForEach_t*>(content), out); break;
			case "Return"_id: transformReturn(static_cast<Return_t*>(content), out); break;
			case "Local"_id: transformLocal(static_cast<Local_t*>(content), out); break;
			case "Export"_id: transformExport(static_cast<Export_t*>(content), out); break;
			case "BreakLoop"_id: transformBreakLoop(static_cast<BreakLoop_t*>(content), out); break;
			case "ExpListAssign"_id: {
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
								case "If"_id: transformIf(static_cast<If_t*>(value), out, ExpUsage::Common); break;
								case "ClassDecl"_id: transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Common); break;
								case "Unless"_id: transformUnless(static_cast<Unless_t*>(value), out, ExpUsage::Common); break;
								case "Switch"_id: transformSwitch(static_cast<Switch_t*>(value), out); break;
								case "With"_id: transformWith(static_cast<With_t*>(value), out); break;
								case "ForEach"_id: transformForEach(static_cast<ForEach_t*>(value), out); break;
								case "For"_id: transformFor(static_cast<For_t*>(value), out); break;
								case "While"_id: transformWhile(static_cast<While_t*>(value), out); break;
								case "Do"_id: transformDo(static_cast<Do_t*>(value), out); break;
								case "Comprehension"_id: transformCompCommon(static_cast<Comprehension_t*>(value), out); break;
								default: specialSingleValue = false; break;
							}
							if (specialSingleValue) {
								break;
							}
						}
						if (auto chainValue = singleValue->item.as<ChainValue_t>()) {
							if (isChainValueCall(chainValue)) {
								transformChainValue(chainValue, out, ExpUsage::Common);
								break;
							}
						}
					}
					throw std::logic_error(debugInfo("Expression list must appear at the end of body block."sv, expList));
				}
				break;
			}
			default: break;
		}
	}

	str_list getAssignVars(ExpListAssign_t* assignment) {
		str_list vars;
		if (!assignment->action.is<Assign_t>()) return vars;
		for (auto exp : assignment->expList->exprs.objects()) {
			auto var = singleVariableFrom(exp);
			vars.push_back(var.empty() ? Empty : var);
		}
		return vars;
	}

	str_list getAssignVars(With_t* with) {
		str_list vars;
		for (auto exp : with->valueList->exprs.objects()) {
			auto var = singleVariableFrom(exp);
			vars.push_back(var.empty() ? Empty : var);
		}
		return vars;
	}

	str_list getAssignDefs(ExpList_t* expList) {
		str_list preDefs;
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
						name = toString(var);
					} else if (auto self = callable->item.as<SelfName_t>()) {
						if (self->name.is<self_t>()) name = "self"sv;
					}
					BREAK_IF(name.empty());
					if (!isDefined(name)) {
						preDefs.push_back(name);
					}
					BLOCK_END
				}
			} else {
				throw std::logic_error(debugInfo("Left hand expression is not assignable."sv, exp));
			}
		}
		return preDefs;
	}

	str_list transformAssignDefs(ExpList_t* expList) {
		str_list preDefs;
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
						name = toString(var);
					} else if (auto self = callable->item.as<SelfName_t>()) {
						if (self->name.is<self_t>()) name = "self"sv;
					}
					BREAK_IF(name.empty());
					if (addToScope(name)) {
						preDefs.push_back(name);
					}
					BLOCK_END
				}
			} else {
				throw std::logic_error(debugInfo("Left hand expression is not assignable."sv, exp));
			}
		}
		return preDefs;
	}

	std::string getPredefine(const str_list& defs) {
		if (defs.empty()) return Empty;
		return indent() + s("local "sv) + join(defs, ", "sv);
	}

	std::string getDestrucureDefine(ExpListAssign_t* assignment) {
		auto info = extractDestructureInfo(assignment, true);
		if (!info.first.empty()) {
			for (const auto& destruct : info.first) {
				str_list defs;
				for (const auto& item : destruct.items) {
					if (item.isVariable && addToScope(item.name)) {
						defs.push_back(item.name);
					}
				}
				if (!defs.empty()) _buf << indent() << "local "sv << join(defs,", "sv);
			}
		}
		return clearBuf();
	}

	std::string getPredefine(ExpListAssign_t* assignment) {
		auto preDefine = getDestrucureDefine(assignment);
		if (preDefine.empty()) {
			preDefine = getPredefine(transformAssignDefs(assignment->expList));
		}
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

	void assignLastExplist(ExpList_t* expList, Body_t* body) {
		auto last = lastStatementFrom(body);
		if (!last) return;
		bool lastAssignable = expListFrom(last) || ast_is<For_t, ForEach_t, While_t>(last->content);
		if (lastAssignable) {
			auto x = last;
			auto newAssignment = x->new_ptr<ExpListAssign_t>();
			newAssignment->expList.set(expList);
			auto assign = x->new_ptr<Assign_t>();
			if (auto valueList = last->content.as<ExpListAssign_t>()) {
				assign->values.dup(valueList->expList->exprs);
			} else {
				auto simpleValue = x->new_ptr<SimpleValue_t>();
				simpleValue->value.set(last->content);
				auto value = x->new_ptr<Value_t>();
				value->item.set(simpleValue);
				auto exp = x->new_ptr<Exp_t>();
				exp->value.set(value);
				assign->values.push_back(exp);
			}
			newAssignment->action.set(assign);
			last->content.set(newAssignment);
		}
	}

	void transformAssignment(ExpListAssign_t* assignment, str_list& out) {
		checkAssignable(assignment->expList);
		BLOCK_START
		auto assign = ast_cast<Assign_t>(assignment->action);
		BREAK_IF(!assign || assign->values.objects().size() != 1);
		auto value = assign->values.objects().front();
		if (ast_is<Exp_t>(value)) {
			if (auto val = simpleSingleValueFrom(value)) {
				value = val->value.get();
			}
		}
		switch (value->getId()) {
			case "If"_id:
			case "Unless"_id: {
				auto expList = assignment->expList.get();
				str_list temp;
				auto defs = transformAssignDefs(expList);
				if (!defs.empty()) temp.push_back(getPredefine(defs) + nll(expList));
				value->traverse([&](ast_node* node) {
					switch (node->getId()) {
						case "IfCond"_id: return traversal::Return;
						case "Body"_id: {
							auto body = static_cast<Body_t*>(node);
							assignLastExplist(expList, body);
							return traversal::Return;
						}
						default: return traversal::Continue;
					}
				});
				switch (value->getId()) {
					case "If"_id: transformIf(static_cast<If_t*>(value), temp, ExpUsage::Common); break;
					case "Unless"_id: transformUnless(static_cast<Unless_t*>(value), temp, ExpUsage::Common); break;
				}
				out.push_back(join(temp));
				return;
			}
			case "Switch"_id: {
				auto switchNode = static_cast<Switch_t*>(value);
				auto expList = assignment->expList.get();
				for (auto branch_ : switchNode->branches.objects()) {
					auto branch = static_cast<SwitchCase_t*>(branch_);
					assignLastExplist(expList, branch->body);
				}
				if (switchNode->lastBranch) {
					assignLastExplist(expList, switchNode->lastBranch);
				}
				std::string preDefine = getPredefine(assignment);
				transformSwitch(switchNode, out);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "With"_id: {
				auto withNode = static_cast<With_t*>(value);
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformWith(withNode, out, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "Do"_id: {
				auto expList = assignment->expList.get();
				auto doNode = static_cast<Do_t*>(value);
				assignLastExplist(expList, doNode->body);
				std::string preDefine = getPredefine(assignment);
				transformDo(doNode, out);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "Comprehension"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "TblComprehension"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "For"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformForInPlace(static_cast<For_t*>(value), out, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "ForEach"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformForEachInPlace(static_cast<ForEach_t*>(value), out, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "ClassDecl"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Assignment, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
			case "While"_id: {
				auto expList = assignment->expList.get();
				std::string preDefine = getPredefine(assignment);
				transformWhileInPlace(static_cast<While_t*>(value), out, expList);
				out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
				return;
			}
		}
		auto exp = ast_cast<Exp_t>(value);
		BREAK_IF(!exp);
		if (auto chainValue = exp->value->item.as<ChainValue_t>()) {
			auto type = specialChainValue(chainValue);
			auto expList = assignment->expList.get();
			switch (type) {
				case ChainType::HasEOP:
				case ChainType::EndWithColon: {
					std::string preDefine = getPredefine(assignment);
					transformChainValue(chainValue, out, ExpUsage::Assignment, expList);
					out.back().insert(0, preDefine.empty() ? Empty : preDefine + nll(assignment));
					return;
				}
				case ChainType::HasKeyword:
					transformChainValue(chainValue, out, ExpUsage::Assignment, expList);
					return;
				case ChainType::Common:
				case ChainType::EndWithEOP:
					break;
			}
		}
		BLOCK_END
		auto info = extractDestructureInfo(assignment);
		if (info.first.empty()) {
			transformAssignmentCommon(assignment, out);
		} else {
			str_list temp;
			for (const auto& destruct : info.first) {
				if (destruct.items.size() == 1) {
					auto& pair = destruct.items.front();
					_buf << indent();
					if (pair.isVariable && !isDefined(pair.name)) {
						_buf << s("local "sv);
					}
					_buf << pair.name << " = "sv << info.first.front().value << pair.structure << nll(assignment);
					addToScope(pair.name);
					temp.push_back(clearBuf());
				} else if (matchAst(Name, destruct.value)) {
					str_list defs, names, values;
					for (const auto& item : destruct.items) {
						if (item.isVariable && addToScope(item.name)) {
							defs.push_back(item.name);
						}
						names.push_back(item.name);
						values.push_back(item.structure);
					}
					for (auto& v : values) v.insert(0, destruct.value);
					if (defs.empty()) {
						_buf << indent() << join(names, ", "sv) << " = "sv << join(values, ", "sv) << nll(assignment);
					} else {
						_buf << indent() << "local "sv;
						if (defs.size() != names.size()) {
							_buf << join(defs,", "sv) << nll(assignment) << indent();
						}
						_buf << join(names, ", "sv) << " = "sv << join(values, ", "sv) << nll(assignment);
					}
					temp.push_back(clearBuf());
				} else {
					str_list defs, names, values;
					for (const auto& item : destruct.items) {
						if (item.isVariable && addToScope(item.name)) {
							defs.push_back(item.name);
						}
						names.push_back(item.name);
						values.push_back(item.structure);
					}
					if (!defs.empty()) _buf << indent() << "local "sv << join(defs,", "sv) << nll(assignment);
					_buf << indent() << "do"sv << nll(assignment);
					pushScope();
					auto objVar = getUnusedName("_obj_");
					for (auto& v : values) v.insert(0, objVar);
					_buf << indent() << "local "sv << objVar << " = "sv << destruct.value << nll(assignment);
					_buf << indent() << join(names, ", "sv) << " = "sv << join(values, ", "sv) << nll(assignment);
					popScope();
					_buf << indent() << "end"sv << nll(assignment);
					temp.push_back(clearBuf());
				}
			}
			if (info.second) {
				transformAssignmentCommon(info.second, temp);
			}
			out.push_back(join(temp));
		}
	}

	void transformAssignItem(ast_node* value, str_list& out) {
		switch (value->getId()) {
			case "With"_id: transformWithClosure(static_cast<With_t*>(value), out); break;
			case "If"_id: transformIf(static_cast<If_t*>(value), out, ExpUsage::Closure); break;
			case "Switch"_id: transformSwitchClosure(static_cast<Switch_t*>(value), out); break;
			case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(value), out); break;
			case "Exp"_id: transformExp(static_cast<Exp_t*>(value), out); break;
			default: break;
		}
	}

	std::list<DestructItem> destructFromExp(ast_node* node) {
		const node_container* tableItems = nullptr;
		if (ast_cast<Exp_t>(node)) {
			auto item = singleValueFrom(node)->item.get();
			if (!item) throw std::logic_error(debugInfo("Invalid destructure value."sv, node));
			auto tbA = item->getByPath<TableLit_t>();
			if (tbA) {
				tableItems = &tbA->values.objects();
			} else {
				auto tbB = ast_cast<simple_table_t>(item);
				if (tbB) tableItems = &tbB->pairs.objects();
			}
		} else if (auto table = ast_cast<TableBlock_t>(node)) {
			tableItems = &table->values.objects();
		}
		std::list<DestructItem> pairs;
		int index = 0;
		for (auto pair : *tableItems) {
			switch (pair->getId()) {
				case "Exp"_id: {
					++index;
					if (!isAssignable(static_cast<Exp_t*>(pair)))  {
						throw std::logic_error(debugInfo("Can't destructure value."sv, pair));
					}
					auto value = singleValueFrom(pair);
					auto item = value->item.get();
					if (ast_cast<simple_table_t>(item) ||
						item->getByPath<TableLit_t>()) {
						auto subPairs = destructFromExp(pair);
						for (auto& p : subPairs) {
							pairs.push_back({p.isVariable, p.name,
								s("["sv) + std::to_string(index) + s("]"sv) + p.structure});
						}
					} else {
						bool lintGlobal = _config.lintGlobalVariable;
						_config.lintGlobalVariable = false;
						auto exp = static_cast<Exp_t*>(pair);
						auto varName = singleVariableFrom(exp);
						bool isVariable = !varName.empty();
						if (!isVariable) {
							str_list temp;
							transformExp(exp, temp);
							varName = std::move(temp.back());
						}
						_config.lintGlobalVariable = lintGlobal;
						pairs.push_back({
							isVariable,
							varName,
							s("["sv) + std::to_string(index) + s("]"sv)
						});
					}
					break;
				}
				case "variable_pair"_id: {
					auto vp = static_cast<variable_pair_t*>(pair);
					auto name = toString(vp->name);
					if (State::keywords.find(name) != State::keywords.end()) {
						pairs.push_back({true, name, s("[\""sv) + name + s("\"]"sv)});
					} else {
						pairs.push_back({true, name, s("."sv) + name});
					}
					break;
				}
				case "normal_pair"_id: {
					auto np = static_cast<normal_pair_t*>(pair);
					auto key = np->key->getByPath<Name_t>();
					if (!key) throw std::logic_error(debugInfo("Invalid key for destructure."sv, np));
					if (auto exp = np->value.as<Exp_t>()) {
						if (!isAssignable(exp)) throw std::logic_error(debugInfo("Can't destructure value."sv, exp));
						auto item = singleValueFrom(exp)->item.get();
						if (ast_cast<simple_table_t>(item) ||
							item->getByPath<TableLit_t>()) {
							auto subPairs = destructFromExp(exp);
							auto name = toString(key);
							for (auto& p : subPairs) {
								if (State::keywords.find(name) != State::keywords.end()) {
									pairs.push_back({p.isVariable, p.name,
										s("[\""sv) + name + s("\"]"sv) + p.structure});
								} else {
									pairs.push_back({p.isVariable, p.name,
										s("."sv) + name + p.structure});
								}
							}
						} else {
							bool lintGlobal = _config.lintGlobalVariable;
							_config.lintGlobalVariable = false;
							auto varName = singleVariableFrom(exp);
							bool isVariable = !varName.empty();
							if (!isVariable) {
								str_list temp;
								transformExp(exp, temp);
								varName = std::move(temp.back());
							}
							_config.lintGlobalVariable = lintGlobal;
							auto name = toString(key);
							if (State::keywords.find(name) != State::keywords.end()) {
								pairs.push_back({
									isVariable,
									varName,
									s("[\""sv) + name + s("\"]"sv)
								});
							} else {
								pairs.push_back({
									isVariable,
									varName,
									s("."sv) + name
								});
							}
						}
						break;
					}
					if (np->value.as<TableBlock_t>()) {
						auto subPairs = destructFromExp(pair);
						for (auto& p : subPairs) {
							pairs.push_back({p.isVariable, p.name,
								s("."sv) + toString(key) + p.structure});
						}
					}
					break;
				}
			}
		}
		return pairs;
	}

	std::pair<std::list<Destructure>, ast_ptr<false, ExpListAssign_t>>
		extractDestructureInfo(ExpListAssign_t* assignment, bool varDefOnly = false) {
		auto x = assignment;
		std::list<Destructure> destructs;
		if (!assignment->action.is<Assign_t>()) return { destructs, nullptr };
		auto exprs = assignment->expList->exprs.objects();
		auto values = assignment->action.to<Assign_t>()->values.objects();
		size_t size = std::max(exprs.size(),values.size());
		ast_ptr<false, Exp_t> var;
		if (exprs.size() < size) {
			var = toAst<Exp_t>("_"sv, Exp, x);
			while (exprs.size() < size) exprs.emplace_back(var);
		}
		ast_ptr<false, Exp_t> nullNode;
		if (values.size() < size) {
			nullNode = toAst<Exp_t>("nil"sv, Exp, x);
			while (values.size() < size) values.emplace_back(nullNode);
		}
		using iter = node_container::iterator;
		std::vector<std::pair<iter,iter>> destructPairs;
		str_list temp;
		for (auto i = exprs.begin(), j = values.begin(); i != exprs.end(); ++i, ++j) {
			auto expr = *i;
			ast_node* destructNode = expr->getByPath<Value_t, SimpleValue_t, TableLit_t>();
			if (destructNode || (destructNode = expr->getByPath<Value_t, simple_table_t>())) {
				destructPairs.push_back({i,j});
				auto& destruct = destructs.emplace_back();
				if (!varDefOnly) {
					pushScope();
					transformAssignItem(*j, temp);
					destruct.value = temp.back();
					temp.pop_back();
					popScope();
				}
				auto pairs = destructFromExp(expr);
				destruct.items = std::move(pairs);
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
		return {std::move(destructs), newAssignment};
	}

	void transformAssignmentCommon(ExpListAssign_t* assignment, str_list& out) {
		auto x = assignment;
		str_list temp;
		auto expList = assignment->expList.get();
		auto action = assignment->action.get();
		switch (action->getId()) {
			case "Update"_id: {
				if (expList->exprs.size() > 1) throw std::logic_error(debugInfo("Can not apply update to multiple values."sv, expList));
				auto update = static_cast<Update_t*>(action);
				auto leftExp = static_cast<Exp_t*>(expList->exprs.objects().front());
				auto leftValue = singleValueFrom(leftExp);
				if (!leftValue) throw std::logic_error(debugInfo("Left hand expression is not assignable."sv, leftExp));
				if (auto chain = leftValue->getByPath<ChainValue_t>()) {
					auto tmpChain = x->new_ptr<ChainValue_t>();
					for (auto item : chain->items.objects()) {
						bool itemAdded = false;
						BLOCK_START
						auto exp = ast_cast<Exp_t>(item);
						BREAK_IF(!exp);
						auto var = singleVariableFrom(exp);
						BREAK_IF(!var.empty());
						auto upVar = getUnusedName("_update_"sv);
						auto assignment = x->new_ptr<ExpListAssign_t>();
						assignment->expList.set(toAst<ExpList_t>(upVar, ExpList, x));
						auto assign = x->new_ptr<Assign_t>();
						assign->values.push_back(exp);
						assignment->action.set(assign);
						transformAssignment(assignment, temp);
						tmpChain->items.push_back(toAst<Exp_t>(upVar, Exp, x));
						itemAdded = true;
						BLOCK_END
						if (!itemAdded) tmpChain->items.push_back(item);
					}
					chain->items.clear();
					chain->items.dup(tmpChain->items);
				}
				transformValue(leftValue, temp);
				auto left = std::move(temp.back());
				temp.pop_back();
				transformExp(update->value, temp);
				auto right = std::move(temp.back());
				temp.pop_back();
				if (!singleValueFrom(update->value)) {
					right = s("("sv) + right + s(")"sv);
				}
				_buf << join(temp) << indent() << left << " = "sv << left <<
					" "sv << toString(update->op) << " "sv << right << nll(assignment);
				out.push_back(clearBuf());
				break;
			}
			case "Assign"_id: {
				auto defs = getAssignDefs(expList);
				bool oneLined = defs.size() == expList->exprs.objects().size() &&
					traversal::Stop != action->traverse([&](ast_node* n) {
					switch (n->getId()) {
						case "Callable"_id: {
							auto callable = static_cast<Callable_t*>(n);
							switch (callable->item->getId()) {
								case "Variable"_id:
									for (const auto& def : defs) {
										if (def == toString(callable->item)) {
											return traversal::Stop;
										}
									}
									return traversal::Return;
								case "SelfName"_id:
									for (const auto& def : defs) {
										if (def == "self"sv) {
											return traversal::Stop;
										}
									}
									return traversal::Return;
								default:
									return traversal::Continue;
							}
						}
						default:
							return traversal::Continue;
					}
				});
				if (oneLined) {
					auto assign = static_cast<Assign_t*>(action);
					for (auto value : assign->values.objects()) {
						transformAssignItem(value, temp);
					}
					std::string preDefine = getPredefine(defs);
					for (const auto& def : defs) {
						addToScope(def);
					}
					if (preDefine.empty()) {
						transformExpList(expList, temp);
						std::string left = std::move(temp.back());
						temp.pop_back();
						out.push_back(indent() + left + s(" = "sv) + join(temp, ", "sv) + nll(assignment));
					} else {
						out.push_back(preDefine + s(" = "sv) + join(temp, ", "sv) + nll(assignment));
					}
				}
				else {
					std::string preDefine = getPredefine(defs);
					for (const auto& def : defs) {
						addToScope(def);
					}
					transformExpList(expList, temp);
					std::string left = temp.back();
					temp.pop_back();
					auto assign = static_cast<Assign_t*>(action);
					for (auto value : assign->values.objects()) {
						transformAssignItem(value, temp);
					}
					out.push_back((preDefine.empty() ? Empty : preDefine + nll(assignment)) + indent() + left + s(" = "sv) + join(temp, ", "sv) + nll(assignment));
				}
				break;
			}
			default: break;
		}
	}

	void transformCond(const node_container& nodes, str_list& out, ExpUsage usage, bool unless = false) {
		std::vector<ast_ptr<false, ast_node>> ns(false);
		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
			ns.push_back(*it);
			if (auto cond = ast_cast<IfCond_t>(*it)) {
				if (*it != nodes.front() && cond->assign) {
					auto x = *it;
					auto newIf = x->new_ptr<If_t>();
					for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
						newIf->nodes.push_back(*j);
					}
					ns.clear();
					auto simpleValue = x->new_ptr<SimpleValue_t>();
					simpleValue->value.set(newIf);
					auto value = x->new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto expListAssign = x->new_ptr<ExpListAssign_t>();
					expListAssign->expList.set(expList);
					auto stmt = x->new_ptr<Statement_t>();
					stmt->content.set(expListAssign);
					auto body = x->new_ptr<Body_t>();
					body->content.set(stmt);
					ns.push_back(body.get());
				}
			}
		}
		if (nodes.size() != ns.size()) {
			auto x = ns.back();
			auto newIf = x->new_ptr<If_t>();
			for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
				newIf->nodes.push_back(*j);
			}
			transformCond(newIf->nodes.objects(), out, usage, unless);
			return;
		}
		str_list temp;
		if (usage == ExpUsage::Closure) {
			temp.push_back(s("(function()"sv) + nll(nodes.front()));
			pushScope();
		}
		std::list<std::pair<IfCond_t*, Body_t*>> ifCondPairs;
		ifCondPairs.emplace_back();
		for (auto node : nodes) {
			switch (node->getId()) {
				case "IfCond"_id:
					ifCondPairs.back().first = static_cast<IfCond_t*>(node);
					break;
				case "Body"_id:
					ifCondPairs.back().second = static_cast<Body_t*>(node);
					ifCondPairs.emplace_back();
					break;
				default: break;
			}
		}
		auto assign = ifCondPairs.front().first->assign.get();
		bool storingValue = false;
		ast_ptr<false, ExpListAssign_t> extraAssignment;
		if (assign) {
			auto exp = ifCondPairs.front().first->condition.get();
			auto x = exp;
			auto var = singleVariableFrom(exp);
			if (var.empty()) {
				storingValue = true;
				auto desVar = getUnusedName("_des_");
				if (assign->values.objects().size() == 1) {
					auto var = singleVariableFrom(assign->values.objects().front());
					if (!var.empty()) {
						desVar = var;
						storingValue = false;
					}
				}
				if (storingValue) {
					if (usage != ExpUsage::Closure) {
						temp.push_back(indent() + s("do"sv) + nll(assign));
						pushScope();
					}
					auto expList = toAst<ExpList_t>(desVar, ExpList, x);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(expList);
					assignment->action.set(assign);
					transformAssignment(assignment, temp);
				}
				{
					auto expList = x->new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto assignOne = x->new_ptr<Assign_t>();
					auto valExp = toAst<Exp_t>(desVar, Exp, x);
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
						temp.push_back(indent() + s("do"sv) + nll(assign));
						pushScope();
					}
				}
				auto expList = x->new_ptr<ExpList_t>();
				expList->exprs.push_back(exp);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(expList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
		}
		for (const auto& pair : ifCondPairs) {
			if (pair.first) {
				str_list tmp;
				auto condition = pair.first->condition.get();
				if (unless) {
					if (auto value = singleValueFrom(condition)) {
						transformValue(value, tmp);
					} else {
						transformExp(condition, tmp);
						tmp.back() = s("("sv) + tmp.back() + s(")"sv);
					}
					tmp.back().insert(0, s("not "sv));
					unless = false;
				} else {
					transformExp(condition, tmp);
				}
				_buf << indent();
				if (pair != ifCondPairs.front()) {
					_buf << "else"sv;
				}
				_buf << "if "sv << tmp.back() << " then"sv << nll(condition);
				temp.push_back(clearBuf());
			}
			if (pair.second) {
				if (!pair.first) {
					temp.push_back(indent() + s("else"sv) + nll(pair.second));
				}
				pushScope();
				if (pair == ifCondPairs.front() && extraAssignment) {
					transformAssignment(extraAssignment, temp);
				}
				transformBody(pair.second, temp, usage != ExpUsage::Common);
				popScope();
			}
			if (!pair.first) {
				temp.push_back(indent() + s("end"sv) + nll(nodes.front()));
				break;
			}
		}
		if (storingValue && usage != ExpUsage::Closure) {
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(nodes.front()));
		}
		if (usage == ExpUsage::Closure) {
			popScope();
			temp.push_back(indent() + s("end)()"sv));
		}
		out.push_back(join(temp));
	}

	void transformIf(If_t* ifNode, str_list& out, ExpUsage usage) {
		transformCond(ifNode->nodes.objects(), out, usage);
	}

	void transformUnless(Unless_t* unless, str_list& out, ExpUsage usage) {
		transformCond(unless->nodes.objects(), out, usage, true);
	}

	void transformExpList(ExpList_t* expList, str_list& out) {
		str_list temp;
		for (auto exp : expList->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformExpListLow(ExpListLow_t* expListLow, str_list& out) {
		str_list temp;
		for (auto exp : expListLow->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformExp(Exp_t* exp, str_list& out) {
		str_list temp;
		transformValue(exp->value, temp);
		for (auto _opValue : exp->opValues.objects()) {
			auto opValue = static_cast<exp_op_value_t*>(_opValue);
			transformBinaryOperator(opValue->op, temp);
			transformValue(opValue->value, temp);
		}
		out.push_back(join(temp, " "sv));
	}

	void transformValue(Value_t* value, str_list& out) {
		auto item = value->item.get();
		switch (item->getId()) {
			case "SimpleValue"_id: transformSimpleValue(static_cast<SimpleValue_t*>(item), out); break;
			case "simple_table"_id: transform_simple_table(static_cast<simple_table_t*>(item), out); break;
			case "ChainValue"_id: {
				auto chainValue = static_cast<ChainValue_t*>(item);
				transformChainValue(chainValue, out, ExpUsage::Closure);
				break;
			}
			case "String"_id: transformString(static_cast<String_t*>(item), out); break;
			default: break;
		}
	}

	void transformCallable(Callable_t* callable, str_list& out, const ast_sel<false,Invoke_t,InvokeArgs_t>& invoke = {}) {
		auto item = callable->item.get();
		switch (item->getId()) {
			case "Variable"_id: {
				transformVariable(static_cast<Variable_t*>(item), out);
				if (_config.lintGlobalVariable && !isDefined(out.back())) {
					if (_globals.find(out.back()) == _globals.end()) {
						_globals[out.back()] = {item->m_begin.m_line, item->m_begin.m_col};
					}
				}
				break;
			}
			case "SelfName"_id: {
				transformSelfName(static_cast<SelfName_t*>(item), out, invoke);
				if (_config.lintGlobalVariable) {
					std::string self("self"sv);
					if (!isDefined(self)) {
						if (_globals.find(self) == _globals.end()) {
							_globals[self] = {item->m_begin.m_line, item->m_begin.m_col};
						}
					}
				}
				break;
			}
			case "VarArg"_id: out.push_back(s("..."sv)); break;
			case "Parens"_id: transformParens(static_cast<Parens_t*>(item), out); break;
			default: break;
		}
	}

	void transformParens(Parens_t* parans, str_list& out) {
		str_list temp;
		transformExp(parans->expr, temp);
		out.push_back(s("("sv) + temp.front() + s(")"sv));
	}

	void transformSimpleValue(SimpleValue_t* simpleValue, str_list& out) {
		auto value = simpleValue->value.get();
		switch (value->getId()) {
			case "const_value"_id: transform_const_value(static_cast<const_value_t*>(value), out); break;
			case "If"_id: transformIf(static_cast<If_t*>(value), out, ExpUsage::Closure); break;
			case "Unless"_id: transformUnless(static_cast<Unless_t*>(value), out, ExpUsage::Closure); break;
			case "Switch"_id: transformSwitchClosure(static_cast<Switch_t*>(value), out); break;
			case "With"_id: transformWithClosure(static_cast<With_t*>(value), out); break;
			case "ClassDecl"_id: transformClassDeclClosure(static_cast<ClassDecl_t*>(value), out); break;
			case "ForEach"_id: transformForEachClosure(static_cast<ForEach_t*>(value), out); break;
			case "For"_id: transformForClosure(static_cast<For_t*>(value), out); break;
			case "While"_id: transformWhileClosure(static_cast<While_t*>(value), out); break;
			case "Do"_id: transformDoClosure(static_cast<Do_t*>(value), out); break;
			case "unary_exp"_id: transform_unary_exp(static_cast<unary_exp_t*>(value), out); break;
			case "TblComprehension"_id: transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Closure); break;
			case "TableLit"_id: transformTableLit(static_cast<TableLit_t*>(value), out); break;
			case "Comprehension"_id: transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Closure); break;
			case "FunLit"_id: transformFunLit(static_cast<FunLit_t*>(value), out); break;
			case "Num"_id: transformNum(static_cast<Num_t*>(value), out); break;
			default: break;
		}
	}

	void transformFunLit(FunLit_t* funLit, str_list& out) {
		str_list temp;
		bool isFatArrow = toString(funLit->arrow) == "=>"sv;
		pushScope();
		if (isFatArrow) {
			forceAddToScope(s("self"sv));
		}
		if (auto argsDef = funLit->argsDef.get()) {
			transformFnArgsDef(argsDef, temp);
			if (funLit->body) {
				transformBody(funLit->body, temp, true);
			} else {
				temp.push_back(Empty);
			}
			auto it = temp.begin();
			auto& args = *it;
			auto& initArgs = *(++it);
			auto& bodyCodes = *(++it);
			_buf << "function("sv <<
				(isFatArrow ? s("self, "sv) : Empty) <<
				args << ')';
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
				transformBody(funLit->body, temp, true);
			} else {
				temp.push_back(Empty);
			}
			auto& bodyCodes = temp.back();
			_buf << "function("sv <<
				(isFatArrow ? s("self"sv) : Empty) <<
				')';
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
	}

	void transformCodes(const node_container& nodes, str_list& out, bool implicitReturn) {
		LocalMode mode = LocalMode::None;
		Local_t* any = nullptr, *capital = nullptr;
		for (auto node : nodes) {
			auto stmt = static_cast<Statement_t*>(node);
			if (auto local = stmt->content.as<Local_t>()) {
				if (auto flag = local->name.as<local_flag_t>()) {
					LocalMode newMode = toString(flag) == "*"sv ? LocalMode::Any : LocalMode::Capital;
					if (int(newMode) > int(mode)) {
						mode = newMode;
					}
					if (mode == LocalMode::Any) {
						if (!any) any = local;
						if (!capital) capital = local;
					} else {
						if (!capital) capital = local;
					}
				} else {
					auto names = local->name.to<NameList_t>();
					for (auto name : names->names.objects()) {
						local->forceDecls.push_back(toString(name));
					}
				}
			} else if (mode != LocalMode::None) {
				ClassDecl_t* classDecl = nullptr;
				if (auto assignment = assignmentFrom(stmt)) {
					auto vars = getAssignVars(assignment);
					for (const auto& var : vars) {
						if (var.empty()) continue;
						if (std::isupper(var[0]) && capital) {
							capital->decls.push_back(var);
						} else if (any) {
							any->decls.push_back(var);
						}
					}
					auto info = extractDestructureInfo(assignment, true);
					if (!info.first.empty()) {
						for (const auto& destruct : info.first)
							for (const auto& item : destruct.items)
								if (item.isVariable) {
									if (std::isupper(item.name[0]) && capital) { capital->decls.push_back(item.name);
									} else if (any) {
										any->decls.push_back(item.name);
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
					classDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
					BLOCK_END
				} else if (auto expList = expListFrom(stmt)) {
					auto value = singleValueFrom(expList);
					classDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
				}
				if (classDecl) {
					if (auto variable = classDecl->name->item.as<Variable_t>()) {
						auto className = toString(variable);
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
		if (implicitReturn) {
			auto last = static_cast<Statement_t*>(nodes.back());
			auto x = last;
			BLOCK_START
			auto expList = expListFrom(last);
			BREAK_IF(!expList ||
				(last->appendix &&
					last->appendix->item.is<CompInner_t>()));
			auto expListLow = x->new_ptr<ExpListLow_t>();
			expListLow->exprs.dup(expList->exprs);
			auto returnNode = x->new_ptr<Return_t>();
			returnNode->valueList.set(expListLow);
			last->content.set(returnNode);
			BLOCK_END
		}
		str_list temp;
		for (auto node : nodes) {
			transformStatement(static_cast<Statement_t*>(node), temp);
		}
		out.push_back(join(temp));
	}

	void transformBody(Body_t* body, str_list& out, bool implicitReturn = false) {
		if (auto stmt = body->content.as<Statement_t>()) {
			transformCodes(node_container{stmt}, out, implicitReturn);
		} else {
			transformCodes(body->content.to<Block_t>()->statements.objects(), out, implicitReturn);
		}
	}

	void transformBlock(Block_t* block, str_list& out, bool implicitReturn = true) {
		transformCodes(block->statements.objects(), out, implicitReturn);
	}

	void transformReturn(Return_t* returnNode, str_list& out) {
		if (auto valueList = returnNode->valueList.get()) {
			if (auto singleValue = singleValueFrom(valueList)) {
				if (auto simpleValue = singleValue->item.as<SimpleValue_t>()) {
					auto value = simpleValue->value.get();
					switch (value->getId()) {
						case "Comprehension"_id:
							transformComprehension(static_cast<Comprehension_t*>(value), out, ExpUsage::Return);
							return;
						case "TblComprehension"_id:
							transformTblComprehension(static_cast<TblComprehension_t*>(value), out, ExpUsage::Return);
							return;
						case "With"_id:
							transformWith(static_cast<With_t*>(value), out, nullptr, true);
							return;
						case "ClassDecl"_id:
							transformClassDecl(static_cast<ClassDecl_t*>(value), out, ExpUsage::Return);
							return;
						case "Do"_id:
							transformDo(static_cast<Do_t*>(value), out, true);
							return;
						case "Switch"_id:
							transformSwitch(static_cast<Switch_t*>(value), out, true);
							return;
						case "While"_id:
							transformWhileInPlace(static_cast<While_t*>(value), out);
							return;
						case "For"_id:
							transformForInPlace(static_cast<For_t*>(value), out);
							return;
						case "ForEach"_id:
							transformForEachInPlace(static_cast<ForEach_t*>(value), out);
							return;
						case "If"_id:
							transformIf(static_cast<If_t*>(value), out, ExpUsage::Return);
							return;
						case "Unless"_id:
							transformUnless(static_cast<Unless_t*>(value), out, ExpUsage::Return);
							return;
					}
				}
				if (auto chainValue = singleValue->item.as<ChainValue_t>()) {
					if (specialChainValue(chainValue) != ChainType::Common) {
						transformChainValue(chainValue, out, ExpUsage::Return);
						return;
					}
				}
				transformValue(singleValue, out);
				out.back() = indent() + s("return "sv) + out.back() + nlr(returnNode);
				return;
			} else {
				str_list temp;
				transformExpListLow(valueList, temp);
				out.push_back(indent() + s("return "sv) + temp.back() + nlr(returnNode));
			}
		} else {
			out.push_back(indent() + s("return"sv) + nll(returnNode));
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

	void transform_outer_var_shadow(outer_var_shadow_t* shadow) {
		markVarShadowed();
		if (shadow->varList) {
			for (auto name : shadow->varList->names.objects()) {
				addToAllowList(toString(name));
			}
		}
	}

	void transformFnArgDefList(FnArgDefList_t* argDefList, str_list& out) {
		auto x = argDefList;
		struct ArgItem {
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
				case "Variable"_id: arg.name = toString(def->name); break;
				case "SelfName"_id: {
					assignSelf = true;
					auto selfName = static_cast<SelfName_t*>(def->name.get());
					switch (selfName->name->getId()) {
						case "self_class_name"_id: {
							auto clsName = static_cast<self_class_name_t*>(selfName->name.get());
							arg.name = toString(clsName->name);
							arg.assignSelf = s("self.__class."sv) + arg.name;
							break;
						}
						case "self_class"_id:
							arg.name = "self.__class"sv;
							break;
						case "self_name"_id: {

							auto sfName = static_cast<self_name_t*>(selfName->name.get());
							arg.name = toString(sfName->name);
							arg.assignSelf = s("self."sv) + arg.name;
							break;
						}
						case "self"_id:
							arg.name = "self"sv;
							break;
						default: break;
					}
					break;
				}
			}
			forceAddToScope(arg.name);
			if (def->defaultValue) {
				pushScope();
				auto expList = toAst<ExpList_t>(arg.name, ExpList, x);
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
			if (varNames.empty()) varNames = arg.name;
			else varNames.append(s(", "sv) + arg.name);
		}
		if (argDefList->varArg) {
			auto& arg = argItems.emplace_back();
			arg.name = "..."sv;
			if (varNames.empty()) varNames = arg.name;
			else varNames.append(s(", "sv) + arg.name);
		}
		std::string initCodes = join(temp);
		if (assignSelf) {
			auto sjoin = [](const decltype(argItems)& items, int index) {
				std::string result;
				for (auto it = items.begin(); it != items.end(); ++it) {
					if (it->assignSelf.empty()) continue;
					if (result.empty()) result = (&it->name)[index];
					else result.append(s(", "sv) + (&it->name)[index]);
				}
				return result;
			};
			std::string sleft = sjoin(argItems, 1);
			std::string sright = sjoin(argItems, 0);
			initCodes.append(indent() + sleft + s(" = "sv) + sright + nll(argDefList));
		}
		out.push_back(varNames);
		out.push_back(initCodes);
	}

	void transformSelfName(SelfName_t* selfName, str_list& out, const ast_sel<false,Invoke_t,InvokeArgs_t>& invoke = {}) {
		auto x = selfName;
		auto name = selfName->name.get();
		switch (name->getId()) {
			case "self_class_name"_id: {
				auto clsName = static_cast<self_class_name_t*>(name);
				auto nameStr = toString(clsName->name);
				if (State::luaKeywords.find(nameStr) != State::luaKeywords.end()) {
					out.push_back(s("self.__class[\""sv) + nameStr + s("\"]"));
					if (invoke) {
						if (auto invokePtr = invoke.as<Invoke_t>()) {
							invokePtr->args.push_front(toAst<Exp_t>("self.__class"sv, Exp, x));
						} else {
							auto invokeArgsPtr = invoke.as<InvokeArgs_t>();
							invokeArgsPtr->args.push_front(toAst<Exp_t>("self.__class"sv, Exp, x));
						}
					}
				} else {
					out.push_back(s("self.__class"sv) + s(invoke ? ":"sv : "."sv) + nameStr);
				}
				break;
			}
			case "self_class"_id:
				out.push_back(s("self.__class"sv));
				break;
			case "self_name"_id: {
				auto sfName = static_cast<self_class_name_t*>(name);
				auto nameStr = toString(sfName->name);
				if (State::luaKeywords.find(nameStr) != State::luaKeywords.end()) {
					out.push_back(s("self[\""sv) + nameStr + s("\"]"));
					if (invoke) {
						if (auto invokePtr = invoke.as<Invoke_t>()) {
							invokePtr->args.push_front(toAst<Exp_t>("self"sv, Exp, x));
						} else {
							auto invokeArgsPtr = invoke.as<InvokeArgs_t>();
							invokeArgsPtr->args.push_front(toAst<Exp_t>("self"sv, Exp, x));
						}
					}
				} else {
					out.push_back(s("self"sv) + s(invoke ? ":"sv : "."sv) + nameStr);
				}
				break;
			}
			case "self"_id:
				out.push_back(s("self"sv));
				break;
		}
	}

	bool transformChainEndWithEOP(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList) {
		auto x = chainList.front();
		if (ast_is<existential_op_t>(chainList.back())) {
			auto parens = x->new_ptr<Parens_t>();
			{
				auto chainValue = x->new_ptr<ChainValue_t>();
				for (auto item : chainList) {
					chainValue->items.push_back(item);
				}
				chainValue->items.pop_back();
				auto value = x->new_ptr<Value_t>();
				value->item.set(chainValue);
				auto opValue = x->new_ptr<exp_op_value_t>();
				opValue->op.set(toAst<BinaryOperator_t>("!="sv, BinaryOperator, x));
				opValue->value.set(toAst<Value_t>("nil"sv, Value, x));
				auto exp = x->new_ptr<Exp_t>();
				exp->value.set(value);
				exp->opValues.push_back(opValue);
				parens->expr.set(exp);
			}
			switch (usage) {
				case ExpUsage::Assignment: {
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(parens);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto value = x->new_ptr<Value_t>();
					value->item.set(chainValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
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
					out.back().insert(0, indent() + s("return "sv));
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

	bool transformChainWithEOP(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList) {
		auto opIt = std::find_if(chainList.begin(), chainList.end(), [](ast_node* node) { return ast_is<existential_op_t>(node); });
		if (opIt != chainList.end()) {
			auto x = chainList.front();
			str_list temp;
			if (usage == ExpUsage::Closure) {
				temp.push_back(s("(function()"sv) + nll(x));
				pushScope();
			}
			auto partOne = x->new_ptr<ChainValue_t>();
			for (auto it = chainList.begin();it != opIt;++it) {
				partOne->items.push_back(*it);
			}
			BLOCK_START
			auto back = ast_cast<Callable_t>(partOne->items.back());
			BREAK_IF(!back);
			auto selfName = ast_cast<SelfName_t>(back->item);
			BREAK_IF(!selfName);
			if (auto sname = ast_cast<self_name_t>(selfName->name)) {
				auto colonItem = x->new_ptr<ColonChainItem_t>();
				colonItem->name.set(sname->name);
				partOne->items.pop_back();
				partOne->items.push_back(toAst<Callable_t>("@"sv, Callable, x));
				partOne->items.push_back(colonItem);
				break;
			}
			if (auto cname = ast_cast<self_class_name_t>(selfName->name)) {
				auto colonItem = x->new_ptr<ColonChainItem_t>();
				colonItem->name.set(cname->name);
				partOne->items.pop_back();
				partOne->items.push_back(toAst<Callable_t>("@@"sv, Callable, x));
				partOne->items.push_back(colonItem);
				break;
			}
			BLOCK_END
			auto objVar = singleVariableFrom(partOne);
			if (objVar.empty()) {
				objVar = getUnusedName("_obj_"sv);
				if (auto colonItem = ast_cast<ColonChainItem_t>(partOne->items.back())) {
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.dup(partOne->items);
					chainValue->items.pop_back();
					if (chainValue->items.empty()) {
						if (_withVars.empty()) {
							throw std::logic_error(debugInfo("Short dot/colon syntax must be called within a with block."sv, x));
						}
					chainValue->items.push_back(toAst<Callable_t>(_withVars.top(), Callable, x));
					}
					auto newObj = singleVariableFrom(chainValue);
					if (!newObj.empty()) {
						objVar = newObj;
					} else {
						auto value = x->new_ptr<Value_t>();
						value->item.set(chainValue);
						auto exp = x->new_ptr<Exp_t>();
						exp->value.set(value);
						auto assign = x->new_ptr<Assign_t>();
						assign->values.push_back(exp);
						auto expListAssign = x->new_ptr<ExpListAssign_t>();
						expListAssign->expList.set(toAst<ExpList_t>(objVar, ExpList, x));
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
					partOne->items.push_back(toAst<Callable_t>(objVar, Callable, x));
					partOne->items.push_back(dotItem);
					auto it = opIt; ++it;
					if (it != chainList.end() && ast_is<Invoke_t, InvokeArgs_t>(*it)) {

						if (auto invoke = ast_cast<Invoke_t>(*it)) {
							invoke->args.push_front(toAst<Exp_t>(objVar, Exp, x));
						} else {
							auto invokeArgs = static_cast<InvokeArgs_t*>(*it);
							invokeArgs->args.push_front(toAst<Exp_t>(objVar, Exp, x));
						}
					}
					objVar = getUnusedName("_obj_"sv);
				}
				auto value = x->new_ptr<Value_t>();
				value->item.set(partOne);
				auto exp = x->new_ptr<Exp_t>();
				exp->value.set(value);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(exp);
				auto expListAssign = x->new_ptr<ExpListAssign_t>();
				expListAssign->expList.set(toAst<ExpList_t>(objVar, ExpList, x));
				expListAssign->action.set(assign);
				transformAssignment(expListAssign, temp);
			}
			_buf << indent() << "if "sv << objVar << " ~= nil then"sv << nll(x);
			temp.push_back(clearBuf());
			pushScope();
			auto partTwo = x->new_ptr<ChainValue_t>();
			partTwo->items.push_back(toAst<Callable_t>(objVar, Callable, x));
			for (auto it = ++opIt;it != chainList.end();++it) {
				partTwo->items.push_back(*it);
			}
			switch (usage) {
				case ExpUsage::Common:
					transformChainValue(partTwo, temp, ExpUsage::Common);
					break;
				case ExpUsage::Assignment: {
					auto value = x->new_ptr<Value_t>();
					value->item.set(partTwo);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(exp);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(assignList);
					assignment->action.set(assign);
					transformAssignment(assignment, temp);
					break;
				}
				case ExpUsage::Return:
				case ExpUsage::Closure: {
					auto value = x->new_ptr<Value_t>();
					value->item.set(partTwo);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					auto ret = x->new_ptr<Return_t>();
					auto expListLow = x->new_ptr<ExpListLow_t>();
					expListLow->exprs.push_back(exp);
					ret->valueList.set(expListLow);
					transformReturn(ret, temp);
					break;
				}
			}
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(x));
			switch (usage) {
				case ExpUsage::Return:
					temp.push_back(indent() + s("return nil"sv) + nlr(x));
					break;
				case ExpUsage::Closure:
					temp.push_back(indent() + s("return nil"sv) + nlr(x));
					popScope();
					temp.push_back(indent() + s("end)()"sv));
					break;
				default:
					break;
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
			switch (usage) {
				case ExpUsage::Assignment:
					temp.push_back(indent() + s("do"sv) + nll(x));
					pushScope();
					break;
				case ExpUsage::Closure:
					temp.push_back(s("(function()"sv) + nll(x));
					pushScope();
					break;
				default:
					break;
			}
			auto baseChain = x->new_ptr<ChainValue_t>();
			switch (chainList.front()->getId()) {
				case "DotChainItem"_id:
				case "ColonChainItem"_id:
					if (_withVars.empty()) {
						throw std::logic_error(debugInfo("Short dot/colon syntax must be called within a with block."sv, chainList.front()));
					} else {
						baseChain->items.push_back(toAst<Callable_t>(_withVars.top(), Callable, x));
					}
					break;
			}
			auto end = --chainList.end();
			for (auto it = chainList.begin(); it != end; ++it) {
				baseChain->items.push_back(*it);
			}
			auto colonChainItem = static_cast<ColonChainItem_t*>(chainList.back());
			auto funcName = toString(colonChainItem->name);
			auto baseVar = getUnusedName("_base_"sv);
			auto fnVar = getUnusedName("_fn_"sv);
			{
				auto value = x->new_ptr<Value_t>();
				value->item.set(baseChain);
				auto exp = x->new_ptr<Exp_t>();
				exp->value.set(value);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(exp);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(baseVar, ExpList, x));
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
			{
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(baseVar + "." + funcName, Exp, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(fnVar, ExpList, x));
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
			}
			auto funLit = toAst<Exp_t>(fnVar + s(" and (...)-> "sv) + fnVar + s(" "sv) + baseVar + s(", ..."sv), Exp, x);
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
					temp.push_back(indent() + s("end"sv) + nlr(x));
					break;
				case ExpUsage::Closure:
					popScope();
					temp.push_back(indent() + s("end)()"sv));
					break;
				default:
					break;
			}
			out.push_back(join(temp));
			return true;
		}
		return false;
	}

	void transformChainList(const node_container& chainList, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		auto x = chainList.front();
		str_list temp;
		switch (x->getId()) {
			case "DotChainItem"_id:
			case "ColonChainItem"_id:
				if (_withVars.empty()) {
					throw std::logic_error(debugInfo("Short dot/colon syntax must be called within a with block."sv, x));
				} else {
					temp.push_back(_withVars.top());
				}
				break;
		}
		for (auto it = chainList.begin(); it != chainList.end(); ++it) {
			auto item = *it;
			switch (item->getId()) {
				case "Invoke"_id:
					transformInvoke(static_cast<Invoke_t*>(item), temp);
					break;
				case "DotChainItem"_id:
					transformDotChainItem(static_cast<DotChainItem_t*>(item), temp);
					break;
				case "ColonChainItem"_id: {
					auto colonItem = static_cast<ColonChainItem_t*>(item);
					auto current = it;
					auto next = current; ++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					if (current != chainList.begin()) {
						--current;
						if (!ast_is<existential_op_t>(*current)) {
							++current;
						}
					}
					if (ast_is<existential_op_t>(followItem)) {
						++next;
						followItem = next != chainList.end() ? *next : nullptr;
						--next;
					}
					if (!ast_is<Invoke_t, InvokeArgs_t>(followItem)) {
						throw std::logic_error(debugInfo("Colon chain item must be followed by invoke arguments."sv, colonItem));
					}
					if (colonItem->name.is<LuaKeyword_t>()) {
						std::string callVar;
						auto block = x->new_ptr<Block_t>();
						{
							auto chainValue = x->new_ptr<ChainValue_t>();
							switch (chainList.front()->getId()) {
								case "DotChainItem"_id:
								case "ColonChainItem"_id:
								chainValue->items.push_back(toAst<Callable_t>(_withVars.top(), Callable, x));
									break;
							}
							for (auto i = chainList.begin(); i != current; ++i) {
								chainValue->items.push_back(*i);
							}
							auto value = x->new_ptr<Value_t>();
							value->item.set(chainValue);
							auto exp = x->new_ptr<Exp_t>();
							exp->value.set(value);
							callVar = singleVariableFrom(exp);
							if (callVar.empty()) {
								callVar = getUnusedName(s("_call_"sv));
								auto assignment = x->new_ptr<ExpListAssign_t>();
								assignment->expList.set(toAst<ExpList_t>(callVar, ExpList, x));
								auto assign = x->new_ptr<Assign_t>();
								assign->values.push_back(exp);
								assignment->action.set(assign);
								auto stmt = x->new_ptr<Statement_t>();
								stmt->content.set(assignment);
								block->statements.push_back(stmt);
							}
						}
						{
							auto name = toString(colonItem->name);
							auto chainValue = x->new_ptr<ChainValue_t>();
							chainValue->items.push_back(toAst<Callable_t>(callVar, Callable, x));
							if (ast_is<existential_op_t>(*current)) {
								chainValue->items.push_back(x->new_ptr<existential_op_t>());
							}
							chainValue->items.push_back(toAst<Exp_t>(s("\""sv) + name + s("\""sv), Exp, x));
							if (auto invoke = ast_cast<Invoke_t>(followItem)) {
								invoke->args.push_front(toAst<Exp_t>(callVar, Exp, x));
							} else {
								auto invokeArgs = static_cast<InvokeArgs_t*>(followItem);
								invokeArgs->args.push_front(toAst<Exp_t>(callVar, Exp, x));
							}
							for (auto i = next; i != chainList.end(); ++i) {
								chainValue->items.push_back(*i);
							}
							auto value = x->new_ptr<Value_t>();
							value->item.set(chainValue);
							auto exp = x->new_ptr<Exp_t>();
							exp->value.set(value);
							auto expList = x->new_ptr<ExpList_t>();
							expList->exprs.push_back(exp);
							auto expListAssign = x->new_ptr<ExpListAssign_t>();
							expListAssign->expList.set(expList);
							auto stmt = x->new_ptr<Statement_t>();
							stmt->content.set(expListAssign);
							block->statements.push_back(stmt);
						}
						switch (usage) {
							case ExpUsage::Common:
								transformBlock(block, out, false);
								return;
							case ExpUsage::Return:
								transformBlock(block, out, true);
								return;
							case ExpUsage::Assignment: {
								auto body = x->new_ptr<Body_t>();
								body->content.set(block);
								assignLastExplist(assignList, body);
								transformBlock(block, out);
								return;
							}
							default:
								break;
						}
						auto body = x->new_ptr<Body_t>();
						body->content.set(block);
						auto funLit = toAst<FunLit_t>("->"sv, FunLit, x);
						funLit->body.set(body);
						auto simpleValue = x->new_ptr<SimpleValue_t>();
						simpleValue->value.set(funLit);
						auto value = x->new_ptr<Value_t>();
						value->item.set(simpleValue);
						auto exp = x->new_ptr<Exp_t>();
						exp->value.set(value);
						auto paren = x->new_ptr<Parens_t>();
						paren->expr.set(exp);
						auto callable = x->new_ptr<Callable_t>();
						callable->item.set(paren);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						auto invoke = x->new_ptr<Invoke_t>();
						chainValue->items.push_back(invoke);
						transformChainValue(chainValue, out, ExpUsage::Closure);
						return;
					}
					transformColonChainItem(colonItem, temp);
					break;
				}
				case "Slice"_id:
					transformSlice(static_cast<Slice_t*>(item), temp);
					break;
				case "Callable"_id: {
					auto next = it; ++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					ast_sel<false, Invoke_t, InvokeArgs_t> invoke;
					if (ast_is<Invoke_t, InvokeArgs_t>(followItem)) {
						invoke.set(followItem);
					}
					transformCallable(static_cast<Callable_t*>(item), temp, invoke);
					break;
				}
				case "String"_id:
					transformString(static_cast<String_t*>(item), temp);
					temp.back() = s("("sv) + temp.back() + s(")"sv);
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(item), temp);
					temp.back() = s("["sv) + temp.back() + s("]"sv);
					break;
				case "InvokeArgs"_id: transformInvokeArgs(static_cast<InvokeArgs_t*>(item), temp); break;
				default: break;
			}
		}
		switch (usage) {
			case ExpUsage::Common:
				out.push_back(indent() + join(temp) + nll(chainList.front()));
				break;
			case ExpUsage::Return:
				out.push_back(indent() + s("return "sv) + join(temp) + nll(chainList.front()));
				break;
			default:
				out.push_back(join(temp));
				break;
		}
	}

	void transformChainValue(ChainValue_t* chainValue, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		const auto& chainList = chainValue->items.objects();
		if (transformChainEndWithColonItem(chainList, out, usage, assignList)) {
			return;
		}
		if (transformChainEndWithEOP(chainList, out, usage, assignList)) {
			return;
		}
		if (transformChainWithEOP(chainList, out, usage, assignList)) {
			return;
		}
		transformChainList(chainList, out, usage, assignList);
	}

	void transformAssignableChain(AssignableChain_t* chain, str_list& out) {
		transformChainList(chain->items.objects(), out, ExpUsage::Closure);
	}

	void transformDotChainItem(DotChainItem_t* dotChainItem, str_list& out) {
		auto name = toString(dotChainItem->name);
		if (State::keywords.find(name) != State::keywords.end()) {
			out.push_back(s("[\""sv) + name + s("\"]"sv));
		} else {
			out.push_back(s("."sv) + name);
		}
	}

	void transformColonChainItem(ColonChainItem_t* colonChainItem, str_list& out) {
		auto name = toString(colonChainItem->name);
		out.push_back(s(colonChainItem->switchToDot ? "."sv : ":"sv) + name);
	}

	void transformSlice(Slice_t* slice, str_list&) {
		throw std::logic_error(debugInfo("Slice syntax not supported here."sv, slice));
	}

	void transformInvoke(Invoke_t* invoke, str_list& out) {
		str_list temp;
		for (auto arg : invoke->args.objects()) {
			switch (arg->getId()) {
				case "Exp"_id: transformExp(static_cast<Exp_t*>(arg), temp); break;
				case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(arg), temp); break;
				case "DoubleString"_id: transformDoubleString(static_cast<DoubleString_t*>(arg), temp); break;
				case "LuaString"_id: transformLuaString(static_cast<LuaString_t*>(arg), temp); break;
				default: break;
			}
		}
		out.push_back(s("("sv) + join(temp, ", "sv) + s(")"sv));
	}

	void transform_unary_exp(unary_exp_t* unary_exp, str_list& out) {
		std::string op = toString(unary_exp->m_begin.m_it, unary_exp->item->m_begin.m_it);
		str_list temp{op + (op == "not"sv ? s(" "sv) : Empty)};
		transformExp(unary_exp->item, temp);
		out.push_back(join(temp));
	}

	void transformVariable(Variable_t* name, str_list& out) {
		out.push_back(toString(name));
	}

	void transformNum(Num_t* num, str_list& out) {
		out.push_back(toString(num));
	}

	void transformTableLit(TableLit_t* table, str_list& out) {
		transformTable(table, table->values.objects(), out);
	}

	void transformCompCommon(Comprehension_t* comp, str_list& out) {
		str_list temp;
		auto x = comp;
		auto compInner = comp->forLoop.get();
		for (auto item : compInner->items.objects()) {
			switch (item->getId()) {
				case "CompForEach"_id:
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case "CompFor"_id:
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(item), temp);
					temp.back() = indent() + s("if "sv) + temp.back() + s(" then"sv) + nll(item);
					pushScope();
					break;
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
		auto value = temp.back();
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
				case "CompForEach"_id:
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case "CompFor"_id:
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(item), temp);
					temp.back() = indent() + s("if "sv) + temp.back() + s(" then"sv) + nll(item);
					pushScope();
					break;
			}
		}
		{
			auto assignLeft = toAst<ExpList_t>(accumVar + s("["sv) + lenVar + s("]"sv), ExpList, x);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(comp->value);
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignLeft);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		}
		auto assignStr = temp.back();
		temp.pop_back();
		for (size_t i = 0; i < compInner->items.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << accumVar << " = { }"sv << nll(comp);
		_buf << indent() << "local "sv << lenVar << " = 1"sv << nll(comp);
		_buf << join(temp);
		_buf << assignStr;
		_buf << indent(int(temp.size())) << lenVar << " = "sv << lenVar << " + 1"sv << nll(comp);
		for (int ind = int(temp.size()) - 1; ind > -1 ; --ind) {
			_buf << indent(ind) << "end"sv << nll(comp);
		}
		switch (usage) {
			case ExpUsage::Common:
				break;
			case ExpUsage::Closure: {
				out.push_back(clearBuf());
				out.back().append(indent() + s("return "sv) + accumVar + nlr(comp));
				popScope();
				out.back().insert(0, s("(function()"sv) + nll(comp));
				out.back().append(indent() + s("end)()"sv));
				break;
			}
			case ExpUsage::Assignment: {
				out.push_back(clearBuf());
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(accumVar, Exp, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				popScope();
				out.back() = indent() + s("do"sv) + nll(comp) +
					out.back() + temp.back() +
					indent() + s("end"sv) + nlr(comp);
				break;
			}
			case ExpUsage::Return:
				out.push_back(clearBuf());
				out.back().append(indent() + s("return "sv) + accumVar + nlr(comp));
				break;
			default:
				break;
		}
	}

	void transformForEachHead(AssignableNameList_t* nameList, ast_node* loopTarget, str_list& out) {
		auto x = nameList;
		str_list temp;
		str_list vars;
		str_list varBefore, varAfter;
		std::list<std::pair<ast_node*, ast_ptr<false, ast_node>>> destructPairs;
		for (auto _item : nameList->items.objects()) {
			auto item = static_cast<NameOrDestructure_t*>(_item)->item.get();
			switch (item->getId()) {
				case "Variable"_id:
					transformVariable(static_cast<Variable_t*>(item), vars);
					varAfter.push_back(vars.back());
					break;
				case "TableLit"_id: {
					auto desVar = getUnusedName("_des_"sv);
					destructPairs.emplace_back(item, toAst<Exp_t>(desVar, Exp, x));
					vars.push_back(desVar);
					varAfter.push_back(desVar);
					break;
				}
				default: break;
			}
		}
		switch (loopTarget->getId()) {
			case "star_exp"_id: {
				auto star_exp = static_cast<star_exp_t*>(loopTarget);
				auto listVar = singleVariableFrom(star_exp->value);
				auto indexVar = getUnusedName("_index_");
				varAfter.push_back(indexVar);
				auto value = singleValueFrom(star_exp->value);
				if (!value) throw std::logic_error(debugInfo("Invalid star syntax."sv, star_exp));
				bool endWithSlice = false;
				BLOCK_START
				auto chainValue = value->item.as<ChainValue_t>();
				BREAK_IF(!chainValue);
				auto chainList = chainValue->items.objects();
				auto slice = ast_cast<Slice_t>(chainList.back());
				BREAK_IF(!slice);
				endWithSlice = true;
				if (listVar.empty() && chainList.size() == 2 &&
					ast_is<Callable_t>(chainList.front())) {
					transformCallable(static_cast<Callable_t*>(chainList.front()), temp);
					listVar = temp.back();
					temp.pop_back();
				}
				chainList.pop_back();
				auto chain = x->new_ptr<ChainValue_t>();
				for (auto item : chainList) {
					chain->items.push_back(item);
				}
				std::string startValue("1"sv);
				if (auto exp = slice->startValue.as<Exp_t>()) {
					transformExp(exp, temp);
					startValue = temp.back();
					temp.pop_back();
				}
				std::string stopValue;
				if (auto exp = slice->stopValue.as<Exp_t>()) {
					transformExp(exp, temp);
					stopValue = temp.back();
					temp.pop_back();
				}
				std::string stepValue;
				if (auto exp = slice->stepValue.as<Exp_t>()) {
					transformExp(exp, temp);
					stepValue = temp.back();
					temp.pop_back();
				}
				if (listVar.empty()) {
					listVar = getUnusedName("_list_");
					varBefore.push_back(listVar);
					transformChainValue(chain, temp, ExpUsage::Closure);
					_buf << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
				}
				std::string maxVar;
				if (!stopValue.empty()) {
					maxVar = getUnusedName("_max_");
					varBefore.push_back(maxVar);
					_buf << indent() << "local "sv << maxVar << " = "sv << stopValue << nll(nameList);
				}
				_buf << indent() << "for "sv << indexVar << " = "sv;
				_buf << startValue << ", "sv;
				if (stopValue.empty()) {
					_buf << "#"sv << listVar;
				} else {
					_buf << maxVar << " < 0 and #"sv << listVar <<" + " << maxVar << " or "sv << maxVar;
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
					listVar = getUnusedName("_list_");
					varBefore.push_back(listVar);
				}
				if (!endWithSlice) {
					transformExp(star_exp->value, temp);
					if (newListVal) _buf << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
					_buf << indent() << "for "sv << indexVar << " = 1, #"sv << listVar << " do"sv << nlr(loopTarget);
					_buf << indent(1) << "local "sv << join(vars) << " = "sv << listVar << "["sv << indexVar << "]"sv << nll(nameList);
					out.push_back(clearBuf());
				}
				break;
			}
			case "Exp"_id:
				transformExp(static_cast<Exp_t*>(loopTarget), temp);
				_buf << indent() << "for "sv << join(vars, ", "sv) << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			case "ExpList"_id:
				transformExpList(static_cast<ExpList_t*>(loopTarget), temp);
				_buf << indent() << "for "sv << join(vars, ", "sv) << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			default: break;
		}
		for (auto& var : varBefore) addToScope(var);
		pushScope();
		for (auto& var : varAfter) addToScope(var);
		if (!destructPairs.empty()) {
			temp.clear();
			for (auto& pair : destructPairs) {
				auto sValue = x->new_ptr<SimpleValue_t>();
				sValue->value.set(pair.first);
				auto value = x->new_ptr<Value_t>();
				value->item.set(sValue);
				auto exp = x->new_ptr<Exp_t>();
				exp->value.set(value);
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
	}

	void transformCompForEach(CompForEach_t* comp, str_list& out) {
		transformForEachHead(comp->nameList, comp->loopValue, out);
	}

	void transformInvokeArgs(InvokeArgs_t* invokeArgs, str_list& out) {
		str_list temp;
		for (auto arg : invokeArgs->args.objects()) {
			switch (arg->getId()) {
				case "Exp"_id: transformExp(static_cast<Exp_t*>(arg), temp); break;
				case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(arg), temp); break;
				default: break;
			}
		}
		out.push_back(s("("sv) + join(temp, ", "sv) + s(")"sv));
	}

	void transformForHead(For_t* forNode, str_list& out) {
		str_list temp;
		std::string varName = toString(forNode->varName);
		transformExp(forNode->startValue, temp);
		transformExp(forNode->stopValue, temp);
		if (forNode->stepValue) {
			transformExp(forNode->stepValue->value, temp);
		} else {
			temp.emplace_back();
		}
		auto it = temp.begin();
		const auto& start = *it;
		const auto& stop = *(++it);
		const auto& step = *(++it);
		_buf << indent() << "for "sv << varName << " = "sv << start << ", "sv << stop << (step.empty() ? Empty : s(", "sv) + step) << " do"sv << nll(forNode);
		pushScope();
		addToScope(varName);
		out.push_back(clearBuf());
	}

	void transformLoopBody(Body_t* body, str_list& out, const std::string& appendContent) {
		str_list temp;
		bool withContinue = traversal::Stop == body->traverse([&](ast_node* node) {
			switch (node->getId()) {
				case "For"_id:
				case "ForEach"_id:
					return traversal::Return;
				case "BreakLoop"_id: {
					return toString(node) == "continue"sv ?
						traversal::Stop : traversal::Return;
				}
				default:
					return traversal::Continue;
			}
		});
		if (withContinue) {
			auto continueVar = getUnusedName("_continue_"sv);
			addToScope(continueVar);
			_buf << indent() << "local "sv << continueVar << " = false"sv << nll(body);
			_buf << indent() << "repeat"sv << nll(body);
			temp.push_back(clearBuf());
			_continueVars.push(continueVar);
			pushScope();
		}
		transformBody(body, temp);
		if (withContinue) {
			if (!appendContent.empty()) {
				_buf << indent() << appendContent;
			}
			_buf << indent() << _continueVars.top() << " = true"sv << nll(body);
			popScope();
			_buf << indent() << "until true"sv << nlr(body);
			_buf << indent() << "if not "sv << _continueVars.top() << " then"sv << nlr(body);
			_buf << indent(1) << "break"sv << nlr(body);
			_buf << indent() << "end"sv << nlr(body);
			temp.push_back(clearBuf());
			_continueVars.pop();
		} else if (!appendContent.empty()) {
			temp.back().append(indent() + appendContent);
		}
		out.push_back(join(temp));
	}

	void transformFor(For_t* forNode, str_list& out) {
		str_list temp;
		transformForHead(forNode, temp);
		transformLoopBody(forNode->body, temp, Empty);
		popScope();
		out.push_back(join(temp) + indent() + s("end"sv) + nlr(forNode));
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
		auto expList = toAst<ExpList_t>(accum + s("["sv) + len + s("]"sv), ExpList, x);
		assignLastExplist(expList, forNode->body);
		auto lenLine = len + s(" = "sv) + len + s(" + 1"sv) + nlr(forNode->body);
		transformLoopBody(forNode->body, out, lenLine);
		popScope();
		out.push_back(indent() + s("end"sv) + nlr(forNode));
		return accum;
	}

	void transformForClosure(For_t* forNode, str_list& out) {
		str_list temp;
		_buf << "(function()"sv << nll(forNode);
		pushScope();
		auto accum = transformForInner(forNode, temp);
		temp.push_back(indent() + s("return "sv) + accum + nlr(forNode));
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformForInPlace(For_t* forNode,  str_list& out, ExpList_t* assignExpList = nullptr) {
		auto x = forNode;
		str_list temp;
		if (assignExpList) {
			_buf << indent() << "do"sv << nll(forNode);
			pushScope();
			auto accum = transformForInner(forNode, temp);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accum, Exp, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignExpList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(forNode));
		} else {
			auto accum = transformForInner(forNode, temp);
			auto returnNode = x->new_ptr<Return_t>();
			auto expListLow = toAst<ExpListLow_t>(accum, ExpListLow, x);
			returnNode->valueList.set(expListLow);
			transformReturn(returnNode, temp);
		}
		out.push_back(join(temp));
	}

	void transformBinaryOperator(BinaryOperator_t* node, str_list& out) {
		auto op = toString(node);
		out.push_back(op == "!="sv ? s("~="sv) : op);
	}

	void transformForEach(ForEach_t* forEach, str_list& out) {
		str_list temp;
		transformForEachHead(forEach->nameList, forEach->loopValue, temp);
		transformLoopBody(forEach->body, temp, Empty);
		popScope();
		out.push_back(temp.front() + temp.back() + indent() + s("end"sv) + nlr(forEach));
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
		auto expList = toAst<ExpList_t>(accum + s("["sv) + len + s("]"sv), ExpList, x);
		assignLastExplist(expList, forEach->body);
		auto lenLine = len + s(" = "sv) + len + s(" + 1"sv) + nlr(forEach->body);
		transformLoopBody(forEach->body, out, lenLine);
		popScope();
		out.push_back(indent() + s("end"sv) + nlr(forEach));
		return accum;
	}

	void transformForEachClosure(ForEach_t* forEach, str_list& out) {
		str_list temp;
		_buf << "(function()"sv << nll(forEach);
		pushScope();
		auto accum = transformForEachInner(forEach, temp);
		temp.push_back(indent() + s("return "sv) + accum + nlr(forEach));
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformForEachInPlace(ForEach_t* forEach,  str_list& out, ExpList_t* assignExpList = nullptr) {
		auto x = forEach;
		str_list temp;
		if (assignExpList) {
			_buf << indent() << "do"sv << nll(forEach);
			pushScope();
			auto accum = transformForEachInner(forEach, temp);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accum, Exp, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignExpList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(forEach));
		} else {
			auto accum = transformForEachInner(forEach, temp);
			auto returnNode = x->new_ptr<Return_t>();
			auto expListLow = toAst<ExpListLow_t>(accum, ExpListLow, x);
			returnNode->valueList.set(expListLow);
			transformReturn(returnNode, temp);
		}
		out.push_back(join(temp));
	}

	void transform_variable_pair(variable_pair_t* pair, str_list& out) {
		auto name = toString(pair->name);
		out.push_back(name + s(" = "sv) + name);
	}

	void transform_normal_pair(normal_pair_t* pair, str_list& out) {
		auto key = pair->key.get();
		str_list temp;
		switch (key->getId()) {
			case "KeyName"_id: {
				transformKeyName(static_cast<KeyName_t*>(key), temp);
				if (State::luaKeywords.find(temp.back()) != State::luaKeywords.end()) {
					temp.back() = s("[\""sv) + temp.back() + s("\"]");
				}
				break;
			}
			case "Exp"_id:
				transformExp(static_cast<Exp_t*>(key), temp);
				temp.back() = s("["sv) + temp.back() + s("]"sv);
				break;
			case "DoubleString"_id:
				transformDoubleString(static_cast<DoubleString_t*>(key), temp);
				temp.back() = s("["sv) + temp.back() + s("]"sv);
				break;
			case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(key), temp);
				temp.back() = s("["sv) + temp.back() + s("]"sv);
				break;
			default: break;
		}
		auto value = pair->value.get();
		switch (value->getId()) {
			case "Exp"_id: transformExp(static_cast<Exp_t*>(value), temp); break;
			case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(value), temp); break;
			default: break;
		}
		out.push_back(temp.front() + s(" = "sv) + temp.back());
	}

	void transformKeyName(KeyName_t* keyName, str_list& out) {
		auto name = keyName->name.get();
		switch (name->getId()) {
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(name), out); break;
			case "Name"_id: out.push_back(toString(name)); break;
			default: break;
		}
	}

	void replace(std::string& str, std::string_view from, std::string_view to) {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.size(), to);
			start_pos += to.size();
		}
	}

	void transformLuaString(LuaString_t* luaString, str_list& out) {
		auto content = toString(luaString->content);
		replace(content, "\r"sv, "");
		if (content[0] == '\n') content.erase(content.begin());
		out.push_back(toString(luaString->open) + content + toString(luaString->close));
	}

	void transformSingleString(SingleString_t* singleString, str_list& out) {
		auto str = toString(singleString);
		replace(str, "\r"sv, "");
		replace(str, "\n"sv, "\\n"sv);
		out.push_back(str);
	}

	void transformDoubleString(DoubleString_t* doubleString, str_list& out) {
		str_list temp;
		for (auto _seg : doubleString->segments.objects()) {
			auto seg = static_cast<double_string_content_t*>(_seg);
			auto content = seg->content.get();
			switch (content->getId()) {
				case "double_string_inner"_id: {
					auto str = toString(content);
					replace(str, "\r"sv, "");
					replace(str, "\n"sv, "\\n"sv);
					temp.push_back(s("\""sv) + str + s("\""sv));
					break;
				}
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(content), temp);
					temp.back() = s("tostring("sv) + temp.back() + s(")"sv);
					break;
				default: break;
			}
		}
		out.push_back(temp.empty() ? s("\"\""sv) : join(temp, " .. "sv));
	}

	void transformString(String_t* string, str_list& out) {
		auto str = string->str.get();
		switch (str->getId()) {
			case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(str), out); break;
			case "DoubleString"_id: transformDoubleString(static_cast<DoubleString_t*>(str), out); break;
			case "LuaString"_id: transformLuaString(static_cast<LuaString_t*>(str), out); break;
			default: break;
		}
	}

	std::pair<std::string,bool> defineClassVariable(Assignable_t* assignable) {
		if (auto variable = assignable->item.as<Variable_t>()) {
			auto name = toString(variable);
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
		temp.push_back(s("(function()"sv) + nll(classDecl));
		pushScope();
		transformClassDecl(classDecl, temp, ExpUsage::Return);
		popScope();
		temp.push_back(s("end)()"sv));
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
				throw std::logic_error(debugInfo("Left hand expression is not assignable."sv, assignable));
			}
			bool newDefined = false;
			std::tie(className, newDefined) = defineClassVariable(assignable);
			if (newDefined) {
				temp.push_back(indent() + s("local "sv) + className + nll(classDecl));
			}
			if (className.empty()) {
				if (auto chain = ast_cast<AssignableChain_t>(assignable->item)) {
					if (auto dotChain = ast_cast<DotChainItem_t>(chain->items.back())) {
						className = s("\""sv) + toString(dotChain->name) + s("\""sv);
					} else if (auto index = ast_cast<Exp_t>(chain->items.back())) {
						if (auto name = index->getByPath<Value_t, String_t>()) {
							transformString(name, temp);
							className = temp.back();
							temp.pop_back();
						}
					}
				}
			} else {
				className = s("\""sv) + className + s("\""sv);
			}
			pushScope();
			transformAssignable(assignable, temp);
			popScope();
			assignItem = temp.back();
			temp.pop_back();
		} else if (expList) {
			auto name = singleVariableFrom(expList);
			if (!name.empty()) {
				className = s("\""sv) + name + s("\""sv);
			}
		}
		temp.push_back(indent() + s("do"sv) + nll(classDecl));
		pushScope();
		auto classVar = getUnusedName("_class_"sv);
		addToScope(classVar);
		temp.push_back(indent() + s("local "sv) + classVar + nll(classDecl));
		if (body) {
			str_list varDefs;
			for (auto item : body->contents.objects()) {
				if (auto statement = ast_cast<Statement_t>(item)) {
					ClassDecl_t* clsDecl = nullptr;
					if (auto assignment = assignmentFrom(statement)) {
						auto names = transformAssignDefs(assignment->expList.get());
						varDefs.insert(varDefs.end(), names.begin(), names.end());
						auto info = extractDestructureInfo(assignment, true);
						if (!info.first.empty()) {
							for (const auto& destruct : info.first)
								for (const auto& item : destruct.items)
									if (item.isVariable && addToScope(item.name))
										varDefs.push_back(item.name);
						}
						BLOCK_START
						auto assign = assignment->action.as<Assign_t>();
						BREAK_IF(!assign);
						BREAK_IF(assign->values.objects().size() != 1);
						auto exp = ast_cast<Exp_t>(assign->values.objects().front());
						BREAK_IF(!exp);
						auto value = singleValueFrom(exp);
						clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
						BLOCK_END
					} else if (auto expList = expListFrom(statement)) {
						auto value = singleValueFrom(expList);
						clsDecl = value->getByPath<SimpleValue_t, ClassDecl_t>();
					}
					if (clsDecl) {
						std::string clsName;
						bool newDefined = false;
						std::tie(clsName,newDefined) = defineClassVariable(clsDecl->name);
						if (newDefined) varDefs.push_back(clsName);
					}
				}
			}
			if (!varDefs.empty()) {
				temp.push_back(indent() + s("local ") + join(varDefs, ", "sv) + nll(body));
			}
		}
		std::string parent, parentVar;
		if (extend) {
			parentVar = getUnusedName("_parent_"sv);
			addToScope(parentVar);
			transformExp(extend, temp);
			parent = temp.back();
			temp.pop_back();
			temp.push_back(indent() + s("local "sv) + parentVar + s(" = "sv) + parent + nll(classDecl));
		}
		auto baseVar = getUnusedName("_base_"sv);
		addToScope(baseVar);
		temp.push_back(indent() + s("local "sv) + baseVar + s(" = "sv));
		str_list builtins;
		str_list commons;
		str_list statements;
		if (body) {
			std::list<ClassMember> members;
			for (auto content : classDecl->body->contents.objects()) {
				switch (content->getId()) {
					case "class_member_list"_id: {
						size_t inc = transform_class_member_list(static_cast<class_member_list_t*>(content), members, classVar);
						auto it = members.end();
						for (size_t i = 0; i < inc; ++i, --it);
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
					case "Statement"_id:
						transformStatement(static_cast<Statement_t*>(content), statements);
						break;
					default: break;
				}
			}
			for (auto& member : members) {
				switch (member.type) {
					case MemType::Common:
						commons.push_back((commons.empty() ? Empty : s(","sv) + nll(member.node)) + member.item);
						break;
					case MemType::Builtin:
						builtins.push_back((builtins.empty() ? Empty : s(","sv) + nll(member.node)) + member.item);
						break;
					default: break;
				}
			}
			if (!commons.empty()) {
				temp.back() += s("{"sv) + nll(body);
				temp.push_back(join(commons) + nll(body));
				temp.push_back(indent() + s("}"sv) + nll(body));
			} else {
				temp.back() += s("{ }"sv) + nll(body);
			}
		} else {
			temp.back() += s("{ }"sv) + nll(classDecl);
		}
		temp.push_back(indent() + baseVar + s(".__index = "sv) + baseVar + nll(classDecl));
		str_list tmp;
		if (usage == ExpUsage::Assignment) {
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(classVar, Exp, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			transformAssignment(assignment, tmp);
		}
		if (extend) {
			_buf << indent() << "setmetatable("sv << baseVar << ", "sv << parentVar << ".__base)"sv << nll(classDecl);
		}
		_buf << indent() << classVar << " = setmetatable({" << nll(classDecl);
		if (!builtins.empty()) {
			_buf << join(builtins) << ","sv << nll(classDecl);
		} else {
			if (extend) {
				_buf << indent(1) << "__init = function(self, ...)"sv << nll(classDecl);
				_buf << indent(2) << "return _class_0.__parent.__init(self, ...)"sv << nll(classDecl);
    			_buf << indent(1) << "end,"sv << nll(classDecl);
			} else {
				_buf << indent(1) << "__init = function() end,"sv << nll(classDecl);
			}
		}
		_buf << indent(1) << "__base = "sv << baseVar;
    	if (!className.empty()) {
    		_buf << ","sv << nll(classDecl) << indent(1) << "__name = "sv << className << (extend ? s(","sv) : Empty) << nll(classDecl);
		} else {
			_buf << nll(classDecl);
		}
		if (extend) {
			_buf << indent(1) << "__parent = "sv << parentVar << nll(classDecl);
		}
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
		_buf << indent(1) << "local " << selfVar << " = setmetatable({}, "sv << baseVar << ")"sv << nll(classDecl);
		_buf << indent(1) << "cls.__init("sv << selfVar << ", ...)"sv << nll(classDecl);
		_buf << indent(1) << "return "sv << selfVar << nll(classDecl);
		popScope();
		_buf << indent(1) << "end"sv << nll(classDecl);
		_buf << indent() << "})"sv << nll(classDecl);
		_buf << indent() << baseVar << ".__class = "sv << classVar << nll(classDecl);
		if (!statements.empty()) _buf << indent() << "local self = "sv << classVar << nll(classDecl);
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
		temp.push_back(indent() + s("end"sv) + nlr(classDecl));
		out.push_back(join(temp));
	}

	size_t transform_class_member_list(class_member_list_t* class_member_list, std::list<ClassMember>& out, const std::string& classVar) {
		str_list temp;
		size_t count = 0;
		for (auto keyValue : class_member_list->values.objects()) {
			MemType type = MemType::Common;
			BLOCK_START
			auto normal_pair = ast_cast<normal_pair_t>(keyValue);
			BREAK_IF(!normal_pair);
			auto keyName = normal_pair->key.as<KeyName_t>();
			BREAK_IF(!keyName);
			std::string newSuperCall;
			auto selfName = keyName->name.as<SelfName_t>();
			if (selfName) {
				type = MemType::Property;
				auto name = ast_cast<self_name_t>(selfName->name);
				if (!name) throw std::logic_error(debugInfo("Invalid class poperty name."sv, selfName->name));
				newSuperCall = classVar + s(".__parent."sv) + toString(name->name);
			} else {
				auto x = keyName;
				auto nameNode = keyName->name.as<Name_t>();
				if (!nameNode) break;
				auto name = toString(nameNode);
				if (name == "new"sv) {
					type = MemType::Builtin;
					keyName->name.set(toAst<Name_t>("__init"sv, Name, x));
					newSuperCall = classVar + s(".__parent.__init"sv);
				} else {
					newSuperCall = classVar + s(".__parent.__base."sv) + name;
				}
			}
			normal_pair->value->traverse([&](ast_node* node) {
				if (node->getId() == "ClassDecl"_id) return traversal::Return;
				if (auto chainValue = ast_cast<ChainValue_t>(node)) {
					if (auto callable = ast_cast<Callable_t>(chainValue->items.front())) {
						auto var = callable->item.get();
						if (toString(var) == "super"sv) {
							auto insertSelfToArguments = [&](ast_node* item) {
								auto x = item;
								switch (item->getId()) {
									case "InvokeArgs"_id: {
										auto invoke = static_cast<InvokeArgs_t*>(item);
										invoke->args.push_front(toAst<Exp_t>("self"sv, Exp, x));
										return true;
									}
									case "Invoke"_id: {
										auto invoke = static_cast<Invoke_t*>(item);
										invoke->args.push_front(toAst<Exp_t>("self"sv, Exp, x));
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
									newSuperCall = classVar + s(".__parent"sv);
								}
							} else {
								newSuperCall = classVar + s(".__parent"sv);
							}
							auto newChain = toAst<ChainValue_t>(newSuperCall, ChainValue, chainValue);
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
				case "variable_pair"_id:
					transform_variable_pair(static_cast<variable_pair_t*>(keyValue), temp);
					break;
				case "normal_pair"_id:
					transform_normal_pair(static_cast<normal_pair_t*>(keyValue), temp);
					break;
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
			case "AssignableChain"_id: transformAssignableChain(static_cast<AssignableChain_t*>(item), out); break;
			case "Variable"_id: transformVariable(static_cast<Variable_t*>(item), out); break;
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(item), out); break;
			default: break;
		}
	}

	void transformWithClosure(With_t* with, str_list& out) {
		str_list temp;
		temp.push_back(s("(function()"sv) + nll(with));
		pushScope();
		transformWith(with, temp, nullptr, true);
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformWith(With_t* with, str_list& out, ExpList_t* assignList = nullptr, bool returnValue = false) {
		auto x = with;
		str_list temp;
		std::string withVar;
		bool scoped = false;
		if (with->assigns) {
			checkAssignable(with->valueList);
			auto vars = getAssignVars(with);
			if (vars.front().empty()) {
				if (with->assigns->values.objects().size() == 1) {
					auto var = singleVariableFrom(with->assigns->values.objects().front());
					if (!var.empty()) {
						withVar = var;
					}
				}
				if (withVar.empty()) {
					withVar = getUnusedName("_with_"sv);
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(toAst<ExpList_t>(withVar, ExpList, x));
					auto assign = x->new_ptr<Assign_t>();
					assign->values.push_back(with->assigns->values.objects().front());
					assignment->action.set(assign);
					if (!returnValue) {
						scoped = true;
						temp.push_back(indent() + s("do"sv) + nll(with));
						pushScope();
					}
					transformAssignment(assignment, temp);
				}
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(with->valueList);
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(withVar, Exp, x));
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
					temp.push_back(indent() + s("do"sv) + nll(with));
					pushScope();
				}
				transformAssignment(assignment, temp);
			}
		} else {
			withVar = singleVariableFrom(with->valueList);
			if (withVar.empty()) {
				withVar = getUnusedName("_with_"sv);
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(toAst<ExpList_t>(withVar, ExpList, x));
				auto assign = x->new_ptr<Assign_t>();
				assign->values.dup(with->valueList->exprs);
				assignment->action.set(assign);
				if (!returnValue) {
					scoped = true;
					temp.push_back(indent() + s("do"sv) + nll(with));
					pushScope();
				}
				transformAssignment(assignment, temp);
			}
		}
		if (!scoped && !returnValue) {
			pushScope();
			scoped = traversal::Stop == with->body->traverse([&](ast_node* node) {
				if (auto statement = ast_cast<Statement_t>(node)) {
					ClassDecl_t* clsDecl = nullptr;
					if (auto assignment = assignmentFrom(statement)) {
						auto names = getAssignDefs(assignment->expList.get());
						if (!names.empty()) {
							return traversal::Stop;
						}
						auto info = extractDestructureInfo(assignment, true);
						if (!info.first.empty()) {
							for (const auto& destruct : info.first)
								for (const auto& item : destruct.items)
									if (item.isVariable && !isDefined(item.name))
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
						if (!isDefined(toString(variable))) return traversal::Stop;
					}
					return traversal::Return;
				}
				return traversal::Continue;
			});
			popScope();
			if (scoped) {
				temp.push_back(indent() + s("do"sv) + nll(with));
				pushScope();
			}
		}
		_withVars.push(withVar);
		transformBody(with->body, temp);
		_withVars.pop();
		if (assignList) {
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(assignList);
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(withVar, Exp, x));
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		}
		if (returnValue) {
			auto stmt = lastStatementFrom(with->body);
			if (!stmt->content.is<Return_t>()) {
				temp.push_back(indent() + s("return "sv) + withVar + nll(with));
			}
		}
		if (scoped) {
			popScope();
			temp.push_back(indent() + s("end"sv) + nll(with));
		}
		out.push_back(join(temp));
	}

	void transform_const_value(const_value_t* const_value, str_list& out) {
		out.push_back(toString(const_value));
	}

	void transformExport(Export_t* exportNode, str_list& out) {
		auto x = exportNode;
		auto item = exportNode->item.get();
		switch (item->getId()) {
			case "ClassDecl"_id: {
				auto classDecl = static_cast<ClassDecl_t*>(item);
				if (classDecl->name && classDecl->name->item->getId() == "Variable"_id) {
					markVarExported(ExportMode::Any, true);
					addExportedVar(toString(classDecl->name->item));
				}
				transformClassDecl(classDecl, out, ExpUsage::Common);
				break;
			}
			case "export_op"_id:
				if (toString(item) == "*"sv) {
					markVarExported(ExportMode::Any, false);
				} else {
					markVarExported(ExportMode::Capital, false);
				}
				break;
			case "export_values"_id: {
				markVarExported(ExportMode::Any, true);
				auto values = exportNode->item.to<export_values_t>();
				if (values->valueList) {
					auto expList = x->new_ptr<ExpList_t>();
					for (auto name : values->nameList->names.objects()) {
						addExportedVar(toString(name));
						auto callable = x->new_ptr<Callable_t>();
						callable->item.set(name);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						auto value = x->new_ptr<Value_t>();
						value->item.set(chainValue);
						auto exp = x->new_ptr<Exp_t>();
						exp->value.set(value);
						expList->exprs.push_back(exp);
					}
					auto assignment = x->new_ptr<ExpListAssign_t>();
					assignment->expList.set(expList);
					auto assign = x->new_ptr<Assign_t>();
					assign->values.dup(values->valueList->exprs);
					assignment->action.set(assign);
					transformAssignment(assignment, out);
				} else {
					for (auto name : values->nameList->names.objects()) {
						addExportedVar(toString(name));
					}
				}
				break;
			}
			default:
				break;
		}
	}

	void transformTable(ast_node* table, const node_container& pairs, str_list& out) {
		if (pairs.empty()) {
			out.push_back(s("{ }"sv));
			return;
		}
		str_list temp;
		pushScope();
		for (auto pair : pairs) {
			switch (pair->getId()) {
				case "Exp"_id: transformExp(static_cast<Exp_t*>(pair), temp); break;
				case "variable_pair"_id: transform_variable_pair(static_cast<variable_pair_t*>(pair), temp); break;
				case "normal_pair"_id: transform_normal_pair(static_cast<normal_pair_t*>(pair), temp); break;
			}
			temp.back() = indent() + temp.back() + (pair == pairs.back() ? Empty : s(","sv)) + nll(pair);
		}
		out.push_back(s("{"sv) + nll(table) + join(temp));
		popScope();
		out.back() += (indent() + s("}"sv));
	}

	void transform_simple_table(simple_table_t* table, str_list& out) {
		transformTable(table, table->pairs.objects(), out);
	}

	void transformTblComprehension(TblComprehension_t* comp, str_list& out, ExpUsage usage, ExpList_t* assignList = nullptr) {
		switch (usage) {
			case ExpUsage::Closure:
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
				case "CompForEach"_id:
					transformCompForEach(static_cast<CompForEach_t*>(item), temp);
					break;
				case "CompFor"_id:
					transformCompFor(static_cast<CompFor_t*>(item), temp);
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(item), temp);
					temp.back() = indent() + s("if "sv) + temp.back() + s(" then"sv) + nll(item);
					pushScope();
					break;
			}
		}
		transformExp(comp->key, kv);
		if (comp->value) {
			transformExp(comp->value->value, kv);
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
		for (int ind = int(temp.size()) - 2; ind > -1 ; --ind) {
			_buf << indent(ind) << "end"sv << nll(comp);
		}
		popScope();
		_buf << indent() << "end"sv << nll(comp);
		switch (usage) {
			case ExpUsage::Closure:
				out.push_back(clearBuf() + indent() + s("return "sv) + tbl + nlr(comp));
				popScope();
				out.back().insert(0, s("(function()"sv) + nll(comp));
				out.back().append(indent() + s("end)()"sv));
				break;
			case ExpUsage::Assignment: {
				out.push_back(clearBuf());
				auto assign = x->new_ptr<Assign_t>();
				assign->values.push_back(toAst<Exp_t>(tbl, Exp, x));
				auto assignment = x->new_ptr<ExpListAssign_t>();
				assignment->expList.set(assignList);
				assignment->action.set(assign);
				transformAssignment(assignment, temp);
				out.back().append(temp.back());
				popScope();
				out.back().insert(0, indent() + s("do"sv) + nll(comp));
				out.back().append(indent() + s("end"sv) + nlr(comp));
				break;
			}
			case ExpUsage::Return:
				out.push_back(clearBuf() + indent() + s("return "sv) + tbl + nlr(comp));
				break;
			default:
				break;
		}
	}

	void transformCompFor(CompFor_t* comp, str_list& out) {
		str_list temp;
		std::string varName = toString(comp->varName);
		transformExp(comp->startValue, temp);
		transformExp(comp->stopValue, temp);
		if (comp->stepValue) {
			transformExp(comp->stepValue->value, temp);
		} else {
			temp.emplace_back();
		}
		auto it = temp.begin();
		const auto& start = *it;
		const auto& stop = *(++it);
		const auto& step = *(++it);
		_buf << indent() << "for "sv << varName << " = "sv << start << ", "sv << stop << (step.empty() ? Empty : s(", "sv) + step) << " do"sv << nll(comp);
		out.push_back(clearBuf());
		pushScope();
		addToScope(varName);
	}

	void transformTableBlock(TableBlock_t* table, str_list& out) {
		transformTable(table, table->values.objects(), out);
	}

	void transformDo(Do_t* doNode, str_list& out, bool implicitReturn = false) {
		str_list temp;
		temp.push_back(indent() + s("do"sv) + nll(doNode));
		pushScope();
		transformBody(doNode->body, temp, implicitReturn);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(doNode));
		out.push_back(join(temp));
	}

	void transformDoClosure(Do_t* doNode, str_list& out) {
		str_list temp;
		temp.push_back(s("(function()"sv) + nll(doNode));
		pushScope();
		transformBody(doNode->body, temp, true);
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformImportFrom(ImportFrom_t* import, str_list& out) {
		str_list temp;
		auto x = import;
		auto objVar = singleVariableFrom(import->exp);
		ast_ptr<false, ExpListAssign_t> objAssign;
		if (objVar.empty()) {
			objVar = getUnusedName("_obj_"sv);
			auto expList = toAst<ExpList_t>(objVar, ExpList, x);
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
				case "Variable"_id: {
					auto var = ast_to<Variable_t>(name);
					{
						auto callable = toAst<Callable_t>(objVar, Callable, x);
						auto dotChainItem = x->new_ptr<DotChainItem_t>();
						dotChainItem->name.set(var->name);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						chainValue->items.push_back(dotChainItem);
						auto value = x->new_ptr<Value_t>();
						value->item.set(chainValue);
						auto exp = x->new_ptr<Exp_t>();
						exp->value.set(value);
						assign->values.push_back(exp);
					}
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(var);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto value = x->new_ptr<Value_t>();
					value->item.set(chainValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					expList->exprs.push_back(exp);
					break;
				}
				case "colon_import_name"_id: {
					auto var = static_cast<colon_import_name_t*>(name)->name.get();
					{
						auto nameNode = var->name.get();
						auto callable = toAst<Callable_t>(objVar, Callable, x);
						auto colonChain = x->new_ptr<ColonChainItem_t>();
						colonChain->name.set(nameNode);
						auto chainValue = x->new_ptr<ChainValue_t>();
						chainValue->items.push_back(callable);
						chainValue->items.push_back(colonChain);
						auto value = x->new_ptr<Value_t>();
						value->item.set(chainValue);
						auto exp = x->new_ptr<Exp_t>();
						exp->value.set(value);
						assign->values.push_back(exp);
					}
					auto callable = x->new_ptr<Callable_t>();
					callable->item.set(var);
					auto chainValue = x->new_ptr<ChainValue_t>();
					chainValue->items.push_back(callable);
					auto value = x->new_ptr<Value_t>();
					value->item.set(chainValue);
					auto exp = x->new_ptr<Exp_t>();
					exp->value.set(value);
					expList->exprs.push_back(exp);
					break;
				}
			}
		}
		if (objAssign) {
			auto preDef = getPredefine(transformAssignDefs(expList));
			if (!preDef.empty()) {
				temp.push_back(preDef + nll(import));
			}
			temp.push_back(indent() + s("do"sv) + nll(import));
			pushScope();
			transformAssignment(objAssign, temp);
		}
		auto assignment = x->new_ptr<ExpListAssign_t>();
		assignment->expList.set(expList);
		assignment->action.set(assign);
		transformAssignment(assignment, temp);
		if (objAssign) {
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(import));
		}
		out.push_back(join(temp));
	}

	void transformImportAs(ImportAs_t* import, str_list& out) {
		auto x = import;
		if (!import->target) {
			auto name = toString(import->literal->inners.back());
			replace(name, "-"sv, "_"sv);
			replace(name, " "sv, "_"sv);
			import->target.set(toAst<Variable_t>(name, Variable, x));
		}
		auto target = import->target.get();
		auto value = x->new_ptr<Value_t>();
		if (auto var = ast_cast<Variable_t>(target)) {
			auto callable = x->new_ptr<Callable_t>();
			callable->item.set(var);
			auto chainValue = x->new_ptr<ChainValue_t>();
			chainValue->items.push_back(callable);
			value->item.set(chainValue);
		} else {
			auto tableLit = ast_to<TableLit_t>(target);
			auto simpleValue = x->new_ptr<SimpleValue_t>();
			simpleValue->value.set(tableLit);
			value->item.set(simpleValue);
		}
		auto exp = x->new_ptr<Exp_t>();
		exp->value.set(value);
		auto assignList = x->new_ptr<ExpList_t>();
		assignList->exprs.push_back(exp);
		auto assign = x->new_ptr<Assign_t>();
		assign->values.push_back(toAst<Exp_t>(s("require ") + toString(import->literal), Exp, x));
		auto assignment = x->new_ptr<ExpListAssign_t>();
		assignment->expList.set(assignList);
		assignment->action.set(assign);
		transformAssignment(assignment, out);
	}

	void transformImport(Import_t* import, str_list& out) {
		auto content = import->content.get();
		switch (content->getId()) {
			case "ImportAs"_id:
				transformImportAs(static_cast<ImportAs_t*>(content), out);
				break;
			case "ImportFrom"_id:
				transformImportFrom(static_cast<ImportFrom_t*>(content), out);
				break;
			default:
				break;
		}
	}

	void transformWhileInPlace(While_t* whileNode, str_list& out, ExpList_t* expList = nullptr) {
		auto x = whileNode;
		str_list temp;
		if (expList) {
			temp.push_back(indent() + s("do"sv) + nll(whileNode));
		}
		pushScope();
		auto accumVar = getUnusedName("_accum_"sv);
		addToScope(accumVar);
		auto lenVar = getUnusedName("_len_"sv);
		addToScope(lenVar);
		temp.push_back(indent() + s("local "sv) + accumVar + s(" = { }"sv) + nll(whileNode));
		temp.push_back(indent() + s("local "sv) + lenVar + s(" = 1"sv) + nll(whileNode));
		transformExp(whileNode->condition, temp);
		temp.back() = indent() + s("while "sv) + temp.back() + s(" do"sv) + nll(whileNode);
		pushScope();
		auto assignLeft = toAst<ExpList_t>(accumVar + s("["sv) + lenVar + s("]"sv), ExpList, x);
		assignLastExplist(assignLeft, whileNode->body);
		auto lenLine = lenVar + s(" = "sv) + lenVar + s(" + 1"sv) + nlr(whileNode);
		transformLoopBody(whileNode->body, temp, lenLine);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(whileNode));
		if (expList) {
			auto assign = x->new_ptr<Assign_t>();
			assign->values.push_back(toAst<Exp_t>(accumVar, Exp, x));
			auto assignment = x->new_ptr<ExpListAssign_t>();
			assignment->expList.set(expList);
			assignment->action.set(assign);
			transformAssignment(assignment, temp);
		} else {
			temp.push_back(indent() + s("return "sv) + accumVar + nlr(whileNode));
		}
		popScope();
		if (expList) {
			temp.push_back(indent() + s("end"sv) + nlr(whileNode));
		}
		out.push_back(join(temp));
	}

	void transformWhileClosure(While_t* whileNode, str_list& out) {
		auto x = whileNode;
		str_list temp;
		temp.push_back(s("(function() "sv) + nll(whileNode));
		pushScope();
		auto accumVar = getUnusedName("_accum_"sv);
		addToScope(accumVar);
		auto lenVar = getUnusedName("_len_"sv);
		addToScope(lenVar);
		temp.push_back(indent() + s("local "sv) + accumVar + s(" = { }"sv) + nll(whileNode));
		temp.push_back(indent() + s("local "sv) + lenVar + s(" = 1"sv) + nll(whileNode));
		transformExp(whileNode->condition, temp);
		temp.back() = indent() + s("while "sv) + temp.back() + s(" do"sv) + nll(whileNode);
		pushScope();
		auto assignLeft = toAst<ExpList_t>(accumVar + s("["sv) + lenVar + s("]"sv), ExpList, x);
		assignLastExplist(assignLeft, whileNode->body);
		auto lenLine = lenVar + s(" = "sv) + lenVar + s(" + 1"sv) + nlr(whileNode);
		transformLoopBody(whileNode->body, temp, lenLine);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(whileNode));
		temp.push_back(indent() + s("return "sv) + accumVar + nlr(whileNode));
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformWhile(While_t* whileNode, str_list& out) {
		str_list temp;
		pushScope();
		transformExp(whileNode->condition, temp);
		transformLoopBody(whileNode->body, temp, Empty);
		popScope();
		_buf << indent() << "while "sv << temp.front() << " do"sv << nll(whileNode);
		_buf << temp.back();
		_buf << indent() << "end"sv << nlr(whileNode);
		out.push_back(clearBuf());
	}

	void transformSwitchClosure(Switch_t* switchNode, str_list& out) {
		str_list temp;
		temp.push_back(s("(function()"sv) + nll(switchNode));
		pushScope();
		transformSwitch(switchNode, temp, true);
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformSwitch(Switch_t* switchNode, str_list& out, bool implicitReturn = false) {
		str_list temp;
		auto objVar = singleVariableFrom(switchNode->target);
		if (objVar.empty()) {
			objVar = getUnusedName("_exp_"sv);
			addToScope(objVar);
			transformExp(switchNode->target, temp);
			_buf << indent() << "local "sv << objVar << " = "sv << temp.back() << nll(switchNode);
			temp.back() = clearBuf();
		}
		const auto& branches = switchNode->branches.objects();
		for (auto branch_ : branches) {
			auto branch = static_cast<SwitchCase_t*>(branch_);
			temp.push_back(indent() + s(branches.front() == branch ? "if"sv : "elseif"sv));
			str_list tmp;
			const auto& exprs = branch->valueList->exprs.objects();
			for (auto exp_ : exprs) {
				auto exp = static_cast<Exp_t*>(exp_);
				transformExp(exp, tmp);
				if (!singleValueFrom(exp)) {
					tmp.back() = s("("sv) + tmp.back() + s(")"sv);
				}
				temp.back().append(s(" "sv) + tmp.back() + s(" == "sv) + objVar +
					s(exp == exprs.back() ? ""sv : " or"sv));
			}
			temp.back().append(s(" then"sv) + nll(branch));
			pushScope();
			transformBody(branch->body, temp, implicitReturn);
			popScope();
		}
		if (switchNode->lastBranch) {
			temp.push_back(indent() + s("else"sv) + nll(switchNode->lastBranch));
			pushScope();
			transformBody(switchNode->lastBranch, temp, implicitReturn);
			popScope();
		}
		temp.push_back(indent() + s("end"sv) + nlr(switchNode));
		out.push_back(join(temp));
	}

	void transformLocal(Local_t* local, str_list& out) {
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
			auto preDefine = getPredefine(defs);
			if (!preDefine.empty()) {
				out.push_back(preDefine + nll(local));
			}
		}
	}

	void transformBreakLoop(BreakLoop_t* breakLoop, str_list& out) {
		auto keyword = toString(breakLoop);
		if (keyword == "break"sv) {
			out.push_back(indent() + keyword + nll(breakLoop));
			return;
		}
		if (_continueVars.empty()) throw std::logic_error(debugInfo("Continue is not inside a loop."sv, breakLoop));
		_buf << indent() << _continueVars.top() << " = true"sv << nll(breakLoop);
		_buf << indent() << "break"sv << nll(breakLoop);
		out.push_back(clearBuf());
	}
};

const std::string MoonCompliler::Empty;

std::tuple<std::string,std::string,GlobalVars> moonCompile(const std::string& codes, const MoonConfig& config) {
	auto compiler = MoonCompliler{};
	auto result = compiler.complile(codes, config);
	auto globals = std::make_unique<std::list<GlobalVar>>();
	for (const auto& var : compiler.getGlobals()) {
		int line,col;
		std::tie(line,col) = var.second;
		globals->push_back({var.first, line, col});
	}
	return std::make_tuple(std::move(result.first),std::move(result.second),std::move(globals));
}

} // namespace MoonP
