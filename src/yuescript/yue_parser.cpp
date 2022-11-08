/* Copyright (c) 2022 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "yuescript/yue_parser.h"

namespace pl = parserlib;

namespace yue {

std::unordered_set<std::string> LuaKeywords = {
	"and"s, "break"s, "do"s, "else"s, "elseif"s,
	"end"s, "false"s, "for"s, "function"s, "goto"s,
	"if"s, "in"s, "local"s, "nil"s, "not"s,
	"or"s, "repeat"s, "return"s, "then"s, "true"s,
	"until"s, "while"s};

std::unordered_set<std::string> Keywords = {
	"and"s, "break"s, "do"s, "else"s, "elseif"s,
	"end"s, "false"s, "for"s, "function"s, "goto"s,
	"if"s, "in"s, "local"s, "nil"s, "not"s,
	"or"s, "repeat"s, "return"s, "then"s, "true"s,
	"until"s, "while"s, // Lua keywords
	"as"s, "class"s, "continue"s, "export"s, "extends"s,
	"from"s, "global"s, "import"s, "macro"s, "switch"s,
	"try"s, "unless"s, "using"s, "when"s, "with"s // Yue keywords
};

// clang-format off
YueParser::YueParser() {
	plain_space = *set(" \t");
	Break = nl(-expr('\r') >> '\n');
	Any = Break | any();
	Stop = Break | eof();
	Indent = plain_space;
	Comment = "--" >> *(not_(set("\r\n")) >> Any) >> and_(Stop);
	multi_line_open = expr("--[[");
	multi_line_close = expr("]]");
	multi_line_content = *(not_(multi_line_close) >> Any);
	MultiLineComment = multi_line_open >> multi_line_content >> multi_line_close;
	EscapeNewLine = expr('\\') >> *(set(" \t") | MultiLineComment) >> -Comment >> Break;
	space_one = set(" \t") | and_(set("-\\")) >> (MultiLineComment | EscapeNewLine);
	Space = *space_one >> -Comment;
	SpaceBreak = Space >> Break;
	White = Space >> *(Break >> Space);
	EmptyLine = SpaceBreak;
	AlphaNum = range('a', 'z') | range('A', 'Z') | range('0', '9') | '_';
	Name = (range('a', 'z') | range('A', 'Z') | '_') >> *AlphaNum;
	num_expo = set("eE") >> -expr('-') >> num_char;
	lj_num = -set("uU") >> set("lL") >> set("lL");
	num_char = range('0', '9') >> *(range('0', '9') | expr('_') >> and_(range('0', '9')));
	num_char_hex = range('0', '9') | range('a', 'f') | range('A', 'F');
	num_lit = num_char_hex >> *(num_char_hex | expr('_') >> and_(num_char_hex));
	Num = (
		"0x" >> +num_lit >> -lj_num
	) | (
		+num_char >> (
			'.' >> +num_char >> -num_expo |
			num_expo |
			lj_num |
			true_()
		)
	) | (
		'.' >> +num_char >> -num_expo
	);

	Cut = false_();
	Seperator = true_();

	empty_block_error = pl::user(true_(), [](const item_t& item) {
		throw ParserError("must be followed by a statement or an indented block", *item.begin, *item.end);
		return false;
	});

	#define sym(str) (Space >> str)
	#define symx(str) expr(str)
	#define ensure(patt, finally) ((patt) >> (finally) | (finally) >> Cut)
	#define key(str) (str >> not_(AlphaNum))
	#define disable_do(patt) (DisableDo >> ((patt) >> EnableDo | EnableDo >> Cut))
	#define disable_chain(patt) (DisableChain >> ((patt) >> EnableChain | EnableChain >> Cut))
	#define disable_do_chain_arg_table_block(patt) (DisableDoChainArgTableBlock >> ((patt) >> EnableDoChainArgTableBlock | EnableDoChainArgTableBlock >> Cut))
	#define disable_arg_table_block(patt) (DisableArgTableBlock >> ((patt) >> EnableArgTableBlock | EnableArgTableBlock >> Cut))
	#define body_with(str) (Space >> (key(str) >> Space >> (InBlock | Statement) | InBlock | empty_block_error))
	#define opt_body_with(str) (Space >> (key(str) >> Space >> (InBlock | Statement) | InBlock))
	#define body (Space >> (InBlock | Statement | empty_block_error))

	Variable = pl::user(Name, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		for (auto it = item.begin->m_it; it != item.end->m_it; ++it) st->buffer += static_cast<char>(*it);
		auto isValid = Keywords.find(st->buffer) == Keywords.end();
		if (isValid) {
			if (st->buffer == st->moduleName) {
				st->moduleFix++;
				st->moduleName = "_module_"s + std::to_string(st->moduleFix);
			}
		}
		st->buffer.clear();
		return isValid;
	});

	LabelName = pl::user(Name, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		for (auto it = item.begin->m_it; it != item.end->m_it; ++it) st->buffer += static_cast<char>(*it);
		auto isValid = LuaKeywords.find(st->buffer) == LuaKeywords.end();
		st->buffer.clear();
		return isValid;
	});

	LuaKeyword = pl::user(Name, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		for (auto it = item.begin->m_it; it != item.end->m_it; ++it) st->buffer += static_cast<char>(*it);
		auto it = LuaKeywords.find(st->buffer);
		st->buffer.clear();
		return it != LuaKeywords.end();
	});

	self = expr('@');
	self_name = '@' >> Name;
	self_class = expr("@@");
	self_class_name = "@@" >> Name;

	SelfName = self_class_name | self_class | self_name | self;
	KeyName = Space >> (SelfName | Name);
	VarArg = expr("...");

	check_indent = pl::user(Indent, [](const item_t& item) {
		int indent = 0;
		for (input_it i = item.begin->m_it; i != item.end->m_it; ++i) {
			switch (*i) {
				case ' ': indent++; break;
				case '\t': indent += 4; break;
			}
		}
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->indents.top() == indent;
	});
	CheckIndent = and_(check_indent);

	advance = pl::user(Indent, [](const item_t& item) {
		int indent = 0;
		for (input_it i = item.begin->m_it; i != item.end->m_it; ++i) {
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
	Advance = and_(advance);

	push_indent = pl::user(Indent, [](const item_t& item) {
		int indent = 0;
		for (input_it i = item.begin->m_it; i != item.end->m_it; ++i) {
			switch (*i) {
				case ' ': indent++; break;
				case '\t': indent += 4; break;
			}
		}
		State* st = reinterpret_cast<State*>(item.user_data);
		st->indents.push(indent);
		return true;
	});
	PushIndent = and_(push_indent);

	PreventIndent = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->indents.push(-1);
		return true;
	});

	PopIndent = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->indents.pop();
		return true;
	});

	InBlock = Space >> +(plain_space >> Break) >> Advance >> ensure(Block, PopIndent);

	local_flag = expr('*') | expr('^');
	local_values = NameList >> -(sym('=') >> (TableBlock | ExpListLow));
	Local = key("local") >> (Space >> local_flag | local_values);

	const_attrib = key("const");
	close_attrib = key("close");
	local_const_item = (Space >> Variable | simple_table | TableLit);
	LocalAttrib = (
		const_attrib >> Seperator >> local_const_item >> *(sym(',') >> local_const_item) |
		close_attrib >> Seperator >> Space >> Variable >> *(sym(',') >> Space >> Variable)
	) >> Assign;

	colon_import_name = sym('\\') >> Space >> Variable;
	ImportName = colon_import_name | Space >> Variable;
	ImportNameList = Seperator >> *SpaceBreak >> ImportName >> *((+SpaceBreak | sym(',') >> *SpaceBreak) >> ImportName);
	ImportFrom = ImportNameList >> *SpaceBreak >> Space >> key("from") >> Exp;

	import_literal_inner = (range('a', 'z') | range('A', 'Z') | set("_-")) >> *(AlphaNum | '-');
	import_literal_chain = Seperator >> import_literal_inner >> *(expr('.') >> import_literal_inner);
	ImportLiteral = sym('\'') >> import_literal_chain >> symx('\'') | sym('"') >> import_literal_chain >> symx('"');

	macro_name_pair = Space >> MacroName >> Space >> symx(':') >> Space >> MacroName;
	import_all_macro = expr('$');
	ImportTabItem = variable_pair | normal_pair | sym(':') >> MacroName | macro_name_pair | Space >> import_all_macro | meta_variable_pair | meta_normal_pair | Exp;
	ImportTabList = ImportTabItem >> *(sym(',') >> ImportTabItem);
	ImportTabLine = (
		PushIndent >> (ImportTabList >> PopIndent | PopIndent)
	) | Space;
	import_tab_lines = SpaceBreak >> ImportTabLine >> *(-sym(',') >> SpaceBreak >> ImportTabLine) >> -sym(',');
	ImportTabLit =
		Seperator >> (sym('{') >>
		-ImportTabList >>
		-sym(',') >>
		-import_tab_lines >>
		White >> sym('}') | KeyValue >> *(sym(',') >> KeyValue));

	ImportAs = ImportLiteral >> -(Space >> key("as") >> Space >> (ImportTabLit | Variable | import_all_macro));

	Import = key("import") >> (ImportAs | ImportFrom);

	Label = expr("::") >> LabelName >> expr("::");

	Goto = key("goto") >> Space >> LabelName;

	ShortTabAppending = expr("[]") >> Assign;

	BreakLoop = (expr("break") | expr("continue")) >> not_(AlphaNum);

	Return = key("return") >> -(TableBlock | ExpListLow);

	WithExp = ExpList >> -Assign;

	With = Space >> key("with") >> -existential_op >> disable_do_chain_arg_table_block(WithExp) >> body_with("do");
	SwitchCase = Space >> key("when") >> disable_chain(disable_arg_table_block(SwitchList)) >> body_with("then");
	SwitchElse = Space >> key("else") >> body;

	SwitchBlock = *EmptyLine >>
		Advance >> Seperator >>
		SwitchCase >>
		*(Break >> *EmptyLine >> CheckIndent >> SwitchCase) >>
		-(Break >> *EmptyLine >> CheckIndent >> SwitchElse) >>
		PopIndent;

	exp_not_tab = not_(simple_table | TableLit) >> Exp;

	SwitchList = Seperator >> (and_(simple_table | TableLit) >> Exp | exp_not_tab >> *(sym(',') >> exp_not_tab));
	Switch = Space >> key("switch") >> disable_do(Exp) >> -(Space >> key("do"))
		>> -Space >> Break >> SwitchBlock;

	assignment = ExpList >> Assign;
	IfCond = disable_chain(disable_arg_table_block(assignment | Exp));
	IfElseIf = -(Break >> *EmptyLine >> CheckIndent) >> Space >> key("elseif") >> IfCond >> body_with("then");
	IfElse = -(Break >> *EmptyLine >> CheckIndent) >> Space >> key("else") >> body;
	IfType = (expr("if") | expr("unless")) >> not_(AlphaNum);
	If = Space >> IfType >> IfCond >> opt_body_with("then") >> *IfElseIf >> -IfElse;

	WhileType = (expr("while") | expr("until")) >> not_(AlphaNum);
	While = WhileType >> disable_do_chain_arg_table_block(Exp) >> opt_body_with("do");
	Repeat = key("repeat") >> Body >> Break >> *EmptyLine >> CheckIndent >> Space >> key("until") >> Exp;

	for_step_value = sym(',') >> Exp;
	for_args = Space >> Variable >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;

	For = key("for") >> disable_do_chain_arg_table_block(for_args) >> opt_body_with("do");

	for_in = star_exp | ExpList;

	ForEach = key("for") >> AssignableNameList >> Space >> key("in") >>
		disable_do_chain_arg_table_block(for_in) >> opt_body_with("do");

	Do = pl::user(Space >> key("do"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noDoStack.empty() || !st->noDoStack.top();
	}) >> Body;

	DisableDo = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.push(true);
		return true;
	});

	EnableDo = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.pop();
		return true;
	});

	DisableDoChainArgTableBlock = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.push(true);
		st->noChainBlockStack.push(true);
		st->noTableBlockStack.push(true);
		return true;
	});

	EnableDoChainArgTableBlock = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.pop();
		st->noChainBlockStack.pop();
		st->noTableBlockStack.pop();
		return true;
	});

	DisableArgTableBlock = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noTableBlockStack.push(true);
		return true;
	});

	EnableArgTableBlock = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noTableBlockStack.pop();
		return true;
	});

	catch_block = Break >> *EmptyLine >> CheckIndent >> Space >> key("catch") >> Space >> Variable >> InBlock;
	Try = Space >> key("try") >> (InBlock | Exp) >> -catch_block;

	Comprehension = sym('[') >> not_('[') >> Exp >> Space >> CompInner >> sym(']');
	comp_value = sym(',') >> Exp;
	TblComprehension = sym('{') >> Exp >> -comp_value >> Space >> CompInner >> sym('}');

	CompInner = Seperator >> (CompForEach | CompFor) >> *CompClause;
	star_exp = sym('*') >> Exp;
	CompForEach = key("for") >> AssignableNameList >> Space >> key("in") >> (star_exp | Exp);
	CompFor = key("for") >> Space >> Variable >> sym('=') >> Exp >> sym(',') >> Exp >> -for_step_value;
	CompClause = Space >> (CompFor | CompForEach | key("when") >> Exp);

	Assign = sym('=') >> Seperator >> (With | If | Switch | TableBlock | Exp >> *(Space >> set(",;") >> Exp));

	update_op =
		expr("..") |
		expr("//") |
		expr("or") |
		expr("and") |
		expr(">>") |
		expr("<<") |
		expr("??") |
		set("+-*/%&|");

	Update = Space >> update_op >> expr("=") >> Exp;

	Assignable = Space >> (AssignableChain | Variable | SelfName);

	unary_value = unary_operator >> *(Space >> unary_operator) >> Value;

	ExponentialOperator = expr('^');
	expo_value = Space >> ExponentialOperator >> *SpaceBreak >> Value;
	expo_exp = Value >> *expo_value;

	unary_operator =
		expr('-') >> not_(set(">=") | space_one) |
		expr('#') |
		expr('~') >> not_(expr('=') | space_one) |
		expr("not") >> not_(AlphaNum);
	unary_exp = *(Space >> unary_operator) >> expo_exp;

	PipeOperator = expr("|>");
	pipe_value = Space >> PipeOperator >> *SpaceBreak >> unary_exp;
	pipe_exp = unary_exp >> *pipe_value;

	BinaryOperator =
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
		set("+-*/%><|&~");
	exp_op_value = Space >> BinaryOperator >> *SpaceBreak >> pipe_exp;
	Exp = Seperator >> pipe_exp >> *exp_op_value >> -(Space >> expr("??") >> Exp);

	DisableChain = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noChainBlockStack.push(true);
		return true;
	});

	EnableChain = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noChainBlockStack.pop();
		return true;
	});

	chain_line = CheckIndent >> Space >> (chain_dot_chain | ColonChain) >> -InvokeArgs;
	chain_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noChainBlockStack.empty() || !st->noChainBlockStack.top();
	}) >> +SpaceBreak >> Advance >> ensure(
		chain_line >> *(+SpaceBreak >> chain_line), PopIndent);
	ChainValue = Space >> Seperator >> (Chain | Callable) >> -existential_op >> -(InvokeArgs | chain_block) >> -table_appending_op;

	simple_table = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
	Value = SimpleValue | simple_table | ChainValue | Space >> String;

	single_string_inner = expr("\\'") | "\\\\" | not_(expr('\'')) >> Any;
	SingleString = symx('\'') >> *single_string_inner >> symx('\'');
	interp = symx("#{") >> Exp >> sym('}');
	double_string_plain = expr("\\\"") | "\\\\" | not_(expr('"')) >> Any;
	double_string_inner = +(not_(interp) >> double_string_plain);
	double_string_content = double_string_inner | interp;
	DoubleString = symx('"') >> Seperator >> *double_string_content >> symx('"');
	String = DoubleString | SingleString | LuaString;

	lua_string_open = '[' >> *expr('=') >> '[';
	lua_string_close = ']' >> *expr('=') >> ']';

	LuaStringOpen = pl::user(lua_string_open, [](const item_t& item) {
		size_t count = std::distance(item.begin->m_it, item.end->m_it);
		State* st = reinterpret_cast<State*>(item.user_data);
		st->stringOpen = count;
		return true;
	});

	LuaStringClose = pl::user(lua_string_close, [](const item_t& item) {
		size_t count = std::distance(item.begin->m_it, item.end->m_it);
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->stringOpen == count;
	});

	LuaStringContent = *(not_(LuaStringClose) >> Any);

	LuaString = LuaStringOpen >> -Break >> LuaStringContent >> LuaStringClose;

	Parens = symx('(') >> *SpaceBreak >> Exp >> *SpaceBreak >> sym(')');
	Callable = Variable | SelfName | MacroName | VarArg | Parens;
	FnArgsExpList = Exp >> *((Break | sym(',')) >> White >> Exp);

	FnArgs = (symx('(') >> *SpaceBreak >> -FnArgsExpList >> *SpaceBreak >> sym(')')) |
		(sym('!') >> not_(expr('=')));

	meta_index = Name | Index | String;
	Metatable = expr('<') >> sym('>');
	Metamethod = expr('<') >> Space >> meta_index >> sym('>');

	existential_op = expr('?') >> not_(expr('?'));
	table_appending_op = expr("[]");
	chain_call = (Callable | String) >> -existential_op >> ChainItems;
	chain_index_chain = Index >> -existential_op >> -ChainItems;
	chain_dot_chain = DotChainItem >> -existential_op >> -ChainItems;

	Chain = chain_call | chain_dot_chain | ColonChain | chain_index_chain;

	AssignableChain = Seperator >> Chain;

	chain_with_colon = +ChainItem >> -ColonChain;
	ChainItems = chain_with_colon | ColonChain;

	Index = symx('[') >> not_('[') >> Exp >> sym(']');
	ChainItem = Invoke >> -existential_op | DotChainItem >> -existential_op | Slice | Index >> -existential_op;
	DotChainItem = symx('.') >> (Name | Metatable | Metamethod);
	ColonChainItem = (expr('\\') | expr("::")) >> (LuaKeyword | Name | Metamethod);
	invoke_chain = Invoke >> -existential_op >> -ChainItems;
	ColonChain = ColonChainItem >> -existential_op >> -invoke_chain;

	default_value = true_();
	Slice =
		symx('[') >> not_('[') >>
		(Exp | default_value) >>
		sym(',') >>
		(Exp | default_value) >>
		(sym(',') >> Exp | default_value) >>
		sym(']');

	Invoke = Seperator >> (
		FnArgs |
		SingleString |
		DoubleString |
		and_(expr('[')) >> LuaString |
		and_(expr('{')) >> TableLit);

	SpreadExp = sym("...") >> Exp;

	TableValue = ((KeyValue | SpreadExp | Exp) >> not_(sym('='))) | meta_default_pair | default_pair;

	table_lit_lines = SpaceBreak >> TableLitLine >> *(-sym(',') >> SpaceBreak >> TableLitLine) >> -sym(',');

	TableLit =
		sym('{') >> Seperator >>
		-TableValueList >>
		-sym(',') >>
		-table_lit_lines >>
		White >> sym('}');

	TableValueList = TableValue >> *(sym(',') >> TableValue);

	TableLitLine = (
		PushIndent >> (TableValueList >> PopIndent | PopIndent)
	) | (
		Space
	);

	TableBlockInner = Seperator >> KeyValueLine >> *(+SpaceBreak >> KeyValueLine);
	TableBlock = +SpaceBreak >> Advance >> ensure(TableBlockInner, PopIndent);
	TableBlockIndent = sym('*') >> Seperator >> disable_arg_table_block(
		KeyValueList >> -sym(',') >>
		-(+SpaceBreak >> Advance >> ensure(KeyValueList >> -sym(',') >> *(+SpaceBreak >> KeyValueLine), PopIndent)));

	class_member_list = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
	ClassLine = CheckIndent >> (class_member_list | Space >> Statement) >> -sym(',');
	ClassBlock = +SpaceBreak >> Advance >> Seperator >> ClassLine >> *(+SpaceBreak >> ClassLine) >> PopIndent;

	ClassDecl =
		Space >> key("class") >> not_(expr(':')) >>
		-Assignable >>
		-(Space >> key("extends") >> PreventIndent >> ensure(Exp, PopIndent)) >>
		-(Space >> key("using") >> PreventIndent >> ensure(ExpList, PopIndent)) >>
		-ClassBlock;

	global_values = NameList >> -(sym('=') >> (TableBlock | ExpListLow));
	global_op = expr('*') | expr('^');
	Global = key("global") >> (ClassDecl | (Space >> global_op) | global_values);

	export_default = key("default");

	Export = pl::user(key("export"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportCount++;
		return true;
	}) >> (pl::user(Space >> export_default >> Exp, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		bool isValid = !st->exportDefault && st->exportCount == 1;
		st->exportDefault = true;
		return isValid;
	})
	| (not_(Space >> export_default) >> pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		if (st->exportDefault && st->exportCount > 1) {
			return false;
		} else {
			return true;
		}
	}) >> ExpList >> -Assign)
	| Space >> pl::user(Macro, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportMacro = true;
		return true;
	})) >> not_(Space >> statement_appendix);

	variable_pair = sym(':') >> Variable;

	normal_pair = (
		KeyName |
		sym('[') >> not_('[') >> Exp >> sym(']') |
		Space >> String
	) >>
	symx(':') >> not_(':') >>
	(Exp | TableBlock | +SpaceBreak >> Exp);

	default_pair = (
		sym(':') >> Variable >> Seperator |
		KeyName >> symx(':') >> not_(':') >> Seperator >> exp_not_tab |
		Space >> String >> symx(':') >> not_(':') >> Seperator >> exp_not_tab |
		exp_not_tab >> Seperator) >> sym('=') >> Exp;

	meta_variable_pair = sym(":<") >> Space >> Variable >> sym('>');

	meta_normal_pair = sym('<') >> Space >> -meta_index >> sym(">:") >>
		(Exp | TableBlock | +(SpaceBreak) >> Exp);

	meta_default_pair = (
		sym(":<") >> Space >> Variable >> sym('>') >> Seperator |
		sym('<') >> Space >> -meta_index >> sym(">:") >> Seperator >> exp_not_tab
	) >> sym('=') >> Exp;

	KeyValue = variable_pair | normal_pair | meta_variable_pair | meta_normal_pair;
	KeyValueList = KeyValue >> *(sym(',') >> KeyValue);
	KeyValueLine = CheckIndent >> (KeyValueList >> -sym(',') | TableBlockIndent | Space >> expr('*') >> (SpreadExp | Exp | TableBlock));

	FnArgDef = (Variable | SelfName >> -existential_op) >> -(sym('=') >> Space >> Exp);

	FnArgDefList = Space >> Seperator >> (
		(
			FnArgDef >>
			*((sym(',') | Break) >> White >> FnArgDef) >>
			-((sym(',') | Break) >> White >> VarArg)
		) | (
			VarArg
		)
	);

	outer_var_shadow = Space >> key("using") >> (NameList | Space >> expr("nil"));

	FnArgsDef = sym('(') >> White >> -FnArgDefList >> -outer_var_shadow >> White >> sym(')');
	fn_arrow = expr("->") | expr("=>");
	FunLit = -FnArgsDef >> Space >> fn_arrow >> -Body;

	MacroName = expr('$') >> Name;
	macro_args_def = sym('(') >> White >> -FnArgDefList >> White >> sym(')');
	MacroLit = -macro_args_def >> Space >> expr("->") >> Body;
	Macro = key("macro") >> Space >> Name >> sym('=') >> MacroLit;
	MacroInPlace = expr('$') >> Space >> expr("->") >> Body;

	NameList = Seperator >> Space >> Variable >> *(sym(',') >> Space >> Variable);
	NameOrDestructure = Space >> Variable | TableLit;
	AssignableNameList = Seperator >> NameOrDestructure >> *(sym(',') >> NameOrDestructure);

	fn_arrow_back = expr('<') >> set("-=");
	Backcall = -FnArgsDef >> Space >> fn_arrow_back >> Space >> ChainValue;

	PipeBody = Seperator >> PipeOperator >> unary_exp >> *(+SpaceBreak >> CheckIndent >> Space >> PipeOperator >> unary_exp);

	ExpList = Seperator >> Exp >> *(sym(',') >> Exp);
	ExpListLow = Seperator >> Exp >> *(Space >> set(",;") >> Exp);

	ArgLine = CheckIndent >> Exp >> *(sym(',') >> Exp);
	ArgBlock = ArgLine >> *(sym(',') >> SpaceBreak >> ArgLine) >> PopIndent;

	arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noTableBlockStack.empty() || !st->noTableBlockStack.top();
	}) >> TableBlock;

	invoke_args_with_table =
		sym(',') >> (
			TableBlock |
			SpaceBreak >> Advance >> ArgBlock >> -arg_table_block
		) | arg_table_block;

	leading_spaces_error = pl::user(+space_one >> expr('(') >> Exp >> +(sym(',') >> Exp) >> sym(')'), [](const item_t& item) {
		throw ParserError("write invoke arguments in parentheses without leading spaces or leading spaces without parentheses", *item.begin, *item.end);
		return false;
	});

	InvokeArgs =
		not_(set("-~")) >> Seperator >>
		(
			(Exp >> *(sym(',') >> Exp) >> -invoke_args_with_table) |
			arg_table_block |
			leading_spaces_error
		);

	const_value = (expr("nil") | expr("true") | expr("false")) >> not_(AlphaNum);

	SimpleValue = Space >> (const_value |
		If | Switch | Try | With | ClassDecl | ForEach | For | While | Do |
		unary_value | TblComprehension | TableLit | Comprehension |
		FunLit | Num);

	ExpListAssign = ExpList >> -(Update | Assign) >> not_(Space >> expr('='));

	if_line = Space >> IfType >> IfCond;
	while_line = Space >> WhileType >> Exp;

	YueLineComment = *(not_(set("\r\n")) >> Any);
	yue_line_comment = expr("--") >> YueLineComment >> and_(Stop);
	YueMultilineComment = multi_line_content;
	yue_multiline_comment = multi_line_open >> YueMultilineComment >> multi_line_close;
	yue_comment = check_indent >> (yue_multiline_comment >> *(set(" \t") | yue_multiline_comment) >> -yue_line_comment | yue_line_comment) >> and_(Break);

	ChainAssign = Seperator >> Exp >> +(sym('=') >> Exp >> Space >> and_('=')) >> Assign;

	statement_appendix = (if_line | while_line | CompInner) >> Space;
	statement_sep = and_(*SpaceBreak >> CheckIndent >> Space >> (set("($'\"") | expr("[[") | expr("[=")));
	Statement = Seperator >> -(yue_comment >> *(Break >> yue_comment) >> Break >> CheckIndent) >> Space >> (
		Import | While | Repeat | For | ForEach |
		Return | Local | Global | Export | Macro |
		MacroInPlace | BreakLoop | Label | Goto | ShortTabAppending |
		LocalAttrib | Backcall | PipeBody | ExpListAssign | ChainAssign |
		statement_appendix >> empty_block_error
	) >> Space >>
	-statement_appendix >> -statement_sep;

	Body = InBlock | Space >> Statement;

	empty_line_break = (
		check_indent >> (MultiLineComment >> Space | Comment) |
		advance >> ensure(MultiLineComment >> Space | Comment, PopIndent) |
		plain_space) >> and_(Break);

	indentation_error = pl::user(not_(PipeOperator | eof()), [](const item_t& item) {
		throw ParserError("unexpected indent", *item.begin, *item.end);
		return false;
	});

	Line =
		CheckIndent >> Statement |
		empty_line_break |
		Advance >> ensure(Space >> (indentation_error | Statement), PopIndent);
	Block = Seperator >> Line >> *(+Break >> Line);

	Shebang = expr("#!") >> *(not_(Stop) >> Any);
	BlockEnd = Block >> White >> Stop;
	File = -Shebang >> -Block >> White >> Stop;
}
// clang-format on

