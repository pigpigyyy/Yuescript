/* Copyright (c) 2021 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <string>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <list>
#include <sstream>
#include <string_view>

#include "MoonP/ast.hpp"
#include "MoonP/moon_ast.h"

namespace MoonP {
using namespace parserlib;

struct ParseInfo {
	ast_ptr<false, ast_node> node;
	std::string error;
	std::unique_ptr<input> codes;
	bool exportDefault = false;
	std::string moduleName;
	std::string errorMessage(std::string_view msg, const input_range* loc) const;
};

template<typename T>
struct identity { typedef T type; };

#define AST_RULE(type) \
	rule type; \
	ast<type##_t> type##_impl = type; \
	inline rule& getRule(identity<type##_t>) { return type; }

extern std::unordered_set<std::string> LuaKeywords;
extern std::unordered_set<std::string> Keywords;

class MoonParser {
public:
	MoonParser();

	template<class AST>
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
		int exportCount = 0;
		int moduleFix = 0;
		size_t stringOpen = 0;
		std::string moduleName = "_module_0";
		std::string buffer;
		std::stack<int> indents;
		std::stack<bool> doStack;
		std::stack<bool> chainBlockStack;
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
		return Cut;
	}

	rule plain_space;
	rule Break;
	rule Any;
	rule White;
	rule Stop;
	rule Comment;
	rule multi_line_open;
	rule multi_line_close;
	rule multi_line_content;
	rule MultiLineComment;
	rule Indent;
	rule EscapeNewLine;
	rule space_one;
	rule Space;
	rule SpaceBreak;
	rule EmptyLine;
	rule AlphaNum;
	rule Cut;
	rule check_indent;
	rule CheckIndent;
	rule advance;
	rule Advance;
	rule push_indent;
	rule PushIndent;
	rule PreventIndent;
	rule PopIndent;
	rule InBlock;
	rule ImportName;
	rule ImportNameList;
	rule import_literal_chain;
	rule ImportTabItem;
	rule ImportTabList;
	rule ImportTabLine;
	rule import_tab_lines;
	rule WithExp;
	rule DisableDo;
	rule PopDo;
	rule SwitchElse;
	rule SwitchBlock;
	rule IfElseIf;
	rule IfElse;
	rule for_args;
	rule for_in;
	rule CompClause;
	rule Chain;
	rule KeyValue;
	rule single_string_inner;
	rule interp;
	rule double_string_plain;
	rule lua_string_open;
	rule lua_string_close;
	rule FnArgsExpList;
	rule FnArgs;
	rule macro_args_def;
	rule chain_call;
	rule chain_item;
	rule ChainItems;
	rule chain_dot_chain;
	rule ColonChain;
	rule chain_with_colon;
	rule ChainItem;
	rule chain_line;
	rule chain_block;
	rule DisableChainBlock;
	rule PopChainBlock;
	rule Index;
	rule invoke_chain;
	rule TableValue;
	rule table_lit_lines;
	rule TableLitLine;
	rule TableValueList;
	rule TableBlockInner;
	rule ClassLine;
	rule KeyValueLine;
	rule KeyValueList;
	rule ArgLine;
	rule ArgBlock;
	rule invoke_args_with_table;
	rule BackcallOperator;
	rule ExponentialOperator;
	rule backcall_value;
	rule backcall_exp;
	rule expo_value;
	rule expo_exp;
	rule empty_line_stop;
	rule Line;
	rule Shebang;

	AST_RULE(Num)
	AST_RULE(Name)
	AST_RULE(Variable)
	AST_RULE(LabelName)
	AST_RULE(LuaKeyword)
	AST_RULE(self)
	AST_RULE(self_name)
	AST_RULE(self_class)
	AST_RULE(self_class_name)
	AST_RULE(SelfName)
	AST_RULE(KeyName)
	AST_RULE(VarArg)
	AST_RULE(Seperator)
	AST_RULE(NameList)
	AST_RULE(local_flag)
	AST_RULE(local_values)
	AST_RULE(Local)
	AST_RULE(LocalAttrib);
	AST_RULE(colon_import_name)
	AST_RULE(import_literal_inner)
	AST_RULE(ImportLiteral)
	AST_RULE(ImportFrom)
	AST_RULE(macro_name_pair)
	AST_RULE(import_all_macro)
	AST_RULE(ImportTabLit)
	AST_RULE(ImportAs)
	AST_RULE(Import)
	AST_RULE(Label)
	AST_RULE(Goto)
	AST_RULE(fn_arrow_back)
	AST_RULE(Backcall)
	AST_RULE(BackcallBody)
	AST_RULE(ExpListLow)
	AST_RULE(ExpList)
	AST_RULE(Return)
	AST_RULE(With)
	AST_RULE(SwitchCase)
	AST_RULE(Switch)
	AST_RULE(IfCond)
	AST_RULE(If)
	AST_RULE(Unless)
	AST_RULE(While)
	AST_RULE(Repeat)
	AST_RULE(for_step_value)
	AST_RULE(For)
	AST_RULE(ForEach)
	AST_RULE(Do)
	AST_RULE(Comprehension)
	AST_RULE(comp_value)
	AST_RULE(TblComprehension)
	AST_RULE(star_exp)
	AST_RULE(CompForEach)
	AST_RULE(CompFor)
	AST_RULE(CompInner)
	AST_RULE(Assign)
	AST_RULE(update_op)
	AST_RULE(Update)
	AST_RULE(BinaryOperator)
	AST_RULE(unary_operator)
	AST_RULE(Assignable)
	AST_RULE(AssignableChain)
	AST_RULE(exp_op_value)
	AST_RULE(Exp)
	AST_RULE(Callable)
	AST_RULE(ChainValue)
	AST_RULE(simple_table)
	AST_RULE(SimpleValue)
	AST_RULE(Value)
	AST_RULE(LuaStringOpen);
	AST_RULE(LuaStringContent);
	AST_RULE(LuaStringClose);
	AST_RULE(LuaString)
	AST_RULE(SingleString)
	AST_RULE(double_string_inner)
	AST_RULE(double_string_content)
	AST_RULE(DoubleString)
	AST_RULE(String)
	AST_RULE(Parens)
	AST_RULE(DotChainItem)
	AST_RULE(ColonChainItem)
	AST_RULE(default_value)
	AST_RULE(Slice)
	AST_RULE(Invoke)
	AST_RULE(existential_op)
	AST_RULE(TableLit)
	AST_RULE(TableBlock)
	AST_RULE(TableBlockIndent)
	AST_RULE(class_member_list)
	AST_RULE(ClassBlock)
	AST_RULE(ClassDecl)
	AST_RULE(global_values)
	AST_RULE(global_op)
	AST_RULE(Global)
	AST_RULE(export_default)
	AST_RULE(Export)
	AST_RULE(variable_pair)
	AST_RULE(normal_pair)
	AST_RULE(FnArgDef)
	AST_RULE(FnArgDefList)
	AST_RULE(outer_var_shadow)
	AST_RULE(FnArgsDef)
	AST_RULE(fn_arrow)
	AST_RULE(FunLit)
	AST_RULE(MacroName)
	AST_RULE(MacroLit)
	AST_RULE(Macro)
	AST_RULE(NameOrDestructure)
	AST_RULE(AssignableNameList)
	AST_RULE(InvokeArgs)
	AST_RULE(const_value)
	AST_RULE(unary_value)
	AST_RULE(unary_exp)
	AST_RULE(ExpListAssign)
	AST_RULE(if_line)
	AST_RULE(unless_line)
	AST_RULE(BreakLoop)
	AST_RULE(statement_appendix)
	AST_RULE(statement_sep)
	AST_RULE(Statement)
	AST_RULE(Body)
	AST_RULE(Block)
	AST_RULE(File)
};

namespace Utils {
	void replace(std::string& str, std::string_view from, std::string_view to);
	void trim(std::string& str);
};

} // namespace MoonP
