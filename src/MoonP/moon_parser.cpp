/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "MoonP/moon_parser.h"

namespace pl = parserlib;

namespace MoonP {
using namespace std::string_view_literals;

std::unordered_set<std::string> LuaKeywords = {
	"and", "break", "do", "else", "elseif",
	"end", "false", "for", "function", "if",
	"in", "local", "nil", "not", "or",
	"repeat", "return", "then", "true", "until",
	"while"
};

std::unordered_set<std::string> Keywords = {
	"and", "break", "do", "else", "elseif",
	"end", "false", "for", "function", "if",
	"in", "local", "nil", "not", "or",
	"repeat", "return", "then", "true", "until",
	"while", // Lua keywords
	"as", "class", "continue", "export", "extends",
	"from", "global", "import", "switch", "unless",
	"using", "when", "with" // Moon keywords
};

MoonParser::MoonParser() {
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
	Space = *(set(" \t") | and_(set("-\\")) >> (MultiLineComment | EscapeNewLine)) >> -Comment;
	SpaceBreak = Space >> Break;
	White = Space >> *(Break >> Space);
	EmptyLine = SpaceBreak;
	AlphaNum = range('a', 'z') | range('A', 'Z') | range('0', '9') | '_';
	Name = (range('a', 'z') | range('A', 'Z') | '_') >> *AlphaNum;
	Num =
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
	Cut = false_();
	Seperator = true_();

	#define sym(str) (Space >> str)
	#define symx(str) expr(str)
	#define ensure(patt, finally) (((patt) >> (finally)) | ((finally) >> (Cut)))
	#define key(str) (Space >> str >> not_(AlphaNum))

	Variable = pl::user(Name, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		for (auto it = item.begin; it != item.end; ++it) st->buffer += static_cast<char>(*it);
		auto isValid = Keywords.find(st->buffer) == Keywords.end();
		if (isValid) {
			if (st->buffer == st->moduleName) {
				st->moduleFix++;
				st->moduleName = std::string("_module_"sv) + std::to_string(st->moduleFix);
			}
		}
		st->buffer.clear();
		return isValid;
	});

