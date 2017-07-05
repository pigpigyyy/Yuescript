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
	struct State
	{
		State()
		{
			indents.push(0);
			stringOpen = -1;
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
	};

	struct White : star<one<' ', '\t', '\r', '\n'>> {};
	struct plain_space : star<one<' ', '\t'>> {};
	struct Break : seq<opt<one<'\r'>>, one<'\n'>> {};
	struct Stop : sor<Break, eof> {};
	struct Comment : seq<string<'-', '-'>, star<not_one<'\r', '\n'>>, Stop> {};
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
	template<char... Cs> struct op : seq<Space, string<Cs...>, not_at<AlphaNum>> {};

	struct Name
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, const State& st)
		{
			const char* current = in.current();
			if (SpaceName::match<A, M, Action, Control>(in, st))
			{
				auto name = slice::Slice(current, in.current() - current + 1);
				name.trimSpace();
				auto it = st.keywords.find(name);
				if (it == st.keywords.end())
				{
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
	struct KeyName : sor<SelfName, SpaceName> {};
	struct VarArg : seq<Space, string<'.', '.', '.'>> {};

	struct CheckIndent
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, const State& st)
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

	struct Advance
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (Indent::match<A, M, Action, Control>(in))
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
				}
				return true;
			}
			return false;
		}
	};

	struct PushIndent
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			const char* current = in.current();
			if (Indent::match<A, M, Action, Control>(in))
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

	struct PreventIndent
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

	struct PopIndent
	{
		using analyze_t = analysis::generic<analysis::rule_type::ANY>;

		template<apply_mode A, rewind_mode M,
			template<typename...> class Action,
			template<typename...> class Control,
			typename Input>
		static bool match(Input& in, State& st)
		{
			if (st.indents.size() > 1) st.indents.pop();
			return true;
		}
	};

	struct Block;

	struct InBlock : seq<Advance, Block, PopIndent> {};

	struct NameList;

	struct Local : seq<key<'l', 'o', 'c', 'a', 'l'>, sor<sor<op<'*'>, op<'^'>>>, NameList> {};

	struct colon_import_name : seq<one<'\\'>, Name> {};
	struct ImportName : sor<colon_import_name, Name> {};
	struct ImportNameList : seq<
		star<SpaceBreak>,
		ImportName,
		star<
			sor<plus<SpaceBreak>, seq<sym<','>, star<SpaceBreak>>>,
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
		string<'e', 'l', 's', 'e'>, Body
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
		>
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
			if (seq<key<'d', 'o'>, Body>::match<A, M, Action, Control>(in, st))
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
		op<'o', 'r'>,
		op<'a', 'n', 'd'>,
		op<'<', '='>,
		op<'>', '='>,
		op<'~', '='>,
		op<'!', '='>,
		op<'=', '='>,
		op<'.', '.'>,
		op<'<', '<'>,
		op<'>', '>'>,
		op<'/', '/'>> {};
	struct BinaryOperator : seq<sor<CharOperators, WordOperators>, star<SpaceBreak>> {};

	struct Chain;

	struct Assignable : sor<Chain, Name, SelfName> {};

	struct Value;

	struct Exp : seq<Value, star<BinaryOperator, Value>> {};

	struct Callable;
	struct InvokeArgs;

	struct ChainValue : seq<sor<Chain, Callable>, opt<InvokeArgs>> {};

	struct KeyValueList;
	struct String;
	struct SimpleValue;

	struct Value : sor<String, SimpleValue, KeyValueList, ChainValue> {};
	struct SliceValue : Exp {};

	struct LuaString;

	struct single_string_inner : sor<string<'\\','\''>, string<'\\', '\\'>, not_one<'\''>> {};
	struct SingleString : seq<symx<'\''>, star<single_string_inner>, sym<'\''>> {};
	struct interp : seq<symx<'#', '{'>, Exp, sym<'}'>> {};
	struct double_string_plain : sor<string<'\\','\"'>, string<'\\', '\\'>, not_one<'\"'>> {};
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

	struct FnArgsExpList : seq<Exp, star<sor<Break, one<','>>, White, Exp>> {};
	struct FnArgs : sor<
		seq<symx<'('>, star<SpaceBreak>, opt<FnArgsExpList>, star<SpaceBreak>, sym<')'>>,
		seq<sym<'!'>, not_at<one<'='>>>
	> {};

	struct ChainItems;
	struct DotChainItem;
	struct ColonChain;

	struct Chain : sor<
		seq<sor<Callable, String, not_one<'.', '\\'>>, ChainItems>,
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
	struct outer_value_no_shadowing : success {};
	struct without_args_def : success {};

	struct FnArgsDef : sor<
		seq<sym<'('>, White, opt<FnArgDefList>,
			sor<
				outer_value_shadow,
				outer_value_no_shadowing
			>,
			White, sym<')'>
		>,
		without_args_def
	> {};

	struct FunLit : seq<FnArgsDef, sor<sym<'-', '>'>, sym<'=', '>'>>, sor<Body, success>> {};

	struct NameList : seq<Name, star<sym<','>, Name>> {};
	struct NameOrDestructure : sor<Name, TableLit> {};
	struct AssignableNameList : seq<NameOrDestructure, star<sym<','>, NameOrDestructure>> {};

	struct ExpList : seq<Exp, star<sym<','>, Exp>> {};
	struct ExpListLow : seq<Exp, star<sor<sym<','>, sym<';'>, Exp>>> {};

	struct ArgLine : seq<CheckIndent, ExpList> {};
	struct ArgBlock : seq<ArgLine, star<sym<','>, SpaceBreak, ArgLine>, PopIndent> {};

	struct InvokeArgs : seq<
		at<not_one<'-'>>,
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

	struct Sentence : sor<
		Import, While, With, For, ForEach,
		Switch, Return, Local, Export, BreakLoop,
		Assignment, ExpList
	> {};

	struct Statement : seq<
		Sentence,
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

	struct BlockWithEnd : seq<Block, eolf> {};

	template<typename Rule>
	struct action : nothing<Rule> {};

	template<>
	struct action<Assignment>
	{
		template<typename Input>
		static void apply(const Input& in, State&)
		{
			std::cout << "Assignment: " << in.string() << '\n';
		}
	};

	template<>
	struct action<Return>
	{
		template<typename Input>
		static void apply(const Input& in, State&)
		{
			std::cout << "Return: " << in.string() << "\n\n";
		}
	};
}

