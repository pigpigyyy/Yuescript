#include <iostream>
#include <string>
#include <codecvt>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <sstream>
#include <vector>
#include "parserlib.hpp"
using namespace parserlib;

template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args): Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};
typedef std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> Converter;

static inline std::string& trim(std::string& s)
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

#define p(expr) user(expr, [](const item_t& item) \
{ \
	stringstream stream; \
	for (input_it i = item.begin; i != item.end; ++i) stream << static_cast<char>(*i); \
	cout << #expr << ": [" << stream.str() << "]\n"; \
	return true; \
})

struct State
{
	State()
	{
		indents.push(0);
		stringOpen = -1;
	}
	std::stringstream buffer;
	size_t stringOpen;
	std::stack<int> indents;
	std::stack<bool> doStack;
	std::unordered_set<std::string> keywords = {
		"and", "while", "else", "using", "continue",
		"local", "not", "then", "return", "from",
		"extends", "for", "do", "or", "export",
		"class", "in", "unless", "when", "elseif",
		"switch", "break", "if", "with", "import"
	};
};

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

rule Any = any();
rule plain_space = *set(" \t");
rule Break = nl(-expr('\r') >> '\n');
rule White = *(set(" \t") | Break);
rule Stop = Break | eof();
rule Comment = "--" >> *(not_(set("\r\n")) >> Any) >> and_(Stop);
rule Indent = *set(" \t");
rule Space = plain_space >> -Comment;
rule SomeSpace = +set(" \t") >> -Comment;
rule SpaceBreak = Space >> Break;
rule EmptyLine = SpaceBreak;
rule AlphaNum = range('a', 'z') | range('A', 'Z') | range('0', '9') | '_';
rule _Name = (range('a', 'z') | range('A', 'Z') | '_') >> *AlphaNum;
rule SpaceName = Space >> _Name;
rule _Num =
	(
		"0x" >>
		+(range('0', '9') | range('a', 'f') | range('A', 'F')) >>
		-(-set("uU") >> set("lL") >> set("lL"))
	) | (
		+range('0', '9') >> -set("uU") >> set("lL") >> set("lL")
	) | (
		(
			(+range('0', '9') >> -('.' >> +range('0', '9'))) |
			('.' >> +range('0', '9'))
		) >> -(set("eE") >> -expr('-') >> +range('0', '9'))
	);
rule Num = Space >> _Num;
rule Cut = false_();
rule Seperator = true_();

#define sym(str) (Space >> str)
#define symx(str) expr(str)
#define ensure(patt, finally) (((patt) >> (finally)) | ((finally) >> (Cut)))
#define key(str) (Space >> str >> not_(AlphaNum))
#define opWord(str) (Space >> str >> not_(AlphaNum))
#define op(str) (Space >> str)

rule Name = user(SpaceName, [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	for (auto it = item.begin; it != item.end; ++it) st->buffer << static_cast<char>(*it);
	std::string name;
	st->buffer >> name;
	st->buffer.str("");
	st->buffer.clear();
	auto it = st->keywords.find(name);
	return it == st->keywords.end();
});

rule self = expr('@');
rule self_name = '@' >> _Name;
rule self_class = expr("@@");
rule self_class_name = "@@" >> _Name;

rule SelfName = Space >> (self_class_name | self_class | self_name | self);
rule KeyName = SelfName | Space >> _Name;
rule VarArg = Space >> "...";

