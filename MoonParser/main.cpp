//
//  main.cpp
//  PegtlStudy
//
//  Created by Li Jin on 2017/6/16.
//  Copyright © 2017年 Li Jin. All rights reserved.
//

#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "pegtl.hpp"
#include "pegtl/analyze.hpp"
#include "slice.h"
using namespace silly;

using namespace tao::pegtl;

namespace helloworld // with operator precedence climbing
{
	struct Stack
	{
		Stack()
		{
			pushStack();
		}
		void pushValue(int value)
		{
			_values.back().push(value);
		}
		void pushOp(char op)
		{
			_ops.back().push(op);
		}
		void pushStack()
		{
			_values.emplace_back();
			_ops.emplace_back();
		}
		int getPriority(char op)
		{
			switch (op)
			{
				case '+': return 1;
				case '-': return 1;
				case '*': return 2;
				case '/': return 2;
			}
			return 0;
		}
		/*
		def compute_expr(tokenizer, min_prec):
			atom_lhs = tokenizer.get_next_token()

			while True:
				cur = tokenizer.cur_token
				if (cur is None or OPINFO_MAP[cur.value].prec < min_prec):
					break
				
				op = cur.value
				prec, assoc = OPINFO_MAP[op]
				next_min_prec = prec + 1 if assoc == 'LEFT' else prec

				atom_rhs = compute_expr(tokenizer, next_min_prec)

				atom_lhs = compute_op(op, atom_lhs, atom_rhs)

			return atom_lhs
		*/
		int parseNext(int minPrecedence)
		{
			int lhs = _values.back().front();
			_values.back().pop();

			while (true)
			{
				if (_ops.back().empty())
				{
					break;
				}
				char op = _ops.back().front();
				if (getPriority(op) < minPrecedence)
				{
					break;
				}
				_ops.back().pop();
				int nextMinPrecedence = getPriority(op) + 1;
				int rhs = parseNext(nextMinPrecedence);
				switch (op)
				{
					case '+':
						std::cout << lhs << " + " << rhs << " = " << lhs+rhs << '\n';
						lhs = lhs + rhs;
						break;
					case '-':
						std::cout << lhs << " - " << rhs << " = " << lhs-rhs << '\n';
						lhs = lhs - rhs;
						break;
					case '*':
						std::cout << lhs << " * " << rhs << " = " << lhs*rhs << '\n';
						lhs = lhs * rhs;
						break;
					case '/':
						std::cout << lhs << " / " << rhs << " = " << lhs/rhs << '\n';
						lhs = lhs / rhs;
						break;
				}
			}
			return lhs;
		}
		void popStack()
		{
			int value = parseNext(0);
			_values.pop_back();
			if (_values.empty())
			{
				_values.emplace_back();
			}
			_ops.pop_back();
			if (_ops.empty())
			{
				_ops.emplace_back();
			}
			pushValue(value);
		}
		int getValue() const
		{
			return _values.back().back();
		}
	private:
		std::vector<std::queue<char>> _ops;
		std::vector<std::queue<int>> _values;
	};

	struct number : seq<opt<one<'+', '-'>>, plus<digit>> { };

	struct expr;

	struct bracket : if_must<one<'('>, expr, one<')'>> { };

	struct atomic : sor<number, bracket> { };

	struct op : one<'+', '-', '*', '/'> { };

	struct expr : list<atomic, op, space> { };

	template<typename Rule>
	struct action : nothing<Rule> { };

	template<>
	struct action<number>
	{
		template<typename Input>
		static void apply(const Input& in, Stack& stack)
		{
			stack.pushValue(std::stoi(in.string()));
		}
	};

	template<>
	struct action<op>
	{
		template<typename Input>
		static void apply(const Input& in, Stack& stack)
		{
			stack.pushOp(*in.begin());
		}
	};

	template<>
	struct action<one<'('>>
	{
		static void apply0(Stack& stack)
		{
			stack.pushStack();
		}
	};

	template<>
	struct action<expr>
	{
		template<typename Input>
		static void apply(const Input& in, Stack& stack)
		{
			stack.popStack();
		}
	};

}  // namespace hello

namespace moon
{
	int moonType = 0;

	template <class T>
	int MoonType()
	{
		static int type = moonType++;
		return type;
	}

	struct Node
	{
		virtual ~Node() {}
		slice::Slice token;
	};

	struct State
	{
		State()
		{
			indents.push(0);
			stringOpen = -1;
			astStack.emplace_back();
		}
		size_t stringOpen;
		std::stack<int> indents;
		std::stack<bool> doStack;
		std::unordered_set<std::string> keywords =
		{
			"and", "while", "else", "using", "continue",
			"local", "not", "then", "return", "from",
			"extends", "for", "do", "or", "export",
			"class", "in", "unless", "when", "elseif",
			"switch", "break", "if", "with", "import"
		};
		std::stack<size_t> indexStack;
		std::vector<std::vector<std::shared_ptr<Node>>> astStack;
	};

