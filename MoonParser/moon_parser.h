#pragma once

#include <string>
#include <codecvt>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <vector>
#include "ast.hpp"
using namespace parserlib;

namespace MoonP {

struct State {
	State() {
		indents.push(0);
		stringOpen = -1;
	}
	std::string buffer;
	size_t stringOpen;
	std::stack<int> indents;
	std::stack<bool> doStack;
	static std::unordered_set<std::string> luaKeywords;
	static std::unordered_set<std::string> keywords;
};

} // namespace MoonP