ParseInfo YueParser::parse(std::string_view codes, rule& r) {
	ParseInfo res;
	if (codes.substr(0, 3) == "\xEF\xBB\xBF"sv) {
		codes = codes.substr(3);
	}
	try {
		res.codes = std::make_unique<input>();
		*(res.codes) = _converter.from_bytes(&codes.front(), &codes.back() + 1);
	} catch (const std::range_error&) {
		res.error = "Invalid text encoding."sv;
		return res;
	}
	error_list errors;
	try {
		State state;
		res.node.set(pl::parse(*(res.codes), r, errors, &state));
		if (state.exportCount > 0) {
			res.moduleName = std::move(state.moduleName);
			res.exportDefault = state.exportDefault;
			res.exportMacro = state.exportMacro;
		}
	} catch (const ParserError& err) {
		res.error = res.errorMessage(err.what(), &err.loc);
		return res;
	} catch (const std::logic_error& err) {
		res.error = err.what();
		return res;
	}
	if (!errors.empty()) {
		std::ostringstream buf;
		for (error_list::iterator it = errors.begin(); it != errors.end(); ++it) {
			const error& err = *it;
			switch (err.m_type) {
				case ERROR_TYPE::ERROR_SYNTAX_ERROR:
					buf << res.errorMessage("syntax error"sv, &err);
					break;
				case ERROR_TYPE::ERROR_INVALID_EOF:
					buf << res.errorMessage("invalid EOF"sv, &err);
					break;
			}
		}
		res.error = buf.str();
	}
	return res;
}

