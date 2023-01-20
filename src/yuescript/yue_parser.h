/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_set>

#include "yuescript/ast.hpp"
#include "yuescript/yue_ast.h"

namespace yue {
using namespace std::string_view_literals;
using namespace std::string_literals;
using namespace parserlib;

struct ParseInfo {
	ast_ptr<false, ast_node> node;
	std::string error;
	std::unique_ptr<input> codes;
	bool exportDefault = false;
	bool exportMacro = false;
	std::string moduleName;
	std::string errorMessage(std::string_view msg, const input_range* loc) const;
};

class ParserError : public std::logic_error {
public:
	explicit ParserError(const std::string& msg, const pos& begin, const pos& end)
		: std::logic_error(msg)
		, loc(begin, end) { }

	explicit ParserError(const char* msg, const pos& begin, const pos& end)
		: std::logic_error(msg)
		, loc(begin, end) { }
	input_range loc;
};

template <typename T>
struct identity {
	typedef T type;
};

#define AST_RULE(type) \
	rule type; \
	ast<type##_t> type##_impl = type; \
	inline rule& getRule(identity<type##_t>) { return type; }

extern std::unordered_set<std::string> LuaKeywords;
extern std::unordered_set<std::string> Keywords;

class YueParser {
public:
	YueParser();

	template <class AST>
	ParseInfo parse(std::string_view codes) {
		return parse(codes, getRule<AST>());
	}

	template <class AST>
	bool match(std::string_view codes) {
		auto rEnd = rule(getRule<AST>() >> eof());
		return parse(codes, rEnd).node;
	}

	std::string toString(ast_node* node);
	std::string toString(input::iterator begin, input::iterator end);

	input encode(std::string_view input);
	std::string decode(const input& input);

protected:
	ParseInfo parse(std::string_view codes, rule& r);

	struct State {
		State() {
			indents.push(0);
		}
		bool exportDefault = false;
		bool exportMacro = false;
		int exportCount = 0;
		int moduleFix = 0;
		size_t stringOpen = 0;
		std::string moduleName = "_module_0"s;
		std::string buffer;
		std::stack<int> indents;
		std::stack<bool> noDoStack;
		std::stack<bool> noChainBlockStack;
		std::stack<bool> noTableBlockStack;
	};

	template <class T>
	inline rule& getRule() {
		return getRule(identity<T>());
	}

private:
	Converter _converter;

	template <class T>
	inline rule& getRule(identity<T>) {
		assert(false);
		return cut;
	}

	rule empty_block_error;
	rule leading_spaces_error;
	rule indentation_error;

	rule num_char;
	rule num_char_hex;
	rule num_lit;
	rule num_expo;
	rule num_expo_hex;
	rule lj_num;
	rule plain_space;
	rule line_break;
	rule any_char;
	rule white;
	rule stop;
	rule comment;
	rule multi_line_open;
	rule multi_line_close;
	rule multi_line_content;
	rule multi_line_comment;
	rule escape_new_line;
	rule space_one;
	rule space;
	rule space_break;
	rule alpha_num;
	rule not_alpha_num;
	rule cut;
	rule check_indent_match;
	rule check_indent;
	rule advance_match;
	rule advance;
	rule push_indent_match;
	rule push_indent;
	rule prevent_indent;
	rule pop_indent;
	rule in_block;
	rule import_name;
	rule import_name_list;
	rule import_literal_chain;
	rule import_tab_item;
	rule import_tab_list;
	rule import_tab_line;
	rule import_tab_lines;
	rule with_exp;
	rule disable_do;
	rule enable_do;
	rule disable_chain;
	rule enable_chain;
	rule disable_do_chain_arg_table_block;
	rule enable_do_chain_arg_table_block;
	rule disable_arg_table_block;
	rule enable_arg_table_block;
	rule switch_else;
	rule switch_block;
	rule if_else_if;
	rule if_else;
	rule for_args;
	rule for_in;
	rule comp_clause;
	rule chain;
	rule chain_list;
	rule key_value;
	rule single_string_inner;
	rule interp;
	rule double_string_plain;
	rule lua_string_open;
	rule lua_string_close;
	rule fn_args_exp_list;
	rule fn_args;
	rule destruct_def;
	rule macro_args_def;
	rule chain_call;
	rule chain_call_list;
	rule chain_index_chain;
	rule chain_items;
	rule chain_dot_chain;
	rule colon_chain;
	rule chain_with_colon;
	rule chain_item;
	rule chain_line;
	rule chain_block;
	rule meta_index;
	rule index;
	rule invoke_chain;
	rule table_value;
	rule table_lit_lines;
	rule table_lit_line;
	rule table_value_list;
	rule table_block_inner;
	rule class_line;
	rule key_value_line;
	rule key_value_list;
	rule arg_line;
	rule arg_block;
	rule invoke_args_with_table;
	rule arg_table_block;
	rule pipe_operator;
	rule exponential_operator;
	rule pipe_value;
	rule pipe_exp;
	rule expo_value;
	rule expo_exp;
	rule exp_not_tab;
	rule local_const_item;
	rule empty_line_break;
	rule yue_comment;
	rule yue_line_comment;
	rule yue_multiline_comment;
	rule line;
	rule shebang;

	AST_RULE(Num)
	AST_RULE(Name)
	AST_RULE(Variable)
	AST_RULE(LabelName)
	AST_RULE(LuaKeyword)
	AST_RULE(Self)
	AST_RULE(SelfName)
	AST_RULE(SelfClass)
	AST_RULE(SelfClassName)
	AST_RULE(SelfItem)
	AST_RULE(KeyName)
	AST_RULE(VarArg)
	AST_RULE(Seperator)
	AST_RULE(NameList)
	AST_RULE(LocalFlag)
	AST_RULE(LocalValues)
	AST_RULE(Local)
	AST_RULE(ConstAttrib)
	AST_RULE(CloseAttrib)
	AST_RULE(LocalAttrib);
	AST_RULE(ColonImportName)
	AST_RULE(ImportLiteralInner)
	AST_RULE(ImportLiteral)
	AST_RULE(ImportFrom)
	AST_RULE(MacroNamePair)
	AST_RULE(ImportAllMacro)
	AST_RULE(ImportTabLit)
	AST_RULE(ImportAs)
	AST_RULE(Import)
	AST_RULE(Label)
	AST_RULE(Goto)
	AST_RULE(ShortTabAppending)
	AST_RULE(FnArrowBack)
	AST_RULE(Backcall)
	AST_RULE(PipeBody)
	AST_RULE(ExpListLow)
	AST_RULE(ExpList)
	AST_RULE(Return)
	AST_RULE(With)
	AST_RULE(SwitchList)
	AST_RULE(SwitchCase)
	AST_RULE(Switch)
	AST_RULE(Assignment)
	AST_RULE(IfCond)
	AST_RULE(IfType)
	AST_RULE(If)
	AST_RULE(WhileType)
	AST_RULE(While)
	AST_RULE(Repeat)
	AST_RULE(ForStepValue)
	AST_RULE(For)
	AST_RULE(ForEach)
	AST_RULE(Do)
	AST_RULE(CatchBlock)
	AST_RULE(Try)
	AST_RULE(Comprehension)
	AST_RULE(CompValue)
	AST_RULE(TblComprehension)
	AST_RULE(StarExp)
	AST_RULE(CompForEach)
	AST_RULE(CompFor)
	AST_RULE(CompInner)
	AST_RULE(Assign)
	AST_RULE(UpdateOp)
	AST_RULE(Update)
	AST_RULE(BinaryOperator)
	AST_RULE(UnaryOperator)
	AST_RULE(Assignable)
	AST_RULE(AssignableChain)
	AST_RULE(ExpOpValue)
	AST_RULE(Exp)
	AST_RULE(Callable)
	AST_RULE(ChainValue)
	AST_RULE(SimpleTable)
	AST_RULE(SimpleValue)
	AST_RULE(Value)
	AST_RULE(LuaStringOpen);
	AST_RULE(LuaStringContent);
	AST_RULE(LuaStringClose);
	AST_RULE(LuaString)
	AST_RULE(SingleString)
	AST_RULE(DoubleStringInner)
	AST_RULE(DoubleStringContent)
	AST_RULE(DoubleString)
	AST_RULE(String)
	AST_RULE(Parens)
	AST_RULE(DotChainItem)
	AST_RULE(ColonChainItem)
	AST_RULE(Metatable)
	AST_RULE(Metamethod)
	AST_RULE(DefaultValue)
	AST_RULE(Slice)
	AST_RULE(Invoke)
	AST_RULE(ExistentialOp)
	AST_RULE(TableAppendingOp)
	AST_RULE(SpreadExp)
	AST_RULE(TableLit)
	AST_RULE(TableBlock)
	AST_RULE(TableBlockIndent)
	AST_RULE(ClassMemberList)
	AST_RULE(ClassBlock)
	AST_RULE(ClassDecl)
	AST_RULE(GlobalValues)
	AST_RULE(GlobalOp)
	AST_RULE(Global)
	AST_RULE(ExportDefault)
	AST_RULE(Export)
	AST_RULE(VariablePair)
	AST_RULE(NormalPair)
	AST_RULE(MetaVariablePair)
	AST_RULE(MetaNormalPair)
	AST_RULE(VariablePairDef)
	AST_RULE(NormalPairDef)
	AST_RULE(NormalDef)
	AST_RULE(MetaVariablePairDef)
	AST_RULE(MetaNormalPairDef)
	AST_RULE(FnArgDef)
	AST_RULE(FnArgDefList)
	AST_RULE(OuterVarShadow)
	AST_RULE(FnArgsDef)
	AST_RULE(FnArrow)
	AST_RULE(FunLit)
	AST_RULE(MacroName)
	AST_RULE(MacroLit)
	AST_RULE(Macro)
	AST_RULE(MacroInPlace)
	AST_RULE(NameOrDestructure)
	AST_RULE(AssignableNameList)
	AST_RULE(InvokeArgs)
	AST_RULE(ConstValue)
	AST_RULE(UnaryValue)
	AST_RULE(UnaryExp)
	AST_RULE(ExpListAssign)
	AST_RULE(IfLine)
	AST_RULE(WhileLine)
	AST_RULE(BreakLoop)
	AST_RULE(StatementAppendix)
	AST_RULE(StatementSep)
	AST_RULE(Statement)
	AST_RULE(YueLineComment)
	AST_RULE(YueMultilineComment)
	AST_RULE(ChainAssign)
	AST_RULE(Body)
	AST_RULE(Block)
	AST_RULE(BlockEnd)
	AST_RULE(File)
};

namespace Utils {
void replace(std::string& str, std::string_view from, std::string_view to);
void trim(std::string& str);
} // namespace Utils

} // namespace yue