rule check_indent = user(Indent, [](const item_t& item)
{
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i)
	{
		switch (*i)
		{
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	return st->indents.top() == indent;
});
rule CheckIndent = and_(check_indent);

rule advance = user(Indent, [](const item_t& item)
{
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i)
	{
		switch (*i)
		{
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	int top = st->indents.top();
	if (top != -1 && indent > top)
	{
		st->indents.push(indent);
		return true;
	}
	return false;
});
rule Advance = and_(advance);

rule push_indent = user(Indent, [](const item_t& item)
{
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i)
	{
		switch (*i)
		{
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.push(indent);
	return true;
});
rule PushIndent = and_(push_indent);

rule PreventIndent = user(true_(), [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.push(-1);
	return true;
});

rule PopIndent = user(true_(), [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.pop();
	return true;
});

extern rule Block;

rule InBlock = Advance >> Block >> PopIndent;

extern rule NameList;

rule local_flag = op('*') | op('^');
rule Local = key("local") >> (local_flag | NameList);

rule colon_import_name = sym('\\') >> Name;
rule ImportName = colon_import_name | Name;
rule ImportNameList = Seperator >> *SpaceBreak >> ImportName >> *((+SpaceBreak | sym(',') >> *SpaceBreak) >> ImportName);

extern rule Exp;

rule Import = key("import") >> ImportNameList >> *SpaceBreak >> key("from") >> Exp;
rule BreakLoop = key("break") | key("continue");

extern rule ExpListLow, ExpList, Assign;

rule Return = key("return") >> -ExpListLow;
rule WithExp = ExpList >> -Assign;

extern rule DisableDo, PopDo, Body;

rule With = key("with") >> DisableDo >> ensure(WithExp, PopDo) >> -key("do") >> Body;
rule SwitchCase = key("when") >> ExpList >> -key("then") >> Body;
rule SwitchElse = key("else") >> Body;

rule SwitchBlock = *EmptyLine >>
	Advance >> Seperator >>
	SwitchCase >>
	*(+Break >> SwitchCase) >>
	-(+Break >> SwitchElse) >>
	PopIndent;

rule Switch = key("switch") >>
	DisableDo >> ensure(Exp, PopDo) >>
	-key("do") >> -Space >> Break >> SwitchBlock;

rule IfCond = Exp >> -Assign;
rule IfElseIf = -(Break >> *EmptyLine >> CheckIndent) >> key("elseif") >> IfCond >> -key("then") >> Body;
rule IfElse = -(Break >> *EmptyLine >> CheckIndent) >> key("else") >> Body;
rule If = key("if") >> IfCond >> -key("then") >> Body >> Seperator >> *IfElseIf >> -IfElse;
rule Unless = key("unless") >> IfCond >> -key("then") >> Body >> Seperator >> *IfElseIf >> -IfElse;

rule While = key("while") >> DisableDo >> ensure(Exp, PopDo) >> -key("do") >> Body;

rule for_step_value = sym(',') >> Exp;
rule for_args = Name >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;

rule For = key("for") >> DisableDo >>
	ensure(for_args, PopDo) >>
	-key("do") >> Body;

extern rule AssignableNameList;

rule for_in = sym('*') >> Exp | ExpList;

rule ForEach = key("for") >> AssignableNameList >> key("in") >>
	DisableDo >> ensure(for_in, PopDo) >>
	-key("do") >> Body;

rule Do = user(key("do") >> Body, [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	return st->doStack.empty() || st->doStack.top();
});

rule DisableDo = user(true_(), [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->doStack.push(false);
	return true;
});

rule PopDo = user(true_(), [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->doStack.pop();
	return true;
});

extern rule CompInner;

rule Comprehension = sym('[') >> Exp >> CompInner >> sym(']');
rule comp_value = sym(',') >> Exp;
rule TblComprehension = sym('{') >> (Exp >> -comp_value) >> CompInner >> sym('}');

extern rule CompForEach, CompFor, CompClause;

rule CompInner = (CompForEach | CompFor) >> Seperator >> *CompClause;
rule star_exp = sym('*') >> Exp;
rule CompForEach = key("for") >> AssignableNameList >> key("in") >> (star_exp | Exp);
rule CompFor = key("for") >> Name >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;
rule CompClause = CompFor | CompForEach | key("when") >> Exp;

extern rule TableBlock;

rule Assign = sym('=') >> (With | If | Switch | TableBlock | ExpListLow);

rule update_op =
	sym("..=") |
	sym("+=") |
	sym("-=") |
	sym("*=") |
	sym("/=") |
	sym("%=") |
	sym("or=") |
	sym("and=") |
	sym("&=") |
	sym("|=") |
	sym(">>=") |
	sym("<<=");

rule Update = update_op >> Exp;

rule CharOperators = Space >> set("+-*/%^><|&");
rule WordOperators =
	opWord("or") |
	opWord("and") |
	op("<=") |
	op(">=") |
	op("~=") |
	op("!=") |
	op("==") |
	op("..") |
	op("<<") |
	op(">>") |
	op("//");

rule BinaryOperator = (WordOperators | CharOperators) >> *SpaceBreak;

extern rule Chain;

rule Assignable = Chain | Name | SelfName;

extern rule Value;

rule exp_op_value = BinaryOperator >> Value;
rule Exp = Value >> *exp_op_value;

extern rule Callable, InvokeArgs;

rule ChainValue = (Chain | Callable) >> -InvokeArgs;

extern rule KeyValue, String, SimpleValue;

rule simple_table = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
rule Value = SimpleValue | simple_table | ChainValue | String;

extern rule LuaString;

rule single_string_inner = expr("\\'") | "\\\\" | not_(expr('\'')) >> Any;
rule SingleString = symx('\'') >> *single_string_inner >> sym('\'');
rule interp = symx("#{") >> Exp >> sym('}');
rule double_string_plain = expr("\\\"") | "\\\\" | not_(expr('"')) >> Any;
rule double_string_inner = +(not_(interp) >> double_string_plain);
rule double_string_content = double_string_inner | interp;
rule DoubleString = symx('"') >> Seperator >> *double_string_content >> sym('"');
rule String = Space >> (DoubleString | SingleString | LuaString);

rule lua_string_open = '[' >> *expr('=') >> '[';
rule lua_string_close = ']' >> *expr('=') >> ']';

rule LuaStringOpen = user(lua_string_open, [](const item_t& item)
{
	size_t count = std::distance(item.begin, item.end);
	State* st = reinterpret_cast<State*>(item.user_data);
	st->stringOpen = count;
	return true;
});

rule LuaStringClose = user(lua_string_close, [](const item_t& item)
{
	size_t count = std::distance(item.begin, item.end);
	State* st = reinterpret_cast<State*>(item.user_data);
	return st->stringOpen == count;
});

rule LuaStringContent = *(not_(LuaStringClose) >> (Break | Any));

rule LuaString = user(LuaStringOpen >> -Break >> LuaStringContent >> LuaStringClose, [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->stringOpen = -1;
	return true;
});

rule Parens = sym('(') >> *SpaceBreak >> Exp >> *SpaceBreak >> sym(')');
rule Callable = Name | SelfName | VarArg | Parens;
rule FnArgsExpList = Exp >> *((Break | sym(',')) >> White >> Exp);

rule FnArgs = Seperator >>
(
	(
		symx('(') >> *SpaceBreak >> -FnArgsExpList >> *SpaceBreak >> sym(')')
	) | (
		sym('!') >> not_(expr('='))
	)
);

extern rule ChainItems, DotChainItem, ColonChain;

rule chain_call = (Callable | String) >> ChainItems;
rule chain_item = not_(set(".\\")) >> ChainItems;
rule chain_dot_chain = DotChainItem >> -ChainItems;

rule Chain =
	chain_call |
	chain_item |
	Space >> (chain_dot_chain | ColonChain);

extern rule ChainItem;

rule chain_with_colon = +ChainItem >> -ColonChain;
rule ChainItems = Seperator >> (chain_with_colon | ColonChain);

extern rule Invoke, Slice;

rule Index = symx('[') >> Exp >> sym(']');
rule ChainItem = Invoke | DotChainItem | Slice | Index;
rule DotChainItem = symx('.') >> _Name;
rule ColonChainItem = symx('\\') >> _Name;
rule invoke_chain = Invoke >> -ChainItems;
rule ColonChain = ColonChainItem >> -invoke_chain;

rule default_value = true_();
rule Slice =
	symx('[') >>
	(Exp | default_value) >>
	sym(',') >>
	(Exp | default_value) >>
	(sym(',') >> Exp | default_value) >>
	sym(']');

rule Invoke =
	FnArgs |
	SingleString |
	DoubleString |
	and_(expr('[')) >> LuaString;

extern rule TableValueList, TableLitLine;

rule TableValue = KeyValue | Exp;

rule table_lit_lines = SpaceBreak >> TableLitLine >> *(-sym(',') >> SpaceBreak >> TableLitLine) >> -sym(',');

rule TableLit =
	sym('{') >> Seperator >>
	-TableValueList >>
	-sym(',') >>
	-table_lit_lines >>
	White >> sym('}');

rule TableValueList = TableValue >> *(sym(',') >> TableValue);

rule TableLitLine =
(
	PushIndent >> (TableValueList >> PopIndent | PopIndent)
) | (
	Space
);

extern rule KeyValueLine;

rule TableBlockInner = Seperator >> KeyValueLine >> *(+(SpaceBreak) >> KeyValueLine);
rule TableBlock = +(SpaceBreak) >> Advance >> ensure(TableBlockInner, PopIndent);

extern rule Statement;

rule class_member_list = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
rule ClassLine = CheckIndent >> (class_member_list | Statement | Exp) >> -sym(',');
rule ClassBlock = +(SpaceBreak) >> Advance >>Seperator >> ClassLine >> *(+(SpaceBreak) >> ClassLine) >> PopIndent;

rule ClassDecl =
	key("class") >> not_(expr(':')) >>
	-Assignable >>
	-(key("extends")  >> PreventIndent >> ensure(Exp, PopIndent)) >>
	-ClassBlock;

rule export_values = NameList >> -(sym('=') >> ExpListLow);
rule export_op = op('*') | op('^');
rule Export = key("export") >> (ClassDecl | export_op | export_values);

rule variable_pair = sym(':') >> not_(SomeSpace) >> Name;

rule normal_pair =
(
	KeyName |
	sym('[') >> Exp >> sym(']') |
	Space >> DoubleString |
	Space >> SingleString
) >>
symx(':') >>
(Exp | TableBlock | +(SpaceBreak) >> Exp);

rule KeyValue = variable_pair | normal_pair;

rule KeyValueList = KeyValue >> *(sym(',') >> KeyValue);
rule KeyValueLine = CheckIndent >> KeyValueList >> -sym(',');

rule FnArgDef = (Name | SelfName) >> -(sym('=') >> Exp);

rule FnArgDefList = Seperator >>
(
	(
		FnArgDef >>
		*((sym(',') | Break) >> White >> FnArgDef) >>
		-((sym(',') | Break) >> White >> VarArg)
	) | (
		VarArg
	)
);

rule outer_var_shadow = key("using") >> (NameList | Space >> expr("nil"));

rule FnArgsDef = sym('(') >> White >> -FnArgDefList >> -outer_var_shadow >> White >> sym(')');
rule fn_arrow = sym("->") | sym("=>");
rule FunLit = -FnArgsDef >> fn_arrow >> -Body;

rule NameList = Seperator >> Name >> *(sym(',') >> Name);
rule NameOrDestructure = Name | TableLit;
rule AssignableNameList = Seperator >> NameOrDestructure >> *(sym(',') >> NameOrDestructure);

rule ExpList = Seperator >> Exp >> *(sym(',') >> Exp);
rule ExpListLow = Seperator >> Exp >> *((sym(',') | sym(';')) >> Exp);

rule ArgLine = CheckIndent >> Exp >> *(sym(',') >> Exp);
rule ArgBlock = Seperator >> ArgLine >> *(sym(',') >> SpaceBreak >> ArgLine) >> PopIndent;

rule invoke_args_with_table =
	sym(',') >>
	(
		TableBlock |
		SpaceBreak >> Advance >> ArgBlock >> -TableBlock
	);

rule InvokeArgs =
	not_(expr('-')) >>
	(
		ExpList >> -(invoke_args_with_table | TableBlock) |
		TableBlock
	);

rule const_value = key("nil") | key("true") | key("false");
rule minus_exp = sym('-') >> not_(SomeSpace) >> Exp;
rule sharp_exp = sym('#') >> Exp;
rule tilde_exp = sym('~') >> Exp;
rule not_exp = key("not") >> Exp;
rule unary_exp = minus_exp | sharp_exp | tilde_exp | not_exp;

rule SimpleValue =
	const_value |
	If | Unless | Switch | With | ClassDecl | ForEach | For | While | Do |
	unary_exp |
	TblComprehension | TableLit | Comprehension | FunLit | Num;

rule Assignment = ExpList >> (Update | Assign);

rule if_else_line = key("if") >> Exp >> (key("else") >> Exp | default_value);
rule unless_line = key("unless") >> Exp;

rule statement_appendix = (if_else_line | unless_line | CompInner) >> Space;
rule Statement =
(
	Import | While | With | For | ForEach |
	Switch | Return | Local | Export | BreakLoop |
	Assignment | ExpList
) >> Space >>
-statement_appendix;

rule Body = -Space >> Break >> *EmptyLine >> InBlock | Statement;

rule empty_line_stop = Space >> and_(Stop);
rule Line = CheckIndent >> Statement | empty_line_stop;
rule Block = Seperator >> Line >> *(+Break >> Line);
rule BlockEnd = Block >> eof();

class AstLeaf : public ast_node
{
public:
	const std::string& getValue()
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
private:
	std::string _value;
};

#define AST_LEAF(type) \
class type##_t : public AstLeaf \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); }

#define AST_NODE(type) \
class type##_t : public ast_container \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); }

#define AST_END(type) \
}; \
ast<type##_t> __##type##_t(type);

AST_LEAF(Num)
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		std::string str = data->conv.to_bytes(&*m_begin.m_it, &*m_end.m_it);
		data->buffer << trim(str);
	}
AST_END(Num)

AST_LEAF(_Name)
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << getValue();
	}
