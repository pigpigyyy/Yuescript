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
	Break = nl(-expr('\r') >> '\n');
	Any = Break | any();
	Stop = Break | eof();
	Indent = plain_space;
	Comment = "--" >> *(not_(set("\r\n")) >> Any) >> and_(Stop);
	multi_line_open = "--[[";
	multi_line_close = "]]";
	multi_line_content = *(not_(multi_line_close) >> Any);
	MultiLineComment = multi_line_open >> multi_line_content >> multi_line_close;
	EscapeNewLine = '\\' >> *(set(" \t") | MultiLineComment) >> -Comment >> Break;
	space_one = set(" \t") | and_(set("-\\")) >> (MultiLineComment | EscapeNewLine);
	Space = -(and_(set(" \t-\\")) >> *space_one >> -Comment);
	SpaceBreak = Space >> Break;
	White = Space >> *(Break >> Space);
	EmptyLine = SpaceBreak;
	AlphaNum = sel({range('a', 'z'), range('A', 'Z'), range('0', '9'), '_'});
	Name = sel({range('a', 'z'), range('A', 'Z'), '_'}) >> *AlphaNum;
	num_expo = set("eE") >> -set("+-") >> num_char;
	num_expo_hex = set("pP") >> -set("+-") >> num_char;
	lj_num = -set("uU") >> set("lL") >> set("lL");
	num_char = range('0', '9') >> *(range('0', '9') | '_' >> and_(range('0', '9')));
	num_char_hex = sel({range('0', '9'), range('a', 'f'), range('A', 'F')});
	num_lit = num_char_hex >> *(num_char_hex | '_' >> and_(num_char_hex));
	Num = sel({
		"0x" >> (
			+num_lit >> sel({
				seq({'.', +num_lit, -num_expo_hex}),
				num_expo_hex,
				lj_num,
				true_()
			}) | seq({
				'.', +num_lit, -num_expo_hex
			})
		),
		+num_char >> sel({
			seq({'.', +num_char, -num_expo}),
			num_expo,
			lj_num,
			true_()
		}),
		seq({'.', +num_char, -num_expo})
	});

	Cut = false_();
	Seperator = true_();

	empty_block_error = pl::user(true_(), [](const item_t& item) {
		throw ParserError("must be followed by a statement or an indented block", *item.begin, *item.end);
		return false;
	});

	#define ensure(patt, finally) ((patt) >> (finally) | (finally) >> Cut)
	#define key(str) (str >> not_(AlphaNum))
	#define disable_do(patt) (DisableDo >> ((patt) >> EnableDo | EnableDo >> Cut))
	#define disable_chain(patt) (DisableChain >> ((patt) >> EnableChain | EnableChain >> Cut))
	#define disable_do_chain_arg_table_block(patt) (DisableDoChainArgTableBlock >> ((patt) >> EnableDoChainArgTableBlock | EnableDoChainArgTableBlock >> Cut))
	#define disable_arg_table_block(patt) (DisableArgTableBlock >> ((patt) >> EnableArgTableBlock | EnableArgTableBlock >> Cut))
	#define body_with(str) (sel({key(str) >> Space >> (InBlock | Statement), InBlock, empty_block_error}))
	#define opt_body_with(str) (key(str) >> Space >> (InBlock | Statement) | InBlock)
	#define body (sel({InBlock, Statement, empty_block_error}))

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

	self = '@';
	self_name = '@' >> Name;
	self_class = "@@";
	self_class_name = "@@" >> Name;

	SelfName = sel({self_class_name, self_class, self_name, self});
	KeyName = SelfName | Name;
	VarArg = "...";

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

	InBlock = +SpaceBreak >> Advance >> ensure(Block, PopIndent);

	local_flag = sel({'*', '^'});
	local_values = NameList >> -(Space >> '=' >> Space >> (TableBlock | ExpListLow));
	Local = key("local") >> Space >> (local_flag | local_values);

	const_attrib = key("const");
	close_attrib = key("close");
	local_const_item = sel({Variable, simple_table, TableLit});
	LocalAttrib = (
		const_attrib >> Seperator >> Space >> local_const_item >> *(Space >> ',' >> Space >> local_const_item) |
		close_attrib >> Seperator >> Space >> Variable >> *(Space >> ',' >> Space >> Variable)
	) >> Space >> Assign;

	colon_import_name = '\\' >> Space >> Variable;
	ImportName = colon_import_name | Variable;
	ImportNameList = Seperator >> *SpaceBreak >> Space >> ImportName >> *((+SpaceBreak | Space >> ',' >> *SpaceBreak) >> Space >> ImportName);
	ImportFrom = ImportNameList >> *SpaceBreak >> Space >> key("from") >> Space >> Exp;

	import_literal_inner = sel({range('a', 'z'), range('A', 'Z'), set("_-")}) >> *(AlphaNum | '-');
	import_literal_chain = Seperator >> import_literal_inner >> *('.' >> import_literal_inner);
	ImportLiteral = sel({
		'\'' >> import_literal_chain >> '\'',
		'"' >> import_literal_chain >> '"'
	});

	macro_name_pair = MacroName >> ':' >> Space >> MacroName;
	import_all_macro = '$';
	ImportTabItem = sel({
		variable_pair,
		normal_pair,
		':' >> MacroName,
		macro_name_pair,
		import_all_macro,
		meta_variable_pair,
		meta_normal_pair,
		Exp
	});
	ImportTabList = ImportTabItem >> *(Space >> ',' >> Space >> ImportTabItem);
	ImportTabLine = (
		PushIndent >> (Space >> ImportTabList >> PopIndent | PopIndent)
	) | Space;
	import_tab_lines = SpaceBreak >> ImportTabLine >> *(-(Space >> ',') >> SpaceBreak >> ImportTabLine) >> -(Space >> ',');
	ImportTabLit = seq({
		'{', Seperator,
		-(Space >> ImportTabList),
		-(Space >> ','),
		-import_tab_lines,
		White,
		'}'
	}) | seq({
		Seperator, KeyValue, *(Space >> ',' >> Space >> KeyValue)
	});

	ImportAs = ImportLiteral >> -(Space >> key("as") >> Space >> sel({ImportTabLit, Variable, import_all_macro}));

	Import = key("import") >> Space >> (ImportAs | ImportFrom);

	Label = "::" >> LabelName >> "::";

	Goto = key("goto") >> Space >> LabelName;

	ShortTabAppending = "[]" >> Space >> Assign;

	BreakLoop = sel({"break", "continue"}) >> not_(AlphaNum);

	Return = key("return") >> -(Space >> (TableBlock | ExpListLow));

	WithExp = ExpList >> -(Space >> Assign);

	With = key("with") >> -existential_op >> Space >> disable_do_chain_arg_table_block(WithExp) >> Space >> body_with("do");
	SwitchCase = key("when") >> disable_chain(disable_arg_table_block(SwitchList)) >> Space >> body_with("then");
	SwitchElse = key("else") >> Space >> body;

	SwitchBlock =
		*(Break >> *EmptyLine >> CheckIndent >> Space >> SwitchCase) >>
		-(Break >> *EmptyLine >> CheckIndent >> Space >> SwitchElse);

	exp_not_tab = not_(simple_table | TableLit) >> Space >> Exp;

	SwitchList = Seperator >> (
		and_(simple_table | TableLit) >> Space >> Exp |
		exp_not_tab >> *(Space >> ',' >> exp_not_tab)
	);
	Switch = key("switch") >> Space >> Exp >>
		Space >> Seperator >> (
			SwitchCase >> Space >> (
				Break >> *EmptyLine >> CheckIndent >> Space >> SwitchCase >> SwitchBlock |
				*(Space >> SwitchCase) >> -(Space >> SwitchElse)
			) |
			SpaceBreak >> *EmptyLine >> Advance >> Space >> SwitchCase >> SwitchBlock >> PopIndent
		) >> SwitchBlock;

	assignment = ExpList >> Space >> Assign;
	IfCond = disable_chain(disable_arg_table_block(assignment | Exp));
	IfElseIf = -(Break >> *EmptyLine >> CheckIndent) >> Space >> key("elseif") >> Space >> IfCond >> Space >> body_with("then");
	IfElse = -(Break >> *EmptyLine >> CheckIndent) >> Space >> key("else") >> Space >> body;
	IfType = sel({"if", "unless"}) >> not_(AlphaNum);
	If = seq({IfType, Space, IfCond, Space, opt_body_with("then"), *IfElseIf, -IfElse});

	WhileType = sel({"while", "until"}) >> not_(AlphaNum);
	While = WhileType >> Space >> disable_do_chain_arg_table_block(Exp) >> Space >> opt_body_with("do");
	Repeat = seq({key("repeat"), Space, Body, Break, *EmptyLine, CheckIndent, Space, key("until"), Space, Exp});

	for_step_value = ',' >> Space >> Exp;
	for_args = Variable >> Space >> '=' >> Space >> Exp >> Space >> ',' >> Space >> Exp >> Space >> -for_step_value;

	For = key("for") >> Space >> disable_do_chain_arg_table_block(for_args) >> Space >> opt_body_with("do");

	for_in = star_exp | ExpList;

	ForEach = key("for") >> Space >> AssignableNameList >> Space >> key("in") >> Space >>
		disable_do_chain_arg_table_block(for_in) >> Space >> opt_body_with("do");

	Do = pl::user(key("do"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noDoStack.empty() || !st->noDoStack.top();
	}) >> Space >> Body;

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

	catch_block = Break >> *EmptyLine >> CheckIndent >> Space >> key("catch") >> Space >> Variable >> Space >> InBlock;
	Try = key("try") >> Space >> (InBlock | Exp) >> -catch_block;

	Comprehension = '[' >> not_('[') >> Space >> Exp >> Space >> CompInner >> Space >> ']';
	comp_value = ',' >> Space >> Exp;
	TblComprehension = '{' >> Space >> Exp >> Space >> -(comp_value >> Space) >> CompInner >> Space >> '}';

	CompInner = Seperator >> (CompForEach | CompFor) >> *(Space >> CompClause);
	star_exp = '*' >> Space >> Exp;
	CompForEach = key("for") >> Space >> AssignableNameList >> Space >> key("in") >> Space >> (star_exp | Exp);
	CompFor = key("for") >> Space >> Variable >> Space >> '=' >> Space >> Exp >> Space >> ',' >> Space >> Exp >> -for_step_value;
	CompClause = sel({CompFor, CompForEach, key("when") >> Space >> Exp});

	Assign = '=' >> Space >> Seperator >> sel({
		With, If, Switch, TableBlock,
		Exp >> *(Space >> set(",;") >> Space >> Exp)
	});

	update_op = sel({
		"..", "//", "or", "and",
		">>", "<<", "??",
		set("+-*/%&|")
	});

	Update = update_op >> '=' >> Space >> Exp;

	Assignable = sel({AssignableChain, Variable, SelfName});

	unary_value = +(unary_operator >> Space) >> Value;

	ExponentialOperator = '^';
	expo_value = seq({ExponentialOperator, *SpaceBreak, Space, Value});
	expo_exp = Value >> *(Space >> expo_value);

	unary_operator = sel({
		'-' >> not_(set(">=") | space_one),
		'#',
		'~' >> not_('=' | space_one),
		"not" >> not_(AlphaNum)
	});
	unary_exp = *(unary_operator >> Space) >> expo_exp;

	PipeOperator = "|>";
	pipe_value = seq({PipeOperator, *SpaceBreak, Space, unary_exp});
	pipe_exp = unary_exp >> *(Space >> pipe_value);

	BinaryOperator = sel({
		"or" >> not_(AlphaNum),
		"and" >> not_(AlphaNum),
		"<=", ">=", "~=", "!=", "==",
		"..", "<<", ">>", "//",
		set("+-*/%><|&~")
	});
	exp_op_value = seq({BinaryOperator, *SpaceBreak, Space, pipe_exp});
	Exp = seq({Seperator, pipe_exp, *(Space >> exp_op_value), -(Space >> "??" >> Space >> Exp)});

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

	chain_line = seq({CheckIndent, Space, chain_dot_chain | ColonChain, -InvokeArgs});
	chain_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noChainBlockStack.empty() || !st->noChainBlockStack.top();
	}) >> +SpaceBreak >> Advance >> ensure(
		chain_line >> *(+SpaceBreak >> chain_line), PopIndent);
	ChainValue = seq({
		Seperator,
		Chain,
		-existential_op,
		-(InvokeArgs | chain_block),
		-table_appending_op
	});

	simple_table = seq({Seperator, KeyValue, *(Space >> ',' >> Space >> KeyValue)});
	Value = sel({SimpleValue, simple_table, ChainValue, String});

	single_string_inner = '\\' >> set("'\\") | not_('\'') >> Any;
	SingleString = '\'' >> *single_string_inner >> '\'';
	interp = "#{" >> Space >> Exp >> Space >> '}';
	double_string_plain = '\\' >> set("\"\\") | not_('"') >> Any;
	double_string_inner = +(not_(interp) >> double_string_plain);
	double_string_content = double_string_inner | interp;
	DoubleString = '"' >> Seperator >> *double_string_content >> '"';
	String = sel({DoubleString, SingleString, LuaString});

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

	Parens = seq({'(', *SpaceBreak, Space, Exp, *SpaceBreak, Space, ')'});
	Callable = sel({Variable, SelfName, MacroName, VarArg, Parens});
	FnArgsExpList = Space >> Exp >> Space >> *seq({Break | ',', White, Exp});

	FnArgs = sel({
		seq({'(', *SpaceBreak, -FnArgsExpList, *SpaceBreak, Space, ')'}),
		seq({Space, '!', not_('=')})
	});

	meta_index = sel({Name, Index, String});
	Metatable = '<' >> Space >> '>';
	Metamethod = '<' >> Space >> meta_index >> Space >> '>';

	existential_op = '?' >> not_('?');
	table_appending_op = "[]";
	chain_call = seq({
		Callable,
		-existential_op,
		-ChainItems
	}) | seq({
		String,
		ChainItems
	});
	chain_index_chain = seq({Index, -existential_op, -ChainItems});
	chain_dot_chain = seq({DotChainItem, -existential_op, -ChainItems});

	Chain = sel({chain_call, chain_dot_chain, ColonChain, chain_index_chain});

	chain_call_list = seq({
		Callable,
		-existential_op,
		ChainItems
	}) | seq({
		String,
		ChainItems
	});
	ChainList = sel({chain_call_list, chain_dot_chain, ColonChain, chain_index_chain});

	AssignableChain = Seperator >> ChainList;

	chain_with_colon = +ChainItem >> -ColonChain;
	ChainItems = chain_with_colon | ColonChain;

	Index = seq({'[', not_('['), Space, Exp, Space, ']'});
	ChainItem = sel({
		Invoke >> -existential_op,
		DotChainItem >> -existential_op,
		Slice,
		Index >> -existential_op
	});
	DotChainItem = '.' >> sel({Name, Metatable, Metamethod});
	ColonChainItem = sel({'\\', "::"}) >> sel({LuaKeyword, Name, Metamethod});
	invoke_chain = Invoke >> -existential_op >> -ChainItems;
	ColonChain = ColonChainItem >> -existential_op >> -invoke_chain;

	default_value = true_();
	Slice = seq({
		'[', not_('['),
		Space, Exp | default_value,
		Space, ',',
		Space, Exp | default_value,
		Space, ',' >> Space >> Exp | default_value,
		Space, ']'
	});

	Invoke = Seperator >> sel({
		FnArgs,
		SingleString,
		DoubleString,
		and_('[') >> LuaString,
		and_('{') >> TableLit
	});

	SpreadExp = "..." >> Space >> Exp;

	TableValue = sel({
		variable_pair_def,
		normal_pair_def,
		meta_variable_pair_def,
		meta_normal_pair_def,
		SpreadExp,
		normal_def
	});

	table_lit_lines = SpaceBreak >> TableLitLine >> *(-(Space >> ',') >> SpaceBreak >> TableLitLine) >> -(Space >> ',');

	TableLit = seq({
		Space, '{', Seperator,
		-(Space >> TableValueList),
		-(Space >> ','),
		-table_lit_lines,
		White, '}'
	});

	TableValueList = TableValue >> *(Space >> ',' >> Space >> TableValue);

	TableLitLine = (
		PushIndent >> (Space >> TableValueList >> PopIndent | PopIndent)
	) | (
		Space
	);

	TableBlockInner = Seperator >> KeyValueLine >> *(+SpaceBreak >> KeyValueLine);
	TableBlock = +SpaceBreak >> Advance >> ensure(TableBlockInner, PopIndent);
	TableBlockIndent = '*' >> Seperator >> disable_arg_table_block(
		Space >> KeyValueList >> -(Space >> ',') >>
		-(+SpaceBreak >> Advance >> Space >> ensure(KeyValueList >> -(Space >> ',') >> *(+SpaceBreak >> KeyValueLine), PopIndent)));

	class_member_list = Seperator >> KeyValue >> *(Space >> ',' >> Space >> KeyValue);
	ClassLine = CheckIndent >> Space >> (class_member_list | Statement) >> -(Space >> ',');
	ClassBlock = seq({+SpaceBreak, Advance, Seperator, ClassLine, *(+SpaceBreak >> ClassLine), PopIndent});

	ClassDecl = seq({
		key("class"), not_(':'),
		disable_arg_table_block(seq({
			-(Space >> Assignable),
			-seq({Space, key("extends"), PreventIndent, Space, ensure(Exp, PopIndent)}),
			-seq({Space, key("using"), PreventIndent, Space, ensure(ExpList, PopIndent)})
		})),
		-ClassBlock
	});

	global_values = NameList >> -(Space >> '=' >> Space >> (TableBlock | ExpListLow));
	global_op = sel({'*', '^'});
	Global = key("global") >> Space >> sel({ClassDecl, global_op, global_values});

	export_default = key("default");

	Export = pl::user(key("export"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportCount++;
		return true;
	}) >> (pl::user(Space >> export_default >> Space >> Exp, [](const item_t& item) {
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
	| (not_(Space >> export_default) >> pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		if (st->exportDefault && st->exportCount > 1) {
			throw ParserError("can not export any more items when 'export default' is declared", *item.begin, *item.end);
		}
		return true;
	}) >> Space >> ExpList >> -(Space >> Assign))
	| Space >> pl::user(Macro, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportMacro = true;
		return true;
	})) >> not_(Space >> statement_appendix);

	variable_pair = ':' >> Variable;

	normal_pair = seq({
		sel({
			KeyName,
			seq({'[', not_('['), Space, Exp, Space, ']'}),
			String
		}),
		':', not_(':'), Space,
		sel({Exp, TableBlock, +SpaceBreak >> Space >> Exp})
	});

	meta_variable_pair = ":<" >> Space >> Variable >> Space >> '>';

	meta_normal_pair = '<' >> Space >> -meta_index >> Space >> ">:" >> Space >>
		sel({Exp, TableBlock, +(SpaceBreak) >> Space >> Exp});

	destruct_def = -seq({Space, '=', Space, Exp});
	variable_pair_def = variable_pair >> destruct_def;
	normal_pair_def = normal_pair >> destruct_def;
	meta_variable_pair_def = meta_variable_pair >> destruct_def;
	meta_normal_pair_def = meta_normal_pair >> destruct_def;
	normal_def = Exp >> Seperator >> destruct_def;

	KeyValue = sel({
		variable_pair,
		normal_pair,
		meta_variable_pair,
		meta_normal_pair
	});
	KeyValueList = KeyValue >> *(Space >> ',' >> Space >> KeyValue);
	KeyValueLine = CheckIndent >> Space >> sel({
		KeyValueList >> -(Space >> ','),
		TableBlockIndent,
		'*' >> Space >> sel({SpreadExp, Exp, TableBlock})
	});

	FnArgDef = (Variable | SelfName >> -existential_op) >> -(Space >> '=' >> Space >> Exp);

	FnArgDefList = Seperator >> (
		seq({
			FnArgDef,
			*seq({Space, ',' | Break, White, FnArgDef}),
			-seq({Space, ',' | Break, White, VarArg})
		}) |
			VarArg
	);

	outer_var_shadow = key("using") >> Space >> (NameList | key("nil"));

	FnArgsDef = seq({'(', White, -FnArgDefList, -(Space >> outer_var_shadow), White, ')'});
	fn_arrow = sel({"->", "=>"});
	FunLit = seq({-FnArgsDef, Space, fn_arrow, -(Space >> Body)});

	MacroName = '$' >> Name;
	macro_args_def = '(' >> White >> -FnArgDefList >> White >> ')';
	MacroLit = -(macro_args_def >> Space) >> "->" >> Space >> Body;
	Macro = key("macro") >> Space >> Name >> Space >> '=' >> Space >> MacroLit;
	MacroInPlace = '$' >> Space >> "->" >> Space >> Body;

	NameList = Seperator >> Variable >> *(Space >> ',' >> Space >> Variable);
	NameOrDestructure = Variable | TableLit;
	AssignableNameList = Seperator >> NameOrDestructure >> *(Space >> ',' >> Space >> NameOrDestructure);

	fn_arrow_back = '<' >> set("-=");
	Backcall = seq({-(FnArgsDef >> Space), fn_arrow_back, Space, ChainValue});

	PipeBody = seq({
		Seperator,
		PipeOperator,
		Space,
		unary_exp,
		*seq({+SpaceBreak, CheckIndent, Space, PipeOperator, Space, unary_exp})
	});

	ExpList = Seperator >> Exp >> *(Space >> ',' >> Space >> Exp);
	ExpListLow = Seperator >> Exp >> *(Space >> set(",;") >> Space >> Exp);

	ArgLine = CheckIndent >> Space >> Exp >> *(Space >> ',' >> Space >> Exp);
	ArgBlock = ArgLine >> *(Space >> ',' >> SpaceBreak >> ArgLine) >> PopIndent;

	arg_table_block = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->noTableBlockStack.empty() || !st->noTableBlockStack.top();
	}) >> TableBlock;

	invoke_args_with_table =
		',' >> (
			TableBlock |
			SpaceBreak >> Advance >> ArgBlock >> -arg_table_block
		) | arg_table_block;

	leading_spaces_error = pl::user(+space_one >> '(' >> Space >> Exp >> +(Space >> ',' >> Space >> Exp) >> Space >> ')', [](const item_t& item) {
		throw ParserError("write invoke arguments in parentheses without leading spaces or just leading spaces without parentheses", *item.begin, *item.end);
		return false;
	});

	InvokeArgs =
		not_(set("-~")) >> Space >> Seperator >>
		sel({
			Exp >> *(Space >> ',' >> Space >> Exp) >> -(Space >> invoke_args_with_table),
			arg_table_block,
			leading_spaces_error
		});

	const_value = sel({"nil", "true", "false"}) >> not_(AlphaNum);

	SimpleValue = sel({
		TableLit, const_value, If, Switch, Try, With,
		ClassDecl, ForEach, For, While, Do,
		unary_value, TblComprehension, Comprehension,
		FunLit, Num
	});

	ExpListAssign = ExpList >> -(Space >> (Update | Assign)) >> not_(Space >> '=');

	if_line = IfType >> Space >> IfCond;
	while_line = WhileType >> Space >> Exp;

	YueLineComment = *(not_(set("\r\n")) >> Any);
	yue_line_comment = "--" >> YueLineComment >> and_(Stop);
	YueMultilineComment = multi_line_content;
	yue_multiline_comment = multi_line_open >> YueMultilineComment >> multi_line_close;
	yue_comment = check_indent >> sel({
		seq({
			yue_multiline_comment,
			*(set(" \t") | yue_multiline_comment),
			-yue_line_comment
		}),
		yue_line_comment
	}) >> and_(Break);

	ChainAssign = Seperator >> Exp >> +(Space >> '=' >> Space >> Exp >> Space >> and_('=')) >> Space >> Assign;

	statement_appendix = sel({if_line, while_line, CompInner}) >> Space;
	statement_sep = and_(seq({
		*SpaceBreak, CheckIndent, Space,
		sel({
			set("($'\""),
			"[[",
			"[="
		})
	}));
	Statement = seq({
		Seperator,
		-seq({
			yue_comment,
			*(Break >> yue_comment),
			Break,
			CheckIndent
		}),
		Space,
		sel({
			Import, While, Repeat, For, ForEach,
			Return, Local, Global, Export, Macro,
			MacroInPlace, BreakLoop, Label, Goto, ShortTabAppending,
			LocalAttrib, Backcall, PipeBody, ExpListAssign, ChainAssign,
			statement_appendix >> empty_block_error
		}),
		Space,
		-statement_appendix,
		-statement_sep
	});

	Body = InBlock | Statement;

	empty_line_break = sel({
		check_indent >> (MultiLineComment >> Space | Comment),
		advance >> ensure(MultiLineComment >> Space | Comment, PopIndent),
		plain_space
	}) >> and_(Break);

	indentation_error = pl::user(not_(PipeOperator | eof()), [](const item_t& item) {
		throw ParserError("unexpected indent", *item.begin, *item.end);
		return false;
	});

	Line = sel({
		CheckIndent >> Statement,
		empty_line_break,
		Advance >> ensure(Space >> (indentation_error | Statement), PopIndent)
	});
	Block = seq({Seperator, Line, *(+Break >> Line)});

	Shebang = "#!" >> *(not_(Stop) >> Any);
	BlockEnd = seq({Block, White, Stop});
	File = seq({-Shebang, -Block, White, Stop});
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