int main()
{
	analyze<moon::BlockWithEnd>();
	moon::State state;
	string_input<> inName(R"xoxo(
debug_grammar = false
lpeg = require "lpeg"

lpeg.setmaxstack 10000 -- whoa

err_msg = "Failed to parse:%s\n [%d] >>    %s"

import Stack from require "moonscript.data"
import trim, pos_to_line, get_line from require "moonscript.util"
import unpack from require "moonscript.util"
import wrap_env from require "moonscript.parse.env"

{
  :R, :S, :V, :P, :C, :Ct, :Cmt, :Cg, :Cb, :Cc
} = lpeg

{
  :White, :Break, :Stop, :Comment, :Space, :SomeSpace, :SpaceBreak, :EmptyLine,
  :AlphaNum, :Num, :Shebang, :L
  Name: _Name
} = require "moonscript.parse.literals"

SpaceName = Space * _Name
Num = Space * (Num / (v) -> {"number", v})

{
  :Indent, :Cut, :ensure, :extract_line, :mark, :pos, :flatten_or_mark,
  :is_assignable, :check_assignable, :format_assign, :format_single_assign,
  :sym, :symx, :simple_string, :wrap_func_arg, :join_chain,
  :wrap_decorator, :check_lua_string, :self_assign, :got

} = require "moonscript.parse.util"


build_grammar = wrap_env debug_grammar, (root) ->
  _indent = Stack 0
  _do_stack = Stack 0

  state = {
    -- last pos we saw, used to report error location
    last_pos: 0
  }

  check_indent = (str, pos, indent) ->
    state.last_pos = pos
    _indent\top! == indent

  advance_indent = (str, pos, indent) ->
    top = _indent\top!
    if top != -1 and indent > top
      _indent\push indent
      true

  push_indent = (str, pos, indent) ->
    _indent\push indent
    true

  pop_indent = ->
    assert _indent\pop!, "unexpected outdent"
    true

  check_do = (str, pos, do_node) ->
    top = _do_stack\top!
    if top == nil or top
      return true, do_node
    false

  disable_do = ->
    _do_stack\push false
    true

  pop_do = ->
    assert _do_stack\pop! != nil, "unexpected do pop"
    true

  DisableDo = Cmt "", disable_do
  PopDo = Cmt "", pop_do

  keywords = {}
  key = (chars) ->
    keywords[chars] = true
    Space * chars * -AlphaNum

  op = (chars) ->
    patt = Space * C chars
    -- it's a word, treat like keyword
    if chars\match "^%w*$"
      keywords[chars] = true
      patt *= -AlphaNum

    patt

  Name = Cmt(SpaceName, (str, pos, name) ->
    return false if keywords[name]
    true
  ) / trim

  SelfName = Space * "@" * (
    "@" * (_Name / mark"self_class" + Cc"self.__class") +
    _Name / mark"self" +
    Cc"self" -- @ by itself
  )

  KeyName = SelfName + Space * _Name / mark"key_literal"
  VarArg = Space * P"..." / trim

  g = P {
    root or File
    File: Shebang^-1 * (Block + Ct"")
    Block: Ct(Line * (Break^1 * Line)^0)
    CheckIndent: Cmt(Indent, check_indent), -- validates line is in correct indent
    Line: (CheckIndent * Statement + Space * L(Stop))

    Statement: pos(
        Import + While + With + For + ForEach + Switch + Return +
        Local + Export + BreakLoop +
        Ct(ExpList) * (Update + Assign)^-1 / format_assign
      ) * Space * ((
        -- statement decorators
        key"if" * Exp * (key"else" * Exp)^-1 * Space / mark"if" +
        key"unless" * Exp / mark"unless" +
        CompInner / mark"comprehension"
      ) * Space)^-1 / wrap_decorator

    Body: Space^-1 * Break * EmptyLine^0 * InBlock + Ct(Statement) -- either a statement, or an indented block

    Advance: L Cmt(Indent, advance_indent) -- Advances the indent, gives back whitespace for CheckIndent
    PushIndent: Cmt(Indent, push_indent)
    PreventIndent: Cmt(Cc(-1), push_indent)
    PopIndent: Cmt("", pop_indent)
    InBlock: Advance * Block * PopIndent

    Local: key"local" * ((op"*" + op"^") / mark"declare_glob" + Ct(NameList) / mark"declare_with_shadows")

    Import: key"import" * Ct(ImportNameList) * SpaceBreak^0 * key"from" * Exp / mark"import"
    ImportName: (sym"\\" * Ct(Cc"colon" * Name) + Name)
    ImportNameList: SpaceBreak^0 * ImportName * ((SpaceBreak^1 + sym"," * SpaceBreak^0) * ImportName)^0

    BreakLoop: Ct(key"break"/trim) + Ct(key"continue"/trim)

    Return: key"return" * (ExpListLow/mark"explist" + C"") / mark"return"

    WithExp: Ct(ExpList) * Assign^-1 / format_assign
    With: key"with" * DisableDo * ensure(WithExp, PopDo) * key"do"^-1 * Body / mark"with"

    Switch: key"switch" * DisableDo * ensure(Exp, PopDo) * key"do"^-1 * Space^-1 * Break * SwitchBlock / mark"switch"

    SwitchBlock: EmptyLine^0 * Advance * Ct(SwitchCase * (Break^1 * SwitchCase)^0 * (Break^1 * SwitchElse)^-1) * PopIndent
    SwitchCase: key"when" * Ct(ExpList) * key"then"^-1 * Body / mark"case"
    SwitchElse: key"else" * Body / mark"else"

    IfCond: Exp * Assign^-1 / format_single_assign

    IfElse: (Break * EmptyLine^0 * CheckIndent)^-1  * key"else" * Body / mark"else"
    IfElseIf: (Break * EmptyLine^0 * CheckIndent)^-1 * key"elseif" * pos(IfCond) * key"then"^-1 * Body / mark"elseif"

    If: key"if" * IfCond * key"then"^-1 * Body * IfElseIf^0 * IfElse^-1 / mark"if"
    Unless: key"unless" * IfCond * key"then"^-1 * Body * IfElseIf^0 * IfElse^-1 / mark"unless"

    While: key"while" * DisableDo * ensure(Exp, PopDo) * key"do"^-1 * Body / mark"while"

    For: key"for" * DisableDo * ensure(Name * sym"=" * Ct(Exp * sym"," * Exp * (sym"," * Exp)^-1), PopDo) *
      key"do"^-1 * Body / mark"for"

    ForEach: key"for" * Ct(AssignableNameList) * key"in" * DisableDo * ensure(Ct(sym"*" * Exp / mark"unpack" + ExpList), PopDo) * key"do"^-1 * Body / mark"foreach"

    Do: key"do" * Body / mark"do"

    Comprehension: sym"[" * Exp * CompInner * sym"]" / mark"comprehension"

    TblComprehension: sym"{" * Ct(Exp * (sym"," * Exp)^-1) * CompInner * sym"}" / mark"tblcomprehension"

    CompInner: Ct((CompForEach + CompFor) * CompClause^0)
    CompForEach: key"for" * Ct(AssignableNameList) * key"in" * (sym"*" * Exp / mark"unpack" + Exp) / mark"foreach"
    CompFor: key "for" * Name * sym"=" * Ct(Exp * sym"," * Exp * (sym"," * Exp)^-1) / mark"for"
    CompClause: CompFor + CompForEach + key"when" * Exp / mark"when"

    Assign: sym"=" * (Ct(With + If + Switch) + Ct(TableBlock + ExpListLow)) / mark"assign"
    Update: ((sym"..=" + sym"+=" + sym"-=" + sym"*=" + sym"/=" + sym"%=" + sym"or=" + sym"and=" + sym"&=" + sym"|=" + sym">>=" + sym"<<=") / trim) * Exp / mark"update"

    CharOperators: Space * C(S"+-*/%^><|&")
    WordOperators: op"or" + op"and" + op"<=" + op">=" + op"~=" + op"!=" + op"==" + op".." + op"<<" + op">>" + op"//"
    BinaryOperator: (WordOperators + CharOperators) * SpaceBreak^0

    Assignable: Cmt(Chain, check_assignable) + Name + SelfName
    Exp: Ct(Value * (BinaryOperator * Value)^0) / flatten_or_mark"exp"

    SimpleValue:
      If + Unless +
      Switch +
      With +
      ClassDecl +
      ForEach + For + While +
      Cmt(Do, check_do) +
      sym"-" * -SomeSpace * Exp / mark"minus" +
      sym"#" * Exp / mark"length" +
      sym"~" * Exp / mark"bitnot" +
      key"not" * Exp / mark"not" +
      TblComprehension +
      TableLit +
      Comprehension +
      FunLit +
      Num

    -- a function call or an object access
    ChainValue: (Chain + Callable) * Ct(InvokeArgs^-1) / join_chain

    Value: pos(
      SimpleValue +
      Ct(KeyValueList) / mark"table" +
      ChainValue +
      String)

    SliceValue: Exp

    String: Space * DoubleString + Space * SingleString + LuaString
    SingleString: simple_string("'")
    DoubleString: simple_string('"', true)

    LuaString: Cg(LuaStringOpen, "string_open") * Cb"string_open" * Break^-1 *
      C((1 - Cmt(C(LuaStringClose) * Cb"string_open", check_lua_string))^0) *
      LuaStringClose / mark"string"

    LuaStringOpen: sym"[" * P"="^0 * "[" / trim
    LuaStringClose: "]" * P"="^0 * "]"

    Callable: pos(Name / mark"ref") + SelfName + VarArg + Parens / mark"parens"
    Parens: sym"(" * SpaceBreak^0 * Exp * SpaceBreak^0 * sym")"

    FnArgs: symx"(" * SpaceBreak^0 * Ct(FnArgsExpList^-1) * SpaceBreak^0 * sym")" + sym"!" * -P"=" * Ct""
    FnArgsExpList: Exp * ((Break + sym",") * White * Exp)^0

    Chain: (Callable + String + -S".\\") * ChainItems / mark"chain" +
      Space * (DotChainItem * ChainItems^-1 + ColonChain) / mark"chain"

    ChainItems: ChainItem^1 * ColonChain^-1 + ColonChain

    ChainItem:
      Invoke +
      DotChainItem +
      Slice +
      symx"[" * Exp/mark"index" * sym"]"

    DotChainItem: symx"." * _Name/mark"dot"
    ColonChainItem: symx"\\" * _Name / mark"colon"
    ColonChain: ColonChainItem * (Invoke * ChainItems^-1)^-1

    Slice: symx"[" * (SliceValue + Cc(1)) * sym"," * (SliceValue + Cc"")  *
      (sym"," * SliceValue)^-1 *sym"]" / mark"slice"

    Invoke: FnArgs / mark"call" +
      SingleString / wrap_func_arg +
      DoubleString / wrap_func_arg +
      L(P"[") * LuaString / wrap_func_arg

    TableValue: KeyValue + Ct(Exp)

    TableLit: sym"{" * Ct(
        TableValueList^-1 * sym","^-1 *
        (SpaceBreak * TableLitLine * (sym","^-1 * SpaceBreak * TableLitLine)^0 * sym","^-1)^-1
      ) * White * sym"}" / mark"table"

    TableValueList: TableValue * (sym"," * TableValue)^0
    TableLitLine: PushIndent * ((TableValueList * PopIndent) + (PopIndent * Cut)) + Space

    -- the unbounded table
    TableBlockInner: Ct(KeyValueLine * (SpaceBreak^1 * KeyValueLine)^0)
    TableBlock: SpaceBreak^1 * Advance * ensure(TableBlockInner, PopIndent) / mark"table"

    ClassDecl: key"class" * -P":" * (Assignable + Cc(nil)) * (key"extends" * PreventIndent * ensure(Exp, PopIndent) + C"")^-1 * (ClassBlock + Ct("")) / mark"class"

    ClassBlock: SpaceBreak^1 * Advance *
      Ct(ClassLine * (SpaceBreak^1 * ClassLine)^0) * PopIndent
    ClassLine: CheckIndent * ((
        KeyValueList / mark"props" +
        Statement / mark"stm" +
        Exp / mark"stm"
      ) * sym","^-1)

    Export: key"export" * (
      Cc"class" * ClassDecl +
      op"*" + op"^" +
      Ct(NameList) * (sym"=" * Ct(ExpListLow))^-1) / mark"export"

    KeyValue: (sym":" * -SomeSpace *  Name * lpeg.Cp!) / self_assign +
      Ct(
        (KeyName + sym"[" * Exp * sym"]" +Space * DoubleString + Space * SingleString) *
        symx":" *
        (Exp + TableBlock + SpaceBreak^1 * Exp)
      )

    KeyValueList: KeyValue * (sym"," * KeyValue)^0
    KeyValueLine: CheckIndent * KeyValueList * sym","^-1

    FnArgsDef: sym"(" * White * Ct(FnArgDefList^-1) *
      (key"using" * Ct(NameList + Space * "nil") + Ct"") *
      White * sym")" + Ct"" * Ct""

    FnArgDefList: FnArgDef * ((sym"," + Break) * White * FnArgDef)^0 * ((sym"," + Break) * White * Ct(VarArg))^0 + Ct(VarArg)
    FnArgDef: Ct((Name + SelfName) * (sym"=" * Exp)^-1)

    FunLit: FnArgsDef *
      (sym"->" * Cc"slim" + sym"=>" * Cc"fat") *
      (Body + Ct"") / mark"fndef"

    NameList: Name * (sym"," * Name)^0
    NameOrDestructure: Name + TableLit
    AssignableNameList: NameOrDestructure * (sym"," * NameOrDestructure)^0

    ExpList: Exp * (sym"," * Exp)^0
    ExpListLow: Exp * ((sym"," + sym";") * Exp)^0

    -- open args
    InvokeArgs: -P"-" * (ExpList * (sym"," * (TableBlock + SpaceBreak * Advance * ArgBlock * TableBlock^-1) + TableBlock)^-1 + TableBlock)
    ArgBlock: ArgLine * (sym"," * SpaceBreak * ArgLine)^0 * PopIndent
    ArgLine: CheckIndent * ExpList
  }

  g, state

file_parser = ->
  g, state = build_grammar!
  file_grammar = White * g * White * -1

  {
    match: (str) =>
      local tree
      _, err = xpcall (->
        tree = file_grammar\match str
      ), (err) ->
        debug.traceback err, 2

      -- regular error, let it bubble up
      if type(err) == "string"
        return nil, err

      unless tree
        local msg
        err_pos = state.last_pos

        if err
          node, msg = unpack err
          msg = " " .. msg if msg
          err_pos = node[-1]

        line_no = pos_to_line str, err_pos
        line_str = get_line(str, line_no) or ""
        return nil, err_msg\format msg or "", line_no, trim line_str

      tree
  }

{
  :extract_line
  :build_grammar

  -- parse a string as a file
  -- returns tree, or nil and error message
  string: (str) -> file_parser!\match str
}
)xoxo", "abc");
	try
	{
		if (parse<must<moon::BlockWithEnd>, moon::action>(inName, state))
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