AST_END(_Name)

AST_NODE(Name)
	ast_ptr<_Name_t> name;
	virtual void visit(void* ud) override
	{
		name->visit(ud);
	}
AST_END(Name)

AST_LEAF(self)
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << "self";
	}
AST_END(self)

AST_NODE(self_name)
	ast_ptr<_Name_t> name;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << (data->callerStack.top() ? "self:" : "self.");
		name->visit(ud);
	}
AST_END(self_name)

AST_LEAF(self_class)
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << "self.__class";
	}
AST_END(self_class)

AST_NODE(self_class_name)
	ast_ptr<_Name_t> name;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << (data->callerStack.top() ? "self.__class:" : "self.__class.");
		name->visit(ud);
	}
AST_END(self_class_name)

AST_NODE(SelfName)
	ast_ptr<ast_node> name; // self_class_name_t | self_class_t | self_name_t | self_t
	virtual void visit(void* ud) override
	{
		name->visit(ud);
	}
AST_END(SelfName)

AST_NODE(KeyName)
	ast_ptr<ast_node> name; // SelfName_t | _Name_t
	virtual void visit(void* ud) override
	{
		name->visit(ud);
	}
AST_END(KeyName)

AST_LEAF(VarArg)
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << "...";
	}
AST_END(VarArg)

