/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "MoonP/moon_parser.h"

namespace MoonP {

#define AST_LEAF(type, id) \
extern rule type; \
class type##_t : public ast_node \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); } \
	virtual size_t getId() const override { return id; } \

#define AST_NODE(type, id) \
extern rule type; \
class type##_t : public ast_container \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); } \
	virtual size_t getId() const override { return id; } \

#define AST_MEMBER(type, ...) \
	type##_t() { \
		add_members({__VA_ARGS__}); \
	}

#define AST_END(type) \
};

AST_LEAF(Num, "Num"_id)
AST_END(Num)

AST_LEAF(Name, "Name"_id)
AST_END(Name)

AST_NODE(Variable, "Variable"_id)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(Variable, &name)
AST_END(Variable)

AST_NODE(LuaKeyword, "LuaKeyword"_id)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(LuaKeyword, &name)
AST_END(LuaKeyword)

AST_LEAF(self, "self"_id)
AST_END(self)

AST_NODE(self_name, "self_name"_id)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(self_name, &name)
AST_END(self_name)

AST_LEAF(self_class, "self_class"_id)
AST_END(self_class)

AST_NODE(self_class_name, "self_class_name"_id)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(self_class_name, &name)
AST_END(self_class_name)

AST_NODE(SelfName, "SelfName"_id)
	ast_sel<true, self_class_name_t, self_class_t, self_name_t, self_t> name;
	AST_MEMBER(SelfName, &name)
AST_END(SelfName)

AST_NODE(KeyName, "KeyName"_id)
	ast_sel<true, SelfName_t, Name_t> name;
	AST_MEMBER(KeyName, &name)
AST_END(KeyName)

AST_LEAF(VarArg, "VarArg"_id)
AST_END(VarArg)

AST_LEAF(local_flag, "local_flag"_id)
AST_END(local_flag)

AST_LEAF(Seperator, "Seperator"_id)
AST_END(Seperator)

