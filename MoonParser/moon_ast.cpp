#include <string>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <vector>
#include <numeric>
#include <memory>
#include <sstream>
#include <string_view>
using namespace std::string_view_literals;
#include "moon_ast.h"

const input& AstLeaf::getValue() {
	if (_value.empty()) {
		_value.assign(m_begin.m_it, m_end.m_it);
		return trim(_value);
	}
	return _value;
}

#define AST_IMPL(type) \
	ast<type##_t> __##type##_t(type);

AST_IMPL(Num)
AST_IMPL(Name)
AST_IMPL(Variable)
AST_IMPL(self)
AST_IMPL(self_name)
AST_IMPL(self_class)
AST_IMPL(self_class_name)
AST_IMPL(SelfName)
AST_IMPL(KeyName)
AST_IMPL(VarArg)
AST_IMPL(local_flag)
AST_IMPL(Seperator)
AST_IMPL(NameList)
AST_IMPL(Local)
AST_IMPL(colon_import_name)
AST_IMPL(ImportName)
AST_IMPL(Import)
AST_IMPL(ExpListLow)
AST_IMPL(ExpList)
AST_IMPL(Return)
AST_IMPL(With)
AST_IMPL(SwitchCase)
AST_IMPL(Switch)
AST_IMPL(IfCond)
AST_IMPL(If)
AST_IMPL(Unless)
AST_IMPL(While)
AST_IMPL(for_step_value)
AST_IMPL(For)
AST_IMPL(ForEach)
AST_IMPL(Do)
AST_IMPL(Comprehension)
AST_IMPL(comp_value)
AST_IMPL(TblComprehension)
AST_IMPL(star_exp)
AST_IMPL(CompForEach)
AST_IMPL(CompFor)
AST_IMPL(CompClause)
AST_IMPL(CompInner)
AST_IMPL(Assign)
AST_IMPL(update_op)
AST_IMPL(Update)
AST_IMPL(BinaryOperator)
AST_IMPL(Assignable)
AST_IMPL(exp_op_value)
AST_IMPL(Exp)
AST_IMPL(Callable)
AST_IMPL(ChainValue)
AST_IMPL(simple_table)
AST_IMPL(SimpleValue)
AST_IMPL(Chain)
AST_IMPL(Value)
AST_IMPL(LuaString)
AST_IMPL(SingleString)
AST_IMPL(double_string_inner)
AST_IMPL(double_string_content)
AST_IMPL(DoubleString)
AST_IMPL(String)
AST_IMPL(Parens)
AST_IMPL(DotChainItem)
AST_IMPL(ColonChainItem)
AST_IMPL(default_value)
AST_IMPL(Slice)
AST_IMPL(Invoke)
AST_IMPL(TableLit)
AST_IMPL(TableBlock)
AST_IMPL(class_member_list)
AST_IMPL(ClassLine)
AST_IMPL(ClassBlock)
AST_IMPL(ClassDecl)
AST_IMPL(export_values)
AST_IMPL(export_op)
AST_IMPL(Export)
AST_IMPL(variable_pair)
AST_IMPL(normal_pair)
AST_IMPL(FnArgDef)
AST_IMPL(FnArgDefList)
AST_IMPL(outer_var_shadow)
AST_IMPL(FnArgsDef)
AST_IMPL(fn_arrow)
AST_IMPL(FunLit)
AST_IMPL(NameOrDestructure)
AST_IMPL(AssignableNameList)
AST_IMPL(InvokeArgs)
AST_IMPL(const_value)
AST_IMPL(unary_exp)
AST_IMPL(Assignment)
AST_IMPL(if_else_line)
AST_IMPL(unless_line)
AST_IMPL(statement_appendix)
AST_IMPL(BreakLoop)
AST_IMPL(Statement)
AST_IMPL(Body)
AST_IMPL(Line)
AST_IMPL(Block)
AST_IMPL(BlockEnd)

#include <iostream>

inline std::string s(std::string_view sv) {
	return std::string(sv);
}

