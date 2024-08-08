/* Copyright (c) 2024 Li Jin, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "yuescript/ast.hpp"

namespace parserlib {

template <class T>
std::string_view ast_name() { }

#define AST_LEAF(type) \
	COUNTER_INC; \
	namespace yue { \
	class type##_t : public ast_node { \
	public: \
		virtual int get_id() const override { return COUNTER_READ; } \
		virtual std::string to_string(void*) const override; \
		virtual const std::string_view get_name() const override { return #type ""sv; }

#define AST_NODE(type) \
	COUNTER_INC; \
	namespace yue { \
	class type##_t : public ast_container { \
	public: \
		virtual int get_id() const override { return COUNTER_READ; } \
		virtual std::string to_string(void*) const override; \
		virtual const std::string_view get_name() const override { return #type ""sv; }

#define AST_MEMBER(type, ...) \
	type##_t() { \
		add_members({__VA_ARGS__}); \
	}

#define AST_END(type) \
	} \
	; \
	} \
	template <> \
	constexpr int id<yue::type##_t>() { return COUNTER_READ; } \
	template <> \
	constexpr std::string_view ast_name<yue::type##_t>() { return #type ""sv; }

// clang-format off

namespace yue {
class ExpListLow_t;
class TableBlock_t;
class Attrib_t;
class SimpleTable_t;
class TableLit_t;
class Assign_t;
class Exp_t;
class VariablePair_t;
class NormalPair_t;
class MetaVariablePair_t;
class MetaNormalPair_t;
class FnArgsDef_t;
class ChainValue_t;
class ExistentialOp_t;
class Block_t;
class Statement_t;
class Body_t;
class AssignableNameList_t;
class StarExp_t;
class CompInner_t;
class AssignableChain_t;
class UnaryExp_t;
class Parens_t;
class MacroName_t;
class String_t;
class ConstValue_t;
class ClassDecl_t;
class UnaryValue_t;
class FunLit_t;
class DefaultValue_t;
class InvokeArgs_t;
class TableBlockIndent_t;
class Macro_t;
class In_t;
class NormalDef_t;
class SpreadListExp_t;
class Comprehension_t;
class Value_t;
} // namespace yue

AST_LEAF(Num)
AST_END(Num)

AST_LEAF(Name)
AST_END(Name)

AST_LEAF(UnicodeName)
AST_END(UnicodeName)

AST_NODE(Variable)
	ast_sel<true, Name_t, UnicodeName_t> name;
	AST_MEMBER(Variable, &name)
AST_END(Variable)

AST_NODE(LabelName)
	ast_ptr<true, UnicodeName_t> name;
	AST_MEMBER(LabelName, &name)
AST_END(LabelName)

AST_NODE(LuaKeyword)
	ast_ptr<true, Name_t> name;
	AST_MEMBER(LuaKeyword, &name)
AST_END(LuaKeyword)

AST_LEAF(Self)
AST_END(Self)

AST_NODE(SelfName)
	ast_sel<true, Name_t, UnicodeName_t> name;
	AST_MEMBER(SelfName, &name)
AST_END(SelfName)

AST_LEAF(SelfClass)
AST_END(SelfClass)

AST_NODE(SelfClassName)
	ast_sel<true, Name_t, UnicodeName_t> name;
	AST_MEMBER(SelfClassName, &name)
AST_END(SelfClassName)

AST_NODE(SelfItem)
	ast_sel<true, SelfClassName_t, SelfClass_t, SelfName_t, Self_t> name;
	AST_MEMBER(SelfItem, &name)
AST_END(SelfItem)

AST_NODE(KeyName)
	ast_sel<true, SelfItem_t, Name_t, UnicodeName_t> name;
	AST_MEMBER(KeyName, &name)
AST_END(KeyName)

AST_LEAF(VarArg)
AST_END(VarArg)

AST_LEAF(LocalFlag)
AST_END(LocalFlag)

AST_LEAF(Seperator)
AST_END(Seperator)

AST_NODE(NameList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Variable_t> names;
	AST_MEMBER(NameList, &sep, &names)
AST_END(NameList)

AST_NODE(LocalValues)
	ast_ptr<true, NameList_t> nameList;
	ast_sel<false, TableBlock_t, ExpListLow_t> valueList;
	AST_MEMBER(LocalValues, &nameList, &valueList)
AST_END(LocalValues)

AST_NODE(Local)
	ast_sel<true, LocalFlag_t, LocalValues_t> item;
	std::list<std::string> forceDecls;
	std::list<std::string> decls;
	bool collected = false;
	bool defined = false;
	AST_MEMBER(Local, &item)
AST_END(Local)

AST_LEAF(ConstAttrib)
AST_END(ConstAttrib)

AST_LEAF(CloseAttrib)
AST_END(CloseAttrib)

AST_NODE(LocalAttrib)
	ast_sel<true, ConstAttrib_t, CloseAttrib_t> attrib;
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Variable_t, SimpleTable_t, TableLit_t, Comprehension_t> leftList;
	ast_ptr<true, Assign_t> assign;
	AST_MEMBER(LocalAttrib, &attrib, &sep, &leftList, &assign)
AST_END(LocalAttrib)

AST_NODE(ColonImportName)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(ColonImportName, &name)
AST_END(ColonImportName)

AST_LEAF(ImportLiteralInner)
AST_END(ImportLiteralInner)

AST_NODE(ImportLiteral)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, ImportLiteralInner_t> inners;
	AST_MEMBER(ImportLiteral, &sep, &inners)
AST_END(ImportLiteral)

AST_NODE(ImportFrom)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, ColonImportName_t, Variable_t> names;
	ast_sel<true, ImportLiteral_t, Exp_t> item;
	AST_MEMBER(ImportFrom, &sep, &names, &item)
AST_END(ImportFrom)

AST_NODE(FromImport)
	ast_sel<true, ImportLiteral_t, Exp_t> item;
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, ColonImportName_t, Variable_t> names;
	AST_MEMBER(FromImport, &item, &sep, &names)
AST_END(FromImport)

AST_NODE(MacroNamePair)
	ast_ptr<true, MacroName_t> key;
	ast_ptr<true, MacroName_t> value;
	AST_MEMBER(MacroNamePair, &key, &value)
AST_END(MacroNamePair)

AST_LEAF(ImportAllMacro)
AST_END(ImportAllMacro)

AST_NODE(ImportTabLit)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, VariablePair_t, NormalPair_t, MacroName_t, MacroNamePair_t, ImportAllMacro_t, Exp_t, MetaVariablePair_t, MetaNormalPair_t> items;
	AST_MEMBER(ImportTabLit, &sep, &items)
AST_END(ImportTabLit)

AST_NODE(ImportAs)
	ast_ptr<true, ImportLiteral_t> literal;
	ast_sel<false, Variable_t, ImportTabLit_t, ImportAllMacro_t> target;
	AST_MEMBER(ImportAs, &literal, &target)
AST_END(ImportAs)

AST_NODE(Import)
	ast_sel<true, ImportAs_t, ImportFrom_t, FromImport_t> content;
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

AST_NODE(ShortTabAppending)
	ast_ptr<true, Assign_t> assign;
	AST_MEMBER(ShortTabAppending, &assign)
AST_END(ShortTabAppending)

AST_LEAF(FnArrowBack)
AST_END(FnArrowBack)

AST_NODE(Backcall)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_ptr<true, FnArrowBack_t> arrow;
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
	ast_sel<false, TableBlock_t, ExpListLow_t> valueList;
	AST_MEMBER(Return, &valueList)
AST_END(Return)

AST_NODE(With)
	ast_ptr<false, ExistentialOp_t> eop;
	ast_ptr<true, ExpList_t> valueList;
	ast_ptr<false, Assign_t> assigns;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(With, &eop, &valueList, &assigns, &body)
AST_END(With)

AST_NODE(SwitchList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	AST_MEMBER(SwitchList, &sep, &exprs)
AST_END(SwitchList)

AST_NODE(SwitchCase)
	ast_ptr<true, SwitchList_t> condition;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(SwitchCase, &condition, &body)
AST_END(SwitchCase)

AST_NODE(Switch)
	ast_ptr<true, Exp_t> target;
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, SwitchCase_t> branches;
	ast_sel<false, Block_t, Statement_t> lastBranch;
	AST_MEMBER(Switch, &target, &sep, &branches, &lastBranch)
AST_END(Switch)

AST_NODE(Assignment)
	ast_ptr<false, ExpList_t> expList;
	ast_ptr<true, Assign_t> assign;
	AST_MEMBER(Assignment, &expList, &assign)
AST_END(Assignment)

AST_NODE(IfCond)
	ast_ptr<true, Exp_t> condition;
	ast_ptr<false, Assignment_t> assignment;
	AST_MEMBER(IfCond, &condition, &assignment)
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

AST_NODE(Repeat)
	ast_ptr<true, Body_t> body;
	ast_ptr<true, Exp_t> condition;
	AST_MEMBER(Repeat, &body, &condition)
AST_END(Repeat)

AST_NODE(ForStepValue)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(ForStepValue, &value)
AST_END(ForStepValue)

AST_NODE(For)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, ForStepValue_t> stepValue;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(For, &varName, &startValue, &stopValue, &stepValue, &body)
AST_END(For)

AST_NODE(ForEach)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, StarExp_t, ExpList_t> loopValue;
	ast_sel<true, Block_t, Statement_t> body;
	AST_MEMBER(ForEach, &nameList, &loopValue, &body)
AST_END(ForEach)

AST_NODE(Do)
	ast_ptr<true, Body_t> body;
	AST_MEMBER(Do, &body)
AST_END(Do)

AST_NODE(CatchBlock)
	ast_ptr<true, Variable_t> err;
	ast_ptr<true, Block_t> block;
	AST_MEMBER(CatchBlock, &err, &block)
AST_END(CatchBlock)

AST_NODE(Try)
	ast_sel<true, Block_t, Exp_t> func;
	ast_ptr<false, CatchBlock_t> catchBlock;
	AST_MEMBER(Try, &func, &catchBlock)
AST_END(Try)

AST_NODE(Comprehension)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, NormalDef_t, SpreadListExp_t, CompInner_t,
		/*non-syntax-rule*/ Statement_t> items;
	AST_MEMBER(Comprehension, &sep, &items)
