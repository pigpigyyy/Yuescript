#pragma once

#include <string>
#include <tuple>
#include <list>

namespace MoonP {

std::pair<std::string,std::string> moonCompile(const std::string& codes, bool implicitReturnRoot = true, bool lineNumber = true);

struct GlobalVar {
	std::string name;
	int line;
	int col;
};
std::pair<std::string,std::string> moonCompile(const std::string& codes, std::list<GlobalVar>& globals, bool implicitReturnRoot = true, bool lineNumber = true);

} // namespace MoonP