	LuaKeyword = pl::user(Name, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		for (auto it = item.begin; it != item.end; ++it) st->buffer += static_cast<char>(*it);
		auto it = LuaKeywords.find(st->buffer);
		st->buffer.clear();
		return it != LuaKeywords.end();
	});

	self = expr('@');
	self_name = '@' >> Name;
	self_class = expr("@@");
	self_class_name = "@@" >> Name;

	SelfName = Space >> (self_class_name | self_class | self_name | self);
	KeyName = SelfName | Space >> Name;
	VarArg = Space >> "...";

	check_indent = pl::user(Indent, [](const item_t& item) {
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
	CheckIndent = and_(check_indent);

	advance = pl::user(Indent, [](const item_t& item) {
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
	Advance = and_(advance);

	push_indent = pl::user(Indent, [](const item_t& item) {
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

	InBlock = Advance >> ensure(Block, PopIndent);

	local_flag = expr('*') | expr('^');
	Local = key("local") >> ((Space >> local_flag) | NameList);

	colon_import_name = sym('\\') >> Space >> Variable;
	ImportName = colon_import_name | Space >> Variable;
	ImportNameList = Seperator >> *SpaceBreak >> ImportName >> *((+SpaceBreak | sym(',') >> *SpaceBreak) >> ImportName);

	import_literal_inner = (range('a', 'z') | range('A', 'Z') | set("_-")) >> *(AlphaNum | '-');
	import_literal_chain = Seperator >> import_literal_inner >> *(expr('.') >> import_literal_inner);
	ImportLiteral = sym('\'') >> import_literal_chain >> symx('\'') | sym('"') >> import_literal_chain >> symx('"');

	ImportFrom = ImportNameList >> *SpaceBreak >> key("from") >> Exp;
	ImportAs = ImportLiteral >> -(key("as") >> (Space >> Variable | TableLit));

	Import = key("import") >> (ImportAs | ImportFrom);
	BreakLoop = (expr("break") | expr("continue")) >> not_(AlphaNum);

	Return = key("return") >> -ExpListLow;
	WithExp = ExpList >> -Assign;

	With = key("with") >> DisableDo >> ensure(WithExp, PopDo) >> -key("do") >> Body;
	SwitchCase = key("when") >> ExpList >> -key("then") >> Body;
	SwitchElse = key("else") >> Body;

	SwitchBlock = *EmptyLine >>
		Advance >> Seperator >>
		SwitchCase >>
		*(+SpaceBreak >> SwitchCase) >>
		-(+SpaceBreak >> SwitchElse) >>
		PopIndent;

	Switch = key("switch") >>
		DisableDo >> ensure(Exp, PopDo) >>
		-key("do") >> -Space >> Break >> SwitchBlock;

	IfCond = Exp >> -Assign;
	IfElseIf = -(Break >> *EmptyLine >> CheckIndent) >> key("elseif") >> IfCond >> -key("then") >> Body;
	IfElse = -(Break >> *EmptyLine >> CheckIndent) >> key("else") >> Body;
	If = key("if") >> Seperator >> IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;
	Unless = key("unless") >> Seperator >> IfCond >> -key("then") >> Body >> *IfElseIf >> -IfElse;

	While = key("while") >> DisableDo >> ensure(Exp, PopDo) >> -key("do") >> Body;

	for_step_value = sym(',') >> White >> Exp;
	for_args = Space >> Variable >> sym('=') >> Exp >> sym(',') >> White >> Exp >> -for_step_value;

	For = key("for") >> DisableDo >>
		ensure(for_args, PopDo) >>
		-key("do") >> Body;

	for_in = star_exp | ExpList;

	ForEach = key("for") >> AssignableNameList >> White >> key("in") >>
		DisableDo >> ensure(for_in, PopDo) >>
		-key("do") >> Body;

	Do = pl::user(key("do"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->doStack.empty() || st->doStack.top();
	}) >> Body;

	DisableDo = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->doStack.push(false);
		return true;
	});

	PopDo = pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->doStack.pop();
		return true;
	});

	Comprehension = sym('[') >> Exp >> CompInner >> sym(']');
	comp_value = sym(',') >> Exp;
	TblComprehension = sym('{') >> Exp >> -comp_value >> CompInner >> sym('}');

	CompInner = Seperator >> (CompForEach | CompFor) >> *CompClause;
	star_exp = sym('*') >> Exp;
	CompForEach = key("for") >> AssignableNameList >> key("in") >> (star_exp | Exp);
	CompFor = key("for") >> Space >> Variable >> sym('=') >> Exp >> sym(',') >> White >> Exp >> -for_step_value;
	CompClause = CompFor | CompForEach | key("when") >> Exp;

	Assign = sym('=') >> Seperator >> (With | If | Switch | TableBlock | Exp >> *((sym(',') | sym(';')) >> White >> Exp));

	update_op =
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

	Update = Space >> update_op >> expr("=") >> Exp;

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
		set("+-*/%^><|&");

	BackcallOperator = expr("|>");

	Assignable = AssignableChain | Space >> Variable | SelfName;

	exp_op_value = Space >> (BackcallOperator | BinaryOperator) >> *SpaceBreak >> Value;
	Exp = Value >> *exp_op_value;

	ChainValue = Seperator >> (Chain | Callable) >> -existential_op >> -InvokeArgs;

	simple_table = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
	Value = SimpleValue | simple_table | ChainValue | String;

	single_string_inner = expr("\\'") | "\\\\" | not_(expr('\'')) >> Any;
	SingleString = symx('\'') >> *single_string_inner >> symx('\'');
	interp = symx("#{") >> Exp >> sym('}');
	double_string_plain = expr("\\\"") | "\\\\" | not_(expr('"')) >> Any;
	double_string_inner = +(not_(interp) >> double_string_plain);
	double_string_content = double_string_inner | interp;
	DoubleString = symx('"') >> Seperator >> *double_string_content >> symx('"');
	String = Space >> (DoubleString | SingleString | LuaString);

	lua_string_open = '[' >> *expr('=') >> '[';
	lua_string_close = ']' >> *expr('=') >> ']';

	LuaStringOpen = pl::user(lua_string_open, [](const item_t& item) {
		size_t count = std::distance(item.begin, item.end);
		State* st = reinterpret_cast<State*>(item.user_data);
		st->stringOpen = count;
		return true;
	});

	LuaStringClose = pl::user(lua_string_close, [](const item_t& item) {
		size_t count = std::distance(item.begin, item.end);
		State* st = reinterpret_cast<State*>(item.user_data);
		return st->stringOpen == count;
	});

	LuaStringContent = *(not_(LuaStringClose) >> Any);

	LuaString = LuaStringOpen >> -Break >> LuaStringContent >> LuaStringClose;

	Parens = sym('(') >> *SpaceBreak >> Exp >> *SpaceBreak >> sym(')');
	Callable = Space >> Variable | SelfName | VarArg | Parens;
	FnArgsExpList = Exp >> *((Break | sym(',')) >> White >> Exp);

	FnArgs = (symx('(') >> *SpaceBreak >> -FnArgsExpList >> *SpaceBreak >> sym(')')) |
		(sym('!') >> not_(expr('=')));

	existential_op = expr('?');
	chain_call = (Callable | String) >> -existential_op >> ChainItems;
	chain_item = and_(set(".\\")) >> ChainItems;
	chain_dot_chain = DotChainItem >> -existential_op >> -ChainItems;

	Chain = chain_call | chain_item |
		Space >> (chain_dot_chain | ColonChain);

	AssignableChain = Seperator >> Chain;

	chain_with_colon = +ChainItem >> -ColonChain;
	ChainItems = chain_with_colon | ColonChain;

	Index = symx('[') >> Exp >> sym(']');
	ChainItem = Invoke >> -existential_op | DotChainItem >> -existential_op | Slice | Index >> -existential_op;
	DotChainItem = symx('.') >> Name;
	ColonChainItem = symx('\\') >> (LuaKeyword | Name);
	invoke_chain = Invoke >> -existential_op >> -ChainItems;
	ColonChain = ColonChainItem >> -existential_op >> -invoke_chain;

	default_value = true_();
	Slice =
		symx('[') >>
		(Exp | default_value) >>
		sym(',') >>
		(Exp | default_value) >>
		(sym(',') >> Exp | default_value) >>
		sym(']');

	Invoke = Seperator >> (
		FnArgs |
		SingleString |
		DoubleString |
		and_(expr('[')) >> LuaString);

	TableValue = KeyValue | Exp;

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

	TableBlockInner = Seperator >> KeyValueLine >> *(+(SpaceBreak) >> KeyValueLine);
	TableBlock = +(SpaceBreak) >> Advance >> ensure(TableBlockInner, PopIndent);

	class_member_list = Seperator >> KeyValue >> *(sym(',') >> KeyValue);
	ClassLine = CheckIndent >> (class_member_list | Statement) >> -sym(',');
	ClassBlock = +(SpaceBreak) >> Advance >>Seperator >> ClassLine >> *(+(SpaceBreak) >> ClassLine) >> PopIndent;

	ClassDecl =
		key("class") >> not_(expr(':')) >>
		-Assignable >>
		-(key("extends")  >> PreventIndent >> ensure(Exp, PopIndent)) >>
		-ClassBlock;

	global_values = NameList >> -(sym('=') >> ExpListLow);
	global_op = expr('*') | expr('^');
	Global = key("global") >> (ClassDecl | (Space >> global_op) | global_values);

	export_default = key("default");

	Export = pl::user(key("export"), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		st->exportCount++;
		return true;
	}) >> ((pl::user(export_default, [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		bool isValid = !st->exportDefault && st->exportCount == 1;
		st->exportDefault = true;
		return isValid;
	}) >> Exp)
	| (pl::user(true_(), [](const item_t& item) {
		State* st = reinterpret_cast<State*>(item.user_data);
		if (st->exportDefault && st->exportCount > 1) {
			return false;
		} else {
			return true;
		}
	}) >> ExpList >> -Assign)) >> not_(Space >> statement_appendix);

	variable_pair = sym(':') >> Variable;

	normal_pair = (
		KeyName |
		sym('[') >> Exp >> sym(']') |
		Space >> DoubleString |
		Space >> SingleString
	) >>
	symx(':') >>
	(Exp | TableBlock | +(SpaceBreak) >> Exp);

	KeyValue = variable_pair | normal_pair;

	KeyValueList = KeyValue >> *(sym(',') >> KeyValue);
	KeyValueLine = CheckIndent >> KeyValueList >> -sym(',');

	FnArgDef = (Space >> Variable | SelfName) >> -(sym('=') >> Exp);

	FnArgDefList = Seperator >> (
		(
			FnArgDef >>
			*((sym(',') | Break) >> White >> FnArgDef) >>
			-((sym(',') | Break) >> White >> VarArg)
		) | (
			VarArg
		)
	);

	outer_var_shadow = key("using") >> (NameList | Space >> expr("nil"));

	FnArgsDef = sym('(') >> White >> -FnArgDefList >> -outer_var_shadow >> White >> sym(')');
	fn_arrow = expr("->") | expr("=>");
	FunLit = -FnArgsDef >> Space >> fn_arrow >> -Body;

	NameList = Seperator >> Space >> Variable >> *(sym(',') >> White >> Variable);
	NameOrDestructure = Space >> Variable | TableLit;
	AssignableNameList = Seperator >> NameOrDestructure >> *(sym(',') >> White >> NameOrDestructure);

	fn_arrow_back = expr('<') >> set("-=");
	Backcall = -FnArgsDef >> Space >> fn_arrow_back >> Space >> ChainValue;

	ExpList = Seperator >> Exp >> *(sym(',') >> White >> Exp);
	ExpListLow = Seperator >> Exp >> *((sym(',') | sym(';')) >> White >> Exp);

	ArgLine = CheckIndent >> Exp >> *(sym(',') >> Exp);
	ArgBlock = ArgLine >> *(sym(',') >> SpaceBreak >> ArgLine) >> PopIndent;

	invoke_args_with_table =
		sym(',') >>
		(
			TableBlock |
			SpaceBreak >> Advance >> ArgBlock >> -TableBlock
		);

	InvokeArgs =
		not_(expr('-')) >> Seperator >>
		(
			Exp >> *(sym(',') >> Exp) >> -(invoke_args_with_table | TableBlock) |
			TableBlock
		);

	const_value = (expr("nil") | expr("true") | expr("false")) >> not_(AlphaNum);
	minus_exp = expr('-') >> not_(set(" \t")) >> Exp;
	sharp_exp = expr('#') >> Exp;
	tilde_exp = expr('~') >> Exp;
	not_exp = expr("not") >> not_(AlphaNum) >> Exp;
	unary_exp = minus_exp | sharp_exp | tilde_exp | not_exp;

	SimpleValue =
		(Space >> const_value) |
		If | Unless | Switch | With | ClassDecl | ForEach | For | While | Do |
		(Space >> unary_exp) |
		TblComprehension | TableLit | Comprehension | FunLit |
		(Space >> Num);

	ExpListAssign = ExpList >> -(Update | Assign);

	if_else_line = key("if") >> Exp >> -Assign >> (key("else") >> Exp | default_value);
	unless_line = key("unless") >> Exp;

	statement_appendix = (if_else_line | unless_line | CompInner) >> Space;
	Statement = (
		Import | While | For | ForEach |
		Return | Local | Global | Export | Space >> BreakLoop |
		Backcall | ExpListAssign
	) >> Space >>
	-statement_appendix;

	Body = Space >> Break >> *EmptyLine >> InBlock | Statement;

	empty_line_stop = Space >> and_(Stop);
	Line = CheckIndent >> Statement | empty_line_stop;
	Block = Seperator >> Line >> *(+Break >> Line);

	Shebang = expr("#!") >> *(not_(Stop) >> Any);
	File = White >> -Shebang >> Block >> eof();
}