AST_LEAF(local_flag)
AST_END(local_flag)

AST_LEAF(Seperator)
AST_END(Seperator)

AST_NODE(NameList)
	ast_ptr<Seperator_t> sep;
	ast_list<Name_t> names;
	virtual void visit(void* ud) override
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
AST_END(NameList)

AST_NODE(Local)
	ast_ptr<ast_node> name; // local_flag_t | NameList_t
AST_END(Local)

AST_NODE(colon_import_name)
	ast_ptr<Name_t> name;
AST_END(colon_import_name)

AST_NODE(Exp)
	ast_ptr<Value_t> value;
	ast_list<exp_op_value_t> opValues;
AST_END(Exp)

AST_NODE(ImportName)
	ast_ptr<ast_node> name; // colon_import_name_t | Name_t
AST_END(ImportName)

AST_NODE(Import)
	ast_ptr<Seperator_t> sep;
	ast_list<ImportName_t> names;
	ast_ptr<Exp_t> exp;
	virtual void visit(void* ud) override
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
		((ast_node*)exp.get())->visit(ud);
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
AST_END(Import)

AST_NODE(ExpListLow)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> exprs;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		for (Exp_t* expr : exprs.objects())
		{
			((ast_node*)expr)->visit(ud);
			if (expr != exprs.objects().back())
			{
				data->buffer << ", ";
			}
		}
	}
