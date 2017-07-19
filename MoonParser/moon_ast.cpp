#include <string>
#include <codecvt>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <sstream>
#include <vector>
#include "moon_ast.h"

class Data
{
public:
	Data()
	{
		indent = 0;
		callerStack.push(false);
	}

	Converter conv;
	std::stringstream temp;
	std::stringstream buffer;
	std::vector<int> lineTable;
	std::stack<bool> callerStack;
	std::stack<std::string> withStack;

	void beginLine(int line = -1)
	{
		for (int i = 0; i < indent; i++)
		{
			buffer << '\t';
		}
		lineTable.push_back(line == -1 ? lineTable.back() : line);
	}

	void endLine()
	{
		buffer << '\n';
	}

	int indent;
	struct Scope
	{
		Scope()
		{
			localObjIndex = 0;
			localFnIndex = 0;
			localBaseIndex = 0;
			localWithIndex = 0;
		}
		int localObjIndex;
		int localFnIndex;
		int localBaseIndex;
		int localWithIndex;
		std::vector<std::string> locals;
	};

	std::string getNewLocalObj()
	{
		temp << "_obj_" << scope().localObjIndex;
		scope().localObjIndex++;
		std::string local = temp.str();
		temp.str("");
		temp.clear();
		return local;
	}

	std::string getNewLocalFn()
	{
		temp << "_fn_" << scope().localObjIndex;
		scope().localFnIndex++;
		std::string local = temp.str();
		temp.str("");
		temp.clear();
		return local;
	}

	std::string getNewLocalBase()
	{
		temp << "_base_" << scope().localBaseIndex;
		scope().localBaseIndex++;
		std::string local = temp.str();
		temp.str("");
		temp.clear();
		return local;
	}

	bool isLocal(const std::string& var)
	{
		return _localVars.find(var) != _localVars.end();
	}

	void putLocal(const std::string& var)
	{
		if (_localVars.find(var) == _localVars.end())
		{
			_localVars.insert(var);
			_scopeStack.top().locals.push_back(var);
		}
	}

	void pushScope()
	{
		int lastWithIndex = scope().localWithIndex;
		_scopeStack.emplace();
		scope().localWithIndex = lastWithIndex + 1;
		indent++;
	}

	Scope& scope()
	{
		return _scopeStack.top();
	}

	void popScope()
	{
		const auto& scope = _scopeStack.top();
		for (const auto& var : scope.locals)
		{
			_localVars.erase(var);
		}
		_scopeStack.pop();
		indent--;
	}
private:
	std::unordered_set<std::string> _localVars;
	std::stack<Scope> _scopeStack;
};

std::string& trim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
	{
		return !std::isspace(ch);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
	{
		return !std::isspace(ch);
	}).base(), s.end());
	return s;
}

const std::string& AstLeaf::getValue()
{
	if (_value.empty())
	{
		for (auto it = m_begin.m_it; it != m_end.m_it; ++it)
		{
			char ch = static_cast<char>(*it);
			_value.append(&ch, 1);
		}
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

void Num_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	std::string str = data->conv.to_bytes(&*m_begin.m_it, &*m_end.m_it);
	data->buffer << trim(str);
}

void _Name_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << getValue();
}

void Name_t::visit(void* ud)
{
	name->visit(ud);
}

void self_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << "self";
}

void self_name_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << (data->callerStack.top() ? "self:" : "self.");
	name->visit(ud);
}

void self_class_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << "self.__class";
}

void self_class_name_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << (data->callerStack.top() ? "self.__class:" : "self.__class.");
	name->visit(ud);
}

void SelfName_t::visit(void* ud)
{
	name->visit(ud);
}

void KeyName_t::visit(void* ud)
{
	name->visit(ud);
}

void VarArg_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << "...";
}

void NameList_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	auto it = names.objects().begin();
	Name_t* name = *it;
	name->visit(ud);
	++it;
	for (; it != names.objects().end(); ++it)
	{
		name = *it;
		data->buffer << ", ";
		name->visit(ud);
	}
}