AST_END(Comprehension)

AST_NODE(CompValue)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(CompValue, &value)
AST_END(CompValue)

AST_NODE(TblComprehension)
	ast_ptr<true, Exp_t> key;
	ast_ptr<false, CompValue_t> value;
	ast_ptr<true, CompInner_t> forLoop;
	AST_MEMBER(TblComprehension, &key, &value, &forLoop)
AST_END(TblComprehension)

AST_NODE(StarExp)
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(StarExp, &value)
AST_END(StarExp)

AST_NODE(CompForEach)
	ast_ptr<true, AssignableNameList_t> nameList;
	ast_sel<true, StarExp_t, Exp_t> loopValue;
	AST_MEMBER(CompForEach, &nameList, &loopValue)
AST_END(CompForEach)

AST_NODE(CompFor)
	ast_ptr<true, Variable_t> varName;
	ast_ptr<true, Exp_t> startValue;
	ast_ptr<true, Exp_t> stopValue;
	ast_ptr<false, ForStepValue_t> stepValue;
	AST_MEMBER(CompFor, &varName, &startValue, &stopValue, &stepValue)
AST_END(CompFor)

AST_NODE(CompInner)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, CompFor_t, CompForEach_t, Exp_t> items;
	AST_MEMBER(CompInner, &sep, &items)
