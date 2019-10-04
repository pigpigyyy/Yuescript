#include "moon_parser.h"

std::unordered_set<std::string> State::keywords = {
	"and", "while", "else", "using", "continue",
	"local", "not", "then", "return", "from",
	"extends", "for", "do", "or", "export",
	"class", "in", "unless", "when", "elseif",
	"switch", "break", "if", "with", "import", "true", "false", "nil"
};

rule plain_space = *set(" \t");
rule Break = nl(-expr('\r') >> '\n');
rule Any = Break | any();
rule White = *(set(" \t") | Break);
rule Stop = Break | eof();
rule Comment = "--" >> *(not_(set("\r\n")) >> Any) >> and_(Stop);
rule Indent = *set(" \t");
rule Space = plain_space >> -Comment;
rule SomeSpace = +set(" \t") >> -Comment;
rule SpaceBreak = Space >> Break;
rule EmptyLine = SpaceBreak;
rule AlphaNum = range('a', 'z') | range('A', 'Z') | range('0', '9') | '_';
rule Name = (range('a', 'z') | range('A', 'Z') | '_') >> *AlphaNum;
rule Num =
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
rule Cut = false_();
rule Seperator = true_();

#define sym(str) (Space >> str)
#define symx(str) expr(str)
#define ensure(patt, finally) (((patt) >> (finally)) | ((finally) >> (Cut)))
#define key(str) (Space >> str >> not_(AlphaNum))