AST_END(ExpListLow)

AST_NODE(ExpList)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> exprs;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		for (Exp_t* expr : exprs.objects())
		{
			((ast_node*)expr)->visit(ud);
			if (expr != exprs.objects().back())
			{
				data->buffer << ", ";
			}
		}
	}
AST_END(ExpList)

AST_NODE(Return)
	ast_ptr<ExpListLow_t, true> valueList;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->buffer << "return";
		if (valueList && !valueList->exprs.objects().empty())
		{
			data->buffer << ' ';
			valueList->visit(ud);
		}
	}
AST_END(Return)

class Assign_t;
class Body_t;

AST_NODE(With)
	ast_ptr<ExpList_t> valueList;
	ast_ptr<Assign_t, true> assigns;
	ast_ptr<Body_t> body;
	/*
	(function()
    do
      local _with_0 = Something()
      _with_0:write("hello world")
      return _with_0
    end
  end)()
	*/
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		for (Exp_t* expr : valueList->exprs.objects())
		{
		}
			((ast_node*)expr)->visit(ud);
			if (expr != exprs.objects().back())
			{
				data->buffer << ", ";
			}
	}
AST_END(With)

AST_NODE(SwitchCase)
	ast_ptr<ExpList_t> valueList;
	ast_ptr<Body_t> body;
AST_END(SwitchCase)

AST_NODE(Switch)
	ast_ptr<Exp_t> target;
	ast_ptr<Seperator_t> sep;
	ast_list<SwitchCase_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(Switch)

AST_NODE(IfCond)
	ast_ptr<Exp_t> condition;
	ast_ptr<Assign_t, true> assign;
AST_END(IfCond)

AST_NODE(IfElseIf)
	ast_ptr<IfCond_t> condition;
	ast_ptr<Body_t> body;
AST_END(IfElseIf)

AST_NODE(If)
	ast_ptr<IfCond_t> firstCondition;
	ast_ptr<Body_t> firstBody;
	ast_ptr<Seperator_t> sep;
	ast_list<IfElseIf_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(If)

AST_NODE(Unless)
	ast_ptr<IfCond_t> firstCondition;
	ast_ptr<Body_t> firstBody;
	ast_ptr<Seperator_t> sep;
	ast_list<IfElseIf_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(Unless)

AST_NODE(While)
	ast_ptr<Exp_t> condition;
	ast_ptr<Body_t> body;
AST_END(While)

AST_NODE(for_step_value)
	ast_ptr<Exp_t> value;
AST_END(for_step_value)

AST_NODE(For)
	ast_ptr<Name_t> varName;
	ast_ptr<Exp_t> startValue;
	ast_ptr<Exp_t> stopValue;
	ast_ptr<for_step_value_t, true> stepValue;
	ast_ptr<Body_t> body;
AST_END(For)

class AssignableNameList_t;

AST_NODE(ForEach)
	ast_ptr<AssignableNameList_t> nameList;
	ast_ptr<ast_node> loopValue; // Exp_t | ExpList_t
	ast_ptr<Body_t> body;
AST_END(ForEach)

AST_NODE(Do)
	ast_ptr<Body_t> body;
AST_END(Do)

class CompInner_t;

AST_NODE(Comprehension)
	ast_ptr<Exp_t> value;
	ast_ptr<CompInner_t> forLoop;
AST_END(Comprehension)

AST_NODE(comp_value)
	ast_ptr<Exp_t> value;
AST_END(comp_value)

AST_NODE(TblComprehension)
	ast_ptr<Exp_t> key;
	ast_ptr<comp_value_t, true> value;
	ast_ptr<CompInner_t> forLoop;