	struct White : star<one<' ', '\t', '\r', '\n'>> {};
	struct plain_space : star<one<' ', '\t'>> {};
	struct Break : seq<opt<one<'\r'>>, one<'\n'>> {};
	struct Stop : sor<Break, eof> {};
	struct Comment : seq<string<'-', '-'>, star<not_at<one<'\r', '\n'>>, any>, at<Stop>> {};
	struct Indent : star<one<' ', '\t'>> {};
	struct Space : seq<plain_space, opt<Comment>> {};
	struct SomeSpace : seq<plus<blank>, opt<Comment>> {};
	struct SpaceBreak : seq<Space, Break> {};
	typedef SpaceBreak EmptyLine;
	struct AlphaNum : ranges<'a', 'z', 'A', 'Z', '0', '9', '_', '_'> {};
	struct _Name : seq<ranges<'a', 'z', 'A', 'Z', '_', '_'>, star<AlphaNum>> {};
	struct SpaceName : seq<Space, _Name> {};
	struct _Num : sor<
		seq<
			string<'0' ,'x'>,
			plus<ranges<'0', '9', 'a', 'f', 'A', 'F'>>,
			opt<seq<
				opt<one<'u', 'U'>>, one<'l', 'L'>, one<'l', 'L'>
			>>
		>,
		seq<
			plus<range<'0', '9'>>,
			seq<
				opt<one<'u', 'U'>>, one<'l', 'L'>, one<'l', 'L'>
			>
		>,
		seq<
			sor<
				seq<
					plus<range<'0', '9'>>,
					opt<seq<
						one<'.'>, plus<range<'0', '9'>>
					>>
				>,
				seq<
					one<'.'>,
					plus<range<'0', '9'>>
				>
			>,
			opt<seq<
				one<'e', 'E'>, opt<one<'-'>>, plus<range<'0', '9'>>
			>>
		>
	> {};
	struct Num : seq<Space, _Num> {};

	struct Cut : failure {};

	template<char... Cs> struct sym : seq<Space, string<Cs...>> {};
	template<char... Cs> struct symx : string<Cs...> {};

	template<typename patt, typename finally>
	struct ensure : sor<seq<patt, finally>, seq<finally, Cut>> {};

	template<char... Cs> struct key : seq<Space, string<Cs...>, not_at<AlphaNum>> {};
	template<char... Cs> struct opWord : seq<Space, string<Cs...>, not_at<AlphaNum>> {};
	template<char... Cs> struct op : seq<Space, string<Cs...>> {};