AST_END(CompInner)

AST_NODE(Assign)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, With_t, If_t, Switch_t, TableBlock_t, Exp_t> values;
	AST_MEMBER(Assign, &sep, &values)
AST_END(Assign)

AST_LEAF(UpdateOp)
AST_END(UpdateOp)

AST_NODE(Update)
	ast_ptr<true, UpdateOp_t> op;
	ast_ptr<true, Exp_t> value;
	AST_MEMBER(Update, &op, &value)
AST_END(Update)

AST_LEAF(BinaryOperator)
AST_END(BinaryOperator)

AST_LEAF(UnaryOperator)
AST_END(UnaryOperator)

AST_LEAF(NotIn)
AST_END(NotIn)

AST_NODE(In)
	ast_ptr<false, NotIn_t> not_;
	ast_ptr<true, Value_t> value;
	AST_MEMBER(In, &not_, &value)
AST_END(In)

AST_NODE(Assignable)
	ast_sel<true, AssignableChain_t, Variable_t, SelfItem_t> item;
	AST_MEMBER(Assignable, &item)
AST_END(Assignable)

AST_NODE(ExpOpValue)
	ast_ptr<true, BinaryOperator_t> op;
	ast_list<true, UnaryExp_t> pipeExprs;
	AST_MEMBER(ExpOpValue, &op, &pipeExprs)