rule Variable = user(Name, [](const item_t& item) {
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
rule self_name = '@' >> Name;
rule self_class = expr("@@");
rule self_class_name = "@@" >> Name;

rule SelfName = Space >> (self_class_name | self_class | self_name | self);
rule KeyName = SelfName | Space >> Name;
rule VarArg = Space >> "...";

rule check_indent = user(Indent, [](const item_t& item) {
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i) {
		switch (*i) {
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	return st->indents.top() == indent;
});
rule CheckIndent = and_(check_indent);

rule advance = user(Indent, [](const item_t& item) {
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i) {
		switch (*i) {
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	int top = st->indents.top();
	if (top != -1 && indent > top) {
		st->indents.push(indent);
		return true;
	}
	return false;
});
rule Advance = and_(advance);

rule push_indent = user(Indent, [](const item_t& item) {
	int indent = 0;
	for (input_it i = item.begin; i != item.end; ++i) {
		switch (*i) {
			case ' ': indent++; break;
			case '\t': indent += 4; break;
		}
	}
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.push(indent);
	return true;
});
rule PushIndent = and_(push_indent);

rule PreventIndent = user(true_(), [](const item_t& item) {
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.push(-1);
	return true;
});

rule PopIndent = user(true_(), [](const item_t& item) {
	State* st = reinterpret_cast<State*>(item.user_data);
	st->indents.pop();
	return true;
});

extern rule Block;

rule InBlock = Advance >> Block >> PopIndent;

extern rule NameList;

rule local_flag = expr('*') | expr('^');
rule Local = key("local") >> ((Space >> local_flag) | NameList);

rule colon_import_name = sym('\\') >> Space >> Variable;
rule ImportName = colon_import_name | Space >> Variable;
rule ImportNameList = Seperator >> *SpaceBreak >> ImportName >> *((+SpaceBreak | sym(',') >> *SpaceBreak) >> ImportName);

extern rule Exp;

rule Import = key("import") >> ImportNameList >> *SpaceBreak >> key("from") >> Exp;
rule BreakLoop = (expr("break") | expr("continue")) >> not_(AlphaNum);

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
rule If = key("if") >> Seperator >> IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;
rule Unless = key("unless") >> Seperator >>  IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;

rule While = key("while") >> DisableDo >> ensure(Exp, PopDo) >> -key("do") >> Body;

rule for_step_value = sym(',') >> Exp;
rule for_args = Space >> Variable >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;

rule For = key("for") >> DisableDo >>
	ensure(for_args, PopDo) >>
	-key("do") >> Body;

extern rule AssignableNameList;

extern rule star_exp;

rule for_in = star_exp | ExpList;

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
rule TblComprehension = sym('{') >> (Exp >> -comp_value)   >> CompInner >> sym('}');

extern rule CompForEach, CompFor, CompClause;

rule CompInner = Seperator >> (CompForEach | CompFor) >> *CompClause;
rule star_exp = sym('*') >> Exp;
rule CompForEach = key("for") >> AssignableNameList >> key("in") >> (star_exp | Exp);
rule CompFor = key("for") >> Space >> Variable >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;
rule CompClause = CompFor | CompForEach | key("when") >> Exp;

extern rule TableBlock;

rule Assign = sym('=') >> Seperator >> (With | If | Switch | TableBlock | Exp >> *((sym(',') | sym(';')) >> Exp));

rule update_op =
	expr("..") |
	expr("+") |
	expr("-") |
	expr("*") |
	expr("/") |
	expr("%") |
	expr("or") |
	expr("and") |
	expr("&") |
	expr("|") |
	expr(">>") |
	expr("<<");

rule Update = Space >> update_op >> expr("=") >> Exp;

rule BinaryOperator =
	(expr("or") >> not_(AlphaNum)) |
	(expr("and") >> not_(AlphaNum)) |
	expr("<=") |
	expr(">=") |
	expr("~=") |
	expr("!=") |
	expr("==") |
	expr("..") |
	expr("<<") |
	expr(">>") |
	expr("//") |
	set("+-*/%^><|&");

extern rule Chain;

rule Assignable = Chain | Space >> Variable | SelfName;

extern rule Value;

rule exp_op_value = Space >> BinaryOperator >> *SpaceBreak >> Value;
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
rule Callable = Space >> Variable | SelfName | VarArg | Parens;
rule FnArgsExpList = Exp >> *((Break | sym(',')) >> White >> Exp);

rule FnArgs = (symx('(') >> *SpaceBreak >> -FnArgsExpList >> *SpaceBreak >> sym(')')) |
	(sym('!') >> not_(expr('=')));

extern rule ChainItems, DotChainItem, ColonChain;

rule chain_call = (Callable | String) >> ChainItems;
rule chain_item = and_(set(".\\")) >> ChainItems;
rule chain_dot_chain = DotChainItem >> -ChainItems;

rule Chain = Seperator >> (
	chain_call |
	chain_item |
	Space >> (chain_dot_chain | ColonChain));

extern rule ChainItem;

rule chain_with_colon = +ChainItem >> -ColonChain;
rule ChainItems = chain_with_colon | ColonChain;

extern rule Invoke, Slice;

rule Index = symx('[') >> Exp >> sym(']');
rule ChainItem = Invoke | DotChainItem | Slice | Index;
rule DotChainItem = symx('.') >> Name;
rule ColonChainItem = symx('\\') >> Name;
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

rule Invoke = Seperator >> (
	FnArgs |
	SingleString |
	DoubleString |
	and_(expr('[')) >> LuaString);

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
rule ClassLine = CheckIndent >> (class_member_list | Statement) >> -sym(',');
rule ClassBlock = +(SpaceBreak) >> Advance >>Seperator >> ClassLine >> *(+(SpaceBreak) >> ClassLine) >> PopIndent;

rule ClassDecl =
	key("class") >> not_(expr(':')) >>
	-Assignable >>
	-(key("extends")  >> PreventIndent >> ensure(Exp, PopIndent)) >>
	-ClassBlock;

rule export_values = NameList >> -(sym('=') >> ExpListLow);
rule export_op = expr('*') | expr('^');
rule Export = key("export") >> (ClassDecl | (Space >> export_op) | export_values);

rule variable_pair = sym(':') >> not_(SomeSpace) >> Space >> Variable;

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

rule FnArgDef = (Space >> Variable | SelfName) >> -(sym('=') >> Exp);

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
rule fn_arrow = expr("->") | expr("=>");
rule FunLit = -FnArgsDef >> Space >> fn_arrow >> -Body;

rule NameList = Seperator >> Space >> Variable >> *(sym(',') >> Space >> Variable);
rule NameOrDestructure = Space >> Variable | TableLit;
rule AssignableNameList = Seperator >> NameOrDestructure >> *(sym(',') >> NameOrDestructure);

rule ExpList = Seperator >> Exp >> *(sym(',') >> Exp);
rule ExpListLow = Seperator >> Exp >> *((sym(',') | sym(';')) >> Exp);

rule ArgLine = CheckIndent >> Exp >> *(sym(',') >> Exp);
rule ArgBlock = ArgLine >> *(sym(',') >> SpaceBreak >> ArgLine) >> PopIndent;

rule invoke_args_with_table =
	sym(',') >>
	(
		TableBlock |
		SpaceBreak >> Advance >> ArgBlock >> -TableBlock
	);

rule InvokeArgs =
	not_(expr('-')) >> Seperator >> 
	(
		Exp >> *(sym(',') >> Exp) >> -(invoke_args_with_table | TableBlock) |
		TableBlock
	);

rule const_value = (expr("nil") | expr("true") | expr("false")) >> not_(AlphaNum);
rule minus_exp = expr('-') >> not_(SomeSpace) >> Exp;
rule sharp_exp = expr('#') >> Exp;
rule tilde_exp = expr('~') >> Exp;
rule not_exp = expr("not") >> not_(AlphaNum) >> Exp;
rule unary_exp = minus_exp | sharp_exp | tilde_exp | not_exp;

rule SimpleValue =
	(Space >> const_value) |
	If | Unless | Switch | With | ClassDecl | ForEach | For | While | Do |
	(Space >> unary_exp) |
	TblComprehension | TableLit | Comprehension | FunLit |
	(Space >> Num);

rule Assignment = ExpList >> (Update | Assign);

rule if_else_line = key("if") >> Exp >> (key("else") >> Exp | default_value);
rule unless_line = key("unless") >> Exp;

rule statement_appendix = (if_else_line | unless_line | CompInner) >> Space;
rule Statement =
(
	Import | While | With | For | ForEach |
	Return | Local | Export | Space >> BreakLoop |
	Assignment | ExpList
) >> Space >>
-statement_appendix;

rule Body = -Space >> Break >> *EmptyLine >> InBlock | Statement;

rule empty_line_stop = Space >> and_(Stop);
rule Line = CheckIndent >> Statement | empty_line_stop;
rule Block = Seperator >> Line >> *(+Break >> Line);
rule BlockEnd = Block >> eof();