AST_END(TblComprehension)

AST_NODE(star_exp)
	ast_ptr<Exp_t> value;
AST_END(star_exp)

AST_NODE(CompForEach)
	ast_ptr<AssignableNameList_t> nameList;
	ast_ptr<ast_node> loopValue; // star_exp_t | Exp_t
AST_END(CompForEach)

AST_NODE(CompFor)
	ast_ptr<Name_t> varName;
	ast_ptr<Exp_t> startValue;
	ast_ptr<Exp_t> stopValue;
	ast_ptr<for_step_value_t, true> stepValue;
AST_END(CompFor)

AST_NODE(CompClause)
	ast_ptr<ast_node> nestExp; // CompFor_t | CompForEach_t | Exp_t
AST_END(CompClause)

AST_NODE(CompInner)
	ast_ptr<ast_node> compFor; // CompFor_t | CompForEach_t
	ast_ptr<Seperator_t> sep;
	ast_list<CompClause_t> clauses;
AST_END(CompInner)

class TableBlock_t;

AST_NODE(Assign)
	ast_ptr<ast_node> value; // With_t | If_t | Switch_t | TableBlock_t | ExpListLow_t
AST_END(Assign)

AST_LEAF(update_op)
AST_END(update_op)

AST_NODE(Update)
	ast_ptr<update_op_t> op;
	ast_ptr<Exp_t> value;
AST_END(Update)

AST_LEAF(BinaryOperator)
AST_END(BinaryOperator)

class Chain_t;

AST_NODE(Assignable)
	ast_ptr<ast_node> item; // Chain_t | Name_t | SelfName_t
AST_END(Assignable)

class Value_t;

AST_NODE(exp_op_value)
	ast_ptr<BinaryOperator_t> op;
	ast_ptr<Value_t> value;
AST_END(exp_op_value)

AST_NODE(Callable)
	ast_ptr<ast_node> item; // Name_t | SelfName_t | VarArg_t | Parens_t
AST_END(Callable)

class InvokeArgs_t;

AST_NODE(ChainValue)
	ast_ptr<ast_node> caller; // Chain_t | Callable_t
	ast_ptr<InvokeArgs_t, true> arguments;

	virtual void visit(void* ud) override
	{

	}
AST_END(ChainValue)

class KeyValue_t;

AST_NODE(simple_table)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> pairs;
AST_END(simple_table)

class String_t;

AST_NODE(SimpleValue)
	ast_ptr<ast_node> value; /*
	const_value_t |
	If_t | Unless_t | Switch_t | With_t | ClassDecl_t | ForEach_t | For_t | While_t | Do_t |
	unary_exp_t |
	TblComprehension_t | TableLit_t | Comprehension_t | FunLit_t | Num_t;
	*/
AST_END(SimpleValue)

AST_NODE(Chain)
	ast_ptr<ast_node> item; // chain_call_t | chain_item_t | chain_dot_chain_t | ColonChain_t
AST_END(Chain)

AST_NODE(Value)
	ast_ptr<ast_node> item; // SimpleValue_t | simple_table_t | ChainValue_t | String_t
	ast_node* getFlattenedNode()
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
AST_END(Value)

AST_LEAF(LuaString)
AST_END(LuaString)

AST_LEAF(SingleString)
AST_END(SingleString)

AST_LEAF(double_string_inner)
AST_END(double_string_inner)

AST_NODE(double_string_content)
	ast_ptr<ast_node> content; // double_string_inner_t | Exp_t
AST_END(double_string_content)

AST_NODE(DoubleString)
	ast_ptr<Seperator_t> sep;
	ast_list<double_string_content_t> segments;
AST_END(DoubleString)

AST_NODE(String)
	ast_ptr<ast_node> str; // DoubleString_t | SingleString_t | LuaString_t
AST_END(String)

AST_NODE(Parens)
	ast_ptr<Exp_t> expr;
AST_END(Parens)

AST_NODE(FnArgs)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> args;
AST_END(FnArgs)

class ChainItems_t;

AST_NODE(chain_call)
	ast_ptr<ast_node> caller; // Callable_t | String_t
	ast_ptr<ChainItems_t> chain;
AST_END(chain_call)

AST_NODE(chain_item)
	ast_ptr<ChainItems_t> chain;
AST_END(chain_item)

AST_NODE(DotChainItem)
	ast_ptr<_Name_t> name;
AST_END(DotChainItem)

AST_NODE(ColonChainItem)
	ast_ptr<_Name_t> name;
AST_END(ColonChainItem)

AST_NODE(chain_dot_chain)
	ast_ptr<DotChainItem_t> caller;
	ast_ptr<ChainItems_t, true> chain;
AST_END(chain_dot_chain)

class ColonChain_t;

class Invoke_t;
class Slice_t;