AST_END(ExpOpValue)

AST_NODE(Exp)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, UnaryExp_t> pipeExprs;
	ast_list<false, ExpOpValue_t> opValues;
	ast_ptr<false, Exp_t> nilCoalesed;
	AST_MEMBER(Exp, &sep, &pipeExprs, &opValues, &nilCoalesed)
AST_END(Exp)

AST_NODE(Callable)
	ast_sel<true, Variable_t, SelfItem_t, Parens_t, MacroName_t> item;
	AST_MEMBER(Callable, &item)
AST_END(Callable)

AST_NODE(VariablePair)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(VariablePair, &name)
AST_END(VariablePair)

AST_NODE(VariablePairDef)
	ast_ptr<true, VariablePair_t> pair;
	ast_ptr<false, Exp_t> defVal;
	AST_MEMBER(VariablePairDef, &pair, &defVal)
AST_END(VariablePairDef)

AST_NODE(NormalPair)
	ast_sel<true, KeyName_t, Exp_t, String_t> key;
	ast_sel<true, Exp_t, TableBlock_t> value;
	AST_MEMBER(NormalPair, &key, &value)
AST_END(NormalPair)

AST_NODE(NormalPairDef)
	ast_ptr<true, NormalPair_t> pair;
	ast_ptr<false, Exp_t> defVal;
	AST_MEMBER(NormalPairDef, &pair, &defVal)
AST_END(NormalPairDef)

AST_NODE(NormalDef)
	ast_ptr<true, Exp_t> item;
	ast_ptr<true, Seperator_t> sep;
	ast_ptr<false, Exp_t> defVal;
	AST_MEMBER(NormalDef, &item, &sep, &defVal)
AST_END(NormalDef)

AST_NODE(MetaVariablePair)
	ast_ptr<true, Variable_t> name;
	AST_MEMBER(MetaVariablePair, &name)
AST_END(MetaVariablePair)

AST_NODE(MetaVariablePairDef)
	ast_ptr<true, MetaVariablePair_t> pair;
	ast_ptr<false, Exp_t> defVal;
	AST_MEMBER(MetaVariablePairDef, &pair, &defVal)
AST_END(MetaVariablePairDef)

AST_NODE(MetaNormalPair)
	ast_sel<false, Name_t, Exp_t, String_t> key;
	ast_sel<true, Exp_t, TableBlock_t> value;
	AST_MEMBER(MetaNormalPair, &key, &value)
AST_END(MetaNormalPair)

AST_NODE(MetaNormalPairDef)
	ast_ptr<true, MetaNormalPair_t> pair;
	ast_ptr<false, Exp_t> defVal;
	AST_MEMBER(MetaNormalPairDef, &pair, &defVal)
AST_END(MetaNormalPairDef)

AST_NODE(SimpleTable)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, VariablePair_t, NormalPair_t, MetaVariablePair_t, MetaNormalPair_t> pairs;
	AST_MEMBER(SimpleTable, &sep, &pairs)
AST_END(SimpleTable)

AST_NODE(SimpleValue)
	ast_sel<true,
		TableLit_t, ConstValue_t,
		If_t, Switch_t, With_t, ClassDecl_t,
		ForEach_t, For_t, While_t, Do_t, Try_t,
		UnaryValue_t,
		TblComprehension_t, Comprehension_t,
		FunLit_t, Num_t, VarArg_t> value;
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

