/* Copyright (c) 2017-2025 Li Jin <dragon-fly@qq.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "yuescript/yue_ast.h"

#include <sstream>

namespace parserlib {
using namespace std::string_view_literals;
using namespace std::string_literals;

void YueFormat::pushScope() {
	indent++;
}

void YueFormat::popScope() {
	indent--;
}

std::string YueFormat::ind() const {
	if (spaceOverTab) {
		return std::string(indent * tabSpaces, ' ');
	}
	return std::string(indent, '\t');
}

std::string YueFormat::convert(const ast_node* node) {
	return converter.to_bytes(std::wstring(node->m_begin.m_it, node->m_end.m_it));
}

std::string YueFormat::toString(ast_node* node) {
	return node->to_string(this);
}

typedef std::list<std::string> str_list;

static std::string join(const str_list& items, std::string_view sep = {}) {
	if (items.empty())
		return {};
	else if (items.size() == 1)
		return items.front();
	std::ostringstream joinBuf;
	if (sep.empty()) {
		for (const auto& item : items) {
			joinBuf << item;
		}
	} else {
		joinBuf << items.front();
		auto begin = ++items.begin();
		for (auto it = begin; it != items.end(); ++it) {
			joinBuf << sep << *it;
		}
	}
	return joinBuf.str();
}

namespace yue {

std::string Num_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string Name_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string UnicodeName_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string Self_t::to_string(void*) const {
	return "@"s;
}
std::string SelfClass_t::to_string(void*) const {
	return "@@"s;
}
std::string VarArg_t::to_string(void*) const {
	return "..."s;
}
std::string Seperator_t::to_string(void*) const {
	return {};
}
std::string LocalFlag_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string ConstAttrib_t::to_string(void*) const {
	return "const"s;
}
std::string CloseAttrib_t::to_string(void*) const {
	return "close"s;
}
std::string ImportLiteralInner_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string ImportAllMacro_t::to_string(void*) const {
	return "$"s;
}
std::string FnArrowBack_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string IfType_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string WhileType_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string UpdateOp_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string BinaryOperator_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto op = info->convert(this);
	if (op == "!="sv) {
		return "~="s;
	}
	return op;
}
std::string UnaryOperator_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto op = info->convert(this);
	if (op == "not"sv) {
		return "not "s;
	}
	return op;
}
std::string LuaStringOpen_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string LuaStringContent_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string LuaStringClose_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string SingleString_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string DoubleStringInner_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string Metatable_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string DefaultValue_t::to_string(void*) const {
	return {};
}
std::string ExistentialOp_t::to_string(void*) const {
	return "?"s;
}
std::string TableAppendingOp_t::to_string(void*) const {
	return "[]"s;
}
std::string GlobalOp_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string ExportDefault_t::to_string(void*) const {
	return "default"s;
}
std::string FnArrow_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string ConstValue_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string NotIn_t::to_string(void*) const {
	return {};
}
std::string BreakLoop_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string YueLineComment_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return "--"s + info->convert(this);
}
std::string MultilineCommentInner_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	return info->convert(this);
}
std::string Variable_t::to_string(void* ud) const {
	return name->to_string(ud);
}
std::string LabelName_t::to_string(void* ud) const {
	return name->to_string(ud);
}
std::string LuaKeyword_t::to_string(void* ud) const {
	return name->to_string(ud);
}
std::string SelfName_t::to_string(void* ud) const {
	return "@"s + name->to_string(ud);
}
std::string SelfClassName_t::to_string(void* ud) const {
	return "@@"s + name->to_string(ud);
}
std::string SelfItem_t::to_string(void* ud) const {
	return name->to_string(ud);
}
std::string KeyName_t::to_string(void* ud) const {
	return name->to_string(ud);
}
std::string NameList_t::to_string(void* ud) const {
	str_list temp;
	for (auto name : names.objects()) {
		temp.emplace_back(name->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string LocalValues_t::to_string(void* ud) const {
	str_list temp;
	temp.emplace_back(nameList->to_string(ud));
	if (valueList) {
		if (valueList.is<TableBlock_t>()) {
			temp.emplace_back("=\n"s + valueList->to_string(ud));
		} else {
			temp.emplace_back("="s);
			temp.emplace_back(valueList->to_string(ud));
		}
	}
	return join(temp, " "sv);
}
std::string Local_t::to_string(void* ud) const {
	return "local "s + item->to_string(ud);
}
std::string LocalAttrib_t::to_string(void* ud) const {
	str_list temp;
	for (auto item : leftList.objects()) {
		temp.emplace_back(item->to_string(ud));
	}
	return attrib->to_string(ud) + ' ' + join(temp, ", "s) + ' ' + assign->to_string(ud);
}
std::string ColonImportName_t::to_string(void* ud) const {
	return '\\' + name->to_string(ud);
}
std::string ImportLiteral_t::to_string(void* ud) const {
	str_list temp;
	for (auto inner : inners.objects()) {
		temp.emplace_back(inner->to_string(ud));
	}
	return '"' + join(temp, "."sv) + '"';
}
std::string ImportFrom_t::to_string(void* ud) const {
	str_list temp;
	for (auto name : names.objects()) {
		temp.emplace_back(name->to_string(ud));
	}
	return join(temp, ", "sv) + " from "s + item->to_string(ud);
}
std::string FromImport_t::to_string(void* ud) const {
	str_list temp;
	for (auto name : names.objects()) {
		temp.emplace_back(name->to_string(ud));
	}
	return "from "s + item->to_string(ud) + " import "s + join(temp, ", "sv);
}
std::string MacroNamePair_t::to_string(void* ud) const {
	return key->to_string(ud) + ": "s + value->to_string(ud);
}
std::string ImportTabLit_t::to_string(void* ud) const {
	str_list temp;
	for (auto item : items.objects()) {
		if (ast_is<MacroName_t>(item)) {
			temp.emplace_back(':' + item->to_string(ud));
		} else {
			temp.emplace_back(item->to_string(ud));
		}
	}
	return '{' + join(temp, ", "sv) + '}';
}
std::string ImportAs_t::to_string(void* ud) const {
	str_list temp{literal->to_string(ud)};
	if (target) {
		temp.emplace_back("as"s);
		temp.emplace_back(target->to_string(ud));
	}
	return join(temp, " "s);
}
std::string Import_t::to_string(void* ud) const {
	if (ast_is<FromImport_t>(content)) {
		return content->to_string(ud);
	}
	return "import "s + content->to_string(ud);
}
std::string Label_t::to_string(void* ud) const {
	return "::"s + label->to_string(ud) + "::"s;
}
std::string Goto_t::to_string(void* ud) const {
	return "goto "s + label->to_string(ud);
}
std::string ShortTabAppending_t::to_string(void* ud) const {
	return "[] "s + assign->to_string(ud);
}
std::string Backcall_t::to_string(void* ud) const {
	str_list temp;
	if (argsDef) {
		auto def = argsDef->to_string(ud);
		if (!def.empty()) {
			temp.emplace_back(def);
		}
	}
	temp.emplace_back(arrow->to_string(ud));
	temp.emplace_back(value->to_string(ud));
	return join(temp, " "sv);
}
std::string PipeBody_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	for (auto value : values.objects()) {
		temp.emplace_back(info->ind() + "|> "s + value->to_string(ud));
	}
	return join(temp, "\n"sv);
}
std::string ExpListLow_t::to_string(void* ud) const {
	str_list temp;
	for (auto exp : exprs.objects()) {
		temp.emplace_back(exp->to_string(ud));
	}
	return join(temp, "; "sv);
}
std::string ExpList_t::to_string(void* ud) const {
	str_list temp;
	for (auto exp : exprs.objects()) {
		temp.emplace_back(exp->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string Return_t::to_string(void* ud) const {
	str_list temp{"return"s};
	if (valueList) {
		temp.emplace_back(valueList.is<TableBlock_t>() ? "\n"s : " "s);
		temp.emplace_back(valueList->to_string(ud));
	}
	return join(temp);
}
std::string With_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp{
		eop ? "with?"s : "with"s,
		valueList->to_string(ud)};
	if (assigns) {
		temp.push_back(assigns->to_string(ud));
	}
	if (body.is<Statement_t>()) {
		return join(temp, " "sv) + " do "s + body->to_string(ud);
	} else {
		auto line = join(temp, " "sv);
		if (line.find('\n') != std::string::npos) {
			line += " do"s;
		}
		info->pushScope();
		auto code = body->to_string(ud);
		if (code.empty()) {
			code = info->ind() + "--"s;
		}
		info->popScope();
		return line + '\n' + code;
	}
}
std::string SwitchList_t::to_string(void* ud) const {
	str_list temp;
	for (auto exp : exprs.objects()) {
		temp.emplace_back(exp->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string SwitchCase_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	if (body.is<Statement_t>()) {
		return "when "s + condition->to_string(ud) + " then "s + body->to_string(ud);
	} else {
		info->pushScope();
		auto block = body->to_string(ud);
		if (block.empty()) {
			block = info->ind() + "--"s;
		}
		info->popScope();
		auto line = "when "s + condition->to_string(ud);
		if (line.find('\n') != std::string::npos) {
			line += " then"s;
		}
		return line + '\n' + block;
	}
}
std::string Switch_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp{"switch "s + target->to_string(ud)};
	info->pushScope();
	for (auto branch : branches.objects()) {
		temp.emplace_back(info->ind() + branch->to_string(ud));
	}
	if (lastBranch) {
		if (lastBranch.is<Statement_t>()) {
			temp.emplace_back(info->ind() + "else "s + lastBranch->to_string(ud));
		} else {
			temp.emplace_back(info->ind() + "else"s);
			info->pushScope();
			temp.emplace_back(lastBranch->to_string(ud));
			if (temp.back().empty()) {
				temp.back() = info->ind() + "--"s;
			}
			info->popScope();
		}
	}
	info->popScope();
	return join(temp, "\n"sv);
}
std::string Assignment_t::to_string(void* ud) const {
	if (expList) {
		return ", "s + expList->to_string(ud) + " :"s + assign->to_string(ud);
	} else {
		return " :"s + assign->to_string(ud);
	}
}
std::string IfCond_t::to_string(void* ud) const {
	if (assignment) {
		return condition->to_string(ud) + assignment->to_string(ud);
	} else {
		return condition->to_string(ud);
	}
}
std::string If_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto it = nodes.objects().begin();
	str_list temp{
		type->to_string(ud) + ' ' + (*it)->to_string(ud)};
	if (temp.back().find('\n') != std::string::npos) {
		temp.back() += " then"s;
	}
	++it;
	bool condition = true;
	for (; it != nodes.objects().end(); ++it) {
		auto node = *it;
		switch (node->get_id()) {
			case id<IfCond_t>():
				temp.emplace_back(info->ind() + "elseif "s + node->to_string(ud));
				condition = true;
				break;
			case id<Statement_t>(): {
				if (condition) {
					temp.back() += " then "s + node->to_string(ud);
				} else {
					temp.emplace_back(info->ind() + "else "s + node->to_string(ud));
				}
				condition = false;
				break;
			}
			case id<Block_t>(): {
				if (condition) {
					info->pushScope();
					temp.emplace_back(node->to_string(ud));
					if (temp.back().empty()) {
						temp.back() = info->ind() + "--"s;
					}
					info->popScope();
				} else {
					temp.emplace_back(info->ind() + "else"s);
					info->pushScope();
					temp.emplace_back(node->to_string(ud));
					if (temp.back().empty()) {
						temp.back() = info->ind() + "--"s;
					}
					info->popScope();
				}
				condition = false;
				break;
			}
		}
	}
	return join(temp, "\n"sv);
}
std::string While_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp{
		type->to_string(ud) + ' ' + condition->to_string(ud) + (assignment ? assignment->to_string(ud) : std::string())};
	if (body.is<Statement_t>()) {
		temp.back() += " do "s + body->to_string(ud);
	} else {
		if (temp.back().find('\n') != std::string::npos) {
			temp.back() += " do"s;
		}
		info->pushScope();
		temp.emplace_back(body->to_string(ud));
		if (temp.back().empty()) {
			temp.back() = info->ind() + "--"s;
		}
		info->popScope();
	}
	return join(temp, "\n"sv);
}
std::string Repeat_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	if (body->content.is<Statement_t>()) {
		temp.emplace_back("repeat "s + body->to_string(ud));
	} else {
		temp.emplace_back("repeat"s);
		info->pushScope();
		temp.emplace_back(body->to_string(ud));
		if (temp.back().empty()) {
			temp.back() = info->ind() + "--"s;
		}
		info->popScope();
	}
	temp.emplace_back(info->ind() + "until "s + condition->to_string(ud));
	return join(temp, "\n"sv);
}
std::string ForStepValue_t::to_string(void* ud) const {
	return value->to_string(ud);
}
std::string For_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto line = "for "s + varName->to_string(ud) + " = "s + startValue->to_string(ud) + ", "s + stopValue->to_string(ud);
	if (stepValue) {
		line += ", "s + stepValue->to_string(ud);
	}
	if (body.is<Statement_t>()) {
		return line + " do "s + body->to_string(ud);
	} else {
		if (line.find('\n') != std::string::npos) {
			line += " do"s;
		}
		info->pushScope();
		auto block = body->to_string(ud);
		if (block.empty()) {
			block = info->ind() + "--"s;
		}
		info->popScope();
		return line + '\n' + block;
	}
}
std::string ForEach_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto line = "for "s + nameList->to_string(ud) + " in "s + loopValue->to_string(ud);
	if (body.is<Statement_t>()) {
		return line + " do "s + body->to_string(ud);
	} else {
		if (line.find('\n') != std::string::npos) {
			line += " do"s;
		}
		info->pushScope();
		auto block = body->to_string(ud);
		if (block.empty()) {
			block = info->ind() + "--"s;
		}
		info->popScope();
		return line + '\n' + block;
	}
}
std::string Do_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	if (body->content.is<Statement_t>()) {
		return "do "s + body->to_string(ud);
	} else {
		info->pushScope();
		auto block = body->to_string(ud);
		if (block.empty()) {
			block = info->ind() + "--"s;
		}
		info->popScope();
		return "do\n"s + block;
	}
}
std::string CatchBlock_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto line = "catch "s + err->to_string(ud);
	info->pushScope();
	auto blockStr = block->to_string(ud);
	if (blockStr.empty()) {
		blockStr = info->ind() + "--"s;
	}
	info->popScope();
	return line + '\n' + blockStr;
}
std::string Try_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	if (func.is<Exp_t>()) {
		temp.emplace_back("try "s + func->to_string(ud));
	} else {
		temp.emplace_back("try"s);
		info->pushScope();
		temp.emplace_back(func->to_string(ud));
		if (temp.back().empty()) {
			temp.back() = info->ind() + "--"s;
		}
		info->popScope();
	}
	if (catchBlock) {
		temp.emplace_back(info->ind() + catchBlock->to_string(ud));
	}
	return join(temp, "\n"sv);
}
static bool isInBlockExp(ast_node* node, bool last = false) {
	if (auto exp = ast_cast<Exp_t>(node)) {
		UnaryExp_t* unaryExp = nullptr;
		if (exp->opValues.empty()) {
			unaryExp = static_cast<UnaryExp_t*>(exp->pipeExprs.back());
		} else {
			unaryExp = static_cast<UnaryExp_t*>(static_cast<ExpOpValue_t*>(exp->opValues.back())->pipeExprs.back());
		}
		auto value = static_cast<Value_t*>(unaryExp->expos.back());
		if (auto simpleValue = value->item.as<SimpleValue_t>()) {
			switch (simpleValue->value->get_id()) {
				case id<TableLit_t>():
				case id<ConstValue_t>():
				case id<Num_t>():
				case id<VarArg_t>():
				case id<TblComprehension_t>():
				case id<Comprehension_t>():
					return false;
				case id<FunLit_t>():
					if (!last) {
						return true;
					}
					return false;
				default:
					return true;
			}
		} else if (auto chainValue = value->item.as<ChainValue_t>()) {
			if (ast_is<InvokeArgs_t>(chainValue->items.back())) {
				return true;
			}
		} else if (!last && value->item.is<SimpleTable_t>()) {
			return true;
		}
	} else if (ast_is<TableBlock_t>(node)) {
		return true;
	} else {
		switch (node->get_id()) {
			case id<VariablePairDef_t>(): {
				auto pair = static_cast<VariablePairDef_t*>(node);
				if (pair->defVal) {
					return true;
				}
				return false;
			}
			case id<NormalPairDef_t>(): {
				auto pair = static_cast<NormalPairDef_t*>(node);
				if (pair->defVal) {
					return true;
				}
				return isInBlockExp(pair->pair->value);
			}
			case id<SpreadExp_t>(): {
				auto pair = static_cast<SpreadExp_t*>(node);
				return isInBlockExp(pair->exp);
			}
			case id<NormalDef_t>(): {
				auto pair = static_cast<NormalDef_t*>(node);
				if (pair->defVal) {
					return true;
				}
				return isInBlockExp(pair->item);
			}
			case id<MetaVariablePairDef_t>(): {
				auto pair = static_cast<MetaVariablePairDef_t*>(node);
				if (pair->defVal) {
					return true;
				}
				return false;
			}
			case id<MetaNormalPairDef_t>(): {
				auto pair = static_cast<MetaNormalPairDef_t*>(node);
				if (pair->defVal) {
					return true;
				}
				return isInBlockExp(pair->pair->value);
			}
			case id<VariablePair_t>(): {
				return false;
			}
			case id<NormalPair_t>(): {
				auto pair = static_cast<NormalPair_t*>(node);
				return isInBlockExp(pair->value);
			}
			case id<MetaVariablePair_t>(): {
				return false;
			}
			case id<MetaNormalPair_t>(): {
				auto pair = static_cast<MetaNormalPair_t*>(node);
				return isInBlockExp(pair->value);
			}
			case id<TableBlockIndent_t>(): {
				return true;
			}
			case id<SpreadListExp_t>(): {
				auto pair = static_cast<SpreadListExp_t*>(node);
				return isInBlockExp(pair->exp);
			}
			default:
				return false;
		}
	}
	return false;
}
std::string Comprehension_t::to_string(void* ud) const {
	if (items.size() != 2 || !ast_is<CompInner_t>(items.back())) {
		if (items.size() == 1) {
			str_list temp;
			for (const auto& item : items.objects()) {
				temp.push_back(item->to_string(ud));
			}
			if (temp.size() > 0) {
				temp.front().insert(0, temp.front()[0] == '[' ? " "s : ""s);
			}
			return '[' + join(temp, ", "sv) + ",]"s;
		} else {
			bool hasInBlockExp = false;
			for (auto value : items.objects()) {
				if (isInBlockExp(value, value == items.back())) {
					hasInBlockExp = true;
					break;
				}
			}
			if (hasInBlockExp) {
				auto info = reinterpret_cast<YueFormat*>(ud);
				str_list temp;
				temp.emplace_back("["s);
				info->pushScope();
				for (auto value : items.objects()) {
					temp.emplace_back(info->ind() + value->to_string(ud));
				}
				info->popScope();
				temp.emplace_back(info->ind() + ']');
				return join(temp, "\n"sv);
			} else {
				str_list temp;
				for (const auto& item : items.objects()) {
					temp.push_back(item->to_string(ud));
				}
				if (temp.size() > 0) {
					temp.front().insert(0, temp.front()[0] == '[' ? " "s : ""s);
				}
				return '[' + join(temp, ", "sv) + ']';
			}
		}
	} else {
		str_list temp;
		for (const auto& item : items.objects()) {
			temp.push_back(item->to_string(ud));
		}
		if (temp.size() > 0) {
			temp.front().insert(0, temp.front()[0] == '[' ? " "s : ""s);
		}
		return '[' + join(temp, " "sv) + ']';
	}
}
std::string CompValue_t::to_string(void* ud) const {
	return value->to_string(ud);
}
std::string TblComprehension_t::to_string(void* ud) const {
	auto line = '{' + key->to_string(ud);
	if (value) {
		line += ", "s + value->to_string(ud);
	}
	return line + ' ' + forLoop->to_string(ud) + '}';
}
std::string StarExp_t::to_string(void* ud) const {
	return '*' + value->to_string(ud);
}
std::string CompForEach_t::to_string(void* ud) const {
	return "for "s + nameList->to_string(ud) + " in "s + loopValue->to_string(ud);
}
std::string CompFor_t::to_string(void* ud) const {
	auto line = "for "s + varName->to_string(ud) + " = "s + startValue->to_string(ud) + ", "s + stopValue->to_string(ud);
	if (stepValue) {
		line += stepValue->to_string(ud);
	}
	return line;
}
std::string CompInner_t::to_string(void* ud) const {
	str_list temp;
	for (auto item : items.objects()) {
		if (ast_is<Exp_t>(item)) {
			temp.emplace_back("when "s + item->to_string(ud));
		} else {
			temp.emplace_back(item->to_string(ud));
		}
	}
	return join(temp, " "sv);
}
std::string Assign_t::to_string(void* ud) const {
	str_list temp;
	if (values.size() == 1 && ast_is<TableBlock_t>(values.front())) {
		return "=\n"s + values.front()->to_string(ud);
	}
	for (auto value : values.objects()) {
		temp.emplace_back(value->to_string(ud));
	}
	return "= "s + join(temp, "; "sv);
}
std::string Update_t::to_string(void* ud) const {
	return op->to_string(ud) + "= "s + value->to_string(ud);
}
std::string Assignable_t::to_string(void* ud) const {
	return item->to_string(ud);
}
std::string AssignableChain_t::to_string(void* ud) const {
	str_list temp;
	for (auto item : items.objects()) {
		if (ast_is<Exp_t, String_t>(item)) {
			auto valueStr = item->to_string(ud);
			temp.emplace_back('[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']');
		} else {
			temp.emplace_back(item->to_string(ud));
		}
	}
	return join(temp);
}
std::string ExpOpValue_t::to_string(void* ud) const {
	str_list pipes;
	for (auto uexp : pipeExprs.objects()) {
		pipes.emplace_back(uexp->to_string(ud));
	}
	return op->to_string(ud) + ' ' + join(pipes, " |> "sv);
}
std::string Exp_t::to_string(void* ud) const {
	str_list pipes;
	for (auto uexp : pipeExprs.objects()) {
		pipes.emplace_back(uexp->to_string(ud));
	}
	str_list temp{join(pipes, " |> "sv)};
	for (auto opValue : opValues.objects()) {
		temp.emplace_back(opValue->to_string(ud));
	}
	if (nilCoalesed) {
		temp.emplace_back("??"s);
		temp.emplace_back(nilCoalesed->to_string(ud));
	}
	return join(temp, " "sv);
}
std::string Callable_t::to_string(void* ud) const {
	return item->to_string(ud);
}
std::string ChainValue_t::to_string(void* ud) const {
	str_list temp;
	bool isMultilineChain = false;
	for (auto item : items.objects()) {
		if (item != items.back() && ast_is<InvokeArgs_t>(item)) {
			isMultilineChain = true;
			break;
		}
	}
	if (isMultilineChain) {
		auto it = items.objects().begin();
		auto node = *it;
		if (ast_is<Exp_t>(node)) {
			auto valueStr = node->to_string(ud);
			temp.emplace_back('[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']');
		} else {
			temp.emplace_back(node->to_string(ud));
		}
		++it;
		auto info = reinterpret_cast<YueFormat*>(ud);
		info->pushScope();
		for (; it != items.objects().end(); ++it) {
			node = *it;
			switch (node->get_id()) {
				case id<Exp_t>(): {
					auto valueStr = node->to_string(ud);
					temp.emplace_back(info->ind() + '[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']');
					break;
				}
				case id<Invoke_t>():
				case id<InvokeArgs_t>():
					temp.back() += node->to_string(ud);
					break;
				default:
					temp.emplace_back(info->ind() + node->to_string(ud));
					break;
			}
		}
		info->popScope();
		return join(temp, "\n"sv);
	} else {
		for (auto item : items.objects()) {
			if (ast_is<Exp_t>(item)) {
				auto valueStr = item->to_string(ud);
				temp.emplace_back('[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']');
			} else {
				temp.emplace_back(item->to_string(ud));
			}
		}
		return join(temp);
	}
}
std::string SimpleTable_t::to_string(void* ud) const {
	str_list temp;
	for (auto pair : pairs.objects()) {
		temp.emplace_back(pair->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string SimpleValue_t::to_string(void* ud) const {
	return value->to_string(ud);
}
std::string Value_t::to_string(void* ud) const {
	return item->to_string(ud);
}
std::string LuaString_t::to_string(void* ud) const {
	auto str = content->to_string(ud);
	auto newLine = str.find_first_of("\n"s) != std::string::npos ? "\n"s : ""s;
	return open->to_string(ud) + newLine + str + close->to_string(ud);
}
std::string DoubleStringContent_t::to_string(void* ud) const {
	if (content.is<Exp_t>()) {
		return "#{"s + content->to_string(ud) + '}';
	}
	return content->to_string(ud);
}
std::string DoubleString_t::to_string(void* ud) const {
	str_list temp;
	for (auto seg : segments.objects()) {
		temp.emplace_back(seg->to_string(ud));
	}
	return '"' + join(temp) + '"';
}
std::string String_t::to_string(void* ud) const {
	return str->to_string(ud);
}
std::string Parens_t::to_string(void* ud) const {
	return '(' + expr->to_string(ud) + ')';
}
std::string DotChainItem_t::to_string(void* ud) const {
	return '.' + name->to_string(ud);
}
std::string ColonChainItem_t::to_string(void* ud) const {
	return '\\' + name->to_string(ud);
}
std::string Metamethod_t::to_string(void* ud) const {
	if (item.is<Exp_t>()) {
		auto valueStr = item->to_string(ud);
		return "<["s + (valueStr[0] == '[' ? " "s : ""s) + valueStr + "]>"s;
	} else {
		return '<' + item->to_string(ud) + '>';
	}
}
std::string Slice_t::to_string(void* ud) const {
	str_list temp;
	if (startValue.is<Exp_t>()) {
		temp.emplace_back(startValue->to_string(ud));
	}
	if (stopValue.is<Exp_t>()) {
		temp.emplace_back();
		temp.emplace_back(", "s + stopValue->to_string(ud));
	} else {
		temp.emplace_back(","s);
	}
	if (stepValue.is<Exp_t>()) {
		temp.emplace_back(", "s + stepValue->to_string(ud));
	}
	auto valueStr = join(temp);
	return '[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']';
}
std::string Invoke_t::to_string(void* ud) const {
	if (args.empty()) {
		return "!"s;
	} else if (args.size() == 1) {
		auto arg = args.front();
		if (ast_is<Exp_t>(arg)) {
			return '(' + arg->to_string(ud) + ')';
		} else {
			return arg->to_string(ud);
		}
	} else {
		auto info = reinterpret_cast<YueFormat*>(ud);
		bool hasInBlockExp = false;
		for (auto arg : args.objects()) {
			if (isInBlockExp(arg, arg == args.back())) {
				hasInBlockExp = true;
				break;
			}
		}
		str_list temp;
		if (hasInBlockExp) {
			temp.push_back("("s);
			info->pushScope();
			for (auto arg : args.objects()) {
				temp.emplace_back(info->ind() + arg->to_string(ud));
			}
			info->popScope();
			temp.push_back(info->ind() + ')');
			return join(temp, "\n"sv);
		} else {
			for (auto arg : args.objects()) {
				temp.emplace_back(arg->to_string(ud));
			}
			return '(' + join(temp, ", "sv) + ')';
		}
	}
}
std::string SpreadExp_t::to_string(void* ud) const {
	return "..."s + exp->to_string(ud);
}
std::string SpreadListExp_t::to_string(void* ud) const {
	return "..."s + exp->to_string(ud);
}
std::string TableLit_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	if (values.empty()) {
		return "{ }"s;
	}
	bool hasInBlockExp = false;
	for (auto value : values.objects()) {
		if (isInBlockExp(value, value == values.back())) {
			hasInBlockExp = true;
			break;
		}
	}
	if (hasInBlockExp) {
		str_list temp;
		temp.emplace_back("{"s);
		info->pushScope();
		for (auto value : values.objects()) {
			temp.emplace_back(info->ind() + value->to_string(ud));
		}
		info->popScope();
		temp.emplace_back(info->ind() + '}');
		return join(temp, "\n"sv);
	} else {
		str_list temp;
		for (auto value : values.objects()) {
			temp.emplace_back(value->to_string(ud));
		}
		return '{' + join(temp, ", "sv) + '}';
	}
}
std::string TableBlock_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	info->pushScope();
	for (auto value : values.objects()) {
		switch (value->get_id()) {
			case id<Exp_t>():
			case id<SpreadExp_t>():
				temp.emplace_back(info->ind() + "* "s + value->to_string(ud));
				break;
			case id<TableBlock_t>():
				temp.emplace_back(info->ind() + "*\n"s + value->to_string(ud));
				break;
			default:
				temp.emplace_back(info->ind() + value->to_string(ud));
				break;
		}
	}
	info->popScope();
	return join(temp, "\n"sv);
}
std::string TableBlockIndent_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	info->pushScope();
	for (auto value : values.objects()) {
		if (ast_is<TableBlockIndent_t>(value)) {
			temp.emplace_back("\n"s + value->to_string(ud));
		} else {
			if (value == values.front()) {
				temp.emplace_back(" "s + value->to_string(ud));
			} else {
				temp.emplace_back(info->ind() + value->to_string(ud));
			}
		}
	}
	info->popScope();
	return "*"s + join(temp, "\n"sv);
}
std::string ClassMemberList_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	for (auto value : values.objects()) {
		temp.emplace_back(info->ind() + value->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string ClassBlock_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	info->pushScope();
	for (auto content : contents.objects()) {
		if (ast_is<Statement_t>(content)) {
			temp.emplace_back(info->ind() + content->to_string(ud));
		} else {
			temp.emplace_back(content->to_string(ud));
		}
	}
	info->popScope();
	return '\n' + join(temp, "\n"sv);
}
std::string ClassDecl_t::to_string(void* ud) const {
	auto line = "class"s;
	if (name) {
		line += ' ' + name->to_string(ud);
	}
	if (extend) {
		line += " extends "s + extend->to_string(ud);
	}
	if (mixes) {
		line += " using "s + mixes->to_string(ud);
	}
	if (body) {
		line += body->to_string(ud);
	}
	return line;
}
std::string GlobalValues_t::to_string(void* ud) const {
	auto line = nameList->to_string(ud);
	if (valueList) {
		if (valueList.is<TableBlock_t>()) {
			line += " =\n"s + valueList->to_string(ud);
		} else {
			line += " = "s + valueList->to_string(ud);
		}
	}
	return line;
}
std::string Global_t::to_string(void* ud) const {
	return "global "s + item->to_string(ud);
}
std::string Export_t::to_string(void* ud) const {
	auto line = "export"s;
	if (def) {
		line += " default "s;
	}
	switch (target->get_id()) {
		case id<DotChainItem_t>():
			line += target->to_string(ud);
			break;
		case id<Exp_t>(): {
			if (def) {
				line += target->to_string(ud);
			} else {
				auto valueStr = target->to_string(ud);
				line += '[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + ']';
			}
			break;
		}
		default:
			line += ' ' + target->to_string(ud);
			break;
	}
	if (assign) {
		line += ' ' + assign->to_string(ud);
	}
	return line;
}
std::string VariablePair_t::to_string(void* ud) const {
	return ':' + name->to_string(ud);
}
std::string NormalPair_t::to_string(void* ud) const {
	std::string line;
	if (key.is<Exp_t>()) {
		auto valueStr = key->to_string(ud);
		line = '[' + (valueStr[0] == '[' ? " "s : ""s) + valueStr + "]:"s;
	} else {
		line = key->to_string(ud) + ":"s;
	}
	line += (value.is<TableBlock_t>() ? "\n"s : " "s) + value->to_string(ud);
	return line;
}
std::string MetaVariablePair_t::to_string(void* ud) const {
	return ":<"s + name->to_string(ud) + '>';
}
std::string MetaNormalPair_t::to_string(void* ud) const {
	std::string line;
	if (!key) {
		line = "<>:"s;
	} else if (key.is<Exp_t>()) {
		auto valueStr = key->to_string(ud);
		line = "<["s + (valueStr[0] == '[' ? " "s : ""s) + valueStr + "]>:"s;
	} else {
		line = '<' + key->to_string(ud) + ">:"s;
	}
	line += (value.is<TableBlock_t>() ? "\n"s : " "s) + value->to_string(ud);
	return line;
}
std::string VariablePairDef_t::to_string(void* ud) const {
	if (defVal) {
		return pair->to_string(ud) + " = "s + defVal->to_string(ud);
	} else {
		return pair->to_string(ud);
	}
}
std::string NormalPairDef_t::to_string(void* ud) const {
	if (defVal) {
		return pair->to_string(ud) + " = "s + defVal->to_string(ud);
	} else {
		return pair->to_string(ud);
	}
}
std::string NormalDef_t::to_string(void* ud) const {
	if (defVal) {
		return item->to_string(ud) + " = "s + defVal->to_string(ud);
	} else {
		return item->to_string(ud);
	}
}
std::string MetaVariablePairDef_t::to_string(void* ud) const {
	if (defVal) {
		return pair->to_string(ud) + " = "s + defVal->to_string(ud);
	} else {
		return pair->to_string(ud);
	}
}
std::string MetaNormalPairDef_t::to_string(void* ud) const {
	if (defVal) {
		return pair->to_string(ud) + " = "s + defVal->to_string(ud);
	} else {
		return pair->to_string(ud);
	}
}
std::string FnArgDef_t::to_string(void* ud) const {
	auto line = name->to_string(ud);
	if (op) {
		line += op->to_string(ud);
	}
	if (defaultValue) {
		line += " = "s + defaultValue->to_string(ud);
	}
	return line;
}
std::string FnArgDefList_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	bool hasInBlockExp = false;
	for (auto def : definitions.objects()) {
		auto argDef = static_cast<FnArgDef_t*>(def);
		if (argDef->defaultValue && isInBlockExp(argDef->defaultValue, argDef == definitions.back())) {
			hasInBlockExp = true;
			break;
		}
	}
	if (hasInBlockExp) {
		for (auto def : definitions.objects()) {
			temp.emplace_back(info->ind() + def->to_string(ud));
		}
		if (varArg) {
			temp.emplace_back(info->ind() + varArg->to_string(ud));
		}
		return join(temp, "\n"sv);
	} else {
		for (auto def : definitions.objects()) {
			temp.emplace_back(def->to_string(ud));
		}
		if (varArg) {
			temp.emplace_back(varArg->to_string(ud));
		}
		return join(temp, ", "sv);
	}
}
std::string OuterVarShadow_t::to_string(void* ud) const {
	if (varList) {
		return "using "s + varList->to_string(ud);
	} else {
		return "using nil"s;
	}
}
std::string FnArgsDef_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	bool hasInBlockExp = false;
	if (defList) {
		for (auto def : defList->definitions.objects()) {
			auto argDef = static_cast<FnArgDef_t*>(def);
			if (argDef->defaultValue && isInBlockExp(argDef->defaultValue, argDef == defList->definitions.back())) {
				hasInBlockExp = true;
				break;
			}
		}
	}
	if (hasInBlockExp) {
		str_list temp;
		info->pushScope();
		if (defList) {
			temp.push_back(defList->to_string(ud));
		}
		if (shadowOption) {
			temp.push_back(info->ind() + shadowOption->to_string(ud));
		}
		info->popScope();
		return "(\n" + join(temp, "\n"sv) + '\n' + info->ind() + ')';
	} else {
		std::string line;
		if (defList) {
			line += defList->to_string(ud);
		}
		if (shadowOption) {
			line += (line.empty() ? ""s : " "s) + shadowOption->to_string(ud);
		}
		return line.empty() ? (defList ? "()"s : ""s) : '(' + line + ')';
	}
}
std::string FunLit_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	std::string line;
	if (argsDef) {
		line = argsDef->to_string(ud);
	}
	if (defaultReturn) {
		if (defaultReturn.is<DefaultValue_t>()) {
			line += ':';
		} else {
			line += ": "s + defaultReturn->to_string(ud);
		}
	}
	if (!line.empty()) {
		line += ' ';
	}
	line += arrow->to_string(ud);
	if (body) {
		if (body->content.is<Statement_t>()) {
			line += ' ' + body->to_string(ud);
		} else {
			info->pushScope();
			auto bodyStr = body->to_string(ud);
			if (bodyStr.empty()) {
				bodyStr = info->ind() + "--"s;
			}
			line += '\n' + bodyStr;
			info->popScope();
		}
	}
	return line;
}
std::string MacroName_t::to_string(void* ud) const {
	return '$' + name->to_string(ud);
}
std::string MacroLit_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	std::string line;
	if (argsDef) {
		line = '(' + argsDef->to_string(ud) + ") "s;
	}
	line += "->"s;
	if (body->content.is<Statement_t>()) {
		line += ' ' + body->to_string(ud);
	} else {
		info->pushScope();
		auto bodyStr = body->to_string(ud);
		if (bodyStr.empty()) {
			bodyStr = info->ind() + "--"s;
		}
		line += '\n' + bodyStr;
		info->popScope();
	}
	return line;
}
std::string MacroFunc_t::to_string(void* ud) const {
	return name->to_string(ud) + invoke->to_string(ud);
}
std::string Macro_t::to_string(void* ud) const {
	return "macro "s + name->to_string(ud) + " = "s + decl->to_string(ud);
}
std::string MacroInPlace_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	auto line = "$ ->"s;
	if (body->content.is<Statement_t>()) {
		line += ' ' + body->to_string(ud);
	} else {
		info->pushScope();
		auto bodyStr = body->to_string(ud);
		if (bodyStr.empty()) {
			bodyStr = info->ind() + "--"s;
		}
		line += '\n' + bodyStr;
		info->popScope();
	}
	return line;
}
std::string NameOrDestructure_t::to_string(void* ud) const {
	return item->to_string(ud);
}
std::string AssignableNameList_t::to_string(void* ud) const {
	str_list temp;
	for (auto item : items.objects()) {
		temp.emplace_back(item->to_string(ud));
	}
	return join(temp, ", "sv);
}
std::string InvokeArgs_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	bool hasInBlockExp = false;
	for (auto arg : args.objects()) {
		if (isInBlockExp(arg, arg == args.back())) {
			hasInBlockExp = true;
			break;
		}
	}
	if (hasInBlockExp) {
		str_list temp;
		bool newLine = false;
		for (auto arg : args.objects()) {
			if (arg == args.front()) {
				if (ast_is<TableBlock_t>(arg)) {
					newLine = true;
				}
				temp.push_back(arg->to_string(ud));
				info->pushScope();
			} else if (ast_is<TableBlock_t>(arg)) {
				temp.push_back(arg->to_string(ud));
			} else {
				temp.push_back(info->ind() + arg->to_string(ud));
			}
		}
		info->popScope();
		return (newLine ? '\n' : ' ') + join(temp, ",\n"sv);
	} else {
		str_list temp;
		for (auto arg : args.objects()) {
			temp.emplace_back(arg->to_string(ud));
		}
		return ' ' + join(temp, ", "sv);
	}
}
std::string UnaryValue_t::to_string(void* ud) const {
	std::string line;
	for (auto op : ops.objects()) {
		line += op->to_string(ud);
	}
	line += value->to_string(ud);
	return line;
}
std::string UnaryExp_t::to_string(void* ud) const {
	std::string line;
	for (auto op : ops.objects()) {
		line += op->to_string(ud);
	}
	str_list temp;
	for (auto expo : expos.objects()) {
		temp.push_back(expo->to_string(ud));
	}
	line += join(temp, "^"sv);
	if (inExp) {
		line += ' ' + inExp->to_string(ud);
	}
	return line;
}
std::string In_t::to_string(void* ud) const {
	return (not_ ? "not "s : ""s) + "in "s + value->to_string(ud);
}
std::string ExpListAssign_t::to_string(void* ud) const {
	if (action) {
		return expList->to_string(ud) + ' ' + action->to_string(ud);
	} else {
		return expList->to_string(ud);
	}
}
std::string IfLine_t::to_string(void* ud) const {
	return type->to_string(ud) + ' ' + condition->to_string(ud);
}
std::string WhileLine_t::to_string(void* ud) const {
	return type->to_string(ud) + ' ' + condition->to_string(ud);
}
std::string StatementAppendix_t::to_string(void* ud) const {
	return item->to_string(ud);
}
std::string Statement_t::to_string(void* ud) const {
	std::string line;
	if (!comments.empty()) {
		auto info = reinterpret_cast<YueFormat*>(ud);
		str_list temp;
		for (ast_node* comment : comments.objects()) {
			if (comment == comments.front()) {
				temp.push_back(comment->to_string(ud));
			} else {
				temp.push_back(info->ind() + comment->to_string(ud));
			}
		}
		if (appendix) {
			temp.push_back(info->ind() + content->to_string(ud) + ' ' + appendix->to_string(ud));
			return join(temp, "\n"sv);
		} else {
			temp.push_back(info->ind() + content->to_string(ud));
			return join(temp, "\n"sv);
		}
	} else {
		if (appendix) {
			return content->to_string(ud) + ' ' + appendix->to_string(ud);
		} else {
			return content->to_string(ud);
		}
	}
}
std::string StatementSep_t::to_string(void*) const {
	return {};
}
std::string YueMultilineComment_t::to_string(void* ud) const {
	return "--[["s + inner->to_string(ud) + "]]"s;
}
std::string ChainAssign_t::to_string(void* ud) const {
	str_list temp;
	for (auto exp : exprs.objects()) {
		temp.emplace_back(exp->to_string(ud));
	}
	return join(temp, " = "sv) + ' ' + assign->to_string(ud);
}
std::string Body_t::to_string(void* ud) const {
	return content->to_string(ud);
}
std::string Block_t::to_string(void* ud) const {
	auto info = reinterpret_cast<YueFormat*>(ud);
	str_list temp;
	for (auto stmt_ : statements.objects()) {
		auto stmt = static_cast<Statement_t*>(stmt_);
		if (stmt->content.is<PipeBody_t>()) {
			info->pushScope();
			temp.emplace_back(stmt->to_string(ud));
			info->popScope();
		} else {
			temp.emplace_back(info->ind() + stmt->to_string(ud));
		}
	}
	return join(temp, "\n"sv);
}
std::string BlockEnd_t::to_string(void* ud) const {
	return block->to_string(ud);
}
std::string File_t::to_string(void* ud) const {
	if (block) {
		return block->to_string(ud);
	} else {
		return {};
	}
}

} // namespace yue

} // namespace parserlib
