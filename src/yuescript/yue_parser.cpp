/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

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
	line_break = nl(-expr('\r') >> '\n');
	any_char = line_break | any();
	stop = line_break | eof();
	comment = "--" >> *(not_(set("\r\n")) >> any_char) >> and_(stop);
	multi_line_open = "--[[";
	multi_line_close = "]]";
	multi_line_content = *(not_(multi_line_close) >> any_char);
	multi_line_comment = multi_line_open >> multi_line_content >> multi_line_close;
	escape_new_line = '\\' >> *(set(" \t") | multi_line_comment) >> -comment >> line_break;
	space_one = set(" \t") | and_(set("-\\")) >> (multi_line_comment | escape_new_line);
	space = -(and_(set(" \t-\\")) >> *space_one >> -comment);
	space_break = space >> line_break;
	white = space >> *(line_break >> space);
	alpha_num = range('a', 'z') | range('A', 'Z') | range('0', '9') | '_';
	not_alpha_num = not_(alpha_num);
	Name = (range('a', 'z') | range('A', 'Z') | '_') >> *alpha_num;
	num_expo = set("eE") >> -set("+-") >> num_char;
	num_expo_hex = set("pP") >> -set("+-") >> num_char;
	lj_num = -set("uU") >> set("lL") >> set("lL");
	num_char = range('0', '9') >> *(range('0', '9') | '_' >> and_(range('0', '9')));
	num_char_hex = range('0', '9') | range('a', 'f') | range('A', 'F');
	num_lit = num_char_hex >> *(num_char_hex | '_' >> and_(num_char_hex));
	Num =
		"0x" >> (
			+num_lit >> (
				'.' >> +num_lit >> -num_expo_hex |
				num_expo_hex |
				lj_num |
				true_()
			) | (
				'.' >> +num_lit >> -num_expo_hex
			)
		) |
		+num_char >> (
			'.' >> +num_char >> -num_expo |
			num_expo |
			lj_num |
			true_()
		) |
		'.' >> +num_char >> -num_expo;

	cut = false_();
	Seperator = true_();

	empty_block_error = pl::user(true_(), [](const item_t& item) {
		throw ParserError("must be followed by a statement or an indented block", *item.begin, *item.end);
		return false;
	});

	#define ensure(patt, finally) ((patt) >> (finally) | (finally) >> cut)

	#define key(str) (expr(str) >> not_alpha_num)

	#define disable_do_rule(patt) ( \
		disable_do >> ( \
			(patt) >> enable_do | \
			enable_do >> cut \
		) \
	)

	#define disable_chain_rule(patt) ( \
		disable_chain >> ( \
			(patt) >> enable_chain | \
			enable_chain >> cut \
		) \
	)

	#define disable_do_chain_arg_table_block_rule(patt) ( \
		disable_do_chain_arg_table_block >> ( \
			(patt) >> enable_do_chain_arg_table_block | \
			enable_do_chain_arg_table_block >> cut \
		) \
	)

	#define disable_arg_table_block_rule(patt) ( \
		disable_arg_table_block >> ( \
			(patt) >> enable_arg_table_block | \
			enable_arg_table_block >> cut \
		) \
	)

	#define body_with(str) ( \
		key(str) >> space >> (in_block | Statement) | \
		in_block | \
		empty_block_error \
	)

	#define opt_body_with(str) ( \
		key(str) >> space >> (in_block | Statement) | \
		in_block \
	)

	#define body (in_block | Statement | empty_block_error)

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

	Self = '@';
	SelfName = '@' >> Name;
	SelfClass = "@@";
	SelfClassName = "@@" >> Name;

	SelfItem = SelfClassName | SelfClass | SelfName | Self;
	KeyName = SelfItem | Name;
	VarArg = "...";

	check_indent = pl::user(plain_space, [](const item_t& item) {
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
	check_indent_match = and_(check_indent);

	advance = pl::user(plain_space, [](const item_t& item) {
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
	advance_match = and_(advance);

	push_indent = pl::user(plain_space, [](const item_t& item) {
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
	push_indent_match = and_(push_indent);

	prevent_indent = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->indents.push(-1);
		return true;
	});

	pop_indent = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->indents.pop();
		return true;
	});

	in_block = +space_break >> advance_match >> ensure(Block, pop_indent);

	LocalFlag = expr('*') | '^';
	LocalValues = NameList >> -(space >> '=' >> space >> (TableBlock | ExpListLow));
	Local = key("local") >> space >> (LocalFlag | LocalValues);

	ConstAttrib = key("const");
	CloseAttrib = key("close");
	local_const_item = Variable | SimpleTable | TableLit;
	LocalAttrib = (
		ConstAttrib >> Seperator >> space >> local_const_item >> *(space >> ',' >> space >> local_const_item) |
		CloseAttrib >> Seperator >> space >> Variable >> *(space >> ',' >> space >> Variable)
	) >> space >> Assign;

	ColonImportName = '\\' >> space >> Variable;
	import_name = ColonImportName | Variable;
	import_name_list = Seperator >> *space_break >> space >> import_name >> *((+space_break | space >> ',' >> *space_break) >> space >> import_name);
	ImportFrom = import_name_list >> *space_break >> space >> key("from") >> space >> Exp;

	ImportLiteralInner = (range('a', 'z') | range('A', 'Z') | set("_-")) >> *(alpha_num | '-');
	import_literal_chain = Seperator >> ImportLiteralInner >> *('.' >> ImportLiteralInner);
	ImportLiteral = (
			'\'' >> import_literal_chain >> '\''
		) | (
			'"' >> import_literal_chain >> '"'
		);

	MacroNamePair = MacroName >> ':' >> space >> MacroName;
	ImportAllMacro = '$';
	import_tab_item =
		VariablePair |
		NormalPair |
		':' >> MacroName |
		MacroNamePair |
		ImportAllMacro |
		MetaVariablePair |
		MetaNormalPair |
		Exp;
	import_tab_list = import_tab_item >> *(space >> ',' >> space >> import_tab_item);
	import_tab_line = (
		push_indent_match >> (space >> import_tab_list >> pop_indent | pop_indent)
	) | space;
	import_tab_lines = space_break >> import_tab_line >> *(-(space >> ',') >> space_break >> import_tab_line) >> -(space >> ',');
	ImportTabLit = (
		'{' >> Seperator >>
		-(space >> import_tab_list) >>
		-(space >> ',') >>
		-import_tab_lines >>
		white >>
		'}'
	) | (
		Seperator >> key_value >> *(space >> ',' >> space >> key_value)
	);

	ImportAs = ImportLiteral >> -(space >> key("as") >> space >> (ImportTabLit | Variable | ImportAllMacro));

	Import = key("import") >> space >> (ImportAs | ImportFrom);

	Label = "::" >> LabelName >> "::";

	Goto = key("goto") >> space >> LabelName;

	ShortTabAppending = "[]" >> space >> Assign;

	BreakLoop = (expr("break") | "continue") >> not_alpha_num;

	Return = key("return") >> -(space >> (TableBlock | ExpListLow));

	with_exp = ExpList >> -(space >> Assign);

	With = key("with") >> -ExistentialOp >> space >> disable_do_chain_arg_table_block_rule(with_exp) >> space >> body_with("do");
	SwitchCase = key("when") >> disable_chain_rule(disable_arg_table_block_rule(SwitchList)) >> space >> body_with("then");
	switch_else = key("else") >> space >> body;

	switch_block =
		*(line_break >> *space_break >> check_indent_match >> space >> SwitchCase) >>
		-(line_break >> *space_break >> check_indent_match >> space >> switch_else);

	exp_not_tab = not_(SimpleTable | TableLit) >> space >> Exp;

	SwitchList = Seperator >> (
		and_(SimpleTable | TableLit) >> space >> Exp |
		exp_not_tab >> *(space >> ',' >> exp_not_tab)
	);
	Switch = key("switch") >> space >> Exp >>
		space >> Seperator >> (
			SwitchCase >> space >> (
				line_break >> *space_break >> check_indent_match >> space >> SwitchCase >> switch_block |
				*(space >> SwitchCase) >> -(space >> switch_else)
			) |
			+space_break >> advance_match >> space >> SwitchCase >> switch_block >> pop_indent
		) >> switch_block;

	Assignment = ExpList >> space >> Assign;
	IfCond = disable_chain_rule(disable_arg_table_block_rule(Assignment | Exp));
	if_else_if = -(line_break >> *space_break >> check_indent_match) >> space >> key("elseif") >> space >> IfCond >> space >> body_with("then");
	if_else = -(line_break >> *space_break >> check_indent_match) >> space >> key("else") >> space >> body;
	IfType = (expr("if") | "unless") >> not_alpha_num;
	If = IfType >> space >> IfCond >> space >> opt_body_with("then") >> *if_else_if >> -if_else;

	WhileType = (expr("while") | "until") >> not_alpha_num;
	While = WhileType >> space >> disable_do_chain_arg_table_block_rule(Exp) >> space >> opt_body_with("do");
	Repeat = key("repeat") >> space >> Body >> line_break >> *space_break >> check_indent_match >> space >> key("until") >> space >> Exp;

	ForStepValue = ',' >> space >> Exp;
	for_args = Variable >> space >> '=' >> space >> Exp >> space >> ',' >> space >> Exp >> space >> -ForStepValue;

	For = key("for") >> space >> disable_do_chain_arg_table_block_rule(for_args) >> space >> opt_body_with("do");

	for_in = StarExp | ExpList;

	ForEach = key("for") >> space >> AssignableNameList >> space >> key("in") >> space >>
		disable_do_chain_arg_table_block_rule(for_in) >> space >> opt_body_with("do");

	Do = pl::user(key("do"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noDoStack.empty() || !st->noDoStack.top();
	}) >> space >> Body;

	disable_do = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.push(true);
		return true;
	});

	enable_do = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.pop();
		return true;
	});

	disable_do_chain_arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.push(true);
		st->noChainBlockStack.push(true);
		st->noTableBlockStack.push(true);
		return true;
	});

	enable_do_chain_arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noDoStack.pop();
		st->noChainBlockStack.pop();
		st->noTableBlockStack.pop();
		return true;
	});

	disable_arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noTableBlockStack.push(true);
		return true;
	});

	enable_arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noTableBlockStack.pop();
		return true;
	});

	CatchBlock = line_break >> *space_break >> check_indent_match >> space >> key("catch") >> space >> Variable >> space >> in_block;
	Try = key("try") >> space >> (in_block | Exp) >> -CatchBlock;

	Comprehension = '[' >> not_('[') >> space >> Exp >> space >> CompInner >> space >> ']';
	CompValue = ',' >> space >> Exp;
	TblComprehension = '{' >> (space >> Exp >> space >> -(CompValue >> space) >> CompInner >> space >> '}' | braces_expression_error);

	CompInner = Seperator >> (CompForEach | CompFor) >> *(space >> comp_clause);
	StarExp = '*' >> space >> Exp;
	CompForEach = key("for") >> space >> AssignableNameList >> space >> key("in") >> space >> (StarExp | Exp);
	CompFor = key("for") >> space >> Variable >> space >> '=' >> space >> Exp >> space >> ',' >> space >> Exp >> -ForStepValue;
	comp_clause = CompFor | CompForEach | key("when") >> space >> Exp;

	Assign = '=' >> space >> Seperator >> (
		With | If | Switch | TableBlock |
		Exp >> *(space >> set(",;") >> space >> Exp)
	);

	UpdateOp =
		expr("..") | "//" | "or" | "and" |
		">>" | "<<" | "??" |
		set("+-*/%&|");

	Update = UpdateOp >> '=' >> space >> Exp;

	Assignable = AssignableChain | Variable | SelfItem;

	UnaryValue = +(UnaryOperator >> space) >> Value;

	exponential_operator = '^';
	expo_value = exponential_operator >> *space_break >> space >> Value;
	expo_exp = Value >> *(space >> expo_value);

	UnaryOperator =
		'-' >> not_(set(">=") | space_one) |
		'#' |
		'~' >> not_('=' | space_one) |
		key("not");
	UnaryExp = *(UnaryOperator >> space) >> expo_exp;

	pipe_operator = "|>";
	pipe_value = pipe_operator >> *space_break >> space >> UnaryExp;
	pipe_exp = UnaryExp >> *(space >> pipe_value);

	BinaryOperator =
		key("or") |
		key("and") |
		"<=" | ">=" | "~=" | "!=" | "==" |
		".." | "<<" | ">>" | "//" |
		set("+-*/%><|&~");

	ExpOpValue = BinaryOperator >> *space_break >> space >> pipe_exp;
	Exp = Seperator >> pipe_exp >> *(space >> ExpOpValue) >> -(space >> "??" >> space >> Exp);

	disable_chain = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noChainBlockStack.push(true);
		return true;
	});

	enable_chain = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->noChainBlockStack.pop();
		return true;
	});

	chain_line = check_indent_match >> space >> (chain_dot_chain | colon_chain) >> -InvokeArgs;
	chain_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noChainBlockStack.empty() || !st->noChainBlockStack.top();
	}) >> +space_break >> advance_match >> ensure(
		chain_line >> *(+space_break >> chain_line), pop_indent);
	ChainValue =
		Seperator >>
		chain >>
		-ExistentialOp >>
		-(InvokeArgs | chain_block) >>
		-TableAppendingOp;

	SimpleTable = Seperator >> key_value >> *(space >> ',' >> space >> key_value);
	Value = SimpleValue | SimpleTable | ChainValue | String;

	single_string_inner = '\\' >> set("'\\") | not_('\'') >> any_char;
	SingleString = '\'' >> *single_string_inner >> '\'';

	interp = "#{" >> space >> Exp >> space >> '}';
	double_string_plain = '\\' >> set("\"\\") | not_('"') >> any_char;
	DoubleStringInner = +(not_(interp) >> double_string_plain);
	DoubleStringContent = DoubleStringInner | interp;
	DoubleString = '"' >> Seperator >> *DoubleStringContent >> '"';
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

	LuaStringContent = *(not_(LuaStringClose) >> any_char);

	LuaString = LuaStringOpen >> -line_break >> LuaStringContent >> LuaStringClose;

	Parens = '(' >> *space_break >> space >> Exp >> *space_break >> space >> ')';
	Callable = Variable | SelfItem | MacroName | VarArg | Parens;
	fn_args_exp_list = space >> Exp >> space >> *((line_break | ',') >> white >> Exp);

	fn_args =
		'(' >> *space_break >> -fn_args_exp_list >> *space_break >> space >> ')' |
		space >> '!' >> not_('=');

	meta_index = Name | index | String;
	Metatable = '<' >> space >> '>';
	Metamethod = '<' >> space >> meta_index >> space >> '>';

	ExistentialOp = '?' >> not_('?');
	TableAppendingOp = "[]";
	chain_call = (
		Callable >> -ExistentialOp >> -chain_items
	) | (
		String >> chain_items
	);
	chain_index_chain = index >> -ExistentialOp >> -chain_items;
	chain_dot_chain = DotChainItem >> -ExistentialOp >> -chain_items;

	chain = chain_call | chain_dot_chain | colon_chain | chain_index_chain;

	chain_call_list = (
		Callable >> -ExistentialOp >> chain_items
	) | (
		String >> chain_items
	);
	chain_list = chain_call_list | chain_dot_chain | colon_chain | chain_index_chain;

	AssignableChain = Seperator >> chain_list;

	chain_with_colon = +chain_item >> -colon_chain;
	chain_items = chain_with_colon | colon_chain;

	index = '[' >> not_('[') >> space >> Exp >> space >> ']';
	chain_item =
		Invoke >> -ExistentialOp |
		DotChainItem >> -ExistentialOp |
		Slice |
		index >> -ExistentialOp;
	DotChainItem = '.' >> (Name | Metatable | Metamethod);
	ColonChainItem = (expr('\\') | "::") >> (LuaKeyword | Name | Metamethod);
	invoke_chain = Invoke >> -ExistentialOp >> -chain_items;
	colon_chain = ColonChainItem >> -ExistentialOp >> -invoke_chain;

	DefaultValue = true_();
	Slice =
		'[' >> not_('[') >>
		space >> (Exp | DefaultValue) >>
		space >> ',' >>
		space >> (Exp | DefaultValue) >>
		space >> (',' >> space >> Exp | DefaultValue) >>
		space >> ']';

	Invoke = Seperator >> (
		fn_args |
		SingleString |
		DoubleString |
		and_('[') >> LuaString |
		and_('{') >> TableLit
	);

	SpreadExp = "..." >> space >> Exp;

	table_value =
		VariablePairDef |
		NormalPairDef |
		MetaVariablePairDef |
		MetaNormalPairDef |
		SpreadExp |
		NormalDef;

	table_lit_lines = space_break >> table_lit_line >> *(-(space >> ',') >> space_break >> table_lit_line) >> -(space >> ',');

	TableLit =
		space >> '{' >> Seperator >>
		-(space >> table_value_list) >>
		-(space >> ',') >>
		-table_lit_lines >>
		white >> '}';

	table_value_list = table_value >> *(space >> ',' >> space >> table_value);

	table_lit_line = (
		push_indent_match >> (space >> table_value_list >> pop_indent | pop_indent)
	) | (
		space
	);

	table_block_inner = Seperator >> key_value_line >> *(+space_break >> key_value_line);
	TableBlock = +space_break >> advance_match >> ensure(table_block_inner, pop_indent);
	TableBlockIndent = '*' >> Seperator >> disable_arg_table_block_rule(
		space >> key_value_list >> -(space >> ',') >>
		-(+space_break >> advance_match >> space >> ensure(key_value_list >> -(space >> ',') >> *(+space_break >> key_value_line), pop_indent)));

	ClassMemberList = Seperator >> key_value >> *(space >> ',' >> space >> key_value);
	class_line = check_indent_match >> space >> (ClassMemberList | Statement) >> -(space >> ',');
	ClassBlock =
		+space_break >>
		advance_match >> Seperator >>
		class_line >> *(+space_break >> class_line) >>
		pop_indent;

	ClassDecl =
		key("class") >> not_(':') >> disable_arg_table_block_rule(
			-(space >> Assignable) >>
			-(space >> key("extends") >> prevent_indent >> space >> ensure(Exp, pop_indent)) >>
			-(space >> key("using") >> prevent_indent >> space >> ensure(ExpList, pop_indent))
		) >> -ClassBlock;

	GlobalValues = NameList >> -(space >> '=' >> space >> (TableBlock | ExpListLow));
	GlobalOp = expr('*') | '^';
	Global = key("global") >> space >> (ClassDecl | GlobalOp | GlobalValues);

	ExportDefault = key("default");

	Export = pl::user(key("export"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportCount++;
		return true;
	}) >> (pl::user(space >> ExportDefault >> space >> Exp, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		if (st->exportDefault) {
			throw ParserError("export default has already been declared", *item.begin, *item.end);
		}
		if (st->exportCount > 1) {
			throw ParserError("there are items already being exported", *item.begin, *item.end);
		}
		st->exportDefault = true;
		return true;
	})
	| (not_(space >> ExportDefault) >> pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		if (st->exportDefault && st->exportCount > 1) {
			throw ParserError("can not export any more items when 'export default' is declared", *item.begin, *item.end);
		}
		return true;
	}) >> space >> ExpList >> -(space >> Assign))
	| space >> pl::user(Macro, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportMacro = true;
		return true;
	})) >> not_(space >> StatementAppendix);

	VariablePair = ':' >> Variable;

	NormalPair =
		(
			KeyName |
			'[' >> not_('[') >> space >> Exp >> space >> ']' |
			String
		) >> ':' >> not_(':') >> space >>
		(Exp | TableBlock | +space_break >> space >> Exp);

	MetaVariablePair = ":<" >> space >> Variable >> space >> '>';

	MetaNormalPair = '<' >> space >> -meta_index >> space >> ">:" >> space >>
		(Exp | TableBlock | +space_break >> space >> Exp);

	destruct_def = -(space >> '=' >> space >> Exp);
	VariablePairDef = VariablePair >> destruct_def;
	NormalPairDef = NormalPair >> destruct_def;
	MetaVariablePairDef = MetaVariablePair >> destruct_def;
	MetaNormalPairDef = MetaNormalPair >> destruct_def;
	NormalDef = Exp >> Seperator >> destruct_def;

	key_value =
		VariablePair |
		NormalPair |
		MetaVariablePair |
		MetaNormalPair;
	key_value_list = key_value >> *(space >> ',' >> space >> key_value);
	key_value_line = check_indent_match >> space >> (
		key_value_list >> -(space >> ',') |
		TableBlockIndent |
		'*' >> space >> (SpreadExp | Exp | TableBlock)
	);

	FnArgDef = (Variable | SelfItem >> -ExistentialOp) >> -(space >> '=' >> space >> Exp);

	FnArgDefList = Seperator >> ((
			FnArgDef >>
			*(space >> (',' | line_break) >> white >> FnArgDef) >>
			-(space >> (',' | line_break) >> white >> VarArg)
		) | VarArg);

	OuterVarShadow = key("using") >> space >> (NameList | key("nil"));

	FnArgsDef = '(' >> white >> -FnArgDefList >> -(space >> OuterVarShadow) >> white >> ')';
	FnArrow = expr("->") | "=>";
	FunLit = -FnArgsDef >> space >> FnArrow >> -(space >> Body);

	MacroName = '$' >> Name;
	macro_args_def = '(' >> white >> -FnArgDefList >> white >> ')';
	MacroLit = -(macro_args_def >> space) >> "->" >> space >> Body;
	Macro = key("macro") >> space >> Name >> space >> '=' >> space >> MacroLit;
	MacroInPlace = '$' >> space >> "->" >> space >> Body;

	NameList = Seperator >> Variable >> *(space >> ',' >> space >> Variable);
	NameOrDestructure = Variable | TableLit;
	AssignableNameList = Seperator >> NameOrDestructure >> *(space >> ',' >> space >> NameOrDestructure);

	FnArrowBack = '<' >> set("-=");
	Backcall = -(FnArgsDef >> space) >> FnArrowBack >> space >> ChainValue;

	PipeBody = Seperator >>
		pipe_operator >> space >> UnaryExp >>
		*(+space_break >> check_indent_match >> space >> pipe_operator >> space >> UnaryExp);

	ExpList = Seperator >> Exp >> *(space >> ',' >> space >> Exp);
	ExpListLow = Seperator >> Exp >> *(space >> set(",;") >> space >> Exp);

	arg_line = check_indent_match >> space >> Exp >> *(space >> ',' >> space >> Exp);
	arg_block = arg_line >> *(space >> ',' >> space_break >> arg_line) >> pop_indent;

	arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noTableBlockStack.empty() || !st->noTableBlockStack.top();
	}) >> TableBlock;

	invoke_args_with_table =
		',' >> (
			TableBlock |
			space_break >> advance_match >> arg_block >> -arg_table_block
		) | arg_table_block;

	leading_spaces_error = pl::user(+space_one >> '(' >> space >> Exp >> +(space >> ',' >> space >> Exp) >> space >> ')', [](const item_t& item) {
		throw ParserError("write invoke arguments in parentheses without leading spaces or just leading spaces without parentheses", *item.begin, *item.end);
		return false;
	});

	InvokeArgs =
		not_(set("-~")) >> space >> Seperator >> (
			Exp >> *(space >> ',' >> space >> Exp) >> -(space >> invoke_args_with_table) |
			arg_table_block |
			leading_spaces_error
		);

	ConstValue = (expr("nil") | "true" | "false") >> not_alpha_num;

	braces_expression_error = pl::user("{", [](const item_t& item) {
		throw ParserError("invalid brace expression", *item.begin, *item.end);
		return false;
	});

	SimpleValue =
		TableLit | ConstValue | If | Switch | Try | With |
		ClassDecl | ForEach | For | While | Do |
		UnaryValue | TblComprehension | Comprehension |
		FunLit | Num;

	ExpListAssign = ExpList >> -(space >> (Update | Assign)) >> not_(space >> '=');

	IfLine = IfType >> space >> IfCond;
	WhileLine = WhileType >> space >> Exp;

	YueLineComment = *(not_(set("\r\n")) >> any_char);
	yue_line_comment = "--" >> YueLineComment >> and_(stop);
	YueMultilineComment = multi_line_content;
	yue_multiline_comment = multi_line_open >> YueMultilineComment >> multi_line_close;
	yue_comment = check_indent >> (
		(
			yue_multiline_comment >>
			*(set(" \t") | yue_multiline_comment) >>
			-yue_line_comment
		) | yue_line_comment
	) >> and_(line_break);

	ChainAssign = Seperator >> Exp >> +(space >> '=' >> space >> Exp >> space >> and_('=')) >> space >> Assign;

	StatementAppendix = (IfLine | WhileLine | CompInner) >> space;
	StatementSep = and_(
		*space_break >> check_indent_match >> space >> (
			set("($'\"") |
			"[[" |
			"[="
		)
	);
	Statement =
		Seperator >>
		-(
			yue_comment >>
			*(line_break >> yue_comment) >>
			line_break >>
			check_indent_match
		) >>
		space >> (
			Import | While | Repeat | For | ForEach |
			Return | Local | Global | Export | Macro |
			MacroInPlace | BreakLoop | Label | Goto | ShortTabAppending |
			LocalAttrib | Backcall | PipeBody | ExpListAssign | ChainAssign |
			StatementAppendix >> empty_block_error
		) >>
		space >>
		-StatementAppendix >>
		-StatementSep;

	Body = in_block | Statement;

	empty_line_break = (
		check_indent >> (multi_line_comment >> space | comment) |
		advance >> ensure(multi_line_comment >> space | comment, pop_indent) |
		plain_space
	) >> and_(line_break);

	indentation_error = pl::user(not_(pipe_operator | eof()), [](const item_t& item) {
		throw ParserError("unexpected indent", *item.begin, *item.end);
		return false;
	});

	line = (
		check_indent_match >> Statement |
		empty_line_break |
		advance_match >> ensure(space >> (indentation_error | Statement), pop_indent)
	);
	Block = Seperator >> line >> *(+line_break >> line);

	shebang = "#!" >> *(not_(stop) >> any_char);
	BlockEnd = Block >> white >> stop;
	File = -shebang >> -Block >> white >> stop;
}
// clang-format on

ParseInfo YueParser::parse(std::string_view codes, rule& r) {
	ParseInfo res;
	if (codes.substr(0, 3) == "\xEF\xBB\xBF"sv) {
		codes = codes.substr(3);
	}
	try {
		res.codes = std::make_unique<input>();
		if (!codes.empty()) {
			*(res.codes) = _converter.from_bytes(&codes.front(), &codes.back() + 1);
		}
	} catch (const std::range_error&) {
		res.error = "invalid text encoding"sv;
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