class MoonCompliler
{
public:
	void complile(const std::string& codes) {
		input input = _converter.from_bytes(codes);
		error_list el;
		State st;
		auto root = parse<BlockEnd_t>(input, BlockEnd, el, &st);
		if (root) {
			std::cout << "compiled!\n\n";
			std::vector<std::string> out;
			pushScope();
			transformBlock(root->block, out);
			popScope();
			std::string result = std::move(out.back());
			std::cout << result << '\n';
		} else {
			std::cout << "compile failed!\n";
			for (error_list::iterator it = el.begin(); it != el.end(); ++it) {
				const error& err = *it;
				std::cout << "line " << err.m_begin.m_line << ", col " << err.m_begin.m_col << ": syntax error\n";
			}
		}
		_codeCache.clear();
		std::stack<std::string> empty;
		_withVars.swap(empty);
	}
private:
	int _indentOffset = 0;
	Converter _converter;
	std::vector<input> _codeCache;
	std::stack<std::string> _withVars;
	std::ostringstream _buf;
	std::string _newLine = "\n";
	std::vector<int> _lineTable;
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
		Common
	};

	enum class IfUsage {
		Return,
		Closure,
		Common
	};

	void pushScope() {
		_scopes.emplace_back();
		_scopes.back().vars = std::make_unique<std::unordered_set<std::string>>();
	}

	void popScope() {
		_scopes.pop_back();
	}

	bool isDefined(const std::string& name, bool checkShadowScopeOnly = false) {
		bool isDefined = false;
		int mode = int(std::isupper(name[0]) ? ExportMode::Capital : ExportMode::Any);
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			if (int(it->mode) >= mode) {
				if (it->exports) {
					if (it->exports->find(name) != it->exports->end()) {
						isDefined = true;
						break;
					}
				} else {
					isDefined = true;
					break;
				}
			}
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
		bool defined = false;
		auto& scope = _scopes.back();
		decltype(scope.allows.get()) allows = nullptr;
		for (auto it  = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			if (it->allows) allows = it->allows.get();
		}
		if (allows) {
			bool shadowed = allows->find(name) == allows->end();
			defined = isDefined(name, shadowed);
		} else {
			defined = isDefined(name);
		}
		if (!defined) scope.vars->insert(name);
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
		// return s(" -- "sv) + std::to_string(node->m_begin.m_line) + _newLine;
		_lineTable.push_back(node->m_begin.m_line);
		return _newLine;
	}

	const std::string nlr(ast_node* node) {
		// return s(" -- "sv) + std::to_string(node->m_end.m_line) + _newLine;
		_lineTable.push_back(node->m_end.m_line);
		return _newLine;
	}

	void setIndentOffset(int offset) {
		_indentOffset = offset;
	}

	std::string indent() {
		return std::string((_scopes.size() - 1 + _indentOffset) * 2, ' ');
	}

	std::string indent(int offset) {
		return std::string((_scopes.size() - 1 + _indentOffset + offset) * 2, ' ');
	}

	std::string clearBuf() {
		std::string str = _buf.str();
		_buf.str("");
		_buf.clear();
		return str;
	}

	std::string join(const std::vector<std::string>& items) {
		if (items.empty()) return Empty;
		else if (items.size() == 1) return items.front();
		return std::accumulate(items.begin()+1, items.end(), items.front(),
					[&](const std::string& a, const std::string& b) { return a + b; });
	}

	std::string join(const std::vector<std::string>& items, std::string_view sep) {
		if (items.empty()) return Empty;
		else if (items.size() == 1) return items.front();
		std::string sepStr = s(sep);
		return std::accumulate(items.begin()+1, items.end(), items.front(),
					[&](const std::string& a, const std::string& b) { return a + sepStr + b; });
	}

	std::string toString(ast_node* node) {
		return _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
	}

	std::string toString(input::iterator begin, input::iterator end) {
		return _converter.to_bytes(std::wstring(begin, end));
	}

	Value_t* singleValueFrom(ast_node* expList) {
		ast_node* singleValue = nullptr;
		expList->traverse([&](ast_node* n) {
			if (n->getId() == "Value"_id) {
				if (!singleValue) {
					singleValue = n;
					return traversal::Return;
				} else {
					singleValue = nullptr;
					return traversal::Stop;
				}
			}
			return traversal::Continue;
		});
		return static_cast<Value_t*>(singleValue);
	}

	SimpleValue_t* simpleSingleValueFrom(ast_node* expList) {
		auto value = singleValueFrom(expList);
		if (value && value->item.is<SimpleValue_t>()) {
			return static_cast<SimpleValue_t*>(value->item.get());
		}
		return nullptr;
	}

	Value_t* firstValueFrom(ast_node* expList) {
		Value_t* firstValue = nullptr;
		expList->traverse([&](ast_node* n) {
			if (n->getId() == "Value"_id) {
				firstValue = static_cast<Value_t*>(n);
				return traversal::Stop;
			}
			return traversal::Continue;
		});
		return firstValue;
	}

	void noop(ast_node* node, std::vector<std::string>& out) {
		auto str = _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
		out.push_back(s("<"sv) + node->getName() + s(">"sv) + trim(str));
		// out.push_back(trim(str));
	}

	void noopnl(ast_node* node, std::vector<std::string>& out) {
		auto str = _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
		out.push_back(s("<"sv) + node->getName() + s(">"sv) + trim(str) + nll(node));
		// out.push_back(trim(str) + nll(node));
	}

	Statement_t* lastStatementFrom(ast_node* body) {
		ast_node* last = nullptr;
		body->traverse([&](ast_node* n) {
			switch (n->getId()) {
				case "Statement"_id:
					last = n;
					return traversal::Return;
				default: return traversal::Continue;
			}
		});
		return static_cast<Statement_t*>(last);
	}

	template <class T>
	ast_ptr<T, false, false> toAst(std::string_view codes, rule& r) {
		_codeCache.push_back(_converter.from_bytes(s(codes)));
		error_list el;
		State st;
		return parse<T>(_codeCache.back(), r, el, &st);
	}

	template <class T>
	bool matchAst(std::string_view codes, rule& r) {
		error_list el;
		State st;
		input i = _converter.from_bytes(s(codes));
		return parse<T>(i, r, el, &st);
	}

	bool isChainValueCall(ChainValue_t* chainValue) {
		if (chainValue->arguments) return true;
		if (auto chain = chainValue->caller.as<Chain_t>()) {
			ast_node* last = chain->items.objects().back();
			return last->getId() == "Invoke"_id;
		}
		return false;
	}

	bool isColonChain(ChainValue_t* chainValue) {
		if (chainValue->arguments) return false;
		if (auto chain = chainValue->caller.as<Chain_t>()) {
			return chain->items.objects().back()->getId() == "ColonChainItem"_id;
		}
		return false;
	}

	std::vector<ast_node*> getChainList(ChainValue_t* chainValue) {
		std::vector<ast_node*> temp;
		switch (chainValue->caller->getId()) {
			case "Callable"_id:
				temp.push_back(chainValue->caller);
				break;
			case "Chain"_id:
				const auto& items = chainValue->caller.to<Chain_t>()->items.objects();
				temp.resize(items.size());
				std::copy(items.begin(), items.end(), temp.begin());
				break;
		}
		if (chainValue->arguments) {
			temp.push_back(chainValue->arguments);
		}
		return temp;
	}

	void transformStatement(Statement_t* statement, std::vector<std::string>& out) {
		if (statement->appendix) {
			if (auto assignment = statement->content.as<Assignment_t>()) {
				auto preDefine = getPredefine(transformAssignDefs(assignment->assignable));
				if (!preDefine.empty()) out.push_back(preDefine + nll(statement));
			}
			auto appendix = statement->appendix.get();
			switch (appendix->item->getId()) {
				case "if_else_line"_id: {
					auto if_else_line = appendix->item.to<if_else_line_t>();
					auto ifNode = new_ptr<If_t>();

					auto ifCond = new_ptr<IfCond_t>();
					ifCond->condition.set(if_else_line->condition);
					ifNode->nodes.push_back(ifCond);

					if (!ast_is<default_value_t>(if_else_line->elseExpr)) {
						auto exprList = new_ptr<ExpList_t>();
						exprList->exprs.push_back(if_else_line->elseExpr);
						auto stmt = new_ptr<Statement_t>();
						stmt->content.set(exprList);
						auto body = new_ptr<Body_t>();
						body->content.set(stmt);
						ifNode->nodes.push_back(body);
					}
					auto stmt = new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					auto body = new_ptr<Body_t>();
					body->content.set(stmt);
					ifNode->nodes.push_back(body);

					statement->appendix.set(nullptr);
					auto simpleValue = new_ptr<SimpleValue_t>();
					simpleValue->value.set(ifNode);
					auto value = new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = new_ptr<Exp_t>();
					exp->value.set(value);
					auto exprList = new_ptr<ExpList_t>();
					exprList->exprs.push_back(exp);
					statement->content.set(exprList);
					break;
				}
				case "unless_line"_id: {
					auto unless_line = appendix->item.to<unless_line_t>();
					auto unless = new_ptr<Unless_t>();

					auto ifCond = new_ptr<IfCond_t>();
					ifCond->condition.set(unless_line->condition);
					unless->nodes.push_back(ifCond);

					auto stmt = new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					auto body = new_ptr<Body_t>();
					body->content.set(stmt);
					unless->nodes.push_back(body);

					statement->appendix.set(nullptr);
					auto simpleValue = new_ptr<SimpleValue_t>();
					simpleValue->value.set(unless);
					auto value = new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = new_ptr<Exp_t>();
					exp->value.set(value);
					auto exprList = new_ptr<ExpList_t>();
					exprList->exprs.push_back(exp);
					statement->content.set(exprList);
					break;
				}
				case "CompInner"_id: {
					auto compInner = appendix->item.to<CompInner_t>();
					switch (compInner->compFor->getId()) {
						case "CompFor"_id: {
							auto compFor = compInner->compFor.to<CompFor_t>();
							auto forNode = new_ptr<For_t>();
							forNode->startValue.set(compFor->startValue);
							forNode->stopValue.set(compFor->stopValue);
							forNode->stepValue.set(compFor->stepValue);
							forNode->varName.set(compFor->varName);
							auto stmt = new_ptr<Statement_t>();
							stmt->content.set(statement->content);
							auto body = new_ptr<Body_t>();
							body->content.set(stmt);
							forNode->body.set(body);
							statement->appendix.set(nullptr);
							statement->content.set(forNode);
							break;
						}
						case "CompForEach"_id: {
							auto compForEach = compInner->compFor.to<CompForEach_t>();
							auto forEach = new_ptr<ForEach_t>();
							forEach->nameList.set(compForEach->nameList);
							forEach->loopValue.set(compForEach->loopValue);
							auto stmt = new_ptr<Statement_t>();
							stmt->content.set(statement->content);
							auto body = new_ptr<Body_t>();
							body->content.set(stmt);
							forEach->body.set(body);
							statement->appendix.set(nullptr);
							statement->content.set(forEach);
							break;
						}
						default: break;
					}
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
			case "Import"_id: transformImport(content, out); break;
			case "While"_id: transformWhile(content, out); break;
			case "With"_id: transformWith(static_cast<With_t*>(content), out); break;
			case "For"_id: transformFor(static_cast<For_t*>(content), out); break;
			case "ForEach"_id: transformForEach(static_cast<ForEach_t*>(content), out); break;
			case "Switch"_id: transformSwitch(content, out); break;
			case "Return"_id: transformReturn(static_cast<Return_t*>(content), out); break;
			case "Local"_id: transformLocal(content, out); break;
			case "Export"_id: transformExport(static_cast<Export_t*>(content), out); break;
			case "BreakLoop"_id: transformBreakLoop(content, out); break;
			case "Assignment"_id: transformAssignment(static_cast<Assignment_t*>(content), out); break;
			case "ExpList"_id: {
				auto expList = static_cast<ExpList_t*>(content);
				if (expList->exprs.objects().empty()) {
					out.push_back(Empty);
					break;
				}
				if (auto singleValue = singleValueFrom(expList)) {
					if (auto simpleValue = singleValue->item.as<SimpleValue_t>()) {
						auto value = simpleValue->value.get();
						bool specialSingleValue = true;
						switch (value->getId()) {
							case "If"_id: transformIf(static_cast<If_t*>(value), out); break;
							case "ClassDecl"_id: transformClassDecl(static_cast<ClassDecl_t*>(value), out); break;
							case "Unless"_id: transformUnless(static_cast<Unless_t*>(value), out); break;
							case "Switch"_id: transformSwitch(static_cast<Switch_t*>(value), out); break;
							case "With"_id: transformWith(static_cast<With_t*>(value), out); break;
							case "ForEach"_id: transformForEach(static_cast<ForEach_t*>(value), out); break;
							case "For"_id: transformFor(static_cast<For_t*>(value), out); break;
							case "While"_id: transformWhile(static_cast<While_t*>(value), out); break;
							case "Do"_id: transformDo(static_cast<Do_t*>(value), out); break;
							default: specialSingleValue = false; break;
						}
						if (specialSingleValue) {
							break;
						}
					}
					if (auto chainValue = singleValue->item.as<ChainValue_t>()) {
						if (isChainValueCall(chainValue)) {
							transformValue(singleValue, out);
							out.back() = indent() + out.back() + nlr(singleValue);
							break;
						}
					}
				}
				auto assign = new_ptr<Assign_t>();
				assign->values.dup(expList->exprs);
				auto assignment = new_ptr<Assignment_t>();
				assignment->assignable.set(toAst<ExpList_t>("_", ExpList));
				assignment->target.set(assign);
				transformAssignment(assignment, out);
				break;
			}
			default: break;
		}
	}

	std::vector<std::string> getAssignVars(ExpList_t* expList) {
		std::vector<std::string> vars;
		expList->traverse([&](ast_node* child) {
			if (child->getId() == "Value"_id) {
				if (auto target = child->getByPath<ChainValue_t, Callable_t, Variable_t>()) {
					auto name = toString(target);
					vars.push_back(name);
				} else {
					vars.push_back(Empty);
				}
				return traversal::Return;
			}
			return traversal::Continue;
		});
		return vars;
	}

	std::vector<std::string> transformAssignDefs(ExpList_t* expList) {
		std::vector<std::string> preDefs;
		expList->traverse([&](ast_node* child) {
			if (child->getId() == "Value"_id) {
				if (auto callable = child->getByPath<ChainValue_t, Callable_t>()) {
					if (ast_is<Variable_t>(callable->item)) {
						auto name = toString(callable->item);
						if (addToScope(name)) {
							preDefs.push_back(name);
						}
					} else if (callable->getByPath<SelfName_t, self_t>()) {
						auto self = s("self"sv);
						if (addToScope(self)) {
							preDefs.push_back(self);
						}
					}
				}
				return traversal::Return;
			}
			return traversal::Continue;
		});
		return preDefs;
	}

	std::string getPredefine(const std::vector<std::string>& defs) {
		if (defs.empty()) return Empty;
		return indent() + s("local "sv) + join(defs, ", "sv);
	}

	void transformAssignment(Assignment_t* assignment, std::vector<std::string>& out) {
		auto assign = ast_cast<Assign_t>(assignment->target);
		do {
			if (!assign || assign->values.objects().size() != 1) break;
			auto value = assign->values.objects().front();
			ast_node* item = nullptr;
			if (ast_is<If_t>(value)) {
				item = value;
			} else if (auto val = simpleSingleValueFrom(value)) {
				if (ast_is<If_t,Unless_t>(val->value)) {
					item = val->value;
				}
			}
			if (item) {
				auto expList = assignment->assignable.get();
				std::vector<std::string> temp;
				auto defs = transformAssignDefs(expList);
				if (!defs.empty()) temp.push_back(getPredefine(defs) + nll(expList));
				item->traverse([&](ast_node* node) {
					switch (node->getId()) {
						case "IfCond"_id: return traversal::Return;
						case "Body"_id: {
							auto body = static_cast<Body_t*>(node);
							auto last = lastStatementFrom(body);
							auto valueList = last ? last->content.as<ExpList_t>() : nullptr;
							if (last && valueList) {
								auto newAssignment = new_ptr<Assignment_t>();
								newAssignment->assignable.set(expList);
								auto assign = new_ptr<Assign_t>();
								assign->values.dup(valueList->exprs);
								newAssignment->target.set(assign);
								last->content.set(newAssignment);
							}
							return traversal::Return;
						}
						default: return traversal::Continue;
					}
				});
				switch (item->getId()) {
					case "If"_id: transformIf(static_cast<If_t*>(item), temp); break;
					case "Unless"_id: transformUnless(static_cast<Unless_t*>(item), temp); break;
				}
				out.push_back(join(temp));
				return;
			}
			auto exp = ast_cast<Exp_t>(value);
			if (!exp) break;
			auto simpleVal = exp->value->item.as<SimpleValue_t>();
			if (!simpleVal) break;
			auto valueItem = simpleVal->value.get();
			switch (valueItem->getId()) {
				case "Do"_id: {
					auto expList = assignment->assignable.get();
					auto doNode = static_cast<Do_t*>(valueItem);
					auto last = lastStatementFrom(doNode->body);
					auto valueList = last ? last->content.as<ExpList_t>() : nullptr;
					if (last && valueList) {
						auto newAssignment = new_ptr<Assignment_t>();
						newAssignment->assignable.set(expList);
						auto assign = new_ptr<Assign_t>();
						assign->values.dup(valueList->exprs);
						newAssignment->target.set(assign);
						last->content.set(newAssignment);
						std::string preDefine = getPredefine(transformAssignDefs(expList));
						auto nl = preDefine.empty() ? Empty : nll(assignment);
						transformDo(doNode, out);
						out.back() = preDefine + nl + out.back();
					}
					return;
				}
				case "Comprehension"_id: {
					auto expList = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(expList));
					auto nl = preDefine.empty() ? Empty : nll(assignment);
					transformCompInPlace(static_cast<Comprehension_t*>(valueItem), expList, out);
					out.back() = preDefine + nl + out.back();
					return;
				}
				case "TblComprehension"_id: {
					auto expList = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(expList));
					auto nl = preDefine.empty() ? Empty : nll(assignment);
					transformTblCompInPlace(static_cast<TblComprehension_t*>(valueItem), expList, out);
					out.back() = preDefine + nl + out.back();
					return;
				}
				case "For"_id: {
					std::vector<std::string> temp;
					auto expList = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(expList));
					auto nl = preDefine.empty() ? Empty : nll(assignment);
					transformForInPlace(static_cast<For_t*>(valueItem), temp, expList);
					out.push_back(preDefine + nl + temp.front());
					return;
				}
				case "ForEach"_id: {
					std::vector<std::string> temp;
					auto expList = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(expList));
					transformForEachInPlace(static_cast<ForEach_t*>(valueItem), temp, expList);
					auto nl = preDefine.empty() ? Empty : nll(assignment);
					out.push_back(preDefine + nl + temp.front());
					return;
				}
				case "ClassDecl"_id: {
					std::vector<std::string> temp;
					auto expList = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(expList));
					transformClassDecl(static_cast<ClassDecl_t*>(valueItem), temp, ExpUsage::Assignment, expList);
					auto nl = preDefine.empty() ? Empty : nll(assignment);
					out.push_back(preDefine + nl + temp.front());
					return;
				}
			}
			if (auto chainValue = exp->value->item.as<ChainValue_t>()) {
				if (isColonChain(chainValue)) {
					auto assignable = assignment->assignable.get();
					std::string preDefine = getPredefine(transformAssignDefs(assignable));
					transformColonChain(chainValue, out, ExpUsage::Assignment, static_cast<ExpList_t*>(assignable));
					auto nl = preDefine.empty() ? Empty : nll(chainValue);
					if (!preDefine.empty()) out.back() = preDefine + nl + out.back();
					return;
				}
			}
		} while (false);
		auto info = extractDestructureInfo(assignment);
		if (info.first.empty()) {
			transformAssignmentCommon(assignment, out);
		} else {
			std::vector<std::string> temp;
			for (const auto& destruct : info.first) {
				if (destruct.items.size() == 1) {
					auto& pair = destruct.items.front();
					_buf << indent();
					if (addToScope(pair.name)) {
						_buf << s("local "sv);
					}
					_buf << pair.name << " = "sv << info.first.front().value << pair.structure << nll(assignment);
					temp.push_back(clearBuf());
				} else if (matchAst<Name_t>(destruct.value, Name)) {
					std::vector<std::string> defs, names, values;
					for (const auto& item : destruct.items) {
						if (addToScope(item.name)) {
							defs.push_back(item.name);
						}
						names.push_back(item.name);
						values.push_back(item.structure);
					}
					if (!defs.empty()) _buf << indent() << "local "sv << join(defs,", "sv) << nll(assignment);
					for (auto& v : values) v.insert(0, destruct.value);
					_buf << indent() << join(names, ", "sv) << " = "sv << join(values, ", "sv) << nll(assignment);
					temp.push_back(clearBuf());
				} else {
					std::vector<std::string> defs, names, values;
					for (const auto& item : destruct.items) {
						if (addToScope(item.name)) {
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

	void transformAssignItem(ast_node* value, std::vector<std::string>& out) {
		switch (value->getId()) {
			case "With"_id: transformWith(static_cast<With_t*>(value), out); break;
			case "If"_id: transformIf(static_cast<If_t*>(value), out, IfUsage::Closure); break;
			case "Switch"_id: transformSwitch(value, out); break;
			case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(value), out); break;
			case "Exp"_id: transformExp(static_cast<Exp_t*>(value), out); break;
			default: break;
		}
	}

	std::list<std::pair<std::string, std::string>> destructFromExp(ast_node* node) {
		const std::list<ast_node*>* tableItems = nullptr;
		if (ast_cast<Exp_t>(node)) {
			auto item = singleValueFrom(node)->item.get();
			if (!item) throw std::logic_error("Invalid destructure value");
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
		std::list<std::pair<std::string, std::string>> pairs;
		int index = 0;
		for (auto pair : *tableItems) {
			switch (pair->getId()) {
				case "Exp"_id: {
					++index;
					auto item = singleValueFrom(pair)->item.get();
					if (!item) throw std::logic_error("Invalid destructure value");
					if (auto value = item->getByPath<Callable_t, Variable_t>()) {
						auto name = toString(value);
						//"Can't destructure value of type: parens"
						pairs.push_back({name, s("["sv) + std::to_string(index) + s("]"sv)});
						break;
					}
					if (ast_cast<simple_table_t>(item) ||
						item->getByPath<TableLit_t>()) {
						auto subPairs = destructFromExp(pair);
						for (auto& p : subPairs) {
							pairs.push_back({p.first, s("["sv) + std::to_string(index) + s("]"sv) + p.second});
						}
						break;
					}
					break;
				}
				case "variable_pair"_id: {
					auto vp = static_cast<variable_pair_t*>(pair);
					auto name = toString(vp->name);
					pairs.push_back({name, s("."sv) + name});
					break;
				}
				case "normal_pair"_id: {
					auto np = static_cast<normal_pair_t*>(pair);
					auto key = np->key->getByPath<Name_t>();
					if (!key) throw std::logic_error("Invalid key for destructure");
					if (auto exp = np->value.as<Exp_t>()) {
						auto item = singleValueFrom(exp)->item.get();
						if (!item) throw std::logic_error("Invalid destructure value");
						if (auto var = item->getByPath<Callable_t, Variable_t>()) {
							pairs.push_back({toString(var), s("."sv) + toString(key)});
							break;
						}
						if (ast_cast<simple_table_t>(item) ||
							item->getByPath<TableLit_t>()) {
							auto subPairs = destructFromExp(exp);
							for (auto& p : subPairs) {
								pairs.push_back({p.first, s("."sv) + toString(key) + p.second});
							}
							break;
						}
					}
					if (np->value.as<TableBlock_t>()) {
						auto subPairs = destructFromExp(pair);
						for (auto& p : subPairs) {
							pairs.push_back({p.first, s("."sv) + toString(key) + p.second});
						}
					}
					break;
				}
			}
		}
		return pairs;
	}

	std::pair<std::list<Destructure>, ast_ptr<Assignment_t, false, false>>
		extractDestructureInfo(Assignment_t* assignment) {
		std::list<Destructure> destructs;
		auto exprs = assignment->assignable->exprs.objects();
		auto values = assignment->target.to<Assign_t>()->values.objects();
		size_t size = std::max(exprs.size(),values.size());
		ast_ptr<Exp_t, false, false> var;
		if (exprs.size() < size) {
			var = toAst<Exp_t>("_"sv, Exp);
			while (exprs.size() < size) exprs.emplace_back(var);
		}
		ast_ptr<Exp_t, false, false> nullNode;
		if (values.size() < size) {
			nullNode = toAst<Exp_t>("nil"sv, Exp);
			while (values.size() < size) values.emplace_back(nullNode);
		}
		using iter = std::list<ast_node*>::iterator;
		std::vector<std::pair<iter, iter>> destructPairs;
		std::vector<std::string> temp;
		for (auto i = exprs.begin(), j = values.begin(); i != exprs.end(); ++i, ++j) {
			auto expr = *i;
			ast_node* destructNode = expr->getByPath<Value_t, SimpleValue_t, TableLit_t>();
			if (destructNode || (destructNode = expr->getByPath<Value_t, simple_table_t>())) {
				destructPairs.push_back({i,j});
				transformAssignItem(*j, temp);
				auto& destruct = destructs.emplace_back();
				destruct.value = temp.back();
				temp.pop_back();
				auto pairs = destructFromExp(expr);
				for (auto& pair : pairs) {
					auto& item = destruct.items.emplace_back();
					item.name = std::move(pair.first);
					item.structure = std::move(pair.second);
				}
			}
		}
		for (const auto& p : destructPairs) {
			exprs.erase(p.first);
			values.erase(p.second);
		}
		ast_ptr<Assignment_t, false, false> newAssignment;
		if (!destructPairs.empty() && !exprs.empty()) {
			auto expList = new_ptr<ExpList_t>();
			auto newAssign = new_ptr<Assignment_t>();
			newAssign->assignable.set(expList);
			for (auto expr : exprs) expList->exprs.push_back(expr);
			auto assign = new_ptr<Assign_t>();
			for (auto value : values) assign->values.push_back(value);
			newAssign->target.set(assign);
			newAssignment = newAssign;
		}
		return {std::move(destructs), newAssignment};
	}

	void transformAssignmentCommon(Assignment_t* assignment, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto expList = assignment->assignable.get();
		auto action = assignment->target.get();
		auto defs = transformAssignDefs(expList);
		std::string preDefine = getPredefine(defs);
		bool oneLined = defs.size() == expList->exprs.objects().size() &&
			traversal::Stop != action->traverse([&](ast_node* n) {
				if (n->getId() == "Callable"_id) {
					if (auto name = n->getByPath<Variable_t>()) {
						for (const auto& def : defs) {
							if (def ==toString(name)) {
								return traversal::Stop;
							}
						}
					}
				}
				return traversal::Continue;
			});
		transformExpList(expList, temp);
		std::string left = temp.back();
		temp.clear();
		switch (action->getId()) {
			case "Update"_id: transformUpdate(action, temp); break;
			case "Assign"_id: {
				auto assign = static_cast<Assign_t*>(action);
				for (auto value : assign->values.objects()) {
					transformAssignItem(value, temp);
				}
				break;
			}
			default: break;
		}
		if (oneLined) {
			out.push_back((preDefine.empty() ? indent() + left : preDefine) + s(" = "sv) + join(temp, ", "sv) + nll(assignment));
		} else {
			out.push_back((preDefine.empty() ? Empty : preDefine + nll(assignment)) + indent() + left + s(" = "sv) + join(temp, ", "sv) + nll(assignment));
		}
	}

	void transformCond(const std::list<ast_node*>& nodes, std::vector<std::string>& out, IfUsage usage = IfUsage::Common, bool unless = false) {
		std::vector<ast_ptr<ast_node, false, false>> ns;
		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
			ns.push_back(*it);
			if (auto cond = ast_cast<IfCond_t>(*it)) {
				if (*it != nodes.front() && cond->assign) {
					auto newIf = new_ptr<If_t>();
					for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
						newIf->nodes.push_back(*j);
					}
					ns.clear();
					auto simpleValue = new_ptr<SimpleValue_t>();
					simpleValue->value.set(newIf);
					auto value = new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = new_ptr<Exp_t>();
					exp->value.set(value);
					auto expList = new_ptr<ExpList_t>();
					expList->exprs.push_back(exp);
					auto stmt = new_ptr<Statement_t>();
					stmt->content.set(expList);
					auto body = new_ptr<Body_t>();
					body->content.set(stmt);
					ns.push_back(body.get());
				}
			}
		}
		if (nodes.size() != ns.size()) {
			auto newIf = new_ptr<If_t>();
			for (auto j = ns.rbegin(); j != ns.rend(); ++j) {
				newIf->nodes.push_back(*j);
			}
			transformCond(newIf->nodes.objects(), out, usage, unless);
			return;
		}
		std::vector<std::string> temp;
		if (usage == IfUsage::Closure) {
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
		if (assign) {
			auto exp = ifCondPairs.front().first->condition.get();
			auto expList = new_ptr<ExpList_t>();
			expList->exprs.push_back(exp);
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(expList);
			assignment->target.set(assign);
			if (usage != IfUsage::Closure) {
				temp.push_back(indent() + s("do"sv) + nll(assign));
				pushScope();
			}
			transformAssignment(assignment, temp);
		}
		for (const auto& pair : ifCondPairs) {
			if (pair.first) {
				std::vector<std::string> tmp;
				auto condition = pair.first->condition.get();
				transformExp(condition, tmp);
				_buf << indent() << (pair == ifCondPairs.front() ? ""sv : "else"sv) <<
					"if "sv << (unless ? "not ("sv : ""sv) << tmp.front() << (unless ? ") then"sv : " then"sv) << nll(condition);
				temp.push_back(clearBuf());
			}
			if (pair.second) {
				if (!pair.first) {
					temp.push_back(indent() + s("else"sv) + nll(pair.second));
				}
				pushScope();
				transformBody(pair.second, temp, usage != IfUsage::Common);
				popScope();
			}
			if (!pair.first) {
				temp.push_back(indent() + s("end"sv) + nll(nodes.front()));
				break;
			}
		}
		if (assign && usage != IfUsage::Closure) {
			popScope();
			temp.push_back(indent() + s("end"sv) + nlr(nodes.front()));
		}
		if (usage == IfUsage::Closure) {
			popScope();
			temp.push_back(indent() + s("end)()"sv));
		}
		out.push_back(join(temp));
	}

	void transformIf(If_t* ifNode, std::vector<std::string>& out, IfUsage usage = IfUsage::Common) {
		transformCond(ifNode->nodes.objects(), out, usage);
	}

	void transformUnless(Unless_t* unless, std::vector<std::string>& out, IfUsage usage = IfUsage::Common) {
		transformCond(unless->nodes.objects(), out, usage, true);
	}

	void transformExpList(ExpList_t* expList, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto exp : expList->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformExpListLow(ExpListLow_t* expListLow, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto exp : expListLow->exprs.objects()) {
			transformExp(static_cast<Exp_t*>(exp), temp);
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformExp(Exp_t* exp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformValue(exp->value, temp);
		for (auto _opValue : exp->opValues.objects()) {
			auto opValue = static_cast<exp_op_value_t*>(_opValue);
			transformBinaryOperator(opValue->op, temp);
			transformValue(opValue->value, temp);
		}
		out.push_back(join(temp, " "sv));
	}

	void transformValue(Value_t* value, std::vector<std::string>& out) {
		auto item = value->item.get();
		switch (item->getId()) {
			case "SimpleValue"_id: transformSimpleValue(static_cast<SimpleValue_t*>(item), out); break;
			case "simple_table"_id: transform_simple_table(static_cast<simple_table_t*>(item), out); break;
			case "ChainValue"_id: {
				auto chainValue = static_cast<ChainValue_t*>(item);
				if (isColonChain(chainValue)) {
					transformColonChainClosure(chainValue, out);
				} else {
					transformChainValue(chainValue, out);
				}
				break;
			}
			case "String"_id: transformString(static_cast<String_t*>(item), out); break;
			default: break;
		}
	}

	void transformChainValue(ChainValue_t* chainValue, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto caller = chainValue->caller.get();
		bool hasArgs = chainValue->arguments;
		switch (caller->getId()) {
			case "Chain"_id: transformChain(static_cast<Chain_t*>(caller), temp); break;
			case "Callable"_id: transformCallable(static_cast<Callable_t*>(caller), temp, hasArgs); break;
			default: break;
		}
		if (hasArgs) {
			transformInvokeArgs(chainValue->arguments, temp);
			out.push_back(temp[0] + s("("sv) + temp[1] + s(")"sv));
		} else {
			out.push_back(temp[0]);
		}
	}

	void transformCallable(Callable_t* callable, std::vector<std::string>& out, bool invoke) {
		auto item = callable->item.get();
		switch (item->getId()) {
			case "Variable"_id: transformVariable(static_cast<Variable_t*>(item), out); break;
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(item), out, invoke); break;
			case "VarArg"_id: out.push_back(s("..."sv)); break;
			case "Parens"_id: transformParens(static_cast<Parens_t*>(item), out); break;
			default: break;
		}
	}

	void transformParens(Parens_t* parans, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformExp(parans->expr, temp);
		out.push_back(s("("sv) + temp.front() + s(")"sv));
	}

	void transformSimpleValue(SimpleValue_t* simpleValue, std::vector<std::string>& out) {
		auto value = simpleValue->value.get();
		switch (value->getId()) {
			case "const_value"_id: transform_const_value(static_cast<const_value_t*>(value), out); break;
			case "If"_id: transformIf(static_cast<If_t*>(value), out, IfUsage::Closure); break;
			case "Unless"_id: transformUnless(static_cast<Unless_t*>(value), out, IfUsage::Closure); break;
			case "Switch"_id: transformSwitch(value, out); break;
			case "With"_id: transformWith(static_cast<With_t*>(value), out); break;
			case "ClassDecl"_id: transformClassDeclClosure(static_cast<ClassDecl_t*>(value), out); break;
			case "ForEach"_id: transformForEachClosure(static_cast<ForEach_t*>(value), out); break;
			case "For"_id: transformForClosure(static_cast<For_t*>(value), out); break;
			case "While"_id: transformWhile(value, out); break;
			case "Do"_id: transformDoClosure(static_cast<Do_t*>(value), out); break;
			case "unary_exp"_id: transform_unary_exp(static_cast<unary_exp_t*>(value), out); break;
			case "TblComprehension"_id: transformTblCompClosure(static_cast<TblComprehension_t*>(value), out); break;
			case "TableLit"_id: transformTableLit(static_cast<TableLit_t*>(value), out); break;
			case "Comprehension"_id: transformCompClosure(static_cast<Comprehension_t*>(value), out); break;
			case "FunLit"_id: transformFunLit(static_cast<FunLit_t*>(value), out); break;
			case "Num"_id: transformNum(static_cast<Num_t*>(value), out); break;
			default: break;
		}
	}

	void transformFunLit(FunLit_t* funLit, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		bool isFatArrow = toString(funLit->arrow) == "=>"sv;
		pushScope();
		if (auto argsDef = funLit->argsDef.get()) {
			transformFnArgsDef(argsDef, temp);
			if (funLit->body) {
				transformBody(funLit->body, temp, true);
			} else {
				temp.push_back(Empty);
			}
			auto& args = temp[0];
			auto& initArgs = temp[1];
			auto& bodyCodes = temp[2];
			_buf << "function("sv <<
				(isFatArrow ? s("self, "sv) : Empty) <<
				args << ')';
			if (!initArgs.empty() || !bodyCodes.empty()) {
				_buf << nlr(argsDef) << initArgs << bodyCodes;
				popScope();
				_buf << indent() << "end"sv;
			} else {
				_buf << " end"sv;
				popScope();
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
				_buf << " end"sv;
				popScope();
			}
		}
		out.push_back(clearBuf());
	}

	void transformCodes(ast_node* nodes, std::vector<std::string>& out, bool implicitReturn) {
		if (implicitReturn) {
			auto last = lastStatementFrom(nodes);
			if (ast_is<ExpList_t>(last->content) && (!last->appendix ||
				!last->appendix->item.is<CompInner_t>())) {
				auto expList = static_cast<ExpList_t*>(last->content.get());
				auto expListLow = new_ptr<ExpListLow_t>();
				expListLow->exprs = expList->exprs;
				auto returnNode = new_ptr<Return_t>();
				returnNode->valueList.set(expListLow);
				auto statement = ast_cast<Statement_t>(last);
				statement->content.set(returnNode);
			}
		}
		std::vector<std::string> temp;
		nodes->traverse([&](ast_node* node) {
			switch (node->getId()) {
				case "Statement"_id:
					transformStatement(static_cast<Statement_t*>(node), temp);
					return traversal::Return;
				default: return traversal::Continue;
			}
		});
		out.push_back(join(temp));
	}

	void transformBody(Body_t* body, std::vector<std::string>& out, bool implicitReturn = false) {
		transformCodes(body, out, implicitReturn);
	}

	void transformBlock(Block_t* block, std::vector<std::string>& out, bool implicitReturn = true) {
		transformCodes(block, out, implicitReturn);
	}

	void transformReturn(Return_t* returnNode, std::vector<std::string>& out) {
		if (auto valueList = returnNode->valueList.get()) {
			if (auto singleValue = singleValueFrom(valueList)) {
				if (auto comp = singleValue->getByPath<SimpleValue_t, Comprehension_t>()) {
					transformCompReturn(comp, out);
					return;
				}
				if (auto comp = singleValue->getByPath<SimpleValue_t, TblComprehension_t>()) {
					transformTblCompReturn(comp, out);
					return;
				}
				if (auto classDecl = singleValue->getByPath<SimpleValue_t, ClassDecl_t>()) {
					transformClassDecl(classDecl, out, ExpUsage::Return);
					return;
				}
				if (auto doNode = singleValue->getByPath<SimpleValue_t, Do_t>()) {
					transformDo(doNode, out, true);
					return;
				}
				if (auto chainValue = singleValue->getByPath<ChainValue_t>()) {
					if (isColonChain(chainValue)) {
						transformColonChain(chainValue, out, ExpUsage::Return);
						return;
					}
				}
				if (auto ifNode = singleValue->getByPath<SimpleValue_t, If_t>()) {
					transformIf(ifNode, out, IfUsage::Return);
					return;
				}
				transformValue(singleValue, out);
				out.back() = indent() + s("return "sv) + out.back() + nlr(returnNode);
				return;
			} else {
				std::vector<std::string> temp;
				transformExpListLow(valueList, temp);
				out.push_back(indent() + s("return "sv) + temp.back() + nlr(returnNode));
			}
		} else {
			out.push_back(s("return"sv) + nll(returnNode));
		}
	}

	void transformFnArgsDef(FnArgsDef_t* argsDef, std::vector<std::string>& out) {
		if (!argsDef->defList) {
			out.push_back(Empty);
			return;
		}
		transformFnArgDefList(argsDef->defList, out);
		if (argsDef->shadowOption) {
			transform_outer_var_shadow(argsDef->shadowOption, out);
		}
	}

	void transform_outer_var_shadow(outer_var_shadow_t* shadow, std::vector<std::string>& out) {
		markVarShadowed();
		if (shadow->varList) {
			for (auto name : shadow->varList->names.objects()) {
				addToAllowList(toString(name));
			}
		}
	}

	void transformFnArgDefList(FnArgDefList_t* argDefList, std::vector<std::string>& out) {
		struct ArgItem {
			std::string name;
			std::string assignSelf;
			std::string defaultVal;
		};
		std::list<ArgItem> argItems;
		std::vector<std::string> temp;
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
						case "self_class_name"_id:
							arg.name = toString(selfName->name->getFirstChild());
							arg.assignSelf = s("self.__class."sv) + arg.name;
							break;
						case "self_class"_id:
							arg.name = "self.__class"sv;
							break;
						case "self_name"_id:
							arg.name = toString(selfName->name->getFirstChild());
							arg.assignSelf = s("self."sv) + arg.name;
							break;
						case "self"_id:
							arg.name = "self"sv;
							break;
						default: break;
					}
					break;
				}
			}
			if (def->defaultValue) {
				transformExp(static_cast<Exp_t*>(def->defaultValue.get()), temp);
				arg.defaultVal = temp.front();
				temp.clear();
				_buf << indent() << "if "sv << arg.name << " == nil then"sv << nll(def) <<
					indent(1) << arg.name << " = "sv << arg.defaultVal << nll(def) <<
					indent() << "end"sv << nll(def);
			}
			if (varNames.empty()) varNames = arg.name;
			else varNames.append(s(", "sv) + arg.name);
			forceAddToScope(arg.name);
		}
		if (argDefList->varArg) {
			auto& arg = argItems.emplace_back();
			arg.name = "..."sv;
			if (varNames.empty()) varNames = arg.name;
			else varNames.append(s(", "sv) + arg.name);
		}
		std::string initCodes = clearBuf();
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

	void transformSelfName(SelfName_t* selfName, std::vector<std::string>& out, bool invoke) {
		auto name = selfName->name.get();
		switch (name->getId()) {
			case "self_class_name"_id:
				out.push_back(s("self.__class"sv) + s(invoke ? ":"sv : "."sv) + toString(name->getFirstChild()));
				break;
			case "self_class"_id:
				out.push_back(s("self.__class"sv));
				break;
			case "self_name"_id:
				out.push_back(s("self"sv) + s(invoke ? ":"sv : "."sv) + toString(name->getFirstChild()));
				break;
			case "self"_id:
				out.push_back(s("self"sv));
				break;
		}
	}

	void transformColonChainClosure(ChainValue_t* chainValue, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		temp.push_back(s("(function()"sv) + nll(chainValue));
		pushScope();
		transformColonChain(chainValue, temp, ExpUsage::Return);
		popScope();
		temp.push_back(s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformColonChain(ChainValue_t* chainValue, std::vector<std::string>& out, ExpUsage usage = ExpUsage::Common, ExpList_t* expList = nullptr) {
		std::vector<std::string> temp;
		auto chain = chainValue->caller.to<Chain_t>();
		const auto& chainList = chain->items.objects();
		auto end = --chainList.end();
		for (auto it = chainList.begin(); it != end; ++it) {
			auto item = *it;
			switch (item->getId()) {
				case "Invoke"_id: transformInvoke(static_cast<Invoke_t*>(item), temp); break;
				case "DotChainItem"_id: transformDotChainItem(static_cast<DotChainItem_t*>(item), temp); break;
				case "ColonChainItem"_id: transformColonChainItem(static_cast<ColonChainItem_t*>(item), temp); break;
				case "Slice"_id: transformSlice(static_cast<Slice_t*>(item), temp); break;
				case "Callable"_id: {
					auto next = it; ++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					transformCallable(static_cast<Callable_t*>(item), temp,
						followItem && followItem->getId() == "Invoke"_id);
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
				default: break;
			}
		}
		auto caller = join(temp);
		auto colonChainItem = static_cast<ColonChainItem_t*>(chainList.back());
		auto funcName = toString(colonChainItem->name);
		std::string assignList;
		if (expList) {
			std::vector<std::string> tmp;
			transformExpList(expList, tmp);
			assignList = tmp.back();
		}
		if (usage != ExpUsage::Return) pushScope();
		auto baseVar = getUnusedName("_base_"sv);
		addToScope(baseVar);
		auto fnVar = getUnusedName("_fn_"sv);
		addToScope(fnVar);
		if (usage != ExpUsage::Return) {
			_buf << indent(-1) << "do"sv << nll(chain);
		}
		_buf << indent() << "local "sv << baseVar << " = "sv << caller << nll(chain);
		_buf << indent() << "local "sv << fnVar << " = "sv << baseVar << "."sv << funcName << nll(chain);
		switch (usage) {
			case ExpUsage::Return:
				_buf << indent() << "return function(...)" << nll(chain);
				break;
			case ExpUsage::Assignment:
				_buf << indent() << assignList << " = function(...)"sv << nll(chain);
				break;
			case ExpUsage::Common:
				_buf << indent() << "_ = function(...)" << nll(chain);
				break;
			default: break;
		}
		_buf << indent(1) << "return "sv << fnVar << "("sv << baseVar << ", ...)"sv << nll(chain);
		_buf << indent() << "end"sv << nll(chain);
		if (usage != ExpUsage::Return) {
			popScope();
			_buf << indent() << "end"sv << nll(chain);
		}
		out.push_back(clearBuf());
	}

	void transformChain(Chain_t* chain, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		const auto& chainList = chain->items.objects();
		switch (chainList.front()->getId()) {
			case "DotChainItem"_id:
			case "ColonChainItem"_id:
				if (_withVars.empty()) {
					throw std::logic_error("Short-dot syntax must be called within a with block.");
				} else {
					temp.push_back(_withVars.top());
				}
				break;
		}
		for (auto it = chainList.begin(); it != chainList.end(); ++it) {
			auto item = *it;
			switch (item->getId()) {
				case "Invoke"_id: transformInvoke(static_cast<Invoke_t*>(item), temp); break;
				case "DotChainItem"_id: transformDotChainItem(static_cast<DotChainItem_t*>(item), temp); break;
				case "ColonChainItem"_id: transformColonChainItem(static_cast<ColonChainItem_t*>(item), temp); break;
				case "Slice"_id: transformSlice(static_cast<Slice_t*>(item), temp); break;
				case "Callable"_id: {
					auto next = it; ++next;
					auto followItem = next != chainList.end() ? *next : nullptr;
					transformCallable(static_cast<Callable_t*>(item), temp,
						followItem && followItem->getId() == "Invoke"_id);
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
				default: break;
			}
		}
		out.push_back(join(temp));
	}

	void transformDotChainItem(DotChainItem_t* dotChainItem, std::vector<std::string>& out) {
		out.push_back(s("."sv) + toString(dotChainItem->name));
	}

	void transformColonChainItem(ColonChainItem_t* colonChainItem, std::vector<std::string>& out) {
		out.push_back(s(colonChainItem->switchToDot ? "."sv : ":"sv) + toString(colonChainItem->name));
	}

	void transformSlice(Slice_t* slice, std::vector<std::string>& out) {
		throw std::logic_error("Slice syntax not supported here");
	}

	void transformInvoke(Invoke_t* invoke, std::vector<std::string>& out) {
		std::vector<std::string> temp;
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

	void transform_unary_exp(unary_exp_t* unary_exp, std::vector<std::string>& out) {
		std::string op = toString(unary_exp->m_begin.m_it, unary_exp->item->m_begin.m_it);
		std::vector<std::string> temp{op + (op == "not"sv ? op + " " : Empty)};
		transformExp(unary_exp->item, temp);
		out.push_back(join(temp));
	}

	void transformVariable(Variable_t* name, std::vector<std::string>& out) {
		out.push_back(toString(name));
	}

	void transformNum(Num_t* num, std::vector<std::string>& out) {
		out.push_back(toString(num));
	}

	void transformTableLit(TableLit_t* tableLit, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		ast_node* lastNode = nullptr;
		for (auto value : tableLit->values.objects()) {
			switch (value->getId()) {
				case "variable_pair"_id:
					transform_variable_pair(static_cast<variable_pair_t*>(value), temp);
					break;
				case "normal_pair"_id:
					transform_normal_pair(static_cast<normal_pair_t*>(value), temp);
					break;
				case "Exp"_id:
					pushScope();
					transformExp(static_cast<Exp_t*>(value), temp);
					popScope();
					break;
				default: break;
			}
			temp.back() = (lastNode ? s(","sv) + nll(lastNode) : Empty) + indent(1) + temp.back();
			lastNode = value;
		}
		out.push_back(s("{"sv) + nll(tableLit) + join(temp) + nlr(tableLit) + indent() + s("}"sv));
	}

	void transformComprehension(Comprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string accum = getUnusedName("_accum_");
		std::string len = getUnusedName("_len_");
		addToScope(accum);
		addToScope(len);
		auto compInner = comp->forLoop.get();
		switch (compInner->compFor->getId()) {
			case "CompForEach"_id:
				transformCompForEach(compInner->compFor.to<CompForEach_t>(), temp);
				break;
			case "CompFor"_id:
				transformCompFor(compInner->compFor.to<CompFor_t>(), temp);
				break;
			default: break;
		}
		std::vector<std::string> clauseCodes;
		for (auto clause : compInner->clauses.objects()) {
			pushScope();
			auto child = clause->getFirstChild();
			switch (child->getId()) {
				case "CompForEach"_id:
					transformCompForEach(static_cast<CompForEach_t*>(child), clauseCodes);
					break;
				case "CompFor"_id: transformCompFor(static_cast<CompFor_t*>(child), clauseCodes); break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(child), clauseCodes);
					clauseCodes.back() = indent() + s("if "sv) + clauseCodes.back() + s(" then"sv) + nll(clause);
					break;
				default: break;
			}
		}
		pushScope();
		transformExp(comp->value, temp);
		popScope();
		auto value = temp.back();
		temp.pop_back();
		for (size_t i = 0; i < compInner->clauses.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(comp);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(comp);
		_buf << temp.back();
		pushScope();
		if (clauseCodes.empty()) {
			_buf << indent() << accum << "["sv << len << "] = "sv << value << nll(comp);
			_buf << indent() << len << " = "sv << len << " + 1"sv << nll(comp);
		} else {
			_buf << join(clauseCodes);
			_buf << indent(int(clauseCodes.size())) << accum << "["sv << len << "] = "sv << value << nll(comp);
			_buf << indent(int(clauseCodes.size())) << len << " = "sv << len << " + 1"sv << nll(comp);
			for (int ind = int(clauseCodes.size()) - 1; ind > -1 ; --ind) {
				_buf << indent(ind) << "end"sv << nll(comp);
			}
		}
		popScope();
		_buf << indent() << "end"sv << nll(comp);
		out.push_back(accum);
		out.push_back(clearBuf());
	}

	void transformCompInPlace(Comprehension_t* comp, ExpList_t* expList, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		pushScope();
		transformComprehension(comp, temp);
		transformExpList(expList, temp);
		out.push_back(
			s("do"sv) + nll(comp) +
			temp[1] +
			indent() + temp.back() + s(" = "sv) + temp.front() + nll(comp));
		popScope();
		out.back() = out.back() + indent() + s("end"sv) + nlr(comp);
	}

	void transformCompReturn(Comprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformComprehension(comp, temp);
		out.push_back(temp.back() + indent() + s("return "sv) + temp.front() + nlr(comp));
	}

	void transformCompClosure(Comprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string before = s("(function()"sv) + nll(comp);
		pushScope();
		transformComprehension(comp, temp);
		out.push_back(
			before +
			temp.back() +
			indent() + s("return "sv) + temp.front() + nlr(comp));
		popScope();
		out.back() = out.back() + indent() + s("end)()"sv);
	}

	void transformForEachHead(AssignableNameList_t* nameList, ast_node* loopTarget, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::vector<std::string> vars;
		std::list<std::pair<ast_node*, ast_ptr<ast_node,false,false>>> destructPairs;
		for (auto _item : nameList->items.objects()) {
			auto item = static_cast<NameOrDestructure_t*>(_item)->item.get();
			switch (item->getId()) {
				case "Variable"_id:
					transformVariable(static_cast<Variable_t*>(item), vars);
					break;
				case "TableLit"_id: {
					auto desVar = getUnusedName("_des_"sv);
					destructPairs.emplace_back(item, toAst<Exp_t>(desVar, Exp));
					vars.push_back(desVar);
					break;
				}
				default: break;
			}
		}
		switch (loopTarget->getId()) {
			case "star_exp"_id: {
				auto star_exp = static_cast<star_exp_t*>(loopTarget);
				auto listVar = getUnusedName("_list_");
				auto indexVar = getUnusedName("_index_");
				addToScope(listVar);
				addToScope(indexVar);
				auto value = singleValueFrom(star_exp->value);
				if (!value) throw std::logic_error("Invalid star syntax");
				bool endWithSlice = false;
				do {
					auto chainValue = value->item.as<ChainValue_t>();
					if (!chainValue) break;
					auto chainList = getChainList(chainValue);
					auto slice = ast_cast<Slice_t>(chainList.back());
					if (!slice) break;
					endWithSlice = true;
					chainList.pop_back();
					auto chain = new_ptr<Chain_t>();
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
					transformChain(chain, temp);
					_buf << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
					std::string maxVar;
					if (!stopValue.empty()) {
						maxVar = getUnusedName("_max_");
						addToScope(maxVar);
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
				} while (false);
				if (!endWithSlice) {
					transformExp(star_exp->value, temp);
					_buf << indent() << "local "sv << listVar << " = "sv << temp.back() << nll(nameList);
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
		if (!destructPairs.empty()) {
			temp.clear();
			pushScope();
			for (auto& pair : destructPairs) {
				auto sValue = new_ptr<SimpleValue_t>();
				sValue->value.set(pair.first);
				auto value = new_ptr<Value_t>();
				value->item.set(sValue);
				auto exp = new_ptr<Exp_t>();
				exp->value.set(value);
				auto expList = new_ptr<ExpList_t>();
				expList->exprs.push_back(exp);
				auto assign = new_ptr<Assign_t>();
				assign->values.push_back(pair.second);
				auto assignment = new_ptr<Assignment_t>();
				assignment->assignable.set(expList);
				assignment->target.set(assign);
				transformAssignment(assignment, temp);
			}
			out.back().append(join(temp));
			popScope();
		}
	}

	void transformCompForEach(CompForEach_t* comp, std::vector<std::string>& out) {
		transformForEachHead(comp->nameList, comp->loopValue, out);
	}

	void transformInvokeArgs(InvokeArgs_t* invokeArgs, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto arg : invokeArgs->args.objects()) {
			switch (arg->getId()) {
				case "Exp"_id: transformExp(static_cast<Exp_t*>(arg), temp); break;
				case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(arg), temp); break;
				default: break;
			}
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformForHead(For_t* forNode, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string varName = toString(forNode->varName);
		transformExp(forNode->startValue, temp);
		transformExp(forNode->stopValue, temp);
		if (forNode->stepValue) {
			transformExp(forNode->stepValue->value, temp);
		} else {
			temp.emplace_back();
		}
		_buf << indent() << "for "sv << varName << " = "sv << temp[0] << ", "sv << temp[1] << (temp[2].empty() ? Empty : s(", "sv) + temp[2]) << " do"sv << nll(forNode);
		out.push_back(clearBuf());
	}

	void transformFor(For_t* forNode, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformForHead(forNode, temp);
		pushScope();
		transformBody(forNode->body, temp);
		popScope();
		out.push_back(temp[0] + temp[1] + indent() + s("end"sv) + nlr(forNode));
	}

	void transformForClosure(For_t* forNode, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string accum = getUnusedName("_accum_");
		std::string len = getUnusedName("_len_");
		addToScope(accum);
		addToScope(len);
		_buf << "(function()"sv << nll(forNode);
		pushScope();
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forNode);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forNode);
		temp.push_back(clearBuf());
		transformForHead(forNode, temp);
		auto last = lastStatementFrom(forNode->body);
		bool hasTableItem = ast_is<ExpList_t>(last->content);
		if (hasTableItem) {
			_buf << accum << "["sv << len << "]"sv;
			std::string assignLeft = clearBuf();
			auto expList = toAst<ExpList_t>(assignLeft, ExpList);
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(expList);
			auto assign = new_ptr<Assign_t>();
			assign->values.dup(ast_cast<ExpList_t>(last->content)->exprs);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forNode->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forNode->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forNode) + indent() + s("return "sv) + accum + nlr(forNode));
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformForInPlace(For_t* forNode,  std::vector<std::string>& out, ExpList_t* assignExpList) {
		std::vector<std::string> temp;
		std::string accum = getUnusedName("_accum_");
		std::string len = getUnusedName("_len_");
		_buf << indent() << "do"sv << nll(forNode);
		pushScope();
		addToScope(accum);
		addToScope(len);
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forNode);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forNode);
		temp.push_back(clearBuf());
		transformForHead(forNode, temp);
		auto last = lastStatementFrom(forNode->body);
		bool hasTableItem = ast_is<ExpList_t>(last->content);
		if (hasTableItem) {
			_buf << accum << "["sv << len << "]"sv;
			std::string assignLeft = clearBuf();
			auto expList = toAst<ExpList_t>(assignLeft, ExpList);
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(expList);
			auto assign = new_ptr<Assign_t>();
			assign->values.dup(ast_cast<ExpList_t>(last->content)->exprs);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forNode->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forNode->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forNode));
		transformExpList(assignExpList, temp);
		temp.back() = indent() + temp.back() + s(" = "sv) + accum + nlr(forNode);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forNode));
		out.push_back(join(temp));
	}

	void transformBinaryOperator(BinaryOperator_t* node, std::vector<std::string>& out) {
		auto op = toString(node);
		out.push_back(op == "!="sv ? s("~="sv) : op);
	}

	void transformForEach(ForEach_t* forEach, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformForEachHead(forEach->nameList, forEach->loopValue, temp);
		pushScope();
		transformBody(forEach->body, temp);
		popScope();
		out.push_back(temp[0] + temp[1] + indent() + s("end"sv) + nlr(forEach));
	}

	void transformForEachClosure(ForEach_t* forEach, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string accum = getUnusedName("_accum_");
		std::string len = getUnusedName("_len_");
		addToScope(accum);
		addToScope(len);
		_buf << "(function()"sv << nll(forEach);
		pushScope();
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forEach);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forEach);
		temp.push_back(clearBuf());
		transformForEachHead(forEach->nameList, forEach->loopValue, temp);
		auto last = lastStatementFrom(forEach->body);
		bool hasTableItem = ast_is<ExpList_t>(last->content);
		if (hasTableItem) {
			_buf << accum << "["sv << len << "]"sv;
			std::string assignLeft = clearBuf();
			auto expList = toAst<ExpList_t>(assignLeft, ExpList);
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(expList);
			auto assign = new_ptr<Assign_t>();
			assign->values.dup(ast_cast<ExpList_t>(last->content)->exprs);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forEach->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forEach->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forEach) + indent() + s("return "sv) + accum + nlr(forEach));
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformForEachInPlace(ForEach_t* forEach,  std::vector<std::string>& out, ExpList_t* assignExpList) {
		std::vector<std::string> temp;
		std::string accum = getUnusedName("_accum_");
		std::string len = getUnusedName("_len_");
		_buf << indent() << "do"sv << nll(forEach);
		pushScope();
		addToScope(accum);
		addToScope(len);
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(forEach);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(forEach);
		temp.push_back(clearBuf());
		transformForEachHead(forEach->nameList, forEach->loopValue, temp);
		auto last = lastStatementFrom(forEach->body);
		bool hasTableItem = ast_is<ExpList_t>(last->content);
		if (hasTableItem) {
			_buf << accum << "["sv << len << "]"sv;
			std::string assignLeft = clearBuf();
			auto expList = toAst<ExpList_t>(assignLeft, ExpList);
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(expList);
			auto assign = new_ptr<Assign_t>();
			assign->values.dup(ast_cast<ExpList_t>(last->content)->exprs);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forEach->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forEach->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forEach));
		transformExpList(assignExpList, temp);
		temp.back() = indent() + temp.back() + s(" = "sv) + accum + nlr(forEach);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forEach));
		out.push_back(join(temp));
	}

	void transform_variable_pair(variable_pair_t* pair, std::vector<std::string>& out) {
		auto name = toString(pair->name);
		out.push_back(name + s(" = "sv) + name);
	}

	void transform_normal_pair(normal_pair_t* pair, std::vector<std::string>& out) {
		pushScope();
		auto key = pair->key.get();
		std::vector<std::string> temp;
		switch (key->getId()) {
			case "KeyName"_id: transformKeyName(static_cast<KeyName_t*>(key), temp); break;
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
		out.push_back(temp[0] + s(" = "sv) + temp[1]);
		popScope();
	}

	void transformKeyName(KeyName_t* keyName, std::vector<std::string>& out) {
		auto name = keyName->name.get();
		switch (name->getId()) {
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(name), out, false); break;
			case "Name"_id: out.push_back(toString(name)); break;
			default: break;
		}
	}

	void transformLuaString(LuaString_t* luaString, std::vector<std::string>& out) {
		out.push_back(toString(luaString));
	}

	void transformSingleString(SingleString_t* singleString, std::vector<std::string>& out) {
		out.push_back(toString(singleString));
	}

	void transformDoubleString(DoubleString_t* doubleString, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto _seg : doubleString->segments.objects()) {
			auto seg = static_cast<double_string_content_t*>(_seg);
			auto content = seg->content.get();
			switch (content->getId()) {
				case "double_string_inner"_id:
					temp.push_back(s("\""sv) + toString(content) + s("\""sv));
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(content), temp);
					temp.back() = s("tostring("sv) + temp.back() + s(")"sv);
					break;
				default: break;
			}
		}
		out.push_back(join(temp, " .. "sv));
	}

	void transformString(String_t* string, std::vector<std::string>& out) {
		auto str = string->str.get();
		switch (str->getId()) {
			case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(str), out); break;
			case "DoubleString"_id: transformDoubleString(static_cast<DoubleString_t*>(str), out); break;
			case "LuaString"_id: transformLuaString(static_cast<LuaString_t*>(str), out); break;
			default: break;
		}
	}

	std::pair<std::string,bool> defineClassVariable(Assignable_t* assignable) {
		if (assignable->item->getId() == "Variable"_id) {
			auto variable = static_cast<Variable_t*>(assignable->item.get());
			auto name = toString(variable);
			if (addToScope(name)) {
				return {name, true};
			} else {
				return {name, false};
			}
		}
		return {Empty, false};
	}

	void transformClassDeclClosure(ClassDecl_t* classDecl, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		temp.push_back(s("(function()"sv) + nll(classDecl));
		pushScope();
		transformClassDecl(classDecl, temp, ExpUsage::Return);
		popScope();
		temp.push_back(s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformClassDecl(ClassDecl_t* classDecl, std::vector<std::string>& out, ExpUsage usage = ExpUsage::Common, ExpList_t* expList = nullptr) {
		std::vector<std::string> temp;
		auto body = classDecl->body.get();
		auto assignable = classDecl->name.get();
		auto extend = classDecl->extend.get();
		std::string className;
		std::string assignItem;
		if (assignable) {
			bool newDefined = false;
			std::tie(className, newDefined) = defineClassVariable(assignable);
			if (newDefined) {
				temp.push_back(indent() + s("local "sv) + className + nll(classDecl));
			}
			if (className.empty()) {
				if (auto chain = ast_cast<Chain_t>(assignable->item)) {
					if (auto dotChain = ast_cast<DotChainItem_t>(chain->items.objects().back())) {
						className = s("\""sv) + toString(dotChain->name) + s("\""sv);
					} else if (auto index = ast_cast<Exp_t>(chain->items.objects().back())) {
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
			auto firstValue = firstValueFrom(expList);
			if (auto name = firstValue->getByPath<ChainValue_t, Callable_t, Variable_t>()) {
				className = s("\""sv) + toString(name) + s("\""sv);
			}
		}
		temp.push_back(indent() + s("do"sv) + nll(classDecl));
		pushScope();
		auto classVar = getUnusedName("_class_"sv);
		addToScope(classVar);
		temp.push_back(indent() + s("local "sv) + classVar + nll(classDecl));
		if (body) {
			std::vector<std::string> varDefs;
			body->traverse([&](ast_node* node) {
				if (node->getId() == "Statement"_id) {
					if (auto assignment = node->getByPath<Assignment_t>()) {
						auto names = transformAssignDefs(assignment->assignable.get());
						varDefs.insert(varDefs.end(), names.begin(), names.end());
						auto info = extractDestructureInfo(assignment);
						if (!info.first.empty()) {
							for (const auto& destruct : info.first)
								for (const auto& item : destruct.items)
									if (addToScope(item.name))
										varDefs.push_back(item.name);
						}
					}
					return traversal::Return;
				}
				return traversal::Continue;
			});
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
		auto selfVar = getUnusedName("_self_"sv);
		addToScope(baseVar);
		addToScope(selfVar);
		temp.push_back(indent() + s("local "sv) + baseVar + s(" = "sv));
		std::vector<std::string> builtins;
		std::vector<std::string> commons;
		std::vector<std::string> statements;
		if (body) {
			std::list<ClassMember> members;
			for (auto _classLine : classDecl->body->lines.objects()) {
				auto classLine = static_cast<ClassLine_t*>(_classLine);
				auto content = classLine->content.get();
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
		_buf << indent(2) << "local " << selfVar << " = setmetatable({}, "sv << baseVar << ")"sv << nll(classDecl);
		_buf << indent(2) << "cls.__init("sv << selfVar << ", ...)"sv << nll(classDecl);
		_buf << indent(2) << "return "sv << selfVar << nll(classDecl);
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
				std::vector<std::string> tmp;
				transformExpList(expList, tmp);
				_buf << indent() << tmp.back() << " = "sv << classVar << nlr(classDecl);
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
		std::vector<std::string> temp;
		size_t count = 0;
		for (auto keyValue : class_member_list->values.objects()) {
			MemType type = MemType::Common;
			do {
				auto normal_pair = ast_cast<normal_pair_t>(keyValue);
				if (!normal_pair) break;
				auto keyName = normal_pair->key.as<KeyName_t>();
				if (!keyName) break;
				input newSuperCall;
				auto selfName = keyName->name.as<SelfName_t>();
				if (selfName) {
					type = MemType::Property;
					auto name = ast_cast<self_name_t>(selfName->name);
					if (!name) throw std::logic_error("Invalid class poperty name");
					newSuperCall = _converter.from_bytes(classVar) + L".__parent." + _converter.from_bytes(toString(name->name));
				} else {
					auto nameNode = keyName->name.as<Name_t>();
					if (!nameNode) break;
					auto name = toString(nameNode);
					if (name == "new"sv) {
						type = MemType::Builtin;
						keyName->name.set(toAst<Name_t>("__init"sv, Name));
						newSuperCall = _converter.from_bytes(classVar) + L".__parent.__init";
					} else {
						newSuperCall = _converter.from_bytes(classVar) + L".__parent.__base." + _converter.from_bytes(name);
					}
				}
				normal_pair->value->traverse([&](ast_node* node) {
					if (node->getId() == "ClassDecl"_id) return traversal::Return;
					if (auto chainValue = ast_cast<ChainValue_t>(node)) {
						if (auto var = chainValue->caller->getByPath<Variable_t>()) {
							if (toString(var) == "super"sv) {
								if (chainValue->arguments) {
									chainValue->arguments->args.push_front(toAst<Exp_t>("self"sv, Exp));
									_codeCache.push_back(newSuperCall);
									var->m_begin.m_it = _codeCache.back().begin();
									var->m_end.m_it = _codeCache.back().end();
								} else {
									_codeCache.push_back(_converter.from_bytes(classVar) + L".__parent");
									var->m_begin.m_it = _codeCache.back().begin();
									var->m_end.m_it = _codeCache.back().end();
								}
							}
						} else if (auto var = chainValue->caller->getByPath<Callable_t, Variable_t>()) {
							if (toString(var) == "super"sv) {
								auto insertSelfToArguments = [&](ast_node* item) {
									switch (item->getId()) {
										case "InvokeArgs"_id: {
											auto invoke = static_cast<InvokeArgs_t*>(item);
											invoke->args.push_front(toAst<Exp_t>("self"sv, Exp));
											return true;
										}
										case "Invoke"_id: {
											auto invoke = static_cast<Invoke_t*>(item);
											invoke->args.push_front(toAst<Exp_t>("self"sv, Exp));
											return true;
										}
										default:
											return false;
									}
								};
								auto chainList = getChainList(chainValue);
								if (chainList.size() >= 2) {
									if (insertSelfToArguments(chainList[1])) {
										_codeCache.push_back(newSuperCall);
									} else {
										if (auto colonChainItem = ast_cast<ColonChainItem_t>(chainList[1])) {
											if (chainList.size() > 2 && insertSelfToArguments(chainList[2])) {
												colonChainItem->switchToDot = true;
											}
										}
										_codeCache.push_back(_converter.from_bytes(classVar) + L".__parent");
									}
									var->m_begin.m_it = _codeCache.back().begin();
									var->m_end.m_it = _codeCache.back().end();
								} else {
									_codeCache.push_back(_converter.from_bytes(classVar) + L".__parent");
									var->m_begin.m_it = _codeCache.back().begin();
									var->m_end.m_it = _codeCache.back().end();
								}
							}
						}
					}
					return traversal::Continue;
				});
			} while (false);
			if (type == MemType::Property) {
				setIndentOffset(-1);
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
				setIndentOffset(0);
			}
			out.push_back({temp.back(), type, keyValue});
			temp.clear();
			++count;
		}
		return count;
	}

	void transformAssignable(Assignable_t* assignable, std::vector<std::string>& out) {
		auto item = assignable->item.get();
		switch (item->getId()) {
			case "Chain"_id: transformChain(static_cast<Chain_t*>(item), out); break;
			case "Variable"_id: transformVariable(static_cast<Variable_t*>(item), out); break;
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(item), out, false); break;
			default: break;
		}
	}

	void transformWith(With_t* with, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		temp.push_back(indent() + s("do"sv) + nll(with));
		pushScope();
		std::string withVar;
		if (with->assigns) {
			auto vars = getAssignVars(with->valueList);
			if (vars.front().empty()) {
				withVar = getUnusedName("_with_");
				{
					auto assignment = new_ptr<Assignment_t>();
					assignment->assignable.set(toAst<ExpList_t>(withVar, ExpList));
					auto assign = new_ptr<Assign_t>();
					assign->values.push_back(with->assigns->values.objects().front());
					assignment->target.set(assign);
					transformAssignment(assignment, temp);
				}
				{
					auto assignment = new_ptr<Assignment_t>();
					assignment->assignable.set(with->valueList);
					auto assign = new_ptr<Assign_t>();
					assign->values.push_back(toAst<Exp_t>(withVar, Exp));
					bool skipFirst = true;
					for (auto value : with->assigns->values.objects()) {
						if (skipFirst) {
							skipFirst = false;
							continue;
						}
						assign->values.push_back(value);
					}
					assignment->target.set(assign);
					transformAssignment(assignment, temp);
				}
			} else {
				withVar = vars.front();
				auto assignment = new_ptr<Assignment_t>();
				assignment->assignable.set(with->valueList);
				assignment->target.set(with->assigns);
				transformAssignment(assignment, temp);
			}
		} else {
			withVar = getUnusedName("_with_");
			auto assignment = new_ptr<Assignment_t>();
			assignment->assignable.set(toAst<ExpList_t>(withVar, ExpList));
			auto assign = new_ptr<Assign_t>();
			assign->values.dup(with->valueList->exprs);
			assignment->target.set(assign);
			transformAssignment(assignment, temp);
		}
		_withVars.push(withVar);
		transformBody(with->body, temp);
		_withVars.pop();
		popScope();
		temp.push_back(indent() + s("end"sv) + nll(with));
		out.push_back(join(temp));
	}

	void transform_const_value(const_value_t* const_value, std::vector<std::string>& out) {
		out.push_back(toString(const_value));
	}

	void transformExport(Export_t* exportNode, std::vector<std::string>& out) {
		auto item = exportNode->item.get();
		switch (item->getId()) {
			case "ClassDecl"_id: {
				auto classDecl = static_cast<ClassDecl_t*>(item);
				if (classDecl->name && classDecl->name->item->getId() == "Variable"_id) {
					markVarExported(ExportMode::Any, true);
					addExportedVar(toString(classDecl->name->item));
				}
				transformClassDecl(classDecl, out);
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
					auto expList = new_ptr<ExpList_t>();
					for (auto name : values->nameList->names.objects()) {
						addExportedVar(toString(name));
						auto callable = new_ptr<Callable_t>();
						callable->item.set(name);
						auto chainValue = new_ptr<ChainValue_t>();
						chainValue->caller.set(callable);
						auto value = new_ptr<Value_t>();
						value->item.set(chainValue);
						auto exp = new_ptr<Exp_t>();
						exp->value.set(value);
						expList->exprs.push_back(exp);
					}
					auto assignment = new_ptr<Assignment_t>();
					assignment->assignable.set(expList);
					auto assign = new_ptr<Assign_t>();
					assign->values.dup(values->valueList->exprs);
					assignment->target.set(assign);
					transformAssignment(assignment, out);
				}
				break;
			}
			default:
				break;
		}
	}

	void transform_simple_table(simple_table_t* table, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		pushScope();
		const auto& pairs = table->pairs.objects();
		for (auto pair : pairs) {
			switch (pair->getId()) {
				case "variable_pair"_id: transform_variable_pair(static_cast<variable_pair_t*>(pair), temp); break;
				case "normal_pair"_id: transform_normal_pair(static_cast<normal_pair_t*>(pair), temp); break;
			}
			temp.back() = indent() + temp.back() + (pair == pairs.back() ? Empty : s(","sv)) + nll(pair);
		}
		popScope();
		out.push_back(s("{"sv) + nll(table) + join(temp) + s("}"sv));
	}

	void transformTblComprehension(TblComprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> kv;
		std::string tbl = getUnusedName("_tbl_");
		addToScope(tbl);
		std::vector<std::string> temp;
		auto compInner = comp->forLoop.get();
		switch (compInner->compFor->getId()) {
			case "CompForEach"_id:
				transformCompForEach(compInner->compFor.to<CompForEach_t>(), temp);
				break;
			case "CompFor"_id:
				transformCompFor(compInner->compFor.to<CompFor_t>(), temp);
				break;
			default: break;
		}
		std::vector<std::string> clauseCodes;
		for (auto clause : compInner->clauses.objects()) {
			pushScope();
			auto child = clause->getFirstChild();
			switch (child->getId()) {
				case "CompForEach"_id:
					transformCompForEach(static_cast<CompForEach_t*>(child), clauseCodes);
					break;
				case "CompFor"_id:
					transformCompFor(static_cast<CompFor_t*>(child), clauseCodes);
					break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(child), clauseCodes);
					clauseCodes.back() = indent() + s("if "sv) + clauseCodes.back() + s(" then"sv) + nll(clause);
					break;
				default: break;
			}
		}
		pushScope();
		transformExp(comp->key, kv);
		popScope();
		if (comp->value) {
			transformExp(comp->value->value, kv);
		}
		for (size_t i = 0; i < compInner->clauses.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << tbl << " = { }"sv << nll(comp);
		_buf << temp.back();
		pushScope();
		if (!comp->value) {
			auto keyVar = getUnusedName("_key_");
			auto valVar = getUnusedName("_val_");
			_buf << indent() << "local "sv << keyVar << ", "sv << valVar << " = "sv << kv.front() << nll(comp);
			kv.front() = keyVar;
			kv.push_back(valVar);
		}
		if (clauseCodes.empty()) {
			_buf << indent() << tbl << "["sv << kv.front() << "] = "sv << kv.back() << nll(comp);
		} else {
			_buf << join(clauseCodes);
			_buf << indent(int(clauseCodes.size())) << tbl << "["sv << kv.front() << "] = "sv << kv.back() << nll(comp);
			for (int ind = int(clauseCodes.size()) - 1; ind > -1 ; --ind) {
				_buf << indent(ind) << "end"sv << nll(comp);
			}
		}
		popScope();
		_buf << indent() << "end"sv << nll(comp);
		out.push_back(tbl);
		out.push_back(clearBuf());
	}

	void transformTblCompInPlace(TblComprehension_t* comp, ExpList_t* expList, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		pushScope();
		transformTblComprehension(comp, temp);
		const auto& tbVar = temp.front();
		const auto& compBody = temp.back();
		transformExpList(expList, temp);
		const auto& assignLeft = temp.back();
		out.push_back(
			s("do"sv) + nll(comp) +
			compBody +
			indent() + assignLeft + s(" = "sv) + tbVar + nll(comp));
		popScope();
		out.back() = out.back() + indent() + s("end"sv) + nlr(comp);
	}

	void transformTblCompReturn(TblComprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformTblComprehension(comp, temp);
		out.push_back(temp.back() + indent() + s("return "sv) + temp.front() + nlr(comp));
	}

	void transformTblCompClosure(TblComprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string before = s("(function()"sv) + nll(comp);
		pushScope();
		transformTblComprehension(comp, temp);
		const auto& tbVar = temp.front();
		const auto& compBody = temp.back();
		out.push_back(
			before +
			compBody +
			indent() + s("return "sv) + tbVar + nlr(comp));
		popScope();
		out.back() = out.back() + indent() + s("end)()"sv);
	}

	void transformCompFor(CompFor_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string varName = toString(comp->varName);
		transformExp(comp->startValue, temp);
		transformExp(comp->stopValue, temp);
		if (comp->stepValue) {
			transformExp(comp->stepValue->value, temp);
		} else {
			temp.emplace_back();
		}
		_buf << indent() << "for "sv << varName << " = "sv << temp[0] << ", "sv << temp[1] << (temp[2].empty() ? Empty : s(", "sv) + temp[2]) << " do"sv << nll(comp);
		out.push_back(clearBuf());
	}

	void transformTableBlock(TableBlock_t* table, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		pushScope();
		const auto& pairs = table->values.objects();
		for (auto pair : pairs) {
			switch (pair->getId()) {
				case "Exp"_id: transformExp(static_cast<Exp_t*>(pair), temp); break;
				case "variable_pair"_id: transform_variable_pair(static_cast<variable_pair_t*>(pair), temp); break;
				case "normal_pair"_id: transform_normal_pair(static_cast<normal_pair_t*>(pair), temp); break;
			}
			temp.back() = indent() + temp.back() + (pair == pairs.back() ? Empty : s(","sv)) + nll(pair);
		}
		popScope();
		out.push_back(s("{"sv) + nll(table) + join(temp) + s("}"sv));
	}

	void transformDo(Do_t* doNode, std::vector<std::string>& out, bool implicitReturn = false) {
		std::vector<std::string> temp;
		temp.push_back(indent() + s("do"sv) + nll(doNode));
		pushScope();
		transformBody(doNode->body, temp, implicitReturn);
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(doNode));
		out.push_back(join(temp));
	}

	void transformDoClosure(Do_t* doNode, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		temp.push_back(s("(function()"sv) + nll(doNode));
		pushScope();
		transformBody(doNode->body, temp, true);
		popScope();
		temp.push_back(indent() + s("end)()"sv));
		out.push_back(join(temp));
	}

	void transformUpdate(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformImport(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformWhile(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformSwitch(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformLocal(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformBreakLoop(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transform_unless_line(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
};

const std::string MoonCompliler::Empty;

int main()
{
	std::string s = R"TestCodesHere(a = 1 + do
  123

a = do
  123

do
  123

do
  123)TestCodesHere";
	MoonCompliler{}.complile(s);
	return 0;
}
