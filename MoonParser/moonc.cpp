#include <iostream>
#include "moon_compiler.h"

int main()
{
	std::string s = R"TestCodesHere(
print nil + 1
)TestCodesHere";
	std::list<MoonP::GlobalVar> globals;
	auto result = MoonP::moonCompile(s, globals, true, false);
	if (!result.first.empty()) {
		std::cout << result.first << '\n';
	} else {
		std::cout << result.second << '\n';
	}
	std::cout << "globals:\n";
	for (const auto& var : globals) {
		std::cout << var.name << '\n';
	}
	return 0;
}
