/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <list>
#include <memory>
#include <functional>

namespace MoonP {

extern const std::string_view version;
extern const std::string_view extension;

struct MoonConfig {
	bool lintGlobalVariable = false;
	bool implicitReturnRoot = true;
	bool reserveLineNumber = true;
	bool useSpaceOverTab = false;
	int lineOffset = 0;
};

struct GlobalVar {
	std::string name;
	int line;
	int col;
};

using GlobalVars = std::unique_ptr<std::list<GlobalVar>>;

class MoonCompilerImpl;

class MoonCompiler {
public:
	MoonCompiler(void* luaState = nullptr,
		const std::function<void(void*)>& luaOpen = nullptr,
		bool sameModule = false);
	virtual ~MoonCompiler();
	std::tuple<std::string,std::string,GlobalVars> compile(std::string_view codes, const MoonConfig& config = {});
private:
	std::unique_ptr<MoonCompilerImpl> _compiler;
};

} // namespace MoonP