std::string YueParser::toString(ast_node* node) {
	return _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
}

std::string YueParser::toString(input::iterator begin, input::iterator end) {
	return _converter.to_bytes(std::wstring(begin, end));
}

input YueParser::encode(std::string_view codes) {
	return _converter.from_bytes(&codes.front(), &codes.back() + 1);
}

std::string YueParser::decode(const input& codes) {
	return _converter.to_bytes(codes);
}

namespace Utils {
void replace(std::string& str, std::string_view from, std::string_view to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.size(), to);
		start_pos += to.size();
	}
}

void trim(std::string& str) {
	if (str.empty()) return;
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
}
} // namespace Utils

std::string ParseInfo::errorMessage(std::string_view msg, const input_range* loc) const {
	const int ASCII = 255;
	int length = loc->m_begin.m_line;
	auto begin = codes->begin();
	auto end = codes->end();
	int count = 0;
	for (auto it = codes->begin(); it != codes->end(); ++it) {
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
	int oldCol = loc->m_begin.m_col;
	int col = std::max(0, oldCol - 1);
	auto it = begin;
	for (int i = 0; i < oldCol && it != end; ++i) {
		if (*it > ASCII) {
			++col;
		}
		++it;
	}
	auto line = Converter{}.to_bytes(std::wstring(begin, end));
	while (col < static_cast<int>(line.size())
		&& (line[col] == ' ' || line[col] == '\t')) {
		col++;
	}
	Utils::replace(line, "\t"sv, " "sv);
	std::ostringstream buf;
	buf << loc->m_begin.m_line << ": "sv << msg << '\n'
		<< line << '\n'
		<< std::string(col, ' ') << "^"sv;
	return buf.str();
}

} // namespace yue