AST_NODE(NameList, "NameList"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Variable_t> names;
	AST_MEMBER(NameList, &sep, &names)
AST_END(NameList)

AST_NODE(Local, "Local"_id)
	ast_sel<true, local_flag_t, NameList_t> name;
	std::list<std::string> forceDecls;
	std::list<std::string> decls;
	AST_MEMBER(Local, &name)
AST_END(Local)

AST_NODE(colon_import_name, "colon_import_name"_id)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(colon_import_name, &name)
AST_END(colon_import_name)

class Exp_t;
class TableLit_t;

AST_LEAF(import_literal_inner, "import_literal_inner"_id)
AST_END(import_literal_inner)

AST_NODE(ImportLiteral, "ImportLiteral"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, import_literal_inner_t> inners;
	AST_MEMBER(ImportLiteral, &sep, &inners)
AST_END(ImportLiteral)

AST_NODE(ImportAs, "ImportAs"_id)
	ast_ptr<true, ImportLiteral_t> literal;
	ast_sel<false, Variable_t, TableLit_t> target;
	AST_MEMBER(ImportAs, &literal, &target)
AST_END(ImportAs)

AST_NODE(ImportFrom, "ImportFrom"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, colon_import_name_t, Variable_t> names;
	ast_ptr<true, Exp_t> exp;
	AST_MEMBER(ImportFrom, &sep, &names, &exp)
AST_END(ImportFrom)

AST_NODE(Import, "Import"_id)
	ast_sel<true, ImportAs_t, ImportFrom_t> content;
	AST_MEMBER(Import, &content)
AST_END(Import)

class FnArgsDef_t;
class ChainValue_t;

AST_NODE(Backcall, "Backcall"_id)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_ptr<true, ChainValue_t> value;
	AST_MEMBER(Backcall, &argsDef, &value)
AST_END(Backcall)

AST_NODE(ExpListLow, "ExpListLow"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	AST_MEMBER(ExpListLow, &sep, &exprs)
AST_END(ExpListLow)

AST_NODE(ExpList, "ExpList"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	AST_MEMBER(ExpList, &sep, &exprs)
AST_END(ExpList)

AST_NODE(Return, "Return"_id)
	ast_ptr<false, ExpListLow_t> valueList;
	AST_MEMBER(Return, &valueList)
AST_END(Return)

class Assign_t;
class Body_t;

AST_NODE(With, "With"_id)
	ast_ptr<true, ExpList_t> valueList;
	ast_ptr<false, Assign_t> assigns;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(With, &valueList, &assigns, &body)
AST_END(With)

AST_NODE(SwitchCase, "SwitchCase"_id)
	ast_ptr<true, ExpList_t> valueList;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(SwitchCase, &valueList, &body)
AST_END(SwitchCase)

AST_NODE(Switch, "Switch"_id)
	ast_ptr<true, Exp_t> target;
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, SwitchCase_t> branches;
	ast_ptr<false, Body_t> lastBranch;
	AST_MEMBER(Switch, &target, &sep, &branches, &lastBranch)
AST_END(Switch)

AST_NODE(IfCond, "IfCond"_id)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<false, Assign_t> assign;
	AST_MEMBER(IfCond, &condition, &assign)
AST_END(IfCond)

AST_NODE(If, "If"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, IfCond_t, Body_t> nodes;
	AST_MEMBER(If, &sep, &nodes)
AST_END(If)

AST_NODE(Unless, "Unless"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, IfCond_t, Body_t> nodes;
	AST_MEMBER(Unless, &sep, &nodes)
AST_END(Unless)

AST_NODE(While, "While"_id)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(While, &condition, &body)
AST_END(While)

AST_NODE(for_step_value, "for_step_value"_id)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(for_step_value, &value)
AST_END(for_step_value)

AST_NODE(For, "For"_id)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, for_step_value_t> stepValue;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(For, &varName, &startValue, &stopValue, &stepValue, &body)
AST_END(For)

class AssignableNameList_t;
class star_exp_t;

AST_NODE(ForEach, "ForEach"_id)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, star_exp_t, ExpList_t> loopValue;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(ForEach, &nameList, &loopValue, &body)
AST_END(ForEach)

AST_NODE(Do, "Do"_id)
	ast_ptr<true, Body_t> body;
	AST_MEMBER(Do, &body)
AST_END(Do)

class CompInner_t;
class Statement_t;

AST_NODE(Comprehension, "Comprehension"_id)
	ast_sel<true, Exp_t, Statement_t> value;
	ast_ptr<true, CompInner_t> forLoop;
	AST_MEMBER(Comprehension, &value, &forLoop)
AST_END(Comprehension)

AST_NODE(comp_value, "comp_value"_id)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(comp_value, &value)
AST_END(comp_value)

AST_NODE(TblComprehension, "TblComprehension"_id)
	ast_ptr<true, Exp_t> key;
	ast_ptr<false, comp_value_t> value;
	ast_ptr<true, CompInner_t> forLoop;
	AST_MEMBER(TblComprehension, &key, &value, &forLoop)
AST_END(TblComprehension)

AST_NODE(star_exp, "star_exp"_id)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(star_exp, &value)
AST_END(star_exp)

AST_NODE(CompForEach, "CompForEach"_id)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, star_exp_t, Exp_t> loopValue;
	AST_MEMBER(CompForEach, &nameList, &loopValue)
AST_END(CompForEach)

AST_NODE(CompFor, "CompFor"_id)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, for_step_value_t> stepValue;
	AST_MEMBER(CompFor, &varName, &startValue, &stopValue, &stepValue)
AST_END(CompFor)

AST_NODE(CompInner, "CompInner"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, CompFor_t, CompForEach_t, Exp_t> items;
	AST_MEMBER(CompInner, &sep, &items)
AST_END(CompInner)

class TableBlock_t;

AST_NODE(Assign, "Assign"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, With_t, If_t, Switch_t, TableBlock_t, Exp_t> values;
	AST_MEMBER(Assign, &sep, &values)
AST_END(Assign)

AST_LEAF(update_op, "update_op"_id)
AST_END(update_op)

AST_NODE(Update, "Update"_id)
	ast_ptr<true, update_op_t> op;
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(Update, &op, &value)
AST_END(Update)

AST_LEAF(BinaryOperator, "BinaryOperator"_id)
AST_END(BinaryOperator)

AST_LEAF(BackcallOperator, "BackcallOperator"_id)
AST_END(BackcallOperator)

class AssignableChain_t;

AST_NODE(Assignable, "Assignable"_id)
	ast_sel<true, AssignableChain_t, Variable_t, SelfName_t> item;
	AST_MEMBER(Assignable, &item)
AST_END(Assignable)

class Value_t;

AST_NODE(exp_op_value, "exp_op_value"_id)
	ast_sel<true, BinaryOperator_t, BackcallOperator_t> op;
	ast_ptr<true, Value_t> value;
	AST_MEMBER(exp_op_value, &op, &value)
AST_END(exp_op_value)

AST_NODE(Exp, "Exp"_id)
	ast_ptr<true, Value_t> value;
	ast_list<false, exp_op_value_t> opValues;
	AST_MEMBER(Exp, &value, &opValues)
AST_END(Exp)

class Parens_t;

AST_NODE(Callable, "Callable"_id)
	ast_sel<true, Variable_t, SelfName_t, VarArg_t, Parens_t> item;
	AST_MEMBER(Callable, &item)
AST_END(Callable)

AST_NODE(variable_pair, "variable_pair"_id)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(variable_pair, &name)
AST_END(variable_pair)

class DoubleString_t;
class SingleString_t;

AST_NODE(normal_pair, "normal_pair"_id)
	ast_sel<true, KeyName_t, Exp_t, DoubleString_t, SingleString_t> key;
	ast_sel<true, Exp_t, TableBlock_t> value;
	AST_MEMBER(normal_pair, &key, &value)
AST_END(normal_pair)

AST_NODE(simple_table, "simple_table"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, variable_pair_t, normal_pair_t> pairs;
	AST_MEMBER(simple_table, &sep, &pairs)
AST_END(simple_table)

class String_t;
class const_value_t;
class ClassDecl_t;
class unary_exp_t;
class TableLit_t;
class FunLit_t;

AST_NODE(SimpleValue, "SimpleValue"_id)
	ast_sel<true, const_value_t,
	If_t, Unless_t, Switch_t, With_t, ClassDecl_t,
	ForEach_t, For_t, While_t, Do_t, unary_exp_t,
	TblComprehension_t, TableLit_t, Comprehension_t,
	FunLit_t, Num_t> value;
	AST_MEMBER(SimpleValue, &value)
AST_END(SimpleValue)

AST_LEAF(LuaStringOpen, "LuaStringOpen"_id)
AST_END(LuaStringOpen)

AST_LEAF(LuaStringContent, "LuaStringContent"_id)
AST_END(LuaStringContent)

AST_LEAF(LuaStringClose, "LuaStringClose"_id)
AST_END(LuaStringClose)

AST_NODE(LuaString, "LuaString"_id)
	ast_ptr<true, LuaStringOpen_t> open;
	ast_ptr<true, LuaStringContent_t> content;
	ast_ptr<true, LuaStringClose_t> close;
	AST_MEMBER(LuaString, &open, &content, &close)
AST_END(LuaString)

AST_LEAF(SingleString, "SingleString"_id)
AST_END(SingleString)

AST_LEAF(double_string_inner, "double_string_inner"_id)
AST_END(double_string_inner)

AST_NODE(double_string_content, "double_string_content"_id)
	ast_sel<true, double_string_inner_t, Exp_t> content;
	AST_MEMBER(double_string_content, &content)
AST_END(double_string_content)

AST_NODE(DoubleString, "DoubleString"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, double_string_content_t> segments;
	AST_MEMBER(DoubleString, &sep, &segments)
AST_END(DoubleString)

AST_NODE(String, "String"_id)
	ast_sel<true, DoubleString_t, SingleString_t, LuaString_t> str;
	AST_MEMBER(String, &str)
AST_END(String)

AST_NODE(DotChainItem, "DotChainItem"_id)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(DotChainItem, &name)
AST_END(DotChainItem)

AST_NODE(ColonChainItem, "ColonChainItem"_id)
	ast_sel<true, LuaKeyword_t, Name_t> name;
	bool switchToDot = false;
	AST_MEMBER(ColonChainItem, &name)
AST_END(ColonChainItem)

class default_value_t;

AST_NODE(Slice, "Slice"_id)
	ast_sel<true, Exp_t, default_value_t> startValue;
	ast_sel<true, Exp_t, default_value_t> stopValue;
	ast_sel<true, Exp_t, default_value_t> stepValue;
	AST_MEMBER(Slice, &startValue, &stopValue, &stepValue)
AST_END(Slice)

AST_NODE(Parens, "Parens"_id)
	ast_ptr<true, Exp_t> expr;
	AST_MEMBER(Parens, &expr)
AST_END(Parens)

AST_NODE(Invoke, "Invoke"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, Exp_t, SingleString_t, DoubleString_t, LuaString_t> args;
	AST_MEMBER(Invoke, &sep, &args)
AST_END(Invoke)

AST_LEAF(existential_op, "existential_op"_id)
AST_END(existential_op)

class InvokeArgs_t;

AST_NODE(ChainValue, "ChainValue"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Slice_t, Exp_t, String_t, InvokeArgs_t, existential_op_t> items;
	AST_MEMBER(ChainValue, &sep, &items)
AST_END(ChainValue)

AST_NODE(AssignableChain, "AssignableChain"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Exp_t, String_t> items;
	AST_MEMBER(AssignableChain, &sep, &items)
AST_END(AssignableChain)

AST_NODE(Value, "Value"_id)
	ast_sel<true, SimpleValue_t, simple_table_t, ChainValue_t, String_t> item;
	AST_MEMBER(Value, &item)
AST_END(Value)

AST_LEAF(default_value, "default_value"_id)
AST_END(default_value)

AST_NODE(TableLit, "TableLit"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t, Exp_t> values;
	AST_MEMBER(TableLit, &sep, &values)
AST_END(TableLit)

AST_NODE(TableBlock, "TableBlock"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, variable_pair_t, normal_pair_t> values;
	AST_MEMBER(TableBlock, &sep, &values)
AST_END(TableBlock)

AST_NODE(class_member_list, "class_member_list"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, variable_pair_t, normal_pair_t> values;
	AST_MEMBER(class_member_list, &sep, &values)
AST_END(class_member_list)

AST_NODE(ClassBlock, "ClassBlock"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, class_member_list_t, Statement_t> contents;
	AST_MEMBER(ClassBlock, &sep, &contents)
AST_END(ClassBlock)

AST_NODE(ClassDecl, "ClassDecl"_id)
	ast_ptr<false, Assignable_t> name;
	ast_ptr<false, Exp_t> extend;
	ast_ptr<false, ClassBlock_t> body;
	AST_MEMBER(ClassDecl, &name, &extend, &body)
AST_END(ClassDecl)

AST_NODE(export_values, "export_values"_id)
	ast_ptr<true, NameList_t> nameList;
	ast_ptr<false, ExpListLow_t> valueList;
	AST_MEMBER(export_values, &nameList, &valueList)
AST_END(export_values)

AST_LEAF(export_op, "export_op"_id)
AST_END(export_op)

AST_NODE(Export, "Export"_id)
	ast_sel<true, ClassDecl_t, export_op_t, export_values_t> item;
	AST_MEMBER(Export, &item)
AST_END(Export)

AST_NODE(FnArgDef, "FnArgDef"_id)
	ast_sel<true, Variable_t, SelfName_t> name;
	ast_ptr<false, Exp_t> defaultValue;
	AST_MEMBER(FnArgDef, &name, &defaultValue)
AST_END(FnArgDef)

AST_NODE(FnArgDefList, "FnArgDefList"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, FnArgDef_t> definitions;
	ast_ptr<false, VarArg_t> varArg;
	AST_MEMBER(FnArgDefList, &sep, &definitions, &varArg)
AST_END(FnArgDefList)

AST_NODE(outer_var_shadow, "outer_var_shadow"_id)
	ast_ptr<false, NameList_t> varList;
	AST_MEMBER(outer_var_shadow, &varList)
AST_END(outer_var_shadow)

AST_NODE(FnArgsDef, "FnArgsDef"_id)
	ast_ptr<false, FnArgDefList_t> defList;
	ast_ptr<false, outer_var_shadow_t> shadowOption;
	AST_MEMBER(FnArgsDef, &defList, &shadowOption)
AST_END(FnArgsDef)

AST_LEAF(fn_arrow, "fn_arrow"_id)
AST_END(fn_arrow)

AST_NODE(FunLit, "FunLit"_id)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_ptr<true, fn_arrow_t> arrow;
	ast_ptr<false, Body_t> body;
	AST_MEMBER(FunLit, &argsDef, &arrow, &body)
AST_END(FunLit)

AST_NODE(NameOrDestructure, "NameOrDestructure"_id)
	ast_sel<true, Variable_t, TableLit_t> item;
	AST_MEMBER(NameOrDestructure, &item)
AST_END(NameOrDestructure)

AST_NODE(AssignableNameList, "AssignableNameList"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, NameOrDestructure_t> items;
	AST_MEMBER(AssignableNameList, &sep, &items)
AST_END(AssignableNameList)

AST_NODE(InvokeArgs, "InvokeArgs"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Exp_t, TableBlock_t> args;
	AST_MEMBER(InvokeArgs, &sep, &args)
AST_END(InvokeArgs)

AST_LEAF(const_value, "const_value"_id)
AST_END(const_value)

AST_NODE(unary_exp, "unary_exp"_id)
	ast_ptr<true, Exp_t> item;
	AST_MEMBER(unary_exp, &item)
AST_END(unary_exp)

AST_NODE(ExpListAssign, "ExpListAssign"_id)
	ast_ptr<true, ExpList_t> expList;
	ast_sel<false, Update_t, Assign_t> action;
	AST_MEMBER(ExpListAssign, &expList, &action)
AST_END(ExpListAssign)

AST_NODE(if_else_line, "if_else_line"_id)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<false, Assign_t> assign;
	ast_sel<false, Exp_t, default_value_t> elseExpr;
	AST_MEMBER(if_else_line, &condition, &assign, &elseExpr)
AST_END(if_else_line)

AST_NODE(unless_line, "unless_line"_id)
	ast_ptr<true, Exp_t> condition;
	AST_MEMBER(unless_line, &condition)
AST_END(unless_line)

AST_NODE(statement_appendix, "statement_appendix"_id)
	ast_sel<true, if_else_line_t, unless_line_t, CompInner_t> item;
	AST_MEMBER(statement_appendix, &item)
AST_END(statement_appendix)

AST_LEAF(BreakLoop, "BreakLoop"_id)
AST_END(BreakLoop)

AST_NODE(Statement, "Statement"_id)
	ast_sel<true, Import_t, While_t, For_t, ForEach_t,
		Return_t, Local_t, Export_t, BreakLoop_t,
		Backcall_t, ExpListAssign_t> content;
	ast_ptr<false, statement_appendix_t> appendix;
	AST_MEMBER(Statement, &content, &appendix)
AST_END(Statement)

class Block_t;

AST_NODE(Body, "Body"_id)
	ast_sel<true, Block_t, Statement_t> content;
	AST_MEMBER(Body, &content)
AST_END(Body)

AST_NODE(Block, "Block"_id)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, Statement_t> statements;
	AST_MEMBER(Block, &sep, &statements)
AST_END(Block)

AST_NODE(File, "File"_id)
	ast_ptr<true, Block_t> block;
	AST_MEMBER(File, &block)
AST_END(File)

} // namespace MoonP