AST_NODE(ChainItem)
	ast_ptr<ast_node> item; // Invoke_t | DotChainItem_t | Slice_t | [Exp_t]
AST_END(ChainItem)

AST_NODE(ChainItems)
	ast_ptr<Seperator_t> sep;
	ast_list<ChainItem_t> simpleChain;
	ast_ptr<ColonChain_t, true> colonChain;
AST_END(ChainItems)

AST_NODE(invoke_chain)
	ast_ptr<Invoke_t> invoke;
	ast_ptr<ChainItems_t, true> chain;
AST_END(invoke_chain)

AST_NODE(ColonChain)
	ast_ptr<ColonChainItem_t> colonChain;
	ast_ptr<invoke_chain_t, true> invokeChain;
AST_END(ColonChain)

AST_LEAF(default_value)
AST_END(default_value)

AST_NODE(Slice)
	ast_ptr<ast_node> startValue; // Exp_t | default_value_t
	ast_ptr<ast_node> stopValue; // Exp_t | default_value_t
	ast_ptr<ast_node> stepValue; // Exp_t | default_value_t
AST_END(Slice)

AST_NODE(Invoke)
	ast_ptr<ast_node> argument; // FnArgs_t | SingleString_t | DoubleString_t | LuaString_t
AST_END(Invoke)

class KeyValue_t;

AST_NODE(TableValue)
	ast_ptr<ast_node> value; // KeyValue_t | Exp_t
AST_END(TableValue)

AST_NODE(TableLit)
	ast_ptr<Seperator_t> sep;
	ast_list<TableValue_t> values;
AST_END(TableLit)

