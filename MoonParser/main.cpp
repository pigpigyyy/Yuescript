#include <iostream>
using std::cout;
#include <string>
using std::string;
#include <unordered_set>
using std::unordered_set;
#include <stack>
using std::stack;
#include <algorithm>
#include <sstream>
using std::stringstream;
#include "parserlib.hpp"
using namespace parserlib;

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
	size_t stringOpen;
	stack<int> indents;
	stack<bool> doStack;
	unordered_set<string> keywords = {
		"and", "while", "else", "using", "continue",
		"local", "not", "then", "return", "from",
		"extends", "for", "do", "or", "export",
		"class", "in", "unless", "when", "elseif",
		"switch", "break", "if", "with", "import"
	};
};

rule Any = any();
rule White = *set(" \t\r\n");
rule plain_space = *set(" \t");
rule Break = nl(-expr('\r') >> '\n');
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
rule Nothing = true_();

#define sym(str) (Space >> str)
#define symx(str) expr(str)
#define ensure(patt, finally) (((patt) >> (finally)) | ((finally) >> (Cut)))
#define key(str) (Space >> str >> not_(AlphaNum))
#define opWord(str) (Space >> str >> not_(AlphaNum))
#define op(str) (Space >> str)

rule Name = user(SpaceName, [](const item_t& item)
{
	stringstream stream;
	for (input_it i = item.begin; i != item.end; ++i) stream << static_cast<char>(*i);
	string name;
	stream >> name;
	State* st = reinterpret_cast<State*>(item.user_data);
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

rule Local = key("local") >> (op('*') | op('^') | NameList);

rule colon_import_name = sym('\\') >> Name;
rule ImportName = colon_import_name | Name;
rule ImportNameList = *SpaceBreak >> ImportName >> *((+SpaceBreak | sym(',') >> *SpaceBreak) >> ImportName);

extern rule Exp;

rule Import = key("import") >> ImportNameList >> *SpaceBreak >> key("from") >> Exp;
rule BreakLoop = key("break") | key("continue");

extern rule ExpListLow, ExpList, Assign;

rule Return = key("return") >> (ExpListLow | Nothing);
rule WithExp = ExpList >> -Assign;

extern rule DisableDo, PopDo, Body;

rule With = key("with") >> DisableDo >> ensure(WithExp, PopDo) >> -key("do") >> Body;
rule SwitchCase = key("when") >> ExpList >> -key("then") >> Body;
rule SwitchElse = key("else") >> Body;

rule SwitchBlock = *EmptyLine >>
	Advance >>
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
rule If = key("if") >> IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;
rule Unless = key("unless") >> IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;

rule While = key("while") >> DisableDo >> ensure(Exp, PopDo) >> -key("do") >> Body;

rule for_args = Name >> sym('=') >> Exp >> sym(',') >> Exp >> (sym(',') >> Exp | Nothing);

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
rule TblComprehension = sym('{') >> (Exp >> -(sym(',') >> Exp)) >> CompInner >> sym('}');

extern rule CompForEach, CompFor, CompClause;

rule CompInner = (CompForEach | CompFor) >> *CompClause;
rule CompForEach = key("for") >> AssignableNameList >> key("in") >> (sym('*') >> Exp | Exp);
rule CompFor = key("for") >> Name >> sym('=') >> Exp >> sym(',') >> Exp >> (sym(',') >> Exp | Nothing);
rule CompClause = CompFor | CompForEach | key("when") >> Exp;

extern rule TableBlock;

rule Assign = sym('=') >> (With | If | Switch | TableBlock | ExpListLow);

rule Update =
(
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
	sym("<<=")
) >> Exp;

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

rule Exp = Value >> *(BinaryOperator >> Value);

extern rule Callable, InvokeArgs;

rule ChainValue = (Chain | Callable) >> (InvokeArgs | Nothing);

extern rule KeyValueList, String, SimpleValue;

rule Value = SimpleValue | KeyValueList | ChainValue | String;
rule SliceValue = Exp;

extern rule LuaString;

rule single_string_inner = expr("\\'") | "\\\\" | not_(expr('\'')) >> Any;
rule SingleString = symx('\'') >> *single_string_inner >> sym('\'');
rule interp = symx("#{") >> Exp >> sym('}');
rule double_string_plain = expr("\\\"") | "\\\\" | not_(expr('"')) >> Any;
rule double_string_inner = +(not_(interp) >> double_string_plain);
rule DoubleString = symx('"') >> *(double_string_inner | interp) >> sym('"');
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

rule LuaStringContent = *(not_(LuaStringClose) >> Any);

rule LuaString = user(LuaStringOpen >> -Break >> LuaStringContent >> LuaStringClose, [](const item_t& item)
{
	State* st = reinterpret_cast<State*>(item.user_data);
	st->stringOpen = -1;
	return true;
});

rule Parens = sym('(') >> *SpaceBreak >> Exp >> *SpaceBreak >> sym(')');
rule Callable = Name | SelfName | VarArg | Parens;
rule FnArgsExpList = Exp >> *((Break | sym(',')) >> White >> Exp);

rule FnArgs =
(
	symx('(') >> *SpaceBreak >> (FnArgsExpList | Nothing) >> *SpaceBreak >> sym(')')
) | (
	sym('!') >> not_(expr('=')) >> Nothing
);

extern rule ChainItems, DotChainItem, ColonChain;

rule chain_call = (Callable | String) >> ChainItems;
rule chain_item = not_(set(".\\")) >> ChainItems;
rule chain_dot_chain = DotChainItem >> (ChainItems | Nothing);

rule Chain =
	chain_call |
	chain_item |
	Space >> (chain_dot_chain | ColonChain);

extern rule ChainItem;

rule chain_items = +ChainItem >> (ColonChain | Nothing);
rule ChainItems = chain_items | ColonChain;

extern rule Invoke, Slice;

rule ChainItem = Invoke | DotChainItem | Slice | symx('[') >> Exp >> sym(']');
rule DotChainItem = symx('.') >> _Name;
rule ColonChainItem = symx('\\') >> _Name;
rule ColonChain = ColonChainItem >> ((Invoke >> (ChainItems | Nothing)) | Nothing >> Nothing);

rule Slice =
	symx('[') >>
	(SliceValue | Nothing) >>
	sym(',') >>
	(SliceValue | Nothing) >>
	(sym(',') >>
	SliceValue | Nothing) >>
	sym(']');

rule Invoke =
	FnArgs |
	SingleString |
	DoubleString |
	and_(expr('[')) >> LuaString;

extern rule KeyValue, TableValueList, TableLitLine;

rule TableValue = KeyValue | Exp;

rule table_lit_lines = SpaceBreak >> TableLitLine >> *(-sym(',') >> SpaceBreak >> TableLitLine) >> -sym(',');

rule TableLit =
	sym('{') >>
	(TableValueList | Nothing) >>
	-sym(',') >>
	(table_lit_lines | Nothing) >>
	White >> sym('}');

rule TableValueList = TableValue >> *(sym(',') >> TableValue);

rule TableLitLine =
(
	PushIndent >> (TableValueList >> PopIndent | PopIndent)
) | (
	Space >> Nothing
);

extern rule KeyValueLine;

rule TableBlockInner = KeyValueLine >> *(+(SpaceBreak) >> KeyValueLine);
rule TableBlock = +(SpaceBreak) >> Advance >> ensure(TableBlockInner, PopIndent);

extern rule Statement;

rule ClassLine = CheckIndent >> (KeyValueList | Statement | Exp) >> -sym(',');
rule ClassBlock = +(SpaceBreak) >> Advance >> ClassLine >> *(+(SpaceBreak) >> ClassLine) >> PopIndent;

rule ClassDecl =
	key("class") >> not_(expr(':')) >>
	(Assignable | Nothing) >>
	(key("extends")  >> PreventIndent >> ensure(Exp, PopIndent) | Nothing) >>
	(ClassBlock | Nothing);

rule export_values = sym('=') >> ExpListLow;
rule Export = key("export") >> (ClassDecl | op('*') | op('^') | NameList >> (export_values | Nothing));

rule KeyValue =
(
	sym(':') >> not_(SomeSpace) >> Name
) | (
	(KeyName |
		sym('[') >> Exp >> sym(']') |
		Space >> DoubleString |
		Space >> SingleString
	) >>
	symx(':') >>
	(Exp | TableBlock | +(SpaceBreak) >> Exp)
);

rule KeyValueList = KeyValue >> *(sym(',') >> KeyValue);
rule KeyValueLine = CheckIndent >> KeyValueList >> -sym(',');

rule FnArgDef = (Name | SelfName) >> (sym('=') >> Exp | Nothing);

rule FnArgDefList =
(
	FnArgDef >>
	*((sym(',') | Break) >> White >> FnArgDef) >>
	((sym(',') | Break) >> White >> VarArg | Nothing)
) | (
	VarArg
);

rule outer_value_shadow = key("using") >> (NameList | Space >> expr("nil"));

rule normal_fn_args_def =
	sym('(') >> White >> (FnArgDefList | Nothing) >> (outer_value_shadow | Nothing) >> White >> sym(')');

rule FnArgsDef = normal_fn_args_def | Nothing;
rule fn_arrow = sym("->");
rule fat_arrow = sym("=>");
rule FunLit = FnArgsDef >> (fn_arrow | fat_arrow) >> (Body | Nothing);

rule NameList = Name >> *(sym(',') >> Name);
rule NameOrDestructure = Name | TableLit;
rule AssignableNameList = NameOrDestructure >> *(sym(',') >> NameOrDestructure);

rule ExpList = Exp >> *(sym(',') >> Exp);
rule ExpListLow = Exp >> *((sym(',') | sym(';')) >> Exp);

rule ArgLine = CheckIndent >> ExpList;
rule ArgBlock = ArgLine >> *(sym(',') >> SpaceBreak >> ArgLine) >> PopIndent;

rule invoke_args_with_table =
	sym(',') >>
	(
		TableBlock >> Nothing |
		SpaceBreak>> Advance >> ArgBlock >> (TableBlock | Nothing)
	);

rule InvokeArgs =
	not_(expr('-')) >>
	(
		ExpList >> (invoke_args_with_table | TableBlock | Nothing) |
		TableBlock >> Nothing
	);

rule SimpleValue =
	key("nil") | key("true") | key("false") |
	If | Unless | Switch | With | ClassDecl | ForEach | For | While | Do |
	sym('-') >> not_(SomeSpace) >> Exp |
	sym('#') >> Exp |
	sym('~') >> Exp |
	key("not") >> Exp |
	TblComprehension | TableLit | Comprehension | FunLit | Num;

rule Assignment = ExpList >> (Update | Assign);

rule if_else_line = key("if") >> Exp >> (key("else") >> Exp | Nothing);
rule unless_line = key("unless") >> Exp;

rule Statement =
(
	Import | While | With | For | ForEach |
	Switch | Return | Local | Export | BreakLoop |
	Assignment | ExpList
) >> Space >>
(
	(if_else_line | unless_line | CompInner) >> Space | Nothing
);

rule Body = -Space >> Break >> *EmptyLine >> InBlock | Statement;

rule empty_line_stop = Space >> and_(Stop);
rule Line = CheckIndent >> Statement | empty_line_stop;
rule Block = Line >> *(+Break >> Line);
rule BlockWithEnd = Block >> eof();

class AstNode : public ast_container
{
public:
	virtual void construct(ast_stack& st)
	{
		stringstream stream;
		for (input::iterator it = m_begin.m_it; it != m_end.m_it; ++it)
		{
			stream << (char)*it;
		}
		_value = stream.str();
	}
	void print()
	{
		cout << _value << '\n';
	}
private:
	string _value;
};

rule ExprEnd = Block >> eof();

ast<AstNode> testNode(ExprEnd);

int main()
{
	string s = R"baddog()baddog";
	input i(s.begin(), s.end());

	error_list el;
	AstNode* root = nullptr;
	State st;
	if (parse(i, ExprEnd, el, root, &st))
	{
		cout << "matched!\n";
	}
	else
	{
		cout << "not matched!\n";
		for (error_list::iterator it = el.begin(); it != el.end(); ++it)
		{
			const error& err = *it;
			cout << "line " << err.m_begin.m_line << ", col " << err.m_begin.m_col << ": syntax error\n";
		}
	}
	system("pause");
	return 0;
}
