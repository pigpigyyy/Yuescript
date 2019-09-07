#pragma once

#include <string>
#include <codecvt>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <sstream>
#include <vector>
#include "parserlib.hpp"
using namespace parserlib;

struct State
{
	State()
	{
		indents.push(0);
		stringOpen = -1;
	}
	std::stringstream buffer;
	size_t stringOpen;
	std::stack<int> indents;
	std::stack<bool> doStack;
	std::unordered_set<std::string> keywords = {
		"and", "while", "else", "using", "continue",
		"local", "not", "then", "return", "from",
		"extends", "for", "do", "or", "export",
		"class", "in", "unless", "when", "elseif",
		"switch", "break", "if", "with", "import", "true", "false", "nil"
	};
};
