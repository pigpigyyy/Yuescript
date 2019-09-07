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
		BlockEnd_t* root = nullptr;
		State st;
		if (parse(input, BlockEnd, el, root, &st)) {
			std::cout << "matched!\n";
			std::vector<std::string> out;
			root->eachChild([&](ast_node* node) {
				switch (node->getId()) {
					case "Block"_id:
						pushScope();
						transformBlock(node, out);
						popScope();
						break;
					default: break;
				}
			});
			std::string result;
			if (out.size() == 1) {
				result = std::move(out.front());
			} else if (out.size() > 1) {
				result = join(out, "\n");
			}
			std::cout << result << '\n';
		} else {
			std::cout << "not matched!\n";
			for (error_list::iterator it = el.begin(); it != el.end(); ++it) {
				const error& err = *it;
				std::cout << "line " << err.m_begin.m_line << ", col " << err.m_begin.m_col << ": syntax error\n";
			}
		}
	}

private:
	Converter _converter;
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

	bool isDefined(const std::string& name, bool checkShadowScope = false) {
		bool isDefined = false;
		for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
			auto vars = it->vars.get();
			if (vars->find(name) != vars->end()) {
				isDefined = true;
				break;
			}
			if (checkShadowScope && it->allows) break;
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

	void popScope() {
		_scopes.pop_back();
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
		return std::string(_scopes.size() - 1, '\t');
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
		auto str = _converter.to_bytes(std::u32string(node->m_begin.m_it, node->m_end.m_it));
		return trim(str);
	}

	void noop(ast_node* node, std::vector<std::string>& out) {
		auto str = _converter.to_bytes(std::u32string(node->m_begin.m_it, node->m_end.m_it));
		out.push_back(s("<"sv) + node->getName() + s(">"sv) + trim(str));
		// out.push_back(trim(str));
	}

	void noopnl(ast_node* node, std::vector<std::string>& out) {
		auto str = _converter.to_bytes(std::u32string(node->m_begin.m_it, node->m_end.m_it));
		out.push_back(s("<"sv) + node->getName() + s(">"sv) + trim(str) + nll(node));
		// out.push_back(trim(str) + nll(node));
	}

	void transformBlock(ast_node* block, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		block->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Line"_id: transformLine(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp));
	}

	void transformLine(ast_node* line, std::vector<std::string>& out) {
		line->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Statement"_id: transformStatement(node, out); break;
				default: break;
			}
		});
	}

	void transformStatement(ast_node* statement, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		auto transformContent = [&](ast_node* node, std::vector<std::string>& out) {
			switch (node->getId()) {
				case "Import"_id: transformImport(node, temp); break;
				case "While"_id: transformWhile(node, temp); break;
				case "With"_id: transformWith(node, temp); break;
				case "For"_id: transformFor(node, temp); break;
				case "ForEach"_id: transformForEach(node, temp); break;
				case "Switch"_id: transformSwitch(node, temp); break;
				case "Return"_id: transformReturn(node, temp); break;
				case "Local"_id: transformLocal(node, temp); break;
				case "Export"_id: transformExport(node, temp); break;
				case "BreakLoop"_id: transformBreakLoop(node, temp); break;
				case "Assignment"_id: transformAssignment(node, temp); break;
				case "ExpList"_id:
					transformExpList(node, temp);
					temp.back() = indent() + temp.back() + nll(node);
					break;
				default: break;
			}
		};
		if (statement->getChildCount() > 1) {
			pushScope();
			transformContent(statement->getChild(0), out);
			popScope();
			transform_statement_appendix(statement->getChild(1), temp);
		} else {
			transformContent(statement->getChild(0), out);
		}
		switch (temp.size()) {
			case 1: // body
				out.push_back(std::move(temp.front()));
				break;
			case 2: // body, if
				out.push_back(join({std::move(temp[1]), std::move(temp[0]), s("end"sv) + nlr(statement)}));
				break;
			case 3: // body, if, else
				out.push_back(join({std::move(temp[1]), std::move(temp[0]), std::move(temp[2]), s("end"sv) + nlr(statement)}));
				break;
		}
	}

	void transform_statement_appendix(ast_node* appendix, std::vector<std::string>& out) {
		appendix->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "if_else_line"_id: transform_if_else_line(node, out); break;
				case "unless_line"_id: transform_unless_line(node, out); break;
				case "CompInner"_id: transformCompInner(node, out); break;
				default: break;
			}
		});
	}

	void transform_if_else_line(ast_node* if_else_line, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		if_else_line->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Exp"_id:
					pushScope();
					transformExp(node, temp);
					popScope();
					break;
				default: break;
			}
		});
		out.push_back(indent() + s("if "sv) + temp[0] + s(" then"sv) + nll(if_else_line));
		out.push_back(indent() + s("else "sv) + nll(if_else_line) + indent() + '\t' + temp[1] + nll(if_else_line));
	}

	void transformAssignment(ast_node* assignment, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		std::string preDefined;
		assignment->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "ExpList"_id: {
					std::vector<std::string> preDefs;
					std::vector<ast_node*> values;
					node->traverse([&](ast_node* child) {
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
						preDefined = indent() + s("local "sv) + join(preDefs, ", "sv) + nll(node);
					}
					transformExpList(node, temp);
					break;
				}
				case "Update"_id: transformUpdate(node, temp); break;
				case "Assign"_id: transformAssign(node, temp); break;
				default: break;
			}
		});
		out.push_back(preDefined + indent() + temp[0] + s(" = "sv) + temp[1] + nll(assignment));
	}

	void transformExpList(ast_node* expList, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		expList->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Exp"_id: transformExp(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp, ", "sv));
	}

	void transformExpListLow(ast_node* expListLow, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		expListLow->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Exp"_id: transformExp(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp, ", "sv));
	}

	void transformAssign(ast_node* assign, std::vector<std::string>& out) {
		assign->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "With"_id: transformWith(node, out); break;
				case "If"_id: transformIf(node, out); break;
				case "Switch"_id: transformSwitch(node, out); break;
				case "TableBlock"_id: transformTableBlock(node, out); break;
				case "ExpListLow"_id: transformExpListLow(node, out); break;
				default: break;
			}
		});
	}

	void transformExp(ast_node* exp, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		exp->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Value"_id: transformValue(node, temp); break;
				case "exp_op_value"_id: transform_exp_op_value(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp, " "sv));
	}

	void transform_exp_op_value(ast_node* exp_op_value, std::vector<std::string>& out) {
		exp_op_value->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "BinaryOperator"_id: transformBinaryOperator(node, out); break;
				case "Value"_id: transformValue(node, out); break;
				default: break;
			}
		});
	}

	void transformValue(ast_node* value, std::vector<std::string>& out) {
		value->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "SimpleValue"_id: transformSimpleValue(node, out); break;
				case "simple_table"_id: transform_simple_table(node, out); break;
				case "ChainValue"_id: transformChainValue(node, out); break;
				case "String"_id: transformString(node, out); break;
				default: break;
			}
		});
	}

	void transformChainValue(ast_node* chainValue, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		bool hasInvokeArgs = false;
		chainValue->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Chain"_id: transformChain(node, temp); break;
				case "Callable"_id: transformCallable(node, temp); break;
				case "InvokeArgs"_id:
					hasInvokeArgs = true;
					transformInvokeArgs(node, temp);
					break;
				default: break;
			}
		});
		out.push_back(hasInvokeArgs ? (temp[0] + s("("sv) + temp[1] + s(")"sv)) : temp[0]);
	}

	void transformCallable(ast_node* callable, std::vector<std::string>& out) {
		callable->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Name"_id: transformName(node, out); break;
				case "SelfName"_id: transformSelfName(node, out); break;
				case "VarArg"_id: transformVarArg(node, out); break;
				case "Parens"_id: transformParens(node, out); break;
				default: break;
			}
		});
	}

	void transformParens(ast_node* parans, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		parans->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Exp"_id: transformExp(node, temp); break;
				default: break;
			}
		});
		out.push_back(s("("sv) + temp.front() + s(")"sv));
	}

	void transformSimpleValue(ast_node* simpleValue, std::vector<std::string>& out) {
		simpleValue->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "const_value"_id: transform_const_value(node, out); break;
				case "If"_id: transformIf(node, out); break;
				case "Switch"_id: transformSwitch(node, out); break;
				case "With"_id: transformWith(node, out); break;
				case "ClassDecl"_id: transformClassDecl(node, out); break;
				case "ForEach"_id: transformForEach(node, out); break;
				case "For"_id: transformFor(node, out); break;
				case "While"_id: transformWhile(node, out); break;
				case "Do"_id: transformDo(node, out); break;
				case "unary_exp"_id: transform_unary_exp(node, out); break;
				case "TblComprehension"_id: transformTblComprehension(node, out); break;
				case "TableLit"_id: transformTableLit(node, out); break;
				case "Comprehension"_id: transformComprehension(node, out); break;
				case "FunLit"_id: transformFunLit(node, out); break;
				case "Num"_id: transformNum(node, out); break;
				default: break;
			}
		});
	}

	void transformFunLit(ast_node* funLit, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		bool isFatArrow = false;
		bool hasArgsDef = false;
		ast_node* body = nullptr;
		pushScope();
		funLit->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "FnArgsDef"_id:
					hasArgsDef = true;
					transformFnArgsDef(node, temp);
					break;
				case "fn_arrow"_id:
					isFatArrow = toString(node) == "=>"sv;
					break;
				case "Body"_id:
					transformBody(node, temp);
					body = node;
					break;
				default: break;
			}
		});
		popScope();
		if (hasArgsDef) {
			auto& args = temp[0];
			auto& initArgs = temp[1];
			auto& bodyCodes = temp[2];
			_buf << "function("sv <<
				(isFatArrow ? s("self"sv) + s(args.empty() ? ""sv : ", "sv) : Empty) <<
				args << ')' << nll(funLit) <<
				(initArgs.empty() ? Empty : initArgs) <<
				(body ? bodyCodes : Empty) <<
				indent() << "end"sv;
			out.push_back(clearBuf());
		} else {
			auto& bodyCodes = temp[0];
			out.push_back(
				s("function()"sv) + nll(funLit) +
				(body ? bodyCodes : Empty) +
				indent() + s("end"sv)
			);
		}
	}

	void transformBody(ast_node* body, std::vector<std::string>& out) {
		body->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Block"_id: transformBlock(node, out); break;
				case "Statement"_id: transformStatement(node, out); break;
				default: break;
			}
		});
	}

	void transformFnArgsDef(ast_node* argsDef, std::vector<std::string>& out) {
		argsDef->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "FnArgDefList"_id: transformFnArgDefList(node, out); break;
				case "outer_var_shadow"_id: transform_outer_var_shadow(node, out); break;
				default: break;
			}
		});
	}

	void transform_outer_var_shadow(ast_node* shadow, std::vector<std::string>& out) {
		markVarShadowed();
		shadow->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "NameList"_id:
					node->eachChild([&](ast_node* child) {
						if (child->getId() == "Name"_id) {
							this->addToAllowList(toString(child));
						}
					});
					break;
				default: break;
			}
		});
	}

	void transformFnArgDefList(ast_node* argDefList, std::vector<std::string>& out) {
		std::vector<std::vector<std::string>> argItems;
		const int Name = 0;
		const int AssignSelf = 1;
		const int DefaultVal = 2;
		argDefList->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "FnArgDef"_id: {
					argItems.emplace_back(2);
					auto& arg = argItems.back();
					node->eachChild([&](ast_node* child) {
						switch (child->getId()) {
							case "Name"_id: arg[Name] = toString(child); break;
							case "SelfName"_id:
								child->eachChild([&](ast_node* inner) {
									switch (inner->getId()) {
										case "self_class_name"_id:
											arg[Name] = toString(inner->getChild(0));
											arg[AssignSelf] = s("self.__class."sv) + arg.front();
											break;
										case "self_class"_id:
											arg[Name] = "self.__class"sv;
											break;
										case "self_name"_id:
											arg[Name] = toString(inner->getChild(0));
											arg[AssignSelf] = s("self."sv) + arg.front();
											break;
										case "self"_id:
											arg[Name] = "self"sv;
											break;
									}
								});
								break;
							case "Exp"_id: transformExp(child, arg); break;
							default: break;
						}
					});
					break;
				}
				case "VarArg"_id:
					argItems.emplace_back(2);
					argItems.back()[Name] = "..."sv;
					break;
				default: break;
			}
		});
		std::string varNames;
		for (const auto& item : argItems) {
			if (varNames.empty()) {
				varNames = item[Name];
			} else {
				varNames.append(s(", "sv) + item[Name]);
			}
			forceAddToScope(item[Name]);
		}
		for (const auto& item : argItems) {
			if (item.size() == 3 && !item[DefaultVal].empty()) {
				_buf << indent() << "if "sv << item[Name] << " == nil then"sv << nll(argDefList) <<
					indent() << '\t' << item[Name] << " = "sv << item[DefaultVal] << nll(argDefList) <<
					indent() << "end"sv << nll(argDefList);
			}
		}
		std::string initCodes = clearBuf();
		std::vector<std::array<const std::string*, 2>> assignSelfVars;
		for (const auto& item : argItems) {
			if (!item[AssignSelf].empty()) {
				assignSelfVars.push_back({&item[AssignSelf], &item[Name]});
			}
		}
		auto sjoin = [](const decltype(assignSelfVars)& items, int index) {
			std::string result;
			for (auto it = items.begin(); it != items.end(); ++it) {
				if (result.empty()) result = *((*it)[index]);
				else result.append(s(", "sv) + *((*it)[index]));
			}
			return result;
		};
		std::string sleft = sjoin(assignSelfVars, 0);
		std::string sright = sjoin(assignSelfVars, 1);
		if (!assignSelfVars.empty()) {
			initCodes.append(sleft + s(" = "sv) + sright + nll(argDefList));
		}
		out.push_back(varNames);
		out.push_back(initCodes);
	}

	void transformChain(ast_node* chain, std::vector<std::string>& out) {
		chain->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "chain_call"_id: transform_chain_call(node, out); break;
				case "chain_item"_id: transform_chain_item(node, out); break;
				case "chain_dot_chain"_id: transform_chain_dot_chain(node, out); break;
				case "ColonChain"_id: transformColonChain(node, out); break;
				default: break;
			}
		});
	}

	void transform_chain_call(ast_node* chain_call, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		chain_call->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Callable"_id: transformCallable(node, temp); break;
				case "String"_id: transformString(node, temp); break;
				case "ChainItems"_id: transformChainItems(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp));
	}

	void transformChainItems(ast_node* chainItems, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		chainItems->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "ChainItem"_id: transformChainItem(node, temp); break;
				case "ColonChain"_id: transformColonChain(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp));
	}

	void transformChainItem(ast_node* chainItem, std::vector<std::string>& out) {
		chainItem->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Invoke"_id: transformInvoke(node, out); break;
				case "DotChainItem"_id:
					out.push_back(s("."sv) + toString(node->getChild(0)));
					break;
				case "Slice"_id: transformSlice(node, out); break;
				case "Exp"_id:
					transformExp(node, out);
					out.back() = s("["sv) + out.back() + s("]"sv);
					break;
				default: break;
			}
		});
	}

	void transformInvoke(ast_node* invoke, std::vector<std::string>& out) {
		invoke->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "FnArgs"_id: transformFnArgs(node, out); break;
				case "SingleString"_id: transformSingleString(node, out); break;
				case "DoubleString"_id: transformDoubleString(node, out); break;
				case "LuaString"_id: transformLuaString(node, out); break;
				default: break;
			}
		});
	}

	void transformFnArgs(ast_node* fnArgs, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		fnArgs->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Exp"_id: transformExp(node, temp); break;
				default: break;
			}
		});
		std::string args = join(temp, ", ");
		out.push_back(args.empty() ? "()" : s("(") + args + ")");
	}

	void transformColonChain(ast_node* colonChain, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		colonChain->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "ColonChainItem"_id:
					temp.push_back(s(":"sv) + toString(node->getChild(0)));
					break;
				case "invoke_chain"_id: transform_invoke_chain(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp));
	}

	void transform_invoke_chain(ast_node* invoke_chain, std::vector<std::string>& out) {
		std::vector<std::string> temp;
		invoke_chain->eachChild([&](ast_node* node) {
			switch (node->getId()) {
				case "Invoke"_id: transformInvoke(node, temp); break;
				case "ChainItems"_id: transformChainItems(node, temp); break;
				default: break;
			}
		});
		out.push_back(join(temp));
	}

	void transformUpdate(ast_node* node, std::vector<std::string>& out) {noop(node, out);}

	void transformImport(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformWhile(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformWith(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformIf(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformFor(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformForEach(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformSwitch(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformReturn(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformTableBlock(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformLocal(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformExport(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transformBreakLoop(ast_node* node, std::vector<std::string>& out) {noopnl(node, out);}
	void transform_unless_line(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformCompInner(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_simple_table(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformString(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformInvokeArgs(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformName(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformSelfName(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_const_value(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformClassDecl(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformDo(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_unary_exp(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformTblComprehension(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformTableLit(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformComprehension(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformNum(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformVarArg(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformBinaryOperator(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_chain_item(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transform_chain_dot_chain(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformSlice(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformSingleString(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformDoubleString(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
	void transformLuaString(ast_node* node, std::vector<std::string>& out) {noop(node, out);}
};

const std::string MoonCompliler::Empty;

int main()
{
	std::string s = R"TestCodesHere(a = 998
f, d = (->
  joop = 2302 + 567

  (hi, a, b = Vec2(100,200), c, d, ... using nil) ->
    d = "中文"
    hi = 1021

    a,b,c,d = 1,2,3,4

    hello[232], (5+5)[121], hello, x[99] = 100, 200, 300

    joop = 12), 123 if true else print("a",1,2)\abc(998).x

a, b = if hello
  "hello"
else
  "nothing", "yeah"


a, b = if hello
  if yeah then "one", "two" else "mmhh"
else
  print "the other"
  "nothing", "yeah")TestCodesHere";

	MoonCompliler{}.complile(s);

	return 0;
}
