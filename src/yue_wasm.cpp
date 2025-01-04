/* Copyright (c) 2025 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "yuescript/yue_compiler.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
int luaopen_yue(lua_State* L);
} // extern "C"

static void openlibs(void* state) {
	lua_State* L = static_cast<lua_State*>(state);
	luaL_openlibs(L);
	luaopen_yue(L);
	luaL_requiref(L, "yue", luaopen_yue, 1);
	lua_pop(L, 1);
}

#define YUE_ARGS nullptr, openlibs

#include <string_view>
using namespace std::string_view_literals;
#include <emscripten/bind.h>
using namespace emscripten;

struct YueResult {
	std::string codes;
	std::string err;
};

YueResult tolua(const std::string& codes, bool reserveLineNumber = true, bool implicitReturnRoot = true, bool useSpaceOverTab = true) {
	yue::YueConfig config;
	config.reserveLineNumber = reserveLineNumber;
	config.implicitReturnRoot = implicitReturnRoot;
	config.useSpaceOverTab = useSpaceOverTab;
	auto result = yue::YueCompiler{YUE_ARGS}.compile(codes, config);
	if (result.error) {
		return {std::string(), result.error.value().displayMessage};
	} else {
		return {result.codes, std::string()};
	}
}

std::string version() { return std::string(yue::version); }

#define _DEFER(code, line) std::shared_ptr<void> _defer_##line(nullptr, [&](auto) { code; })
#define DEFER(code) _DEFER(code, __LINE__)

void pushYue(lua_State* L, std::string_view name) {
	lua_getglobal(L, "package"); // package
	lua_getfield(L, -1, "loaded"); // package loaded
	lua_getfield(L, -1, "yue"); // package loaded yue
	lua_pushlstring(L, &name.front(), name.size()); // package loaded yue name
	lua_gettable(L, -2); // loaded[name], package loaded yue item
	lua_insert(L, -4); // item package loaded yue
	lua_pop(L, 3); // item
}

void pushOptions(lua_State* L, int lineOffset) {
	lua_newtable(L);
	lua_pushliteral(L, "lint_global");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);
	lua_pushliteral(L, "implicit_return_root");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "reserve_line_number");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "space_over_tab");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);
	lua_pushliteral(L, "same_module");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "line_offset");
	lua_pushinteger(L, lineOffset);
	lua_rawset(L, -3);
}

std::string exec(const std::string& codes) {
	lua_State* L = luaL_newstate();
	openlibs(L);
	DEFER(lua_close(L));
	auto execStr = [&](const std::string& s) {
		std::string result;
		pushYue(L, "insert_loader"sv);
		if (lua_pcall(L, 0, 0, 0) != 0) {
			result += lua_tostring(L, -1);
			result += '\n';
			return result;
		}
		pushYue(L, "loadstring"sv);
		lua_pushlstring(L, s.c_str(), s.size());
		lua_pushliteral(L, "=(eval str)");
		if (lua_pcall(L, 2, 2, 0) != 0) {
			result += lua_tostring(L, -1);
			result += '\n';
			return result;
		}
		if (lua_isnil(L, -2) != 0) {
			result += lua_tostring(L, -1);
			result += '\n';
			return result;
		}
		lua_pop(L, 1);
		pushYue(L, "pcall"sv);
		lua_insert(L, -2);
		int last = lua_gettop(L) - 2;
		if (lua_pcall(L, 1, LUA_MULTRET, 0) != 0) {
			result = lua_tostring(L, -1);
			result += '\n';
			return result;
		}
		int cur = lua_gettop(L);
		int retCount = cur - last;
		bool success = lua_toboolean(L, -retCount) != 0;
		if (success) {
			if (retCount > 1) {
				for (int i = 1; i < retCount; ++i) {
					result += luaL_tolstring(L, -retCount + i, nullptr);
					result += '\n';
					lua_pop(L, 1);
				}
			}
		} else {
			result += lua_tostring(L, -1);
			result += '\n';
		}
		return result;
	};

	execStr(R"yuescript(
_G.__output = []
_G.print = (...)->
	_G.__output[] = table.concat [tostring select i, ... for i = 1, select "#", ...], " "
	_G.__output[] = "\n"
_io_write = io.write
_G.io.write = (...)->
	_G.__output[] = table.concat [tostring select i, ... for i = 1, select "#", ...]
	_io_write ...
	)yuescript");
	std::string res2 = execStr(codes);
	std::string res1 = execStr(R"yuescript(
res = table.concat _G.__output
return res if res ~= ""
	)yuescript");
	if (res1.empty()) {
		return res2;
	} else {
		if (res2.empty()) {
			return res1;
		} else {
			return res1 + res2;
		}
	}
}

EMSCRIPTEN_BINDINGS(yue) {
	value_array<YueResult>("YueResult")
		.element(&YueResult::codes)
		.element(&YueResult::err);
	function("tolua", &tolua);
	function("exec", &exec);
	function("version", &version);
}