	struct Name
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			memory_input<> memIn(current, in.end() - current, current);
			if (SpaceName::match<A, M, Action, Control>(memIn, st))
			{
				auto name = slice::Slice(current, memIn.current() - current);
				auto trimed = name;
				trimed.trimSpace();
				auto it = st.keywords.find(trimed);
				if (it == st.keywords.end())
				{
					in.bump(name.size());
					return true;
				}
			}
			return false;
		}
	};

	struct self : one<'@'> {};
	struct self_name : seq<one<'@'>, _Name> {};
	struct self_class : string<'@', '@'> {};
	struct self_class_name : seq<string<'@', '@'>, _Name> {};

	struct SelfName : seq<Space, sor<self_class_name, self_class, self_name, self>> {};
	struct KeyName : sor<SelfName, seq<Space, _Name>> {};
	struct VarArg : seq<Space, string<'.', '.', '.'>> {};

	struct CheckIndentBump
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (Indent::match<A, M, Action, Control>(in, st))
			{
				int indent = 0;
				for (const char* ch = current; ch < in.current(); ch++)
				{
					switch (*ch)
					{
						case ' ': indent++; break;
						case '\t': indent += 4; break;
					}
				}
				return st.indents.top() == indent;
			}
			return false;
		}
	};
	struct CheckIndent : at<CheckIndentBump> {};

	struct AdvanceBump
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (Indent::match<A, M, Action, Control>(in, st))
			{
				int indent = 0;
				for (const char* ch = current; ch < in.current(); ch++)
				{
					switch (*ch)
					{
						case ' ': indent++; break;
						case '\t': indent += 4; break;
					}
				}
				int top = st.indents.top();
				if (top != -1 && indent > top)
				{
					st.indents.push(indent);
					return true;
				}
				return false;
			}
			return false;
		}
	};
	struct Advance : at<AdvanceBump> {};

	struct PushIndentBump
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (Indent::match<A, M, Action, Control>(in, st))
			{
				int indent = 0;
				for (const char* ch = current; ch < in.current(); ch++)
				{
					switch (*ch)
					{
						case ' ': indent++; break;
						case '\t': indent += 4; break;
					}
				}
				st.indents.push(indent);
				return true;
			}
			return false;
		}
	};
	struct PushIndent : at<PushIndentBump> {};

	struct PreventIndentBump
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			st.indents.push(-1);
			return true;
		}
	};
	struct PreventIndent : at<PreventIndentBump> {};

	struct PopIndentBump
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			st.indents.pop();
			return true;
		}
	};
	struct PopIndent : at<PopIndentBump> {};

	struct Block;

	struct InBlock : seq<Advance, Block, PopIndent> {};

	struct NameList;

	struct Local : seq<
		key<'l', 'o', 'c', 'a', 'l'>,
		sor<
			sor<op<'*'>, op<'^'>>,
			NameList
		>
	> {};

	struct colon_import_name : seq<sym<'\\'>, Name> {};
	struct ImportName : sor<colon_import_name, Name> {};
	struct ImportNameList : seq<
		star<SpaceBreak>,
		ImportName,
		star<
			sor<
				plus<SpaceBreak>,
				seq<sym<','>, star<SpaceBreak>>
			>,
			ImportName
		>
	> {};

	struct Exp;

	struct Import : seq<key<'i', 'm', 'p', 'o', 'r', 't'>, ImportNameList, star<SpaceBreak>, key<'f', 'r', 'o', 'm'>, Exp> {};

	struct BreakLoop : sor<key<'b', 'r', 'e', 'a', 'k'>, key<'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>> {};

	struct ExpListLow;

	struct ExpList;
	struct Assign;

	struct Return : seq<key<'r', 'e', 't', 'u', 'r', 'n'>, sor<ExpListLow, success>> {};
	struct WithExp : seq<ExpList, opt<Assign>> {};

	struct DisableDo;
	struct PopDo;
	struct Body;

	struct With : seq<key<'w', 'i', 't', 'h'>, DisableDo, ensure<WithExp, PopDo>, opt<key<'d', 'o'>>, Body> {};

	struct SwitchCase : seq<key<'w', 'h', 'e', 'n'>, ExpList, opt<key<'t', 'h', 'e', 'n'>>, Body> {};
	struct SwitchElse : seq<key<'e', 'l', 's', 'e'>, Body> {};
	struct SwitchBlock : seq<
		star<EmptyLine>,
		Advance,
		seq<
			SwitchCase,
			star<plus<Break>, SwitchCase>,
			opt<plus<Break>, SwitchElse>
		>,
		PopIndent
	> {};
	struct Switch : seq<key<'s', 'w', 'i', 't', 'c', 'h'>, DisableDo, ensure<Exp, PopDo>, opt<key<'d', 'o'>>, opt<Space>, Break, SwitchBlock> {};

	struct IfCond : seq<Exp, opt<Assign>> {};

	struct IfElse : seq<
		opt<Break, star<EmptyLine>, CheckIndent>,
		key<'e', 'l', 's', 'e'>, Body
	> {};

	struct IfElseIf : seq<
		opt<Break, star<EmptyLine>, CheckIndent>,
		key<'e', 'l', 's', 'e', 'i', 'f'>, IfCond,
		opt<key<'t', 'h', 'e', 'n'>>, Body
	> {};

	struct If : seq<key<'i', 'f'>, IfCond, opt<key<'t', 'h', 'e', 'n'>>, Body, star<IfElseIf>, opt<IfElse>> {};
	struct Unless : seq<key<'u', 'n', 'l', 'e', 's', 's'>, IfCond, opt<key<'t', 'h', 'e', 'n'>>, Body, star<IfElseIf>, opt<IfElse>> {};

	struct While : seq<key<'w', 'h', 'i', 'l', 'e'>, DisableDo, ensure<Exp, PopDo>, opt<key<'d','o'>>, Body> {};
	struct For : seq<key<'f', 'o', 'r'>, DisableDo,
		ensure<
			seq<
				Name, sym<'='>,
				seq<Exp, sym<','>, Exp, opt<sym<','>, Exp>>
			>,
			PopDo
		>,
		opt<key<'d', 'o'>>, Body> {};

	struct AssignableNameList;

	struct ForEach : seq<
		key<'f', 'o', 'r'>,
		AssignableNameList,
		key<'i', 'n'>,
		DisableDo,
		ensure<
			sor<
				seq<sym<'*'>, Exp>, ExpList
			>,
			PopDo
		>,
		opt<key<'d', 'o'>>,
		Body
	> {};

	struct Do
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			if (at<seq<key<'d', 'o'>, Body>>::match<A, M, Action, Control>(in, st))
			{
				if (st.doStack.empty() || st.doStack.top())
				{
					return true;
				}
			}
			return false;
		}
	};

	struct DisableDo
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			st.doStack.push(false);
			return true;
		}
	};

	struct PopDo
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			st.doStack.pop();
			return true;
		}
	};

	struct CompInner;

	struct Comprehension : seq<sym<'['>, Exp, CompInner, sym<']'>> {};
	struct TblComprehension : seq<sym<'{'>, seq<Exp, opt<sym<','>, Exp>>, CompInner, sym<'}'>> {};

	struct CompForEach;
	struct CompFor;
	struct CompClause;

	struct CompInner : seq<sor<CompForEach, CompFor>, star<CompClause>> {};
	struct CompForEach : seq<
		key<'f', 'o', 'r'>,
		AssignableNameList,
		key<'i', 'n'>,
		sor<
			seq<sym<'*'>, Exp>, Exp
		>
	> {};
	struct CompFor : seq<key<'f', 'o', 'r'>, Name, sym<'='>, seq<Exp, sym<','>, Exp, opt<sym<','>, Exp>>> {};
	struct CompClause : sor<CompFor, CompForEach, seq<key<'w', 'h', 'e', 'n'>, Exp>> {};

	struct TableBlock;

	struct Assign : seq<sym<'='>, sor<With, If, Switch, TableBlock, ExpListLow>> {};

	struct Update : seq<sor<
		sym<'.', '.', '='>,
		sym<'+', '='>,
		sym<'-', '='>,
		sym<'*', '='>,
		sym<'/', '='>,
		sym<'%', '='>,
		sym<'o', 'r', '='>,
		sym<'a', 'n', 'd', '='>,
		sym<'&', '='>,
		sym<'|', '='>,
		sym<'>', '>', '='>,
		sym<'<', '<', '='>
	>, Exp> {};

	struct CharOperators : seq<Space, one<'+', '-', '*' ,'/', '%', '^', '>', '<', '|', '&'>> {};
	struct WordOperators : sor<
		opWord<'o', 'r'>,
		opWord<'a', 'n', 'd'>,
		op<'<', '='>,
		op<'>', '='>,
		op<'~', '='>,
		op<'!', '='>,
		op<'=', '='>,
		op<'.', '.'>,
		op<'<', '<'>,
		op<'>', '>'>,
		op<'/', '/'>> {};
	struct BinaryOperator : seq<sor<WordOperators, CharOperators>, star<SpaceBreak>> {};

	struct Chain;

	struct Assignable : sor<at<Chain>, Name, SelfName> {};

	struct Value;

	struct Exp : seq<Value, star<BinaryOperator, Value>> {};

	struct Callable;
	struct InvokeArgs;

	struct ChainValue : seq<sor<Chain, Callable>, opt<InvokeArgs>> {};

	struct KeyValueList;
	struct String;
	struct SimpleValue;

	struct Value : sor<SimpleValue, KeyValueList, ChainValue, String> {};
	struct SliceValue : Exp {};

	struct LuaString;

	struct single_string_inner : sor<string<'\\', '\''>, string<'\\', '\\'>, not_one<'\''>> {};
	struct SingleString : seq<symx<'\''>, star<single_string_inner>, sym<'\''>> {};
	struct interp : seq<symx<'#', '{'>, Exp, sym<'}'>> {};
	struct double_string_plain : sor<string<'\\', '\"'>, string<'\\', '\\'>, not_one<'\"'>> {};
	struct double_string_inner : plus<seq<not_at<interp>, double_string_plain>> {};
	struct DoubleString : seq<symx<'\"'>, star<sor<double_string_inner, interp>>, sym<'\"'>> {};
	struct String : sor<seq<Space, DoubleString>, seq<Space, SingleString>, LuaString> {};

	struct lua_string_open : seq<one<'['>, star<one<'='>>, one<'['>> {};
	struct lua_string_close : seq<one<']'>, star<one<'='>>, one<']'>> {};

	struct LuaStringOpen
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (lua_string_open::match<A, M, Action, Control>(in, st))
			{
				st.stringOpen = in.current() - current + 1;
				return true;
			}
			return false;
		}
	};

	struct LuaStringClose
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (lua_string_close::match<A, M, Action, Control>(in, st))
			{
				return st.stringOpen == in.current() - current + 1;
			}
			return false;
		}
	};

	struct LuaStringContent : star<not_at<LuaStringClose>, any> {};

	struct LuaString
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			bool result = seq<LuaStringOpen, opt<Break>, LuaStringContent, LuaStringClose>::match<A, M, Action, Control>(in, st);
			st.stringOpen = -1;
			return result;
		}
	};

	struct Parens : seq<sym<'('>, star<SpaceBreak>, Exp, star<SpaceBreak>, sym<')'>> {};
	struct Callable : sor<Name, SelfName, VarArg, Parens> {};

	struct FnArgsExpList : seq<
		Exp,
		star<
			sor<Break, sym<','>>,
			White, Exp
		>
	> {};

	struct FnArgs : sor<
		seq<symx<'('>, star<SpaceBreak>, opt<FnArgsExpList>, star<SpaceBreak>, sym<')'>>,
		seq<sym<'!'>, not_at<one<'='>>>
	> {};

	struct ChainItems;
	struct DotChainItem;
	struct ColonChain;

	struct Chain : sor<
		seq<
			sor<Callable, String, not_at<one<'.', '\\'>>>,
			ChainItems
		>,
		seq<Space,
			sor<
				seq<DotChainItem, opt<ChainItems>>,
				ColonChain
			>
		>
	> {};

	struct ChainItem;

	struct ChainItems : sor<
		seq<plus<ChainItem>, opt<ColonChain>>,
		ColonChain
	> {};

	struct Invoke;
	struct Slice;

	struct ChainItem : sor<
		Invoke,
		DotChainItem,
		Slice,
		seq<symx<'['>, Exp, sym<']'>>
	> {};

	struct DotChainItem : seq<symx<'.'>, _Name> {};
	struct ColonChainItem : seq<symx<'\\'>, _Name> {};
	struct ColonChain : seq<
		ColonChainItem,
		opt<Invoke, opt<ChainItems>>
	> {};

	struct SliceOne : success {};
	struct SliceTwo : success {};
    struct Slice : seq<
		symx<'['>, sor<SliceValue, SliceOne>, sym<','>, sor<SliceValue, SliceTwo>, opt<sym<','>, SliceValue>, sym<']'>> {};

	struct Invoke : sor<
		FnArgs,
		SingleString,
		DoubleString,
		seq<at<one<'['>>, LuaString>
	> {};

	struct KeyValue;
	struct TableValueList;
	struct TableLitLine;

	struct TableValue : sor<KeyValue, Exp> {};
	struct TableLit : seq<
		sym<'{'>,
		seq<
			opt<TableValueList>, opt<sym<','>>,
			opt<
				SpaceBreak, TableLitLine,
				star<opt<sym<','>>, SpaceBreak, TableLitLine>,
				opt<sym<','>>
			>
		>,
		White, sym<'}'>
	> {};

	struct TableValueList : seq<TableValue, star<sym<','>, TableValue>> {};
	struct TableLitLine : sor<
		seq<PushIndent, sor<seq<TableValueList, PopIndent>, seq<PopIndent, Cut>>>,
		Space
	> {};

	struct KeyValueLine;

	struct TableBlockInner : seq<KeyValueLine, star<plus<SpaceBreak>, KeyValueLine>> {};
	struct TableBlock : seq<plus<SpaceBreak>, Advance, ensure<TableBlockInner, PopIndent>> {};

	struct Statement;

	struct ClassLine : seq<
		CheckIndent,
		seq<
			sor<KeyValueList, Statement, Exp>,
			opt<sym<','>>
		>
	> {};

	struct ClassBlock : seq<plus<SpaceBreak>, Advance, ClassLine, star<plus<SpaceBreak>, ClassLine>, PopIndent> {};

	struct class_no_derive : success {};
	struct class_no_extend : success {};
	struct class_no_body : success {};
	struct ClassDecl : seq<
		string<'c', 'l', 'a', 's', 's'>,
		not_at<one<':'>>,
		sor<Assignable, class_no_derive>,
		opt<sor<string<'e', 'x', 't', 'e', 'n', 'd', 's'>, PreventIndent, ensure<Exp, PopIndent>, class_no_extend>>,
		sor<ClassBlock, class_no_body>
	> {};

	struct Export : seq<
		string<'e', 'x', 'p', 'o', 'r', 't'>,
		sor<
			ClassDecl,
			op<'*'>,
			op<'^'>,
			seq<NameList, opt<sym<'='>, ExpListLow>>
		>
	> {};

	struct KeyValue : sor<
		seq<sym<':'>, not_at<SomeSpace>, Name>,
		seq<
			sor<
				KeyName,
				seq<sym<'['>, Exp, sym<']'>>,
				seq<Space, DoubleString>,
				seq<Space, SingleString>
			>,
			symx<':'>,
			sor<
				Exp, TableBlock, seq<plus<SpaceBreak>, Exp>
			>
		>
	> {};

	struct KeyValueList : seq<KeyValue, star<sym<','>, KeyValue>> {};
	struct KeyValueLine : seq<CheckIndent, KeyValueList, opt<sym<','>>> {};

	struct FnArgDef : seq<sor<Name, SelfName>, opt<sym<'='>, Exp>> {};

	struct FnArgDefList : sor<
		seq<
			FnArgDef,
			star<sor<sym<','>, Break>, White, FnArgDef>,
			star<sor<sym<','>, Break>, White, VarArg>
		>,
		VarArg
	> {};

	struct outer_value_shadow : seq<string<'u', 's', 'i', 'n', 'g'>, sor<NameList, seq<Space, string<'n', 'i', 'l'>>>> {};
	struct outer_value_no_shadow : success {};
	struct without_args_def : success {};

	struct FnArgsDef : sor<
		seq<sym<'('>, White, opt<FnArgDefList>,
			sor<
				outer_value_shadow,
				outer_value_no_shadow
			>,
			White, sym<')'>
		>,
		without_args_def
	> {};

	struct FunLit : seq<
		FnArgsDef,
		sor<
			sym<'-', '>'>,
			sym<'=', '>'>
		>,
		sor<Body, success>
	> {};

	struct NameList : seq<Name, star<sym<','>, Name>> {};
	struct NameOrDestructure : sor<Name, TableLit> {};
	struct AssignableNameList : seq<NameOrDestructure, star<sym<','>, NameOrDestructure>> {};

	struct ExpList : seq<Exp, star<sym<','>, Exp>> {};
	struct ExpListLow : seq<Exp, star<sor<sym<','>, sym<';'>>, Exp>> {};

	struct ArgLine : seq<CheckIndent, ExpList> {};
	struct ArgBlock : seq<ArgLine, star<sym<','>, SpaceBreak, ArgLine>, PopIndent> {};

	struct InvokeArgs : seq<
		not_at<one<'-'>>,
		sor<
			seq<
				ExpList,
				opt<sor<
					seq<
						sym<','>,
						sor<
							TableBlock, seq<SpaceBreak, Advance, ArgBlock, opt<TableBlock>>
						>
					>,
					TableBlock
				>>
			>,
			TableBlock
		>
	> {};

	struct SimpleValue : sor<
		key<'n', 'i', 'l'>,
		key<'t', 'r', 'u', 'e'>,
		key<'f', 'a', 'l', 's', 'e'>,
		If, Unless, Switch, With, ClassDecl, ForEach, For, While, Do,
		seq<sym<'-'>, not_at<SomeSpace>, Exp>,
		seq<sym<'#'>, Exp>,
		seq<sym<'~'>, Exp>,
		seq<key<'n', 'o', 't'>, Exp>,
		TblComprehension,
		TableLit,
		Comprehension,
		FunLit,
		Num
	> {};

	struct Assignment : seq<
		ExpList,
		sor<Update, Assign>
	> {};

	struct Statement : seq<
		sor<
			Import, While, With, For, ForEach,
			Switch, Return, Local, Export, BreakLoop,
			Assignment, ExpList
		>,
		Space,
		opt<
			sor<
				seq<
					key<'i', 'f'>, Exp,
					opt<
						key<'e', 'l', 's', 'e'>, Exp
					>,
					Space
				>,
				seq<key<'u', 'n', 'l', 'e', 's', 's'>, Exp>,
				CompInner
			>,
			Space
		>
	> {};

	struct Body : sor<
		seq<opt<Space>, Break, star<EmptyLine>, InBlock>,
		Statement
	> {};

	struct Line : sor<
		seq<CheckIndent, Statement>,
		seq<Space, at<Stop>>
	> {};

	struct Block : seq<Line, star<plus<Break>, Line>> {};

	struct BlockWithEnd : seq<Block, eof> {};

	template <class T>
	struct NodeBase : Node
	{
		static int id;
	};

	template <class T>
	int NodeBase<T>::id = MoonType<T>();

	struct ImportNameNode : Node
	{
	};

	struct ImportNameListNode : NodeBase<ImportNameListNode>
	{
		std::vector<std::shared_ptr<Node>> names;
	};

	struct ImportNode : NodeBase<ImportNode>
	{
		std::shared_ptr<Node> nameList;
		std::shared_ptr<Node> exp;
	};

	template<typename Rule>
	struct action : nothing<Rule> {};

	template<>
	struct action<ImportName>
	{
		template<typename Input>
		static void apply(const Input& in, State& st)
		{
			auto node = std::make_shared<ImportNameNode>();
			node->token = slice::Slice(in.begin(), in.end() - in.begin());
			node->token.trimSpace();
			st.astStack.pop_back();
			st.astStack.back().push_back(node);
		}
	};

	template<>
	struct action<Exp>
	{
		template<typename Input>
		static void apply(const Input& in, State& st)
		{
			auto node = std::make_shared<Node>();
			node->token = slice::Slice(in.begin(), in.end() - in.begin());
			node->token.trimSpace();
			st.astStack.pop_back();
			st.astStack.back().push_back(node);
		}
	};

	template<>
	struct action<Import>
	{
		template<typename Input>
		static void apply(const Input& in, State& st)
		{
			auto node = std::make_shared<ImportNode>();
			node->exp = st.astStack.back().back();
			st.astStack.back().pop_back();
			node->nameList = st.astStack.back().back();
			st.astStack.back().pop_back();
			st.astStack.pop_back();
			st.astStack.back().push_back(node);
		}
	};

	template<>
	struct action<ImportNameListNode>
	{
		template<typename Input>
		static void apply(const Input& in, State& st)
		{
			auto node = std::make_shared<ImportNameListNode>();
			node->names = std::move(st.astStack.back());
			st.astStack.pop_back();
			st.astStack.back().push_back(node);
		}
	};

	template<typename Rule>
	struct control : normal<Rule> {};

	template<>
	struct control<ImportNameListNode> : normal<ImportNameListNode>
	{
		template<typename Input>
		static void start(Input& in, State& st)
		{
			st.astStack.emplace_back();
		}

		template<typename Input>
		static void failure(Input& /*unused*/, State& st)
		{
			st.astStack.pop_back();
		}

		template<typename Input>
		static void success(Input& /*unused*/, State& st)
		{
		}
	};
}

