/* Copyright (c) 2021 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <list>
#include <memory>
#include <unordered_map>
#include <functional>

namespace yue {

extern const std::string_view version;
extern const std::string_view extension;

using Options = std::unordered_map<std::string,std::string>;

struct YueConfig {
	bool lintGlobalVariable = false;
	bool implicitReturnRoot = true;
	bool reserveLineNumber = true;
	bool useSpaceOverTab = false;
	bool exporting = false;
	int lineOffset = 0;
	std::string module;
	Options options;
};

struct GlobalVar {
	std::string name;
	int line;
	int col;
};

using GlobalVars = std::list<GlobalVar>;

struct CompileInfo {
	std::string codes;
	std::string error;
	std::unique_ptr<GlobalVars> globals;
	std::unique_ptr<Options> options;
};

class YueCompilerImpl;

class YueCompiler {
public:
	YueCompiler(void* luaState = nullptr,
		const std::function<void(void*)>& luaOpen = nullptr,
		bool sameModule = false);
	virtual ~YueCompiler();
	CompileInfo compile(std::string_view codes, const YueConfig& config = {});
private:
	std::unique_ptr<YueCompilerImpl> _compiler;
};

} // namespace yue