ParseInfo MoonParser::parse(std::string_view codes, rule& r) {
	ParseInfo res;
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
		}
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
					buf << res.errorMessage("Syntax error."sv, &err);
					break;
				case ERROR_TYPE::ERROR_INVALID_EOF:
					buf << res.errorMessage("Invalid EOF."sv, &err);
					break;
			}
		}
		res.error = buf.str();
	}
	return res;
}

std::string MoonParser::toString(ast_node* node) {
	return _converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
}

std::string MoonParser::toString(input::iterator begin, input::iterator end) {
	return _converter.to_bytes(std::wstring(begin, end));
}

input MoonParser::encode(std::string_view codes) {
	return _converter.from_bytes(&codes.front(), &codes.back() + 1);
}

std::string MoonParser::decode(const input& codes) {
	return _converter.to_bytes(codes);
}

namespace Utils {
	void replace(std::string& str, std::string_view from, std::string_view to) {
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.size(), to);
			start_pos += to.size();
		}
	}
}

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
	for (int i = 0; i < oldCol; ++i) {
		if (*it > ASCII) {
			++col;
		}
		++it;
	}
	auto line = Converter{}.to_bytes(std::wstring(begin, end));
	Utils::replace(line, "\t"sv, " "sv);
	std::ostringstream buf;
	buf << loc->m_begin.m_line << ": "sv << msg <<
		'\n' << line << '\n' << std::string(col, ' ') << "^"sv;
	return buf.str();
}

} // namespace MoonP