int main()
{
	analyze<moon::BlockWithEnd>();
	moon::State state;
	string_input<> inName(R"xoxox(

Dorothy!
EditMenuView = require "View.Control.Operation.EditMenu"
MessageBox = require "Control.Basic.MessageBox"

-- [no signals]
-- [no params]
Class EditMenuView,
	__init:=>
		{:width} = CCDirector.winSize
		isHide = false

		@itemArea\setupMenuScroll @itemMenu
		@itemArea.viewSize = @itemMenu\alignItems!

		for child in *@itemMenu.children
			child.positionX = -35
			child.visible = false
			child.displayed = false
		@showItemButtons {"graphic","physics","logic","data"},true,true

		buttonNames = {
			"sprite","model","body"
			"effect","layer","world"
		}

		clearSelection = ->
			for name in *buttonNames
				with @[name.."Btn"]
					if .selected
						.selected = false
						.color = ccColor3 0x00ffff
						.scaleX = 0
						.scaleY = 0
						\perform oScale 0.3,1,1,oEase.OutBack
						emit .event,nil

		for name in *buttonNames
			with @[name.."Btn"]
				.selected = false
				upperName = name\sub(1,1)\upper!..name\sub(2,-1)
				.event = "Scene."..upperName.."Selected"
				@gslot .event,(item)->
					if item
						.selected = true
						.color = ccColor3 0xff0088
						.scaleX = 0
						.scaleY = 0
						\perform oScale 0.3,1,1,oEase.OutBack
				\slot "Tapped",->
					if not .selected
						emit "Scene.View"..upperName
						clearSelection!
					else
						.selected = false
						.color = ccColor3 0x00ffff
						emit .event,nil

		@triggerBtn\slot "Tapped",->
			clearSelection!
			if @pickPanel.visible
				MessageBox text:"Pick An Item First",okOnly:true
			else
				emit "Scene.Trigger.Open"

		@actionBtn\slot "Tapped",->
			clearSelection!
			if @pickPanel.visible
				MessageBox text:"Pick An Item First",okOnly:true
			else
				emit "Scene.Action.Open"

		@aiBtn\slot "Tapped",->
			clearSelection!
			if @pickPanel.visible
				MessageBox text:"Pick An Item First",okOnly:true
			else
				emit "Scene.AITree.Open"

		@unitBtn\slot "Tapped",->
			clearSelection!
			if @pickPanel.visible
				MessageBox text:"Pick An Item First",okOnly:true
			else
				emit "Scene.Unit.Open"

		@delBtn\slot "Tapped",->
			clearSelection!
			emit "Scene.EditMenu.Delete"

		mode = 0
		@zoomBtn\slot "Tapped",->
			scale = switch mode
				when 0 then 2
				when 1 then 0.5
				when 2 then 1
			mode += 1
			mode %= 3
			@zoomBtn.text = string.format("%d%%",scale*100)
			emit "Scene.ViewArea.ScaleTo",scale

		@originBtn\slot "Tapped",-> editor\moveTo oVec2.zero

		@progressUp.visible = false
		@progressDown.visible = false

		with @upBtn
			.visible = false
			.enabled = false
			\slot "TapBegan",->
				clearSelection!
				\schedule once ->
					sleep 0.4
					@progressUp.visible = true
					@progressUp\play!
			\slot "Tapped",->
				if @progressUp.visible
					if @progressUp.done
						emit "Scene.EditMenu.Top"
				else
					emit "Scene.EditMenu.Up"
			\slot "TapEnded",->
				\unschedule!
				if @progressUp.visible
					@progressUp.visible = false

		with @downBtn
			.visible = false
			.enabled = false
			\slot "TapBegan",->
				clearSelection!
				\schedule once ->
					sleep 0.4
					@progressDown.visible = true
					@progressDown\play!
			\slot "Tapped",->
				if @progressDown.visible
					if @progressDown.done
						emit "Scene.EditMenu.Bottom"
				else
					emit "Scene.EditMenu.Down"
			\slot "TapEnded",->
				\unschedule!
				if @progressDown.visible
					@progressDown.visible = false

		with @foldBtn
			.visible = false
			.enabled = false
			\slot "Tapped",->
				clearSelection!
				emit "Scene.ViewPanel.Fold",editor.currentData

		with @editBtn
			.visible = false
			.enabled = false
			\slot "Tapped",->
				emit "Scene.SettingPanel.Edit",nil
				editor\editCurrentItemInPlace!

		with @menuBtn
			.dirty = false
			\slot "Tapped",->
				clearSelection!
				if .dirty
					editor\save!
					emit "Scene.Dirty",false
				else
					ScenePanel = require "Control.Item.ScenePanel"
					ScenePanel!
				emit "Scene.SettingPanel.Edit",nil

		with @undoBtn
			.visible = false
			\slot "Tapped",->
				clearSelection!
				editor.currentSceneFile = editor.currentSceneFile
				emit "Scene.Dirty",false

		with @xFixBtn
			.visible = false
			\slot "Tapped",(button)->
				editor.xFix = not editor.xFix
				if editor.yFix
					editor.yFix = false
					@yFixBtn.color = ccColor3 0x00ffff
				button.color = ccColor3 editor.xFix and 0xff0088 or 0x00ffff
				emit "Scene.FixChange"

		with @yFixBtn
			.visible = false
			\slot "Tapped",(button)->
				editor.yFix = not editor.yFix
				if editor.xFix
					editor.xFix = false
					@xFixBtn.color = ccColor3 0x00ffff
				button.color = ccColor3 editor.yFix and 0xff0088 or 0x00ffff
				emit "Scene.FixChange"

		@iconCam.visible = false

		currentSubCam = nil
		with @camBtn
			.visible = false
			.editing = false
			\gslot "Scene.Camera.Select",(subCam)->
				currentSubCam = subCam
			\slot "Tapped",->
				.editing = not .editing
				if .editing
					emit "Scene.Camera.Activate",currentSubCam
				else
					emit "Scene.Camera.Activate",nil

		with @zoomEditBtn
			.visible = false
			.editing = false
			\slot "Tapped",->
				.editing = not .editing
				if .editing and currentSubCam
					emit "Scene.Edit.ShowRuler", {currentSubCam.zoom,0.5,10,1,(value)->
						emit "Scene.ViewArea.Scale",value
					}
				else
					emit "Scene.Edit.ShowRuler",nil

		@playBtn\slot "Tapped",->
			settings = editor\getSettings!
			sceneFile = if settings.StartupScene
				editor.sceneFullPath..settings.StartupScene..".scene"
			else
				nil
			if not sceneFile or not oContent\exist sceneFile
				MessageBox text:"Startup Scene\nIs Required!",okOnly:true
				return
			@menuBtn\emit "Tapped" if @menuBtn.dirty
			-- test codes below
			Game = require "Lib.Game.Game"
			game = Game editor.game,false
			editorData = editor\getEditorData!
			editorData.lastScene = editor.lastScene
			emit "Scene.EditorData",editorData
			editor\emit "Quit",game\loadScene!

		setupItemButton = (button,groupLine,subItems)->
			groupLine.data = button
			with button
				.showItem = false
				\slot "Tapped",->
					return if .scheduled
					.showItem = not .showItem
					if .showItem
						groupLine.visible = true
						groupLine.opacity = 0
						groupLine\perform oOpacity 0.3,1
						groupLine.position = button.position-oVec2 25,25
					else
						\schedule once ->
							groupLine\perform oOpacity 0.3,0
							sleep 0.3
							groupLine.visible = false
					@showItemButtons subItems,.showItem
		setupItemButton @graphicBtn,@graphicLine,{"sprite","model","effect","layer"}
		setupItemButton @physicsBtn,@physicsLine,{"body","world"}
		setupItemButton @logicBtn,@logicLine,{"trigger","action","ai"}
		setupItemButton @dataBtn,@dataLine,{"unit"}

		@gslot "Scene.ShowFix",(value)->
			editor.xFix = false
			editor.yFix = false
			emit "Scene.FixChange"
			if value
				with @xFixBtn
					.visible = true
					.color = ccColor3 0x00ffff
					.scaleX = 0
					.scaleY = 0
					\perform oScale 0.5,1,1,oEase.OutBack
				with @yFixBtn
					.visible = true
					.color = ccColor3 0x00ffff
					.scaleX = 0
					.scaleY = 0
					\perform oScale 0.5,1,1,oEase.OutBack
			else
				@xFixBtn.visible = false
				@yFixBtn.visible = false

		@gslot "Scene.Dirty",(dirty)->
			with @menuBtn
				if .dirty ~= dirty
					.dirty = dirty
					if dirty
						.text = "Save"
						with @undoBtn
							if not .visible
								.enabled = true
								.visible = true
								.scaleX = 0
								.scaleY = 0
								\perform oScale 0.3,1,1,oEase.OutBack
					else
						.color = ccColor3 0x00ffff
						.text = "Menu"
						with @undoBtn
							if .visible
								.enabled = false
								\perform CCSequence {
									oScale 0.3,0,0,oEase.InBack
									CCHide!
								}

		itemChoosed = (itemData)->
			return if isHide
			if @camBtn.visible or @iconCam.visible
				@iconCam.visible = false
				@camBtn.visible = false
				emit "Scene.Camera.Activate",nil
				emit "Scene.Camera.Select",nil
			emit "Scene.ViewPanel.FoldState",{
				itemData:itemData
				handler:(state)->
					if state ~= nil
						@setButtonVisible @foldBtn,true
						switch itemData.typeName
							when "Body","Model","Effect"
								@setButtonVisible @editBtn,true
							else
								@setButtonVisible @editBtn,false
						text = @foldBtn.text
						targetText = state and "Un\nFold" or "Fold"
						if text ~= targetText
							@foldBtn.text = targetText
							if @foldBtn.scale.done
								@setButtonVisible @foldBtn,true
					else
						@setButtonVisible @foldBtn,false
						@setButtonVisible @upBtn,false
						@setButtonVisible @downBtn,false
						@setButtonVisible @editBtn,false
			}
			return unless itemData
			switch itemData.typeName
				when "Camera","PlatformWorld","UILayer"
					@setButtonVisible @upBtn,false
					@setButtonVisible @downBtn,false
					{:x,:y} = @upBtn.position
					@foldBtn\runAction oPos 0.3,x,y,oEase.OutQuad
					if itemData.typeName == "Camera"
						clearSelection!
						with @iconCam
							.visible = true
							.scaleX = 0
							.scaleY = 0
							\perform oScale 0.3,0.5,0.5,oEase.OutBack
				else
					item = editor\getItem itemData
					hasChildren = false
					if itemData.typeName == "World"
						hasChildren = #item.parent.parent.children > 1
					else
						hasChildren = #item.parent.children > 1
					if item.parent.children and hasChildren
						@setButtonVisible @upBtn,true
						@setButtonVisible @downBtn,true
						{:x,:y} = @downBtn.position
						@foldBtn\runAction oPos 0.3,x,y-60,oEase.OutQuad
						@editBtn\runAction oPos 0.3,x,y-120,oEase.OutQuad
					else
						@setButtonVisible @upBtn,false
						@setButtonVisible @downBtn,false
						{:x,:y} = @upBtn.position
						@foldBtn\runAction oPos 0.3,x,y,oEase.OutQuad
						@editBtn\runAction oPos 0.3,x,y-60,oEase.OutQuad
		@gslot "Scene.ViewPanel.Pick",itemChoosed
		@gslot "Scene.ViewPanel.Select",itemChoosed

		@gslot "Scene.ViewArea.Scale",(scale)->
			mode = 2 if scale ~= 1
			@zoomBtn.text = string.format("%d%%",scale*100)
		@gslot "Scene.ViewArea.ScaleReset",->
			mode = 0
			@zoomBtn.text = "100%"
			emit "Scene.ViewArea.ScaleTo",1

		@gslot "Scene.Camera.Select",(subCam)->
			if subCam and not @camBtn.visible
				@iconCam.opacity = 0
				with @camBtn
					.visible = true
					.scaleX = 0
					.scaleY = 0
					\perform oScale 0.3,1,1,oEase.OutBack
			else
				@camBtn.visible = false
				with @iconCam
					.opacity = 1
					.scaleX = 0
					.scaleY = 0
					\perform oScale 0.3,0.5,0.5,oEase.OutBack

		changeDisplay = (child)->
			if child.positionX < width/2
				child\perform oPos 0.5,-child.positionX,child.positionY,oEase.OutQuad
			else
				child\perform oPos 0.5,width*2-child.positionX,child.positionY,oEase.OutQuad

		@gslot "Scene.HideEditor",(args)->
			{hide,all} = args
			return if isHide == hide
			isHide = hide
			@enabled = @camBtn.editing or not hide
			for i = 1,#@children
				child = @children[i]
				switch child
					when @camBtn
						posX = @camBtn.editing and width-35 or width-345
						child\perform oPos 0.5,posX,child.positionY,oEase.OutQuad
					when @menuBtn,@undoBtn,@zoomEditBtn,@iconCam
						if all
							changeDisplay child
						else
							continue
					else
						changeDisplay child

		@gslot "Scene.Camera.Activate",(subCam)->
			editor.isFixed = not @camBtn.editing
			if subCam
				with @zoomEditBtn
					.scaleX = 0
					.scaleY = 0
					.visible = true
					\perform CCSequence {
						CCDelay 0.5
						oScale 0.3,1,1,oEase.OutBack
					}
			else
				@zoomEditBtn.visible = false
				@zoomEditBtn.editing = false
				emit "Scene.Edit.ShowRuler",nil

		@gslot "Scene.EditMenu.ClearSelection",clearSelection

	setButtonVisible:(button,visible)=>
		return if visible == button.enabled
		button.enabled = visible
		if visible
			if not button.visible
				button.visible = true
				button.scaleX = 0
				button.scaleY = 0
			button\perform oScale 0.3,1,1,oEase.OutBack
		else
			button\perform CCSequence {
				oScale 0.3,0,0,oEase.InBack
				CCHide!
			}

	showItemButtons:(names,visible,instant=false)=>
		buttonSet = {@["#{name}Btn"],true for name in *names}
		posX = 35
		posY = @itemMenu.height-25
		if visible
			offset = @itemArea.offset
			firstItem = nil
			for child in *@itemMenu.children
				isTarget = buttonSet[child]
				firstItem = child if isTarget and not firstItem
				if child.displayed or isTarget
					offsetX = switch child
						when @graphicBtn,@physicsBtn,@logicBtn,@dataBtn then 0
						else 20
					child.position = offset+oVec2(posX+offsetX,posY)
					if isTarget
						child.displayed = true
						child.visible = true
						if not instant
							child.face.scaleY = 0
							child.face\perform oScale 0.3,1,1,oEase.OutBack
					posY -= 60
				elseif child.data -- data is parentButton
					child.position = child.data.position-oVec2(25,25)
			@itemArea.viewSize = CCSize 70,@itemArea.height-posY-35
			@itemArea\scrollToPosY firstItem.positionY
		else
			@itemMenu\schedule once ->
				if not instant
					for child in *@itemMenu.children
						if buttonSet[child]
							child.face\perform oScale 0.3,1,0,oEase.OutQuad
					sleep 0.3
				offset = @itemArea.offset
				lastPosY = nil
				for child in *@itemMenu.children
					if buttonSet[child]
						child.positionX = -35
						child.displayed = false
						child.visible = false
						lastPosY = child.positionY
					elseif child.displayed
						if lastPosY and child.positionY < lastPosY
							child.face.scaleY = 0
							child.face\perform oScale 0.3,1,1,oEase.OutBack
						offsetX = switch child
							when @graphicBtn,@physicsBtn,@logicBtn,@dataBtn then 0
							else 20
						child.position = offset+oVec2(posX+offsetX,posY)
						posY -= 60
					elseif lastPosY and child.data and child.positionY < lastPosY
						child.opacity = 0
						child\perform oOpacity 0.3,1
						child.position = child.data.position-oVec2(25,25)
				@itemArea.viewSize = CCSize 70,@itemArea.height-posY-35

)xoxox", "abc");

	string_input<> in(R"PIG(import Path, Struct from require "utils")PIG", "bcd");
	try
	{
		if (parse<must<moon::Import, eof>, moon::action, moon::control>(in, state))
		{
			std::cout << "matched.\n";
		}
		else
		{
			std::cout << "not matched.\n";
		}
	}
	catch (parse_error e)
	{
		std::cout << "not matched.\n";
		std::cout << e.what() << '\n';
	}
/*
	analyze<hello::expr>();
	const char* text = "1 + 2 + 3 * 4 / 2";
	string_input<> in(text, "abc");
	try
	{
		hello::Stack stack;
		if (parse<hello::expr, hello::action>(in, stack))
		{
			std::cout << text << " = " << stack.getValue() << '\n';
		}
		return 0;
	}
	catch (parse_error e)
	{
		std::cout << e.what() << '\n';
		return 1;
	}
*/
	return 0;
}
