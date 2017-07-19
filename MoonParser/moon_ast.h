#pragma once

#include "moon_parser.h"

template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args): Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};
typedef std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> Converter;

std::string& trim(std::string& s);

class AstLeaf : public ast_node
{
public:
	const std::string& getValue();
private:
	std::string _value;
};

#define AST_LEAF(type) \
extern rule type; \
class type##_t : public AstLeaf \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); }

#define AST_NODE(type) \
extern rule type; \
class type##_t : public ast_container \
{ \
public: \
	virtual int get_type() override { return ast_type<type##_t>(); }

#define AST_END(type) \
};

AST_LEAF(Num)
AST_END(Num)

AST_LEAF(_Name)
AST_END(_Name)

AST_NODE(Name)
	ast_ptr<_Name_t> name;
AST_END(Name)

AST_LEAF(self)
AST_END(self)

AST_NODE(self_name)
	ast_ptr<_Name_t> name;
AST_END(self_name)

AST_LEAF(self_class)
AST_END(self_class)

AST_NODE(self_class_name)
	ast_ptr<_Name_t> name;
AST_END(self_class_name)

AST_NODE(SelfName)
	ast_ptr<ast_node> name; // self_class_name_t | self_class_t | self_name_t | self_t
AST_END(SelfName)

AST_NODE(KeyName)
	ast_ptr<ast_node> name; // SelfName_t | _Name_t
AST_END(KeyName)

AST_LEAF(VarArg)
AST_END(VarArg)

AST_LEAF(local_flag)
AST_END(local_flag)

AST_LEAF(Seperator)
AST_END(Seperator)

AST_NODE(NameList)
	ast_ptr<Seperator_t> sep;
	ast_list<Name_t> names;
AST_END(NameList)

AST_NODE(Local)
	ast_ptr<ast_node> name; // local_flag_t | NameList_t
AST_END(Local)

AST_NODE(colon_import_name)
	ast_ptr<Name_t> name;
AST_END(colon_import_name)

class Exp_t;

AST_NODE(ImportName)
	ast_ptr<ast_node> name; // colon_import_name_t | Name_t
AST_END(ImportName)

AST_NODE(Import)
	ast_ptr<Seperator_t> sep;
	ast_list<ImportName_t> names;
	ast_ptr<Exp_t> exp;
AST_END(Import)

AST_NODE(ExpListLow)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> exprs;
AST_END(ExpListLow)

AST_NODE(ExpList)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> exprs;
AST_END(ExpList)

AST_NODE(Return)
	ast_ptr<ExpListLow_t, true> valueList;
AST_END(Return)

class Assign_t;
class Body_t;

AST_NODE(With)
	ast_ptr<ExpList_t> valueList;
	ast_ptr<Assign_t, true> assigns;
	ast_ptr<Body_t> body;
AST_END(With)

AST_NODE(SwitchCase)
	ast_ptr<ExpList_t> valueList;
	ast_ptr<Body_t> body;
AST_END(SwitchCase)

AST_NODE(Switch)
	ast_ptr<Exp_t> target;
	ast_ptr<Seperator_t> sep;
	ast_list<SwitchCase_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(Switch)

AST_NODE(IfCond)
	ast_ptr<Exp_t> condition;
	ast_ptr<Assign_t, true> assign;
AST_END(IfCond)

AST_NODE(IfElseIf)
	ast_ptr<IfCond_t> condition;
	ast_ptr<Body_t> body;
AST_END(IfElseIf)

AST_NODE(If)
	ast_ptr<IfCond_t> firstCondition;
	ast_ptr<Body_t> firstBody;
	ast_ptr<Seperator_t> sep;
	ast_list<IfElseIf_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(If)

AST_NODE(Unless)
	ast_ptr<IfCond_t> firstCondition;
	ast_ptr<Body_t> firstBody;
	ast_ptr<Seperator_t> sep;
	ast_list<IfElseIf_t> branches;
	ast_ptr<Body_t, true> lastBranch;
AST_END(Unless)

AST_NODE(While)
	ast_ptr<Exp_t> condition;
	ast_ptr<Body_t> body;
AST_END(While)

AST_NODE(for_step_value)
	ast_ptr<Exp_t> value;
AST_END(for_step_value)

AST_NODE(For)
	ast_ptr<Name_t> varName;
	ast_ptr<Exp_t> startValue;
	ast_ptr<Exp_t> stopValue;
	ast_ptr<for_step_value_t, true> stepValue;
	ast_ptr<Body_t> body;
AST_END(For)

class AssignableNameList_t;

AST_NODE(ForEach)
	ast_ptr<AssignableNameList_t> nameList;
	ast_ptr<ast_node> loopValue; // Exp_t | ExpList_t
	ast_ptr<Body_t> body;
AST_END(ForEach)

AST_NODE(Do)
	ast_ptr<Body_t> body;
AST_END(Do)

class CompInner_t;

AST_NODE(Comprehension)
	ast_ptr<Exp_t> value;
	ast_ptr<CompInner_t> forLoop;
AST_END(Comprehension)

AST_NODE(comp_value)
	ast_ptr<Exp_t> value;
AST_END(comp_value)

AST_NODE(TblComprehension)
	ast_ptr<Exp_t> key;
	ast_ptr<comp_value_t, true> value;
	ast_ptr<CompInner_t> forLoop;
AST_END(TblComprehension)

AST_NODE(star_exp)
	ast_ptr<Exp_t> value;
AST_END(star_exp)

AST_NODE(CompForEach)
	ast_ptr<AssignableNameList_t> nameList;
	ast_ptr<ast_node> loopValue; // star_exp_t | Exp_t
AST_END(CompForEach)

AST_NODE(CompFor)
	ast_ptr<Name_t> varName;
	ast_ptr<Exp_t> startValue;
	ast_ptr<Exp_t> stopValue;
	ast_ptr<for_step_value_t, true> stepValue;
AST_END(CompFor)

AST_NODE(CompClause)
	ast_ptr<ast_node> nestExp; // CompFor_t | CompForEach_t | Exp_t
AST_END(CompClause)

AST_NODE(CompInner)
	ast_ptr<ast_node> compFor; // CompFor_t | CompForEach_t
	ast_ptr<Seperator_t> sep;
	ast_list<CompClause_t> clauses;
AST_END(CompInner)

class TableBlock_t;

AST_NODE(Assign)
	ast_ptr<ast_node> value; // With_t | If_t | Switch_t | TableBlock_t | ExpListLow_t
AST_END(Assign)

AST_LEAF(update_op)
AST_END(update_op)

AST_NODE(Update)
	ast_ptr<update_op_t> op;
	ast_ptr<Exp_t> value;
AST_END(Update)

AST_LEAF(BinaryOperator)
AST_END(BinaryOperator)

class Chain_t;

AST_NODE(Assignable)
	ast_ptr<ast_node> item; // Chain_t | Name_t | SelfName_t
AST_END(Assignable)

class Value_t;

AST_NODE(exp_op_value)
	ast_ptr<BinaryOperator_t> op;
	ast_ptr<Value_t> value;
AST_END(exp_op_value)

AST_NODE(Exp)
	ast_ptr<Value_t> value;
	ast_list<exp_op_value_t> opValues;
AST_END(Exp)

AST_NODE(Callable)
	ast_ptr<ast_node> item; // Name_t | SelfName_t | VarArg_t | Parens_t
AST_END(Callable)

class InvokeArgs_t;

AST_NODE(ChainValue)
	ast_ptr<ast_node> caller; // Chain_t | Callable_t
	ast_ptr<InvokeArgs_t, true> arguments;
AST_END(ChainValue)

class KeyValue_t;

AST_NODE(simple_table)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> pairs;
AST_END(simple_table)

class String_t;

AST_NODE(SimpleValue)
	ast_ptr<ast_node> value; /*
	const_value_t |
	If_t | Unless_t | Switch_t | With_t | ClassDecl_t | ForEach_t | For_t | While_t | Do_t |
	unary_exp_t |
	TblComprehension_t | TableLit_t | Comprehension_t | FunLit_t | Num_t;
	*/
AST_END(SimpleValue)

AST_NODE(Chain)
	ast_ptr<ast_node> item; // chain_call_t | chain_item_t | chain_dot_chain_t | ColonChain_t
AST_END(Chain)

AST_NODE(Value)
	ast_ptr<ast_node> item; // SimpleValue_t | simple_table_t | ChainValue_t | String_t
AST_END(Value)

AST_LEAF(LuaString)
AST_END(LuaString)

AST_LEAF(SingleString)
AST_END(SingleString)

AST_LEAF(double_string_inner)
AST_END(double_string_inner)

AST_NODE(double_string_content)
	ast_ptr<ast_node> content; // double_string_inner_t | Exp_t
AST_END(double_string_content)

AST_NODE(DoubleString)
	ast_ptr<Seperator_t> sep;
	ast_list<double_string_content_t> segments;
AST_END(DoubleString)

AST_NODE(String)
	ast_ptr<ast_node> str; // DoubleString_t | SingleString_t | LuaString_t
AST_END(String)

AST_NODE(Parens)
	ast_ptr<Exp_t> expr;
AST_END(Parens)

AST_NODE(FnArgs)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> args;
AST_END(FnArgs)

class ChainItems_t;

AST_NODE(chain_call)
	ast_ptr<ast_node> caller; // Callable_t | String_t
	ast_ptr<ChainItems_t> chain;
AST_END(chain_call)

AST_NODE(chain_item)
	ast_ptr<ChainItems_t> chain;
AST_END(chain_item)

AST_NODE(DotChainItem)
	ast_ptr<_Name_t> name;
AST_END(DotChainItem)

AST_NODE(ColonChainItem)
	ast_ptr<_Name_t> name;
AST_END(ColonChainItem)

AST_NODE(chain_dot_chain)
	ast_ptr<DotChainItem_t> caller;
	ast_ptr<ChainItems_t, true> chain;
AST_END(chain_dot_chain)

class ColonChain_t;
class Invoke_t;
class Slice_t;

AST_NODE(ChainItem)
	ast_ptr<ast_node> item; // Invoke_t | DotChainItem_t | Slice_t | [Exp_t]
AST_END(ChainItem)

AST_NODE(ChainItems)
	ast_ptr<Seperator_t> sep;
	ast_list<ChainItem_t> simpleChain;
	ast_ptr<ColonChain_t, true> colonChain;
AST_END(ChainItems)

AST_NODE(invoke_chain)
	ast_ptr<Invoke_t> invoke;
	ast_ptr<ChainItems_t, true> chain;
AST_END(invoke_chain)

AST_NODE(ColonChain)
	ast_ptr<ColonChainItem_t> colonChain;
	ast_ptr<invoke_chain_t, true> invokeChain;
AST_END(ColonChain)

AST_LEAF(default_value)
AST_END(default_value)

AST_NODE(Slice)
	ast_ptr<ast_node> startValue; // Exp_t | default_value_t
	ast_ptr<ast_node> stopValue; // Exp_t | default_value_t
	ast_ptr<ast_node> stepValue; // Exp_t | default_value_t
AST_END(Slice)

AST_NODE(Invoke)
	ast_ptr<ast_node> argument; // FnArgs_t | SingleString_t | DoubleString_t | LuaString_t
AST_END(Invoke)

class KeyValue_t;

AST_NODE(TableValue)
	ast_ptr<ast_node> value; // KeyValue_t | Exp_t
AST_END(TableValue)

AST_NODE(TableLit)
	ast_ptr<Seperator_t> sep;
	ast_list<TableValue_t> values;
AST_END(TableLit)

AST_NODE(TableBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> values;
AST_END(TableBlock)

AST_NODE(class_member_list)
	ast_ptr<Seperator_t> sep;
	ast_list<KeyValue_t> values;
AST_END(class_member_list)

AST_NODE(ClassLine)
	ast_ptr<ast_node> content; // class_member_list_t | Statement_t | Exp_t
AST_END(ClassLine)

AST_NODE(ClassBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<ClassLine_t> lines;
AST_END(ClassBlock)

AST_NODE(ClassDecl)
	ast_ptr<Assignable_t, true> name;
	ast_ptr<Exp_t, true> extend;
	ast_ptr<ClassBlock_t, true> body;
AST_END(ClassDecl)

AST_NODE(export_values)
	ast_ptr<NameList_t> nameList;
	ast_ptr<ExpListLow_t, true> valueList;
AST_END(export_values)

AST_LEAF(export_op)
AST_END(export_op)

AST_NODE(Export)
	ast_ptr<ast_node> item; // ClassDecl_t | export_op_t | export_values_t
AST_END(Export)

AST_NODE(variable_pair)
	ast_ptr<Name_t> name;
AST_END(variable_pair)

AST_NODE(normal_pair)
	ast_ptr<ast_node> key; // KeyName_t | [Exp_t] | DoubleString_t | SingleString_t
	ast_ptr<ast_node> value; // Exp_t | TableBlock_t
AST_END(normal_pair)

AST_NODE(KeyValue)
	ast_ptr<ast_node> item; // variable_pair_t | normal_pair_t
AST_END(KeyValue)

AST_NODE(FnArgDef)
	ast_ptr<ast_node> name; // Name_t | SelfName_t
	ast_ptr<Exp_t, true> defaultValue;
AST_END(FnArgDef)

AST_NODE(FnArgDefList)
	ast_ptr<Seperator_t> sep;
	ast_list<FnArgDef_t> definitions;
	ast_ptr<VarArg_t, true> varArg;
AST_END(FnArgDefList)

AST_NODE(outer_var_shadow)
	ast_ptr<NameList_t, true> varList;
AST_END(outer_var_shadow)

AST_NODE(FnArgsDef)
	ast_ptr<FnArgDefList_t, true> defList;
	ast_ptr<outer_var_shadow_t, true> shadowOption;
AST_END(FnArgsDef)

AST_LEAF(fn_arrow)
AST_END(fn_arrow)

AST_NODE(FunLit)
	ast_ptr<FnArgsDef_t, true> argsDef;
	ast_ptr<fn_arrow_t> arrow;
	ast_ptr<Body_t, true> body;
AST_END(FunLit)

AST_NODE(NameOrDestructure)
	ast_ptr<ast_node> item; // Name_t | TableLit_t
AST_END(NameOrDestructure)

AST_NODE(AssignableNameList)
	ast_ptr<Seperator_t> sep;
	ast_list<NameOrDestructure_t> items;
AST_END(AssignableNameList)

AST_NODE(ArgBlock)
	ast_ptr<Seperator_t> sep;
	ast_list<Exp_t> arguments;
AST_END(ArgBlock)

AST_NODE(invoke_args_with_table)
	ast_ptr<ArgBlock_t, true> argBlock;
	ast_ptr<TableBlock_t, true> tableBlock;
AST_END(invoke_args_with_table)

AST_NODE(InvokeArgs)
	ast_ptr<ExpList_t, true> argsList;
	ast_ptr<invoke_args_with_table_t, true> argsTableBlock;
	ast_ptr<TableBlock_t, true> tableBlock;
AST_END(InvokeArgs)

AST_LEAF(const_value)
AST_END(const_value)

AST_NODE(unary_exp)
	ast_ptr<Exp_t> item;
AST_END(unary_exp)

AST_NODE(Assignment)
	ast_ptr<ExpList_t> assignable;
	ast_ptr<ast_node> target; // Update_t | Assign_t
AST_END(Assignment)

AST_NODE(if_else_line)
	ast_ptr<Exp_t> condition;
	ast_ptr<ast_node> elseExpr; // Exp_t | default_value_t
AST_END(if_else_line)

AST_NODE(unless_line)
	ast_ptr<Exp_t> condition;
AST_END(unless_line)

AST_NODE(statement_appendix)
	ast_ptr<ast_node> item; // if_else_line_t | unless_line_t | CompInner_t
AST_END(statement_appendix)

AST_LEAF(BreakLoop)
AST_END(BreakLoop)

AST_NODE(Statement)
	ast_ptr<ast_node> content; /*
	Import_t | While_t | With_t | For_t | ForEach_t |
	Switch_t | Return_t | Local_t | Export_t | BreakLoop_t |
	Assignment_t | ExpList_t
	*/
	ast_ptr<statement_appendix_t, true> appendix;
AST_END(Statement)

class Block_t;

AST_NODE(Body)
	ast_ptr<ast_node> content; // Block | Statement
AST_END(Body)

AST_NODE(Line)
	ast_ptr<Statement_t, true> statment;
AST_END(Line)

AST_NODE(Block)
	ast_ptr<Seperator_t> sep;
	ast_list<Line_t> lines;
AST_END(Block)

AST_NODE(BlockEnd)
	ast_ptr<Block_t> block;
AST_END(BlockEnd)
