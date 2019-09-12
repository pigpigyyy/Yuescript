#include <string>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <vector>
#include <numeric>
#include <memory>
#include <array>
#include <sstream>
#include <string_view>
using namespace std::string_view_literals;
#include "moon_ast.h"

const input& AstLeaf::getValue()
{
	if (_value.empty())
	{
		_value.assign(m_begin.m_it, m_end.m_it);
		return trim(_value);
	}
	return _value;
}

#define AST_IMPL(type) \
	ast<type##_t> __##type##_t(type);

AST_IMPL(Num)
AST_IMPL(_Name)
AST_IMPL(Name)
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
AST_IMPL(IfElseIf)
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
AST_IMPL(FnArgs)
AST_IMPL(chain_call)
AST_IMPL(chain_item)
AST_IMPL(DotChainItem)
AST_IMPL(ColonChainItem)
AST_IMPL(chain_dot_chain)
AST_IMPL(ChainItem)
AST_IMPL(ChainItems)
AST_IMPL(invoke_chain)
AST_IMPL(ColonChain)
AST_IMPL(default_value)
AST_IMPL(Slice)
AST_IMPL(Invoke)
AST_IMPL(TableValue)
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
AST_IMPL(KeyValue)
AST_IMPL(FnArgDef)
AST_IMPL(FnArgDefList)
AST_IMPL(outer_var_shadow)
AST_IMPL(FnArgsDef)
AST_IMPL(fn_arrow)
AST_IMPL(FunLit)
AST_IMPL(NameOrDestructure)
AST_IMPL(AssignableNameList)
AST_IMPL(ArgBlock)
AST_IMPL(invoke_args_with_table)
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

template<class T, class... Args>
inline std::unique_ptr<T> MakeUnique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

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
	}