void Import_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	std::vector<std::tuple<const std::string*, bool>> nameItems;
	nameItems.reserve(names.objects().size());
	for (ImportName_t* importName : names.objects())
	{
		if (Name_t* name = ast_cast<Name_t>(importName->name))
		{
			nameItems.push_back(std::make_tuple(&name->name->getValue(), false));
		}
		else
		{
			colon_import_name_t* colonName = ast_cast<colon_import_name_t>(importName->name);
			nameItems.push_back(std::make_tuple(&colonName->name->name->getValue(), true));
		}
	}
	data->buffer << "local ";
	for (const auto& item : nameItems)
	{
		data->buffer << *std::get<0>(item);
		if (&item != &nameItems.back())
		{
			data->buffer << ", ";
		}
	}
	data->endLine();

	data->beginLine();
	data->pushScope();
	data->buffer << "do";
	data->endLine();

	std::string fromObj = data->getNewLocalObj();

	data->beginLine();
	data->buffer << "local " << fromObj << " = ";
	exp->visit(ud);
	data->endLine();

	data->beginLine();
	for (const auto& item : nameItems)
	{
		data->buffer << *std::get<0>(item);
		if (&item != &nameItems.back())
		{
			data->buffer << ", ";
		}
	}
	data->buffer << " = ";
	for (const auto& item : nameItems)
	{
		if (std::get<1>(item))
		{
			data->pushScope();
			data->buffer << "(function()";
			data->endLine();

			std::string varBase = data->getNewLocalBase();

			data->beginLine();
			data->buffer << "local " << varBase << " = " << fromObj;
			data->endLine();

			std::string varFn = data->getNewLocalFn();

			data->beginLine();
			data->buffer << "local " << varFn << " = " << varBase << '.' << *std::get<0>(item);
			data->endLine();

			data->beginLine();
			data->buffer << "return function(...)";
			data->endLine();

			data->beginLine();
			data->pushScope();
			data->buffer << varFn << '(' << varBase << ", ...)";
			data->endLine();

			data->beginLine();
			data->buffer << "end";
			data->popScope();
			data->endLine();

			data->beginLine();
			data->buffer << "end)()";
			data->popScope();
		}
		else
		{
			data->buffer << fromObj << '.' << *std::get<0>(item);
		}
		if (&item != &nameItems.back())
		{
			data->buffer << ", ";
		}
	}
	data->endLine();

	data->beginLine();
	data->buffer << "end";
	data->popScope();
}

void ExpListLow_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	for (Exp_t* expr : exprs.objects())
	{
		expr->visit(ud);
		if (expr != exprs.objects().back())
		{
			data->buffer << ", ";
		}
	}
}

void ExpList_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	for (Exp_t* expr : exprs.objects())
	{
		expr->visit(ud);
		if (expr != exprs.objects().back())
		{
			data->buffer << ", ";
		}
	}
}

void Return_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << "return";
	if (valueList && !valueList->exprs.objects().empty())
	{
		data->buffer << ' ';
		valueList->visit(ud);
	}
}

void With_t::visit(void* ud)
{
	Data* data = static_cast<Data*>(ud);
	data->buffer << "return";
	for (Exp_t* expr : valueList->exprs.objects())
	{
		if (assigns && (!expr->opValues.objects().empty() || expr->value->getFlattened()))
		{
			throw std::logic_error("left hand expression is not assignable.");
		}
		// TODO
	}
	if (valueList && !valueList->exprs.objects().empty())
	{
		data->buffer << ' ';
		valueList->visit(ud);
	}
}

ast_node* Value_t::getFlattened()
{
	if (SimpleValue_t* simpleValue = ast_cast<SimpleValue_t>(item))
	{
		return simpleValue->value;
	}
	else if (simple_table_t* simple_table = ast_cast<simple_table_t>(item))
	{
		return simple_table;
	}
	else if (ChainValue_t* chainValue = ast_cast<ChainValue_t>(item))
	{
		if (chainValue->arguments)
		{
			return chainValue;
		}
		else
		{
			if (Chain_t* chain = ast_cast<Chain_t>(chainValue->caller))
			{
				return chain->item;
			}
			else if (Callable_t* callable = ast_cast<Callable_t>(chainValue->caller))
			{
				return callable->item;
			}
		}
	}
	return item;
}

#include <iostream>

int main()
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
	std::string s = R"baddog(import \x, func, \memFunc from require "utils")baddog";
	input i = conv.from_bytes(s);

	error_list el;
	Import_t* root = nullptr;
	State st;
	if (parse(i, Import, el, root, &st))
	{
		std::cout << "matched!\n";
		Data data;
		root->visit(&data);
	}
	else
	{
		std::cout << "not matched!\n";
		for (error_list::iterator it = el.begin(); it != el.end(); ++it)
		{
			const error& err = *it;
			std::cout << "line " << err.m_begin.m_line << ", col " << err.m_begin.m_col << ": syntax error\n";
		}
	}
	system("pause");
	return 0;
}