AST_LEAF(DoubleStringInner)
AST_END(DoubleStringInner)

AST_NODE(DoubleStringContent)
	ast_sel<true, DoubleStringInner_t, Exp_t> content;
	AST_MEMBER(DoubleStringContent, &content)
AST_END(DoubleStringContent)

AST_NODE(DoubleString)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, DoubleStringContent_t> segments;
	AST_MEMBER(DoubleString, &sep, &segments)
AST_END(DoubleString)

AST_NODE(String)
	ast_sel<true, DoubleString_t, SingleString_t, LuaString_t> str;
	AST_MEMBER(String, &str)
AST_END(String)

AST_LEAF(Metatable)
AST_END(Metatable)

AST_NODE(Metamethod)
	ast_sel<true, Name_t, Exp_t, String_t> item;
	AST_MEMBER(Metamethod, &item)
AST_END(Metamethod)

AST_NODE(DotChainItem)
	ast_sel<true, Name_t, Metatable_t, Metamethod_t, UnicodeName_t> name;
	AST_MEMBER(DotChainItem, &name)
AST_END(DotChainItem)

AST_NODE(ColonChainItem)
	ast_sel<true, Name_t, LuaKeyword_t, Metamethod_t, UnicodeName_t> name;
	bool switchToDot = false;
	AST_MEMBER(ColonChainItem, &name)
AST_END(ColonChainItem)

AST_NODE(Slice)
	ast_sel<true, Exp_t, DefaultValue_t> startValue;
	ast_sel<true, Exp_t, DefaultValue_t> stopValue;
	ast_sel<true, Exp_t, DefaultValue_t> stepValue;
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

AST_LEAF(ExistentialOp)
AST_END(ExistentialOp)

AST_LEAF(TableAppendingOp)
AST_END(TableAppendingOp)

AST_NODE(ChainValue)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Slice_t, Exp_t, String_t, InvokeArgs_t, ExistentialOp_t, TableAppendingOp_t> items;
	AST_MEMBER(ChainValue, &sep, &items)
AST_END(ChainValue)

AST_NODE(AssignableChain)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, Callable_t, Invoke_t, DotChainItem_t, ColonChainItem_t, Exp_t, String_t> items;
	AST_MEMBER(AssignableChain, &sep, &items)
AST_END(AssignableChain)

AST_NODE(Value)
	ast_sel<true, SimpleValue_t, SimpleTable_t, ChainValue_t, String_t> item;
	AST_MEMBER(Value, &item)
AST_END(Value)

AST_LEAF(DefaultValue)
AST_END(DefaultValue)

AST_NODE(SpreadExp)
	ast_ptr<true, Exp_t> exp;
	AST_MEMBER(SpreadExp, &exp)
AST_END(SpreadExp)

AST_NODE(SpreadListExp)
	ast_ptr<true, Exp_t> exp;
	AST_MEMBER(SpreadListExp, &exp)
AST_END(SpreadListExp)

AST_NODE(TableLit)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false,
		VariablePairDef_t, NormalPairDef_t, SpreadExp_t, NormalDef_t,
		MetaVariablePairDef_t, MetaNormalPairDef_t,
		VariablePair_t, NormalPair_t, Exp_t,
		MetaVariablePair_t, MetaNormalPair_t,
		/*non-syntax-rule*/ TableBlockIndent_t, SpreadListExp_t> values;
	AST_MEMBER(TableLit, &sep, &values)
AST_END(TableLit)

AST_NODE(TableBlockIndent)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false,
		VariablePair_t, NormalPair_t, Exp_t, TableBlockIndent_t,
		MetaVariablePair_t, MetaNormalPair_t> values;
	AST_MEMBER(TableBlockIndent, &sep, &values)
AST_END(TableBlockIndent)

AST_NODE(TableBlock)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, VariablePair_t, NormalPair_t, TableBlockIndent_t, Exp_t, TableBlock_t, SpreadExp_t, MetaVariablePair_t, MetaNormalPair_t> values;
	AST_MEMBER(TableBlock, &sep, &values)
AST_END(TableBlock)