private:
	Converter _converter;
	std::vector<input> _codeCache;
	std::ostringstream _buf;
	std::string _newLine = "\n";
	std::vector<int> _lineTable;
	struct Scope {
		std::unique_ptr<std::unordered_set<std::string>> vars;
		std::unique_ptr<std::unordered_set<std::string>> allows;
	};
	std::vector<Scope> _scopes;
	static const std::string Empty;

	void pushScope() {
		_scopes.emplace_back();
		_scopes.back().vars = MakeUnique<std::unordered_set<std::string>>();
	}

	void popScope() {
		_scopes.pop_back();
	}

	bool isDefined(const std::string& name, bool checkShadowScopeOnly = false) {
		bool isDefined = false;
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

	void markVarShadowed() {
		auto& scope = _scopes.back();
		scope.allows = MakeUnique<std::unordered_set<std::string>>();
	}

	void addToAllowList(const std::string& name) {
		auto& scope = _scopes.back();
		scope.allows->insert(name);
	}

	void forceAddToScope(const std::string& name) {
		auto& scope = _scopes.back();
		scope.vars->insert(name);
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

	std::string getValidName(std::string_view name) {
		int index = 0;
		std::string newName;
		do {
			_buf << name << index;
			newName = clearBuf();
			index++;
		} while (isDefined(newName));
		return newName;
	}

	const std::string nll(ast_node* node) {
		_lineTable.push_back(node->m_begin.m_line);
		return _newLine;
	}

	const std::string nlr(ast_node* node) {
		_lineTable.push_back(node->m_end.m_line);
		return _newLine;
	}

	std::string indent() {
		return std::string((_scopes.size() - 1) * 2, ' ');
	}

	std::string indent(int offset) {
		return std::string((_scopes.size() - 1 + offset) * 2, ' ');
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
		auto str = _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
		return trim(str);
	}

	std::string toString(input::iterator begin, input::iterator end) {
		auto str = _converter.to_bytes(std::wstring(begin, end));
		return trim(str);
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

	void transformStatement(Statement_t* statement, std::vector<std::string>& out) {
		if (statement->appendix) {
			auto appendix = statement->appendix;
			switch (appendix->item->getId()) {
				case "if_else_line"_id: {
					auto if_else_line = static_cast<if_else_line_t*>(appendix->item.get());
					auto ifCond = new_ptr<IfCond_t>();
					ifCond->condition = if_else_line->condition;

					auto exprList = new_ptr<ExpList_t>();
					exprList->exprs.add(if_else_line->elseExpr);
					auto stmt = new_ptr<Statement_t>();
					stmt->content.set(exprList);
					auto body = new_ptr<Body_t>();
					body->content.set(stmt);
					auto ifElseIf = new_ptr<IfElseIf_t>();
					ifElseIf->body.set(body);

					stmt = new_ptr<Statement_t>();
					stmt->content.set(statement->content);
					body = new_ptr<Body_t>();
					body->content.set(stmt);
					auto ifNode = new_ptr<If_t>();
					ifNode->firstCondition.set(ifCond);
					ifNode->firstBody.set(body);
					ifNode->branches.add(ifElseIf);

					statement->appendix.set(nullptr);
					auto simpleValue = new_ptr<SimpleValue_t>();
					simpleValue->value.set(ifNode);
					auto value = new_ptr<Value_t>();
					value->item.set(simpleValue);
					auto exp = new_ptr<Exp_t>();
					exp->value.set(value);
					exprList = new_ptr<ExpList_t>();
					exprList->exprs.add(exp);
					statement->content.set(exprList);
					break;
				}
				case "unless_line"_id: {
					break;
				}
				case "CompInner"_id: {
					break;
				}
				default: break;
			}
		}
		auto node = statement->content.get();
		if (!node) {
			out.push_back(Empty);
			return;
		}
		switch (node->getId()) {
			case "Import"_id: transformImport(node, out); break;
			case "While"_id: transformWhile(node, out); break;
			case "With"_id: transformWith(node, out); break;
			case "For"_id: transformFor(static_cast<For_t*>(node), out); break;
			case "ForEach"_id: transformForEach(static_cast<ForEach_t*>(node), out); break;
			case "Switch"_id: transformSwitch(node, out); break;
			case "Return"_id: transformReturn(static_cast<Return_t*>(node), out); break;
			case "Local"_id: transformLocal(node, out); break;
			case "Export"_id: transformExport(node, out); break;
			case "BreakLoop"_id: transformBreakLoop(node, out); break;
			case "Assignment"_id: transformStatementAssign(statement, out); break;
			case "ExpList"_id: {
				auto expList = static_cast<ExpList_t*>(node);
				if (expList->exprs.objects().empty()) {
					out.push_back(Empty);
					break;
				}
				if (auto singleValue = singleValueFrom(expList)) {
					if (auto ifNode = static_cast<If_t*>(singleValue->getByPath({"SimpleValue"_id, "If"_id}))) {
						transformIf(ifNode, out);
						break;
					}
					if (singleValue->getByPath({"ChainValue"_id, "InvokeArgs"_id})) {
						transformValue(singleValue, out);
						out.back() = indent() + out.back() + nlr(singleValue);
						break;
					}
				}
				std::string preDefine;
				if (addToScope(s("_"sv))) {
					preDefine =  indent() + s("local _"sv) + nll(expList);
				}
				preDefine.append(indent() + s("_ = "sv));
				std::vector<std::string> temp;
				transformExpList(expList, temp);
				out.push_back(preDefine + temp.back() + nlr(expList));
				break;
			}
			default: break;
		}
	}

	std::string transformAssignDefs(ExpList_t* expList) {
		std::vector<std::string> preDefs;
		std::vector<ast_node*> values;
		expList->traverse([&](ast_node* child) {
			if (child->getId() == "Value"_id) {
				auto target = child->getByPath({"ChainValue"_id, "Callable"_id, "Name"_id});
				if (target) {
					auto name = toString(target);
					if (addToScope(name)) {
						preDefs.push_back(name);
					}
				}
				return traversal::Return;
			}
			return traversal::Continue;
		});
		if (!preDefs.empty()) {
			return indent() + s("local "sv) + join(preDefs, ", "sv);
		}
		return std::string();
	}

	void transformStatementAssign(Statement_t* statement, std::vector<std::string>& out) {
		auto assignment = static_cast<Assignment_t*>(statement->content.get());
		if (auto ifNode = assignment->getByPath({"Assign"_id, "If"_id})) {
			auto expList = assignment->assignable.get();
			std::vector<std::string> temp;
			std::list<std::pair<IfCond_t*, Body_t*>> ifCondPairs;
			ifCondPairs.emplace_back();
			std::string preDefine = transformAssignDefs(expList);
			if (!preDefine.empty()) temp.push_back(preDefine + nll(expList));
			ifNode->traverse([&](ast_node* node) {
				switch (node->getId()) {
					case "IfCond"_id:
						ifCondPairs.back().first = static_cast<IfCond_t*>(node);
						return traversal::Return;
					case "Body"_id:
						ifCondPairs.back().second = static_cast<Body_t*>(node);
						ifCondPairs.emplace_back();
						return traversal::Return;
					default: return traversal::Continue;
				}
			});
			for (const auto& pair : ifCondPairs) {
				if (pair.first) {
					std::vector<std::string> tmp;
					auto condition = pair.first->condition.get();
					transformExp(condition, tmp);
					_buf << indent() << (pair == ifCondPairs.front() ? ""sv : "else"sv) <<
						"if "sv << tmp.front() << " then"sv << nll(condition);
					temp.push_back(clearBuf());
				}
				if (pair.second) {
					if (!pair.first) {
						temp.push_back(indent() + s("else"sv) + nll(pair.second));
					}
					auto last = lastStatementFrom(pair.second);
					auto valueList = last ? last->content.as<ExpList_t>() : nullptr;
					if (last && valueList) {
						auto newAssignment = new_ptr<Assignment_t>();
						newAssignment->assignable.set(expList);
						auto assign = new_ptr<Assign_t>();
						if (valueList->getChildCount() == 2) {
							if (auto subIfNode = valueList->getByPath({
									"Exp"_id, "Value"_id, "SimpleValue"_id, "If"_id})) {
								assign->value.set(subIfNode);
							}
						}
						if (!assign->value) {
							auto expListLow = new_ptr<ExpListLow_t>();
							expListLow->exprs = valueList->exprs;
							assign->value.set(expListLow);
						}
						newAssignment->target.set(assign);
						last->content.set(newAssignment);
					}
					pushScope();
					transformBody(pair.second, temp);
					popScope();
					if (!pair.first) {
						temp.push_back(indent() + s("end"sv) + nll(pair.second));
					}
				}
			}
			out.push_back(join(temp));
			return;
		}
		if (auto expList = assignment->getByPath({"Assign"_id, "ExpListLow"_id})) {
			auto singleValue = singleValueFrom(expList);
			if (singleValue && singleValue->item->getId() == "SimpleValue"_id) {
				auto valueItem = singleValue->item->getFirstChild();
				switch (valueItem->getId()) {
					case "Comprehension"_id: {
						std::vector<std::string> temp;
						auto expList = assignment->assignable.get();
						transformExpList(expList, temp);
						transformCompInPlace(static_cast<Comprehension_t*>(valueItem), temp.front(), temp);
						std::string preDefine = transformAssignDefs(expList);
						out.push_back(preDefine + nll(statement) + temp.back());
						return;
					}
					case "For"_id: {
						std::vector<std::string> temp;
						auto expList = assignment->assignable.get();
						std::string preDefine = transformAssignDefs(expList);
						transformForInPlace(static_cast<For_t*>(valueItem), temp, expList);
						out.push_back(preDefine + nll(statement) + temp.front());
						return;
					}
					case "ForEach"_id: {
						std::vector<std::string> temp;
						auto expList = assignment->assignable.get();
						std::string preDefine = transformAssignDefs(expList);
						transformForEachInPlace(static_cast<ForEach_t*>(valueItem), temp, expList);
						out.push_back(preDefine + nll(statement) + temp.front());
						return;
					}
				}
			}
		}
		transformAssignment(assignment, out);
	}

	void transformAssignment(Assignment_t* assignment, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto expList = assignment->assignable.get();
		auto action = assignment->target.get();
		std::string preDefine = transformAssignDefs(expList);
		transformExpList(expList, temp);
		bool oneLined = expList->getChildCount() == 2 &&
			traversal::Stop != action->traverse([&](ast_node* node) {
				if (node->getId() == "FunLit"_id) {
					if (auto body = node->getByPath({"Body"_id})) {
						if (traversal::Stop == body->traverse([&](ast_node* n) {
							if (n->getId() == "Callable"_id) {
								if (auto name = n->getByPath({"Name"_id})) {
									if (temp.front() ==toString(name)) {
										return traversal::Stop;
									}
								}
							}
							return traversal::Continue;
						})) {
							return traversal::Stop;
						}
					}
				}
				return traversal::Continue;
			});
		switch (action->getId()) {
			case "Update"_id: transformUpdate(action, temp); break;
			case "Assign"_id: {
				auto child = action->getFirstChild();
				switch (child->getId()) {
					case "With"_id: transformWith(child, temp); break;
					case "If"_id: transformIfClosure(static_cast<If_t*>(child), temp); break;
					case "Switch"_id: transformSwitch(child, temp); break;
					case "TableBlock"_id: transformTableBlock(child, temp); break;
					case "ExpListLow"_id: transformExpListLow(static_cast<ExpListLow_t*>(child), temp); break;
					default: break;
				}
				break;
			}
			default: break;
		}
		if (oneLined) {
			out.push_back((preDefine.empty() ? indent() + temp[0] : preDefine) + s(" = "sv) + temp[1] + nll(assignment));
		} else {
			out.push_back((preDefine.empty() ? Empty : preDefine + nll(assignment)) + indent() + temp[0] + s(" = "sv) + temp[1] + nll(assignment));
		}
	}

	void transformIf(If_t* ifNode, std::vector<std::string>& out, bool withClosure = false) {
		std::vector<std::string> temp;
		if (withClosure) {
			temp.push_back(s("(function()"sv) + nll(ifNode));
			pushScope();
		}
		std::list<std::pair<IfCond_t*, Body_t*>> ifCondPairs;
		ifCondPairs.emplace_back();
		ifNode->traverse([&](ast_node* node) {
			switch (node->getId()) {
				case "IfCond"_id:
					ifCondPairs.back().first = static_cast<IfCond_t*>(node);
					return traversal::Return;
				case "Body"_id:
					ifCondPairs.back().second = static_cast<Body_t*>(node);
					ifCondPairs.emplace_back();
					return traversal::Return;
				default: return traversal::Continue;
			}
		});
		for (const auto& pair : ifCondPairs) {
			if (pair.first) {
				std::vector<std::string> tmp;
				auto condition = pair.first->condition.get();
				transformExp(condition, tmp);
				_buf << indent() << (pair == ifCondPairs.front() ? ""sv : "else"sv) <<
					"if "sv << tmp.front() << " then"sv << nll(condition);
				temp.push_back(clearBuf());
			}
			if (pair.second) {
				if (!pair.first) {
					temp.push_back(indent() + s("else"sv) + nll(pair.second));
				}
				pushScope();
				transformBody(pair.second, temp, withClosure);
				popScope();
				if (!pair.first) {
					temp.push_back(indent() + s("end"sv) + nll(pair.second));
				}
			}
		}
		if (withClosure) {
			popScope();
			temp.push_back(indent() + s("end)()"sv));
		}
		out.push_back(join(temp));
	}

	void transformIfClosure(If_t* ifNode, std::vector<std::string>& out) {
		transformIf(ifNode, out, true);
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
			case "simple_table"_id: transform_simple_table(item, out); break;
			case "ChainValue"_id: transformChainValue(static_cast<ChainValue_t*>(item), out); break;
			case "String"_id: transformString(static_cast<String_t*>(item), out); break;
			default: break;
		}
	}

	void transformChainValue(ChainValue_t* chainValue, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto caller = chainValue->caller.get();
		switch (caller->getId()) {
			case "Chain"_id: transformChain(static_cast<Chain_t*>(caller), temp); break;
			case "Callable"_id: transformCallable(static_cast<Callable_t*>(caller), temp, chainValue->arguments); break;
			default: break;
		}
		if (chainValue->arguments) {
			transformInvokeArgs(chainValue->arguments, temp);
			out.push_back(temp[0] + s("("sv) + temp[1] + s(")"sv));
		} else {
			out.push_back(temp[0]);
		}
	}

	void transformCallable(Callable_t* callable, std::vector<std::string>& out, bool invoke) {
		auto item = callable->item.get();
		switch (item->getId()) {
			case "Name"_id: transformName(static_cast<Name_t*>(item), out); break;
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
		auto node = simpleValue->value.get();
		switch (node->getId()) {
			case "const_value"_id: transform_const_value(node, out); break;
			case "If"_id: transformIfClosure(static_cast<If_t*>(node), out); break;
			case "Switch"_id: transformSwitch(node, out); break;
			case "With"_id: transformWith(node, out); break;
			case "ClassDecl"_id: transformClassDecl(node, out); break;
			case "ForEach"_id: transformForEachClosure(static_cast<ForEach_t*>(node), out); break;
			case "For"_id: transformForClosure(static_cast<For_t*>(node), out); break;
			case "While"_id: transformWhile(node, out); break;
			case "Do"_id: transformDo(node, out); break;
			case "unary_exp"_id: transform_unary_exp(static_cast<unary_exp_t*>(node), out); break;
			case "TblComprehension"_id: transformTblComprehension(node, out); break;
			case "TableLit"_id: transformTableLit(static_cast<TableLit_t*>(node), out); break;
			case "Comprehension"_id: transformComprehension(static_cast<Comprehension_t*>(node), out); break;
			case "FunLit"_id: transformFunLit(static_cast<FunLit_t*>(node), out); break;
			case "Num"_id: transformNum(static_cast<Num_t*>(node), out); break;
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
				args << ')' << nlr(argsDef) <<
				initArgs << bodyCodes;
		} else {
			if (funLit->body) {
				transformBody(funLit->body, temp, true);
			} else {
				temp.push_back(Empty);
			}
			auto& bodyCodes = temp.back();
			_buf << "function("sv <<
				(isFatArrow ? s("self"sv) : Empty) <<
				')' << nll(funLit) << bodyCodes;
		}
		popScope();
		_buf << indent() << "end"sv;
		out.push_back(clearBuf());
	}

	void transformCodes(ast_node* nodes, std::vector<std::string>& out, bool implicitReturn) {
		if (implicitReturn) {
			auto last = lastStatementFrom(nodes);
			if (ast_is<ExpList_t>(last->content)) {
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
				if (auto comp = singleValue->getByPath({"SimpleValue"_id, "Comprehension"_id})) {
					transformCompReturn(static_cast<Comprehension_t*>(comp), out);
				} else {
					transformValue(singleValue, out);
					out.back() = indent() + s("return "sv) + out.back() + nlr(returnNode);
				}
			} else {
				std::vector<std::string> temp;
				transformExpListLow(valueList, temp);
				out.push_back(indent() + s("return "sv) + temp.front() + nlr(returnNode));
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
				case "Name"_id: arg.name = toString(def->name); break;
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
				out.push_back(s("self.__class."sv) + toString(name->getFirstChild()));
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

	void transformChain(Chain_t* chain, std::vector<std::string>& out) {
		auto item = chain->item.get();
		switch (item->getId()) {
			case "chain_call"_id: transform_chain_call(static_cast<chain_call_t*>(item), out); break;
			case "chain_item"_id: transformChainItems(static_cast<chain_item_t*>(item)->chain, out); break;
			case "chain_dot_chain"_id: transform_chain_dot_chain(item, out); break;
			case "ColonChain"_id: transformColonChain(static_cast<ColonChain_t*>(item), out); break;
			default: break;
		}
	}

	void transform_chain_call(chain_call_t* chain_call, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto caller = chain_call->caller.get();
		switch (caller->getId()) {
			case "Callable"_id: transformCallable(static_cast<Callable_t*>(caller), temp, true); break;
			case "String"_id: transformString(static_cast<String_t*>(caller), temp); break;
			default: break;
		}
		transformChainItems(chain_call->chain, temp);
		out.push_back(join(temp));
	}

	void transformChainItems(ChainItems_t* chainItems, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto _chainItem : chainItems->simpleChain.objects()) {
			auto chainItem = static_cast<ChainItem_t*>(_chainItem);
			transformChainItem(chainItem, temp);
		}
		if (chainItems->colonChain) {
			transformColonChain(chainItems->colonChain, temp);
		}
		out.push_back(join(temp));
	}

	void transformChainItem(ChainItem_t* chainItem, std::vector<std::string>& out) {
		auto item = chainItem->item.get();
		switch (item->getId()) {
			case "Invoke"_id: transformInvoke(static_cast<Invoke_t*>(item), out); break;
			case "DotChainItem"_id:
				out.push_back(s("."sv) + toString(item->getFirstChild()));
				break;
			case "Slice"_id: transformSlice(item, out); break;
			case "Exp"_id:
				transformExp(static_cast<Exp_t*>(item), out);
				out.back() = s("["sv) + out.back() + s("]"sv);
				break;
			default: break;
		}
	}

	void transformInvoke(Invoke_t* invoke, std::vector<std::string>& out) {
		auto argument = invoke->argument.get();
		switch (argument->getId()) {
			case "FnArgs"_id: transformFnArgs(static_cast<FnArgs_t*>(argument), out); break;
			case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(argument), out); break;
			case "DoubleString"_id: transformDoubleString(static_cast<DoubleString_t*>(argument), out); break;
			case "LuaString"_id: transformLuaString(static_cast<LuaString_t*>(argument), out); break;
			default: break;
		}
	}

	void transformFnArgs(FnArgs_t* fnArgs, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto node : fnArgs->args.objects()) {
			transformExp(static_cast<Exp_t*>(node), temp);
		}
		std::string args = join(temp, ", "sv);
		out.push_back(args.empty() ? s("()"sv) : s("("sv) + args + s(")"sv));
	}

	void transformColonChain(ColonChain_t* colonChain, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		temp.push_back(s(":"sv) + toString(colonChain->colonChain->name));
		if (colonChain->invokeChain) {
			transform_invoke_chain(colonChain->invokeChain, temp);
		}
		out.push_back(join(temp));
	}

	void transform_invoke_chain(invoke_chain_t* invoke_chain, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		transformInvoke(invoke_chain->invoke, temp);
		if (invoke_chain->chain) {
			transformChainItems(invoke_chain->chain, temp);
		}
		out.push_back(join(temp));
	}

	void transform_unary_exp(unary_exp_t* unary_exp, std::vector<std::string>& out) {
		std::string op = toString(unary_exp->m_begin.m_it, unary_exp->item->m_begin.m_it);
		std::vector<std::string> temp{op + (op == "not"sv ? op + " " : Empty)};
		transformExp(unary_exp->item, temp);
		out.push_back(join(temp));
	}

	void transformName(Name_t* name, std::vector<std::string>& out) {
		out.push_back(toString(name));
	}

	void transformNum(Num_t* num, std::vector<std::string>& out) {
		out.push_back(toString(num));
	}

	void transformTableLit(TableLit_t* tableLit, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		ast_node* lastNode = nullptr;
		for (auto _tableValue : tableLit->values.objects()) {
			auto tableValue = static_cast<TableValue_t*>(_tableValue);
			auto value = tableValue->value.get();
			switch (value->getId()) {
				case "KeyValue"_id:
				case "Exp"_id:
					if (value->getId() == "Exp"_id) {
						transformExp(static_cast<Exp_t*>(value), temp);
					} else {
						transformKeyValue(static_cast<KeyValue_t*>(value), temp);
					}
					temp.back() = (lastNode ? s(","sv) + nll(lastNode) : Empty) + indent(1) + temp.back();
					lastNode = value;
					break;
				default: break;
			}
		}
		out.push_back(s("{"sv) + nll(tableLit) + join(temp) + nlr(tableLit) + indent() + s("}"sv));
	}

	void transformComprehension(Comprehension_t* comp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string accum = getValidName("_accum_");
		std::string len = getValidName("_len_");
		addToScope(accum);
		addToScope(len);
		transformExp(comp->value, temp);
		auto compInner = comp->forLoop.get();
		switch (compInner->compFor->getId()) {
			case "CompForEach"_id:
				transformCompForEach(
					static_cast<CompForEach_t*>(compInner->compFor.get()), temp);
				break;
			case "CompFor"_id: transformCompFor(compInner->compFor, temp); break;
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
				case "CompFor"_id: transformCompFor(child, clauseCodes); break;
				case "Exp"_id:
					transformExp(static_cast<Exp_t*>(child), clauseCodes);
					clauseCodes.back() = indent() + s("if "sv) + clauseCodes.back() + s(" then"sv) + nll(clause);
					break;
				default: break;
			}
		}
		for (size_t i = 0; i < compInner->clauses.objects().size(); ++i) {
			popScope();
		}
		_buf << indent() << "local "sv << accum << " = { }"sv << nll(comp);
		_buf << indent() << "local "sv << len << " = 1"sv << nll(comp);
		_buf << temp.back();
		pushScope();
		if (clauseCodes.empty()) {
			_buf << indent() << accum << "["sv << len << "] = "sv << temp.front() << nll(comp);
			_buf << indent() << len << " = "sv << len << " + 1"sv << nll(comp);
		} else {
			_buf << join(clauseCodes);
			_buf << indent(int(clauseCodes.size())) << accum << "["sv << len << "] = "sv << temp.front() << nll(comp);
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

	void transformCompInPlace(Comprehension_t* comp, const std::string& expStr, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		pushScope();
		transformComprehension(comp, temp);
		out.push_back(
			s("do"sv) + nll(comp) +
			temp.back() +
			indent() + expStr + s(" = "sv) + temp.front() + nll(comp));
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
		transformAssignableNameList(nameList, temp);
		switch (loopTarget->getId()) {
			case "star_exp"_id: {
				auto star_exp = static_cast<star_exp_t*>(loopTarget);
				auto listName = getValidName("_list_");
				auto indexName = getValidName("_index_");
				addToScope(listName);
				addToScope(indexName);
				transformExp(star_exp->value, temp);
				_buf << indent() << "local "sv << listName << " = "sv << temp.back() << nll(nameList);
				_buf << indent() << "for "sv << indexName << " = 1, #"sv << listName << " do"sv << nlr(loopTarget);
				_buf << indent(1) << "local "sv << temp.front() << " = "sv << listName << "["sv << indexName << "]"sv << nll(nameList);
				out.push_back(clearBuf());
				break;
			}
			case "Exp"_id:
				transformExp(static_cast<Exp_t*>(loopTarget), temp);
				_buf << indent() << "for "sv << temp.front() << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			case "ExpList"_id:
				transformExpList(static_cast<ExpList_t*>(loopTarget), temp);
				_buf << indent() << "for "sv << temp.front() << " in "sv << temp.back() << " do"sv << nlr(loopTarget);
				out.push_back(clearBuf());
				break;
			default: break;
		}
	}

	void transformCompForEach(CompForEach_t* comp, std::vector<std::string>& out) {
		transformForEachHead(comp->nameList, comp->loopValue, out);
	}

	void transformAssignableNameList(AssignableNameList_t* nameList, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		for (auto node : nameList->items.objects()) {
			switch (node->getId()) {
				case "Name"_id:
					transformName(static_cast<Name_t*>(node), temp);
					break;
				case "TableLit"_id:
					transformTableLit(static_cast<TableLit_t*>(node), temp);
					break;
				default: break;
			}
		}
		out.push_back(join(temp, ", "sv));
	}

	void transformInvokeArgs(InvokeArgs_t* invokeArgs, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		if (invokeArgs->argsList) {
			transformExpList(invokeArgs->argsList, temp);
		}
		if (invokeArgs->argsTableBlock) {
			transform_invoke_args_with_table(invokeArgs->argsTableBlock, temp);
		}
		if (invokeArgs->tableBlock) {
			transformTableBlock(invokeArgs->tableBlock, temp);
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
		std::string accum = getValidName("_accum_");
		std::string len = getValidName("_len_");
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
			auto expListLow = new_ptr<ExpListLow_t>();
			expListLow->exprs = ast_cast<ExpList_t>(last->content)->exprs;
			auto assign = new_ptr<Assign_t>();
			assign->value.set(expListLow);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forNode->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forNode->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forNode) + indent() + s("return "sv) + accum + nlr(forNode));
		popScope();
		temp.push_back(indent() + s("end)()"sv) + nlr(forNode));
		out.push_back(join(temp));
	}

	void transformForInPlace(For_t* forNode,  std::vector<std::string>& out, ExpList_t* assignExpList) {
		std::vector<std::string> temp;
		std::string accum = getValidName("_accum_");
		std::string len = getValidName("_len_");
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
			auto expListLow = new_ptr<ExpListLow_t>();
			expListLow->exprs = ast_cast<ExpList_t>(last->content)->exprs;
			auto assign = new_ptr<Assign_t>();
			assign->value.set(expListLow);
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
		out.push_back(toString(node));
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
		std::string accum = getValidName("_accum_");
		std::string len = getValidName("_len_");
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
			auto expListLow = new_ptr<ExpListLow_t>();
			expListLow->exprs = ast_cast<ExpList_t>(last->content)->exprs;
			auto assign = new_ptr<Assign_t>();
			assign->value.set(expListLow);
			assignment->target.set(assign);
			last->content.set(assignment);
		}
		pushScope();
		transformBody(forEach->body, temp);
		temp.push_back(indent() + len + s(" = "sv) + len + s(" + 1"sv) + nlr(forEach->body));
		popScope();
		temp.push_back(indent() + s("end"sv) + nlr(forEach) + indent() + s("return "sv) + accum + nlr(forEach));
		popScope();
		temp.push_back(indent() + s("end)()"sv) + nlr(forEach));
		out.push_back(join(temp));
	}

	void transformForEachInPlace(ForEach_t* forEach,  std::vector<std::string>& out, ExpList_t* assignExpList) {
		std::vector<std::string> temp;
		std::string accum = getValidName("_accum_");
		std::string len = getValidName("_len_");
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
			auto expListLow = new_ptr<ExpListLow_t>();
			expListLow->exprs = ast_cast<ExpList_t>(last->content)->exprs;
			auto assign = new_ptr<Assign_t>();
			assign->value.set(expListLow);
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

	void transformKeyValue(KeyValue_t* keyValue, std::vector<std::string>& out) {
		auto item = keyValue->item.get();
		switch (item->getId()) {
			case "variable_pair"_id:
				out.push_back(toString(static_cast<variable_pair_t*>(item)->name));
				break;
			case "normal_pair"_id: {
				auto pair = static_cast<normal_pair_t*>(item);
				auto key = pair->key.get();
				std::vector<std::string> temp;
				switch (key->getId()) {
					case "KeyName"_id: transformKeyName(static_cast<KeyName_t*>(key), temp); break;
					case "Exp"_id:
						transformExp(static_cast<Exp_t*>(key), temp);
						temp.back() = s("["sv) + temp.back() + s("]"sv);
						break;
					case "DoubleString"_id: transformDoubleString(static_cast<DoubleString_t*>(key), temp); break;
					case "SingleString"_id: transformSingleString(static_cast<SingleString_t*>(key), temp); break;
					default: break;
				}
				auto value = pair->value.get();
				switch (value->getId()) {
					case "Exp"_id: transformExp(static_cast<Exp_t*>(value), temp); break;
					case "TableBlock"_id: transformTableBlock(static_cast<TableBlock_t*>(value), temp); break;
					default: break;
				}
				out.push_back(temp[0] + s(" = "sv) + temp[1]);
				break;
			}
			default: break;
		}
	}

	void transformKeyName(KeyName_t* keyName, std::vector<std::string>& out) {
		auto name = keyName->name.get();
		switch (name->getId()) {
			case "SelfName"_id: transformSelfName(static_cast<SelfName_t*>(name), out, false); break;
			case "_Name"_id: out.push_back(toString(name)); break;
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

	void transformUpdate(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformImport(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformWhile(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformWith(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformSwitch(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformTableBlock(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformLocal(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformExport(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformBreakLoop(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transform_unless_line(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_simple_table(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_const_value(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformClassDecl(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformDo(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformTblComprehension(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_chain_dot_chain(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformSlice(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformCompFor(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformCompClause(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_invoke_args_with_table(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
};

const std::string MoonCompliler::Empty;

int main()
{
	std::string s = R"TestCodesHere(
-- vararg bubbling
f = (...) -> #{...}

dont_bubble = ->
  [x for x in ((...)-> print ...)("hello")]

k = [x for x in ((...)-> print ...)("hello")]

j = for i=1,10
  (...) -> print ...

-- bubble me

m = (...) ->
  [x for x in *{...} when f(...) > 4]

x = for i in *{...} do i
y = [x for x in *{...}]
z = [x for x in hallo when f(...) > 4]


a = for i=1,10 do ...

b = for i=1,10
  -> print ...


)TestCodesHere";

	MoonCompliler{}.complile(s);

	return 0;
}
