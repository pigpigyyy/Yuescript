/* Copyright (c) 2021 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "yuescript/ast.hpp"

namespace parserlib {

#define AST_LEAF(type) \
COUNTER_INC; \
class type##_t : public ast_node \
{ \
public: \
	virtual int getId() const override { return COUNTER_READ; }

#define AST_NODE(type) \
COUNTER_INC; \
class type##_t : public ast_container \
{ \
public: \
	virtual int getId() const override { return COUNTER_READ; }

#define AST_MEMBER(type, ...) \
	type##_t() { \
		add_members({__VA_ARGS__}); \
	}

#define AST_END(type) \
}; \
template<> constexpr int id<type##_t>() { return COUNTER_READ; }

AST_LEAF(Num)
AST_END(Num)

AST_LEAF(Name)
AST_END(Name)

AST_NODE(Variable)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(Variable, &name)
AST_END(Variable)

AST_NODE(LabelName)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(LabelName, &name)
AST_END(LabelName)

AST_NODE(LuaKeyword)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(LuaKeyword, &name)
AST_END(LuaKeyword)

AST_LEAF(self)
AST_END(self)

AST_NODE(self_name)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(self_name, &name)
AST_END(self_name)

AST_LEAF(self_class)
AST_END(self_class)

AST_NODE(self_class_name)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(self_class_name, &name)
AST_END(self_class_name)

AST_NODE(SelfName)
	ast_sel<true, self_class_name_t, self_class_t, self_name_t, self_t> name;
	AST_MEMBER(SelfName, &name)
AST_END(SelfName)

AST_NODE(KeyName)
	ast_sel<true, SelfName_t, Name_t> name;
	AST_MEMBER(KeyName, &name)
AST_END(KeyName)

AST_LEAF(VarArg)
AST_END(VarArg)

AST_LEAF(local_flag)
AST_END(local_flag)

AST_LEAF(Seperator)
AST_END(Seperator)

AST_NODE(NameList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Variable_t> names;
	AST_MEMBER(NameList, &sep, &names)
AST_END(NameList)

class ExpListLow_t;
class TableBlock_t;

AST_NODE(local_values)
	ast_ptr<true, NameList_t> nameList;
	ast_sel<false, TableBlock_t, ExpListLow_t> valueList;
	AST_MEMBER(local_values, &nameList, &valueList)
AST_END(local_values)

AST_NODE(Local)
	ast_sel<true, local_flag_t, local_values_t> item;
	std::list<std::string> forceDecls;
	std::list<std::string> decls;
	bool collected = false;
	bool defined = false;
	AST_MEMBER(Local, &item)
AST_END(Local)

class Assign_t;

AST_NODE(LocalAttrib)
	ast_ptr<true, Name_t> attrib;
	ast_ptr<true, NameList_t> nameList;
	ast_ptr<true, Assign_t> assign;
	AST_MEMBER(LocalAttrib, &attrib, &nameList, &assign)
AST_END(LocalAttrib)

AST_NODE(colon_import_name)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(colon_import_name, &name)
AST_END(colon_import_name)

class Exp_t;
class TableLit_t;

AST_LEAF(import_literal_inner)
AST_END(import_literal_inner)

AST_NODE(ImportLiteral)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, import_literal_inner_t> inners;
	AST_MEMBER(ImportLiteral, &sep, &inners)
AST_END(ImportLiteral)

AST_NODE(ImportFrom)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, colon_import_name_t, Variable_t> names;
	ast_ptr<true, Exp_t> exp;
	AST_MEMBER(ImportFrom, &sep, &names, &exp)
AST_END(ImportFrom)

class MacroName_t;

AST_NODE(macro_name_pair)
	ast_ptr<true, MacroName_t> key;
	ast_ptr<true, MacroName_t> value;
	AST_MEMBER(macro_name_pair, &key, &value)
AST_END(macro_name_pair)

AST_LEAF(import_all_macro)
AST_END(import_all_macro)

class variable_pair_t;
class normal_pair_t;
class meta_variable_pair_t;
class meta_normal_pair_t;

AST_NODE(ImportTabLit)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t, MacroName_t, macro_name_pair_t, import_all_macro_t, Exp_t, meta_variable_pair_t, meta_normal_pair_t> items;
	AST_MEMBER(ImportTabLit, &sep, &items)
AST_END(ImportTabLit)

AST_NODE(ImportAs)
	ast_ptr<true, ImportLiteral_t> literal;
	ast_sel<false, Variable_t, ImportTabLit_t> target;
	AST_MEMBER(ImportAs, &literal, &target)
AST_END(ImportAs)

AST_NODE(Import)
	ast_sel<true, ImportAs_t, ImportFrom_t> content;
	AST_MEMBER(Import, &content)
AST_END(Import)

AST_NODE(Label)
	ast_ptr<true, LabelName_t> label;
	AST_MEMBER(Label, &label)
AST_END(Label)

AST_NODE(Goto)
	ast_ptr<true, LabelName_t> label;
	AST_MEMBER(Goto, &label)
AST_END(Goto)

class FnArgsDef_t;

AST_LEAF(fn_arrow_back)
AST_END(fn_arrow_back)

class ChainValue_t;

AST_NODE(Backcall)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_ptr<true, fn_arrow_back_t> arrow;
	ast_ptr<true, ChainValue_t> value;
	AST_MEMBER(Backcall, &argsDef, &arrow, &value)
AST_END(Backcall)

AST_NODE(ExpListLow)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	AST_MEMBER(ExpListLow, &sep, &exprs)
AST_END(ExpListLow)

AST_NODE(ExpList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	AST_MEMBER(ExpList, &sep, &exprs)
AST_END(ExpList)

AST_NODE(Return)
	bool allowBlockMacroReturn = false;
	ast_ptr<false, ExpListLow_t> valueList;
	AST_MEMBER(Return, &valueList)
AST_END(Return)

class existential_op_t;
class Assign_t;
class Block_t;
class Statement_t;

AST_NODE(With)
	ast_ptr<false, existential_op_t> eop;
	ast_ptr<true, ExpList_t> valueList;
	ast_ptr<false, Assign_t> assigns;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(With, &eop, &valueList, &assigns, &body)
AST_END(With)

AST_NODE(SwitchCase)
	ast_ptr<true, ExpList_t> valueList;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(SwitchCase, &valueList, &body)
AST_END(SwitchCase)

AST_NODE(Switch)
	ast_ptr<true, Exp_t> target;
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, SwitchCase_t> branches;
	ast_sel<false, Block_t, Statement_t> lastBranch;
	AST_MEMBER(Switch, &target, &sep, &branches, &lastBranch)
AST_END(Switch)

AST_NODE(IfCond)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<false, Assign_t> assign;
	AST_MEMBER(IfCond, &condition, &assign)
AST_END(IfCond)

AST_LEAF(IfType)
AST_END(IfType)

AST_NODE(If)
	ast_ptr<true, IfType_t> type;
	ast_sel_list<true, IfCond_t, Block_t, Statement_t> nodes;
	AST_MEMBER(If, &type, &nodes)
AST_END(If)

AST_LEAF(WhileType)
AST_END(WhileType)

AST_NODE(While)
	ast_ptr<true, WhileType_t> type;
	ast_ptr<true, Exp_t> condition;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(While, &type, &condition, &body)
AST_END(While)

class Body_t;

AST_NODE(Repeat)
	ast_ptr<true, Body_t> body;
	ast_ptr<true, Exp_t> condition;
	AST_MEMBER(Repeat, &body, &condition)
AST_END(Repeat)

AST_NODE(for_step_value)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(for_step_value, &value)
AST_END(for_step_value)

AST_NODE(For)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, for_step_value_t> stepValue;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(For, &varName, &startValue, &stopValue, &stepValue, &body)
AST_END(For)

class AssignableNameList_t;
class star_exp_t;

AST_NODE(ForEach)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, star_exp_t, ExpList_t> loopValue;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(ForEach, &nameList, &loopValue, &body)
AST_END(ForEach)

AST_NODE(Do)
	ast_ptr<true, Body_t> body;
	AST_MEMBER(Do, &body)
AST_END(Do)

class CompInner_t;
class Statement_t;

AST_NODE(Comprehension)
	ast_sel<true, Exp_t, Statement_t> value;
	ast_ptr<true, CompInner_t> forLoop;
	AST_MEMBER(Comprehension, &value, &forLoop)
AST_END(Comprehension)

AST_NODE(comp_value)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(comp_value, &value)
AST_END(comp_value)

AST_NODE(TblComprehension)
	ast_ptr<true, Exp_t> key;
	ast_ptr<false, comp_value_t> value;
	ast_ptr<true, CompInner_t> forLoop;
	AST_MEMBER(TblComprehension, &key, &value, &forLoop)
AST_END(TblComprehension)

AST_NODE(star_exp)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(star_exp, &value)
AST_END(star_exp)

AST_NODE(CompForEach)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, star_exp_t, Exp_t> loopValue;
	AST_MEMBER(CompForEach, &nameList, &loopValue)
AST_END(CompForEach)

AST_NODE(CompFor)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, for_step_value_t> stepValue;
	AST_MEMBER(CompFor, &varName, &startValue, &stopValue, &stepValue)
AST_END(CompFor)

AST_NODE(CompInner)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, CompFor_t, CompForEach_t, Exp_t> items;
	AST_MEMBER(CompInner, &sep, &items)
AST_END(CompInner)

class TableBlock_t;

AST_NODE(Assign)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, With_t, If_t, Switch_t, TableBlock_t, Exp_t> values;
	AST_MEMBER(Assign, &sep, &values)
AST_END(Assign)

AST_LEAF(update_op)
AST_END(update_op)

AST_NODE(Update)
	ast_ptr<true, update_op_t> op;
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(Update, &op, &value)
AST_END(Update)

AST_LEAF(BinaryOperator)
AST_END(BinaryOperator)

AST_LEAF(unary_operator)
AST_END(unary_operator)

class AssignableChain_t;

AST_NODE(Assignable)
	ast_sel<true, AssignableChain_t, Variable_t, SelfName_t> item;
	AST_MEMBER(Assignable, &item)
AST_END(Assignable)

class unary_exp_t;

AST_NODE(exp_op_value)
	ast_ptr<true, BinaryOperator_t> op;
	ast_list<true, unary_exp_t> pipeExprs;
	AST_MEMBER(exp_op_value, &op, &pipeExprs)
AST_END(exp_op_value)

AST_NODE(Exp)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, unary_exp_t> pipeExprs;
	ast_list<false, exp_op_value_t> opValues;
	AST_MEMBER(Exp, &sep, &pipeExprs, &opValues)
AST_END(Exp)

class Parens_t;
class MacroName_t;

AST_NODE(Callable)
	ast_sel<true, Variable_t, SelfName_t, VarArg_t, Parens_t, MacroName_t> item;
	AST_MEMBER(Callable, &item)
AST_END(Callable)

AST_NODE(variable_pair)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(variable_pair, &name)
AST_END(variable_pair)

class DoubleString_t;
class SingleString_t;
class LuaString_t;

AST_NODE(normal_pair)
	ast_sel<true, KeyName_t, Exp_t, DoubleString_t, SingleString_t, LuaString_t> key;
	ast_sel<true, Exp_t, TableBlock_t> value;
	AST_MEMBER(normal_pair, &key, &value)
AST_END(normal_pair)

AST_NODE(meta_variable_pair)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(meta_variable_pair, &name)
AST_END(meta_variable_pair)

AST_NODE(meta_normal_pair)
	ast_sel<false, Name_t, Exp_t> key;
	ast_sel<true, Exp_t, TableBlock_t> value;
	AST_MEMBER(meta_normal_pair, &key, &value)
AST_END(meta_normal_pair)

AST_NODE(simple_table)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, variable_pair_t, normal_pair_t, meta_variable_pair_t, meta_normal_pair_t> pairs;
	AST_MEMBER(simple_table, &sep, &pairs)
AST_END(simple_table)

class String_t;
class const_value_t;
class ClassDecl_t;
class unary_value_t;
class TableLit_t;
class FunLit_t;

AST_NODE(SimpleValue)
	ast_sel<true, const_value_t,
	If_t, Switch_t, With_t, ClassDecl_t,
	ForEach_t, For_t, While_t, Do_t,
	unary_value_t,
	TblComprehension_t, TableLit_t, Comprehension_t,
	FunLit_t, Num_t> value;
	AST_MEMBER(SimpleValue, &value)
AST_END(SimpleValue)

AST_LEAF(LuaStringOpen)
AST_END(LuaStringOpen)

AST_LEAF(LuaStringContent)
AST_END(LuaStringContent)

AST_LEAF(LuaStringClose)
AST_END(LuaStringClose)

AST_NODE(LuaString)
	ast_ptr<true, LuaStringOpen_t> open;
	ast_ptr<true, LuaStringContent_t> content;
	ast_ptr<true, LuaStringClose_t> close;
	AST_MEMBER(LuaString, &open, &content, &close)
AST_END(LuaString)

AST_LEAF(SingleString)
AST_END(SingleString)

AST_LEAF(double_string_inner)
AST_END(double_string_inner)

AST_NODE(double_string_content)
	ast_sel<true, double_string_inner_t, Exp_t> content;
	AST_MEMBER(double_string_content, &content)
AST_END(double_string_content)

AST_NODE(DoubleString)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, double_string_content_t> segments;
	AST_MEMBER(DoubleString, &sep, &segments)
AST_END(DoubleString)

AST_NODE(String)
	ast_sel<true, DoubleString_t, SingleString_t, LuaString_t> str;
	AST_MEMBER(String, &str)
AST_END(String)

AST_LEAF(Metatable)
AST_END(Metatable)

AST_NODE(Metamethod)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(Metamethod, &name)
AST_END(Metamethod)

AST_NODE(DotChainItem)
	ast_sel<true, Name_t, Metatable_t, Metamethod_t> name;
	AST_MEMBER(DotChainItem, &name)
AST_END(DotChainItem)

AST_NODE(ColonChainItem)
	ast_sel<true, LuaKeyword_t, Name_t, Metamethod_t> name;
	bool switchToDot = false;
	AST_MEMBER(ColonChainItem, &name)
AST_END(ColonChainItem)

class default_value_t;

AST_NODE(Slice)
	ast_sel<true, Exp_t, default_value_t> startValue;
	ast_sel<true, Exp_t, default_value_t> stopValue;
	ast_sel<true, Exp_t, default_value_t> stepValue;
	AST_MEMBER(Slice, &startValue, &stopValue, &stepValue)
AST_END(Slice)

AST_NODE(Parens)
	ast_ptr<true, Exp_t> expr;
	AST_MEMBER(Parens, &expr)
AST_END(Parens)

AST_NODE(Invoke)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, Exp_t, SingleString_t, DoubleString_t, LuaString_t, TableLit_t> args;
	AST_MEMBER(Invoke, &sep, &args)
AST_END(Invoke)

AST_LEAF(existential_op)
AST_END(existential_op)

class InvokeArgs_t;

AST_NODE(ChainValue)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Slice_t, Exp_t, String_t, InvokeArgs_t, existential_op_t> items;
	AST_MEMBER(ChainValue, &sep, &items)
AST_END(ChainValue)

AST_NODE(AssignableChain)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Exp_t, String_t> items;
	AST_MEMBER(AssignableChain, &sep, &items)
AST_END(AssignableChain)

AST_NODE(Value)
	ast_sel<true, SimpleValue_t, simple_table_t, ChainValue_t, String_t> item;
	AST_MEMBER(Value, &item)
AST_END(Value)

AST_LEAF(default_value)
AST_END(default_value)

AST_NODE(TableLit)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t, Exp_t, meta_variable_pair_t, meta_normal_pair_t> values;
	AST_MEMBER(TableLit, &sep, &values)
AST_END(TableLit)

AST_NODE(TableBlockIndent)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t, TableBlockIndent_t, meta_variable_pair_t, meta_normal_pair_t> values;
	AST_MEMBER(TableBlockIndent, &sep, &values)
AST_END(TableBlockIndent)

AST_NODE(TableBlock)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t, TableBlockIndent_t, Exp_t, TableBlock_t, meta_variable_pair_t, meta_normal_pair_t> values;
	AST_MEMBER(TableBlock, &sep, &values)
AST_END(TableBlock)

AST_NODE(class_member_list)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, variable_pair_t, normal_pair_t> values;
	AST_MEMBER(class_member_list, &sep, &values)
AST_END(class_member_list)

AST_NODE(ClassBlock)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, class_member_list_t, Statement_t> contents;
	AST_MEMBER(ClassBlock, &sep, &contents)
AST_END(ClassBlock)

AST_NODE(ClassDecl)
	ast_ptr<false, Assignable_t> name;
	ast_ptr<false, Exp_t> extend;
	ast_ptr<false, ClassBlock_t> body;
	AST_MEMBER(ClassDecl, &name, &extend, &body)
AST_END(ClassDecl)

AST_NODE(global_values)
	ast_ptr<true, NameList_t> nameList;
	ast_sel<false, TableBlock_t, ExpListLow_t> valueList;
	AST_MEMBER(global_values, &nameList, &valueList)
AST_END(global_values)

AST_LEAF(global_op)
AST_END(global_op)

AST_NODE(Global)
	ast_sel<true, ClassDecl_t, global_op_t, global_values_t> item;
	AST_MEMBER(Global, &item)
AST_END(Global)

AST_LEAF(export_default)
AST_END(export_default)

class Macro_t;

AST_NODE(Export)
	ast_ptr<false, export_default_t> def;
	ast_sel<true, ExpList_t, Exp_t, Macro_t> target;
	ast_ptr<false, Assign_t> assign;
	AST_MEMBER(Export, &def, &target, &assign)
AST_END(Export)

AST_NODE(FnArgDef)
	ast_sel<true, Variable_t, SelfName_t> name;
	ast_ptr<false, Exp_t> defaultValue;
	AST_MEMBER(FnArgDef, &name, &defaultValue)
AST_END(FnArgDef)

AST_NODE(FnArgDefList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, FnArgDef_t> definitions;
	ast_ptr<false, VarArg_t> varArg;
	AST_MEMBER(FnArgDefList, &sep, &definitions, &varArg)
AST_END(FnArgDefList)

AST_NODE(outer_var_shadow)
	ast_ptr<false, NameList_t> varList;
	AST_MEMBER(outer_var_shadow, &varList)
AST_END(outer_var_shadow)

AST_NODE(FnArgsDef)
	ast_ptr<false, FnArgDefList_t> defList;
	ast_ptr<false, outer_var_shadow_t> shadowOption;
	AST_MEMBER(FnArgsDef, &defList, &shadowOption)
AST_END(FnArgsDef)

AST_LEAF(fn_arrow)
AST_END(fn_arrow)

AST_NODE(FunLit)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_ptr<true, fn_arrow_t> arrow;
	ast_ptr<false, Body_t> body;
	AST_MEMBER(FunLit, &argsDef, &arrow, &body)
AST_END(FunLit)

AST_NODE(MacroName)
	ast_ptr<false, Name_t> name;
	AST_MEMBER(MacroName, &name)
AST_END(MacroName)

AST_NODE(MacroLit)
	ast_ptr<false, FnArgDefList_t> argsDef;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(MacroLit, &argsDef, &body)
AST_END(MacroLit)

AST_NODE(Macro)
	ast_ptr<true, Name_t> name;
	ast_ptr<true, MacroLit_t> macroLit;
	AST_MEMBER(Macro, &name, &macroLit)
AST_END(Macro)

AST_NODE(NameOrDestructure)
	ast_sel<true, Variable_t, TableLit_t> item;
	AST_MEMBER(NameOrDestructure, &item)
AST_END(NameOrDestructure)

AST_NODE(AssignableNameList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, NameOrDestructure_t> items;
	AST_MEMBER(AssignableNameList, &sep, &items)
AST_END(AssignableNameList)

AST_NODE(InvokeArgs)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Exp_t, TableBlock_t> args;
	AST_MEMBER(InvokeArgs, &sep, &args)
AST_END(InvokeArgs)

AST_LEAF(const_value)
AST_END(const_value)

AST_NODE(unary_value)
	ast_list<true, unary_operator_t> ops;
	ast_ptr<true, Value_t> value;
	AST_MEMBER(unary_value, &ops, &value)
AST_END(unary_value)

AST_NODE(unary_exp)
	ast_list<false, unary_operator_t> ops;
	ast_list<true, Value_t> expos;
	AST_MEMBER(unary_exp, &ops, &expos)
AST_END(unary_exp)

AST_NODE(ExpListAssign)
	ast_ptr<true, ExpList_t> expList;
	ast_sel<false, Update_t, Assign_t> action;
	AST_MEMBER(ExpListAssign, &expList, &action)
AST_END(ExpListAssign)

AST_NODE(if_line)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<false, Assign_t> assign;
	AST_MEMBER(if_line, &condition, &assign)
AST_END(if_line)

AST_NODE(unless_line)
	ast_ptr<true, Exp_t> condition;
	AST_MEMBER(unless_line, &condition)
AST_END(unless_line)

AST_LEAF(BreakLoop)
AST_END(BreakLoop)

AST_NODE(PipeBody)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, unary_exp_t> values;
	AST_MEMBER(PipeBody, &sep, &values)
AST_END(PipeBody)

AST_NODE(statement_appendix)
	ast_sel<true, if_line_t, unless_line_t, CompInner_t> item;
	AST_MEMBER(statement_appendix, &item)
AST_END(statement_appendix)

AST_LEAF(statement_sep)
AST_END(statement_sep)

AST_NODE(Statement)
	ast_sel<true, Import_t, While_t, Repeat_t, For_t, ForEach_t,
		Return_t, Local_t, Global_t, Export_t, Macro_t, BreakLoop_t,
		Label_t, Goto_t, Backcall_t, LocalAttrib_t, PipeBody_t, ExpListAssign_t> content;
	ast_ptr<false, statement_appendix_t> appendix;
	ast_ptr<false, statement_sep_t> needSep;
	AST_MEMBER(Statement, &content, &appendix, &needSep)
AST_END(Statement)

class Block_t;

AST_NODE(Body)
	ast_sel<true, Block_t, Statement_t> content;
	AST_MEMBER(Body, &content)
AST_END(Body)

AST_NODE(Block)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, Statement_t> statements;
	AST_MEMBER(Block, &sep, &statements)
AST_END(Block)

AST_NODE(BlockEnd)
	ast_ptr<true, Block_t> block;
	AST_MEMBER(BlockEnd, &block)
AST_END(BlockEnd)

AST_NODE(File)
	ast_ptr<true, Block_t> block;
	AST_MEMBER(File, &block)
AST_END(File)

} // namespace parserlib