AST_NODE(ClassMemberList)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, VariablePair_t, NormalPair_t, MetaVariablePair_t, MetaNormalPair_t> values;
	AST_MEMBER(ClassMemberList, &sep, &values)
AST_END(ClassMemberList)

AST_NODE(ClassBlock)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<true, ClassMemberList_t, Statement_t> contents;
	AST_MEMBER(ClassBlock, &sep, &contents)
AST_END(ClassBlock)

AST_NODE(ClassDecl)
	ast_ptr<false, Assignable_t> name;
	ast_ptr<false, Exp_t> extend;
	ast_ptr<false, ExpList_t> mixes;
	ast_ptr<false, ClassBlock_t> body;
	AST_MEMBER(ClassDecl, &name, &extend, &mixes, &body)
AST_END(ClassDecl)

AST_NODE(GlobalValues)
	ast_ptr<true, NameList_t> nameList;
	ast_sel<false, TableBlock_t, ExpListLow_t> valueList;
	AST_MEMBER(GlobalValues, &nameList, &valueList)
AST_END(GlobalValues)

AST_LEAF(GlobalOp)
AST_END(GlobalOp)

AST_NODE(Global)
	ast_sel<true, ClassDecl_t, GlobalOp_t, GlobalValues_t> item;
	AST_MEMBER(Global, &item)
AST_END(Global)

AST_LEAF(ExportDefault)
AST_END(ExportDefault)

AST_NODE(Export)
	ast_ptr<false, ExportDefault_t> def;
	ast_sel<true, ExpList_t, Exp_t, Macro_t, DotChainItem_t> target;
	ast_ptr<false, Assign_t> assign;
	AST_MEMBER(Export, &def, &target, &assign)
AST_END(Export)

AST_NODE(FnArgDef)
	ast_sel<true, Variable_t, SelfItem_t> name;
	ast_ptr<false, ExistentialOp_t> op;
	ast_ptr<false, Exp_t> defaultValue;
	AST_MEMBER(FnArgDef, &name, &op, &defaultValue)
AST_END(FnArgDef)

AST_NODE(FnArgDefList)
	ast_ptr<true, Seperator_t> sep;
	ast_list<false, FnArgDef_t> definitions;
	ast_ptr<false, VarArg_t> varArg;
	AST_MEMBER(FnArgDefList, &sep, &definitions, &varArg)
AST_END(FnArgDefList)

AST_NODE(OuterVarShadow)
	ast_ptr<false, NameList_t> varList;
	AST_MEMBER(OuterVarShadow, &varList)
AST_END(OuterVarShadow)

AST_NODE(FnArgsDef)
	ast_ptr<false, FnArgDefList_t> defList;
	ast_ptr<false, OuterVarShadow_t> shadowOption;
	AST_MEMBER(FnArgsDef, &defList, &shadowOption)
AST_END(FnArgsDef)

AST_LEAF(FnArrow)
AST_END(FnArrow)

AST_NODE(FunLit)
	ast_ptr<false, FnArgsDef_t> argsDef;
	ast_sel<false, ExpListLow_t, DefaultValue_t> defaultReturn;
	ast_ptr<true, FnArrow_t> arrow;
	ast_ptr<false, Body_t> body;
	bool noRecursion = false;
	AST_MEMBER(FunLit, &argsDef, &defaultReturn, &arrow, &body)
AST_END(FunLit)

AST_NODE(MacroName)
	ast_ptr<true, UnicodeName_t> name;
	AST_MEMBER(MacroName, &name)
AST_END(MacroName)

AST_NODE(MacroLit)
	ast_ptr<false, FnArgDefList_t> argsDef;
	ast_ptr<true, Body_t> body;
	AST_MEMBER(MacroLit, &argsDef, &body)
AST_END(MacroLit)

AST_NODE(MacroFunc)
	ast_ptr<true, MacroName_t> name;
	ast_sel<true, Invoke_t, InvokeArgs_t> invoke;
	AST_MEMBER(MacroFunc, &name, &invoke)
AST_END(MacroFunc)

AST_NODE(MacroInPlace)
	ast_ptr<true, Body_t> body;
	AST_MEMBER(MacroInPlace, &body)