AST_NODE(TableBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> values;
AST_END(TableBlock)

AST_NODE(class_member_list)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> values;
AST_END(class_member_list)

AST_NODE(ClassLine)
	ast_ptr<ast_node> content; // class_member_list_t | Statement_t | Exp_t
AST_END(ClassLine)

AST_NODE(ClassBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<ClassLine_t> lines;
AST_END(ClassBlock)

AST_NODE(ClassDecl)
	ast_ptr<Assignable_t, true> name;
	ast_ptr<Exp_t, true> extend;
	ast_ptr<ClassBlock_t, true> body;
AST_END(ClassDecl)

AST_NODE(export_values)
	ast_ptr<NameList_t> nameList;
	ast_ptr<ExpListLow_t, true> valueList;
AST_END(export_values)

AST_LEAF(export_op)
AST_END(export_op)

AST_NODE(Export)
	ast_ptr<ast_node> item; // ClassDecl_t | export_op_t | export_values_t
AST_END(Export)

AST_NODE(variable_pair)
	ast_ptr<Name_t> name;
AST_END(variable_pair)

AST_NODE(normal_pair)
	ast_ptr<ast_node> key; // KeyName_t | [Exp_t] | DoubleString_t | SingleString_t
	ast_ptr<ast_node> value; // Exp_t | TableBlock_t
AST_END(normal_pair)

AST_NODE(KeyValue)
	ast_ptr<ast_node> item; // variable_pair_t | normal_pair_t
AST_END(KeyValue)

AST_NODE(FnArgDef)
	ast_ptr<ast_node> name; // Name_t | SelfName_t
	ast_ptr<Exp_t, true> defaultValue;
AST_END(FnArgDef)

AST_NODE(FnArgDefList)
	ast_ptr<Seperator_t> sep;
	ast_list<FnArgDef_t> definitions;
	ast_ptr<VarArg_t, true> varArg;
AST_END(FnArgDefList)

AST_NODE(outer_var_shadow)
	ast_ptr<NameList_t, true> varList;
AST_END(outer_var_shadow)

AST_NODE(FnArgsDef)
	ast_ptr<FnArgDefList_t, true> defList;
	ast_ptr<outer_var_shadow_t, true> shadowOption;
AST_END(FnArgsDef)

AST_LEAF(fn_arrow)
AST_END(fn_arrow)

AST_NODE(FunLit)
	ast_ptr<FnArgsDef_t, true> argsDef;
	ast_ptr<fn_arrow_t> arrow;
	ast_ptr<Body_t, true> body;
AST_END(FunLit)

AST_NODE(NameOrDestructure)
	ast_ptr<ast_node> item; // Name_t | TableLit_t
AST_END(NameOrDestructure)

AST_NODE(AssignableNameList)
	ast_ptr<Seperator_t> sep;
	ast_list<NameOrDestructure_t> items;
AST_END(AssignableNameList)

AST_NODE(ArgBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> arguments;
AST_END(ArgBlock)

AST_NODE(invoke_args_with_table)
	ast_ptr<ArgBlock_t, true> argBlock;
	ast_ptr<TableBlock_t, true> tableBlock;
AST_END(invoke_args_with_table)

AST_NODE(InvokeArgs)
	ast_ptr<ExpList_t, true> argsList;
	ast_ptr<invoke_args_with_table_t, true> argsTableBlock;
	ast_ptr<TableBlock_t, true> tableBlock;
AST_END(InvokeArgs)

AST_LEAF(const_value)
AST_END(const_value)

AST_NODE(unary_exp)
	ast_ptr<Exp_t> item;
AST_END(unary_exp)

AST_NODE(Assignment)
	ast_ptr<ExpList_t> assignable;
	ast_ptr<ast_node> target; // Update_t | Assign_t
AST_END(Assignment)

AST_NODE(if_else_line)
	ast_ptr<Exp_t> condition;
	ast_ptr<ast_node> elseExpr; // Exp_t | default_value_t
AST_END(if_else_line)

AST_NODE(unless_line)
	ast_ptr<Exp_t> condition;
AST_END(unless_line)

AST_NODE(statement_appendix)
	ast_ptr<ast_node> item; // if_else_line_t | unless_line_t | CompInner_t
AST_END(statement_appendix)

AST_LEAF(BreakLoop)
AST_END(BreakLoop)

AST_NODE(Statement)
	ast_ptr<ast_node> content; /*
	Import_t | While_t | With_t | For_t | ForEach_t |
	Switch_t | Return_t | Local_t | Export_t | BreakLoop_t |
	Assignment_t | ExpList_t
	*/
	ast_ptr<statement_appendix_t, true> appendix;

	virtual void construct(ast_stack& st) override
	{
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
		value = conv.to_bytes(&*m_begin.m_it, &*m_end.m_it);
		ast_container::construct(st);
	}

	virtual void visit(void* ud) override
	{
		std::cout << value << '\n';
	}
	std::string value;
AST_END(Statement)

class Block_t;

AST_NODE(Body)
	ast_ptr<ast_node> content; // Block | Statement
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->pushScope();
		content->visit(ud);
		data->popScope();
	}
AST_END(Body)

AST_NODE(Line)
	ast_ptr<Statement_t, true> statment;
	int line;

	virtual void construct(ast_stack& st) override
	{
		ast_container::construct(st);
		line = m_begin.m_line;
	}

	virtual void visit(void* ud) override
	{
		if (statment)
		{
			std::cout << line << ": ";
			statment->visit(ud);
		}
	}
AST_END(Line)

AST_NODE(Block)
	ast_ptr<Seperator_t> sep;
	ast_list<Line_t> lines;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		const auto& objs = lines.objects();
		for (auto it = objs.begin(); it != objs.end(); ++it)
		{
			Line_t* line = *it;
			if (!line->statment) continue;
			if (Local_t* local = ast_cast<Local_t>(line->statment->content))
			{
				if (local_flag_t* local_flag = ast_cast<local_flag_t>(local->name))
				{
					std::vector<const std::string*> names;
					for (auto cur = it; cur != objs.end(); ++cur)
					{
						Line_t* item = *cur;
						if (!item->statment) continue;
						if (Assignment_t* assignment = ast_cast<Assignment_t>(item->statment->content))
						{
							for (Exp_t* expr : assignment->assignable->exprs.objects())
							{
								if (ChainValue_t* chainValue = ast_cast<ChainValue_t>(expr->value->item))
								if (Callable_t* callable = ast_cast<Callable_t>(chainValue->caller))
								if (Name_t* name = ast_cast<Name_t>(callable->item))
								{
									const std::string& value = name->name->getValue();
									if (local_flag->getValue() == "*")
									{
										names.push_back(&value);
									}
									else if (std::isupper(value[0]))
									{
										names.push_back(&value);
									}
								}
							}
						}
					}
					if (!names.empty())
					{
						data->beginLine();
						data->buffer << "local ";
						auto nameIt = names.begin();
						auto name = *(*nameIt);
						data->putLocal(name);
						data->buffer << name;
						nameIt++;
						for (; nameIt != names.end(); ++nameIt)
						{
							auto name = *(*nameIt);
							data->putLocal(name);
							data->buffer << ", ";
							data->buffer << name;
						}
						data->endLine(line->m_begin.m_line);
					}
				}
				else
				{
					NameList_t* nameList = static_cast<NameList_t*>(local->name.get());
					data->beginLine();
					data->buffer << "local ";
					nameList->visit(ud);
					for (Name_t* name : nameList->names.objects())
					{
						data->putLocal(name->name->getValue());
					}
					data->endLine(line->m_begin.m_line);
				}
			}
			else
			{
				line->visit(ud);
			}
		}
	}
AST_END(Block)

AST_NODE(BlockEnd)
	ast_ptr<Block_t> block;
	virtual void visit(void* ud) override
	{
		Data* data = static_cast<Data*>(ud);
		data->pushScope();
		block->visit(ud);
		data->popScope();
	}
AST_END(BlockEnd)

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