AST_END(MacroInPlace)

AST_NODE(Macro)
	ast_ptr<true, UnicodeName_t> name;
	ast_sel<true, MacroLit_t, MacroFunc_t> decl;
	AST_MEMBER(Macro, &name, &decl)
AST_END(Macro)

AST_NODE(NameOrDestructure)
	ast_sel<true, Variable_t, TableLit_t, Comprehension_t> item;
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

AST_LEAF(ConstValue)
AST_END(ConstValue)

AST_NODE(UnaryValue)
	ast_list<true, UnaryOperator_t> ops;
	ast_ptr<true, Value_t> value;
	AST_MEMBER(UnaryValue, &ops, &value)
AST_END(UnaryValue)

AST_NODE(UnaryExp)
	ast_list<false, UnaryOperator_t> ops;
	ast_list<true, Value_t> expos;
	ast_ptr<false, In_t> inExp;
	AST_MEMBER(UnaryExp, &ops, &expos, &inExp)
AST_END(UnaryExp)

AST_NODE(ExpListAssign)
	ast_ptr<true, ExpList_t> expList;
	ast_sel<false, Update_t, Assign_t> action;
	AST_MEMBER(ExpListAssign, &expList, &action)
AST_END(ExpListAssign)

AST_NODE(IfLine)
	ast_ptr<true, IfType_t> type;
	ast_ptr<true, IfCond_t> condition;
	AST_MEMBER(IfLine, &type, &condition)
AST_END(IfLine)

AST_NODE(WhileLine)
	ast_ptr<true, WhileType_t> type;
	ast_ptr<true, Exp_t> condition;
	AST_MEMBER(WhileLine, &type, &condition)
AST_END(WhileLine)

AST_LEAF(BreakLoop)
AST_END(BreakLoop)

AST_NODE(PipeBody)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, UnaryExp_t> values;
	AST_MEMBER(PipeBody, &sep, &values)
AST_END(PipeBody)

AST_NODE(StatementAppendix)
	ast_sel<true, IfLine_t, WhileLine_t, CompInner_t> item;
	AST_MEMBER(StatementAppendix, &item)
AST_END(StatementAppendix)

AST_LEAF(StatementSep)
AST_END(StatementSep)

AST_LEAF(YueLineComment)
AST_END(YueLineComment)

AST_LEAF(MultilineCommentInner)
AST_END(MultilineCommentInner)

AST_NODE(YueMultilineComment)
	ast_ptr<true, MultilineCommentInner_t> inner;
	AST_MEMBER(YueMultilineComment, &inner)
AST_END(YueMultilineComment)

AST_NODE(ChainAssign)
	ast_ptr<true, Seperator_t> sep;
	ast_list<true, Exp_t> exprs;
	ast_ptr<true, Assign_t> assign;
	AST_MEMBER(ChainAssign, &sep, &exprs, &assign)
AST_END(ChainAssign)

AST_NODE(Statement)
	ast_ptr<true, Seperator_t> sep;
	ast_sel_list<false, YueLineComment_t, YueMultilineComment_t> comments;
	ast_sel<true,
		Import_t, While_t, Repeat_t, For_t, ForEach_t,
		Return_t, Local_t, Global_t, Export_t, Macro_t, MacroInPlace_t,
		BreakLoop_t, Label_t, Goto_t, ShortTabAppending_t,
		Backcall_t, LocalAttrib_t, PipeBody_t, ExpListAssign_t, ChainAssign_t
	> content;
	ast_ptr<false, StatementAppendix_t> appendix;
	AST_MEMBER(Statement, &sep, &comments, &content, &appendix)
AST_END(Statement)

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
	ast_ptr<false, Block_t> block;
	AST_MEMBER(File, &block)
AST_END(File)

// clang-format on

struct YueFormat {
	int indent = 0;
	bool spaceOverTab = false;
	int tabSpaces = 4;
	std::string toString(ast_node* node);

	Converter converter{};
	void pushScope();
	void popScope();
	std::string convert(const ast_node* node);
	std::string ind() const;
};

} // namespace parserlib
