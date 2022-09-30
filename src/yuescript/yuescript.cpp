/* Copyright (c) 2022 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "yuescript/yue_compiler.h"
#include "yuescript/yue_parser.h"

using namespace std::string_literals;

#if defined(YUE_BUILD_AS_DLL)
#define YUE_API __declspec(dllexport)
#else
#define YUE_API
#endif

extern "C" {

#include "lauxlib.h"
#include "lua.h"

static const char yuescriptCodes[] =
#include "yuescript/yuescript.h"
	;

static void init_yuescript(lua_State* L) {
	if (luaL_loadbuffer(L, yuescriptCodes, sizeof(yuescriptCodes) / sizeof(yuescriptCodes[0]) - 1, "=(yuescript)") != 0) {
		std::string err = "failed to load yuescript module.\n"s + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	} else {
		lua_insert(L, -2);
		if (lua_pcall(L, 1, 0, 0) != 0) {
			std::string err = "failed to init yuescript module.\n"s + lua_tostring(L, -1);
			luaL_error(L, err.c_str());
		}
	}
}

static const char stpCodes[] =
#include "yuescript/stacktraceplus.h"
	;

static int init_stacktraceplus(lua_State* L) {
	if (luaL_loadbuffer(L, stpCodes, sizeof(stpCodes) / sizeof(stpCodes[0]) - 1, "=(stacktraceplus)") != 0) {
		std::string err = "failed to load stacktraceplus module.\n"s + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	} else if (lua_pcall(L, 0, 1, 0) != 0) {
		std::string err = "failed to init stacktraceplus module.\n"s + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	}
	return 1;
}

static int yuetolua(lua_State* L) {
	size_t size = 0;
	const char* input = luaL_checklstring(L, 1, &size);
	yue::YueConfig config;
	bool sameModule = false;
	if (lua_gettop(L) == 2) {
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_pushliteral(L, "lint_global");
		lua_gettable(L, -2);
		if (lua_isboolean(L, -1) != 0) {
			config.lintGlobalVariable = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "implicit_return_root");
		lua_gettable(L, -2);
		if (lua_isboolean(L, -1) != 0) {
			config.implicitReturnRoot = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "reserve_line_number");
		lua_gettable(L, -2);
		if (lua_isboolean(L, -1) != 0) {
			config.reserveLineNumber = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "space_over_tab");
		lua_gettable(L, -2);
		if (lua_isboolean(L, -1) != 0) {
			config.useSpaceOverTab = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "same_module");
		lua_gettable(L, -2);
		if (lua_isboolean(L, -1) != 0) {
			sameModule = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "line_offset");
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1) != 0) {
			config.lineOffset = static_cast<int>(lua_tonumber(L, -1));
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "module");
		lua_gettable(L, -2);
		if (lua_isstring(L, -1) != 0) {
			config.module = lua_tostring(L, -1);
		}
		lua_pop(L, 1);
		lua_pushliteral(L, "target");
		lua_gettable(L, -2);
		if (lua_isstring(L, -1) != 0) {
			config.options["target"] = lua_tostring(L, -1);
		}
		lua_pop(L, 1);
	}
	std::string s(input, size);
	auto result = yue::YueCompiler(L, nullptr, sameModule).compile(s, config);
	if (result.codes.empty() && !result.error.empty()) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, result.codes.c_str(), result.codes.size());
	}
	if (result.error.empty()) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, result.error.c_str(), result.error.size());
	}
	if (result.globals) {
		lua_createtable(L, static_cast<int>(result.globals->size()), 0);
		int i = 1;
		for (const auto& var : *result.globals) {
			lua_createtable(L, 3, 0);
			lua_pushlstring(L, var.name.c_str(), var.name.size());
			lua_rawseti(L, -2, 1);
			lua_pushinteger(L, var.line);
			lua_rawseti(L, -2, 2);
			lua_pushinteger(L, var.col);
			lua_rawseti(L, -2, 3);
			lua_rawseti(L, -2, i);
			i++;
		}
	} else {
		lua_pushnil(L);
	}
	return 3;
}

static int yuetoast(lua_State* L) {
	size_t size = 0;
	const char* input = luaL_checklstring(L, 1, &size);
	int flattenLevel = 2;
	if (lua_isnoneornil(L, 2) == 0) {
		flattenLevel = static_cast<int>(luaL_checkinteger(L, 2));
		flattenLevel = std::max(std::min(2, flattenLevel), 0);
	}
	yue::YueParser parser;
	auto info = parser.parse<yue::File_t>({input, size});
	if (info.node) {
		lua_createtable(L, 0, 0);
		int cacheIndex = lua_gettop(L);
		auto getName = [&](yue::ast_node* node) {
			int id = node->getId();
			lua_rawgeti(L, cacheIndex, id);
			if (lua_isnil(L, -1) != 0) {
				lua_pop(L, 1);
				auto name = node->getName();
				lua_pushlstring(L, &name.front(), name.length());
				lua_pushvalue(L, -1);
				lua_rawseti(L, cacheIndex, id);
			}
		};
		std::function<void(yue::ast_node*)> visit;
		visit = [&](yue::ast_node* node) {
			int count = 0;
			bool hasSep = false;
			node->visitChild([&](yue::ast_node* child) {
				if (yue::ast_is<yue::Seperator_t>(child)) {
					hasSep = true;
					return false;
				}
				count++;
				visit(child);
				return false;
			});
			switch (count) {
				case 0: {
					lua_createtable(L, 4, 0);
					getName(node);
					lua_rawseti(L, -2, 1);
					lua_pushinteger(L, node->m_begin.m_line);
					lua_rawseti(L, -2, 2);
					lua_pushinteger(L, node->m_begin.m_col);
					lua_rawseti(L, -2, 3);
					auto str = parser.toString(node);
					yue::Utils::trim(str);
					lua_pushlstring(L, str.c_str(), str.length());
					lua_rawseti(L, -2, 4);
					break;
				}
				case 1: {
					if (flattenLevel > 1 || (flattenLevel == 1 && !hasSep)) {
						getName(node);
						lua_rawseti(L, -2, 1);
						lua_pushinteger(L, node->m_begin.m_line);
						lua_rawseti(L, -2, 2);
						lua_pushinteger(L, node->m_begin.m_col);
						lua_rawseti(L, -2, 3);
						break;
					}
				}
				default: {
					lua_createtable(L, count + 3, 0);
					getName(node);
					lua_rawseti(L, -2, 1);
					lua_pushinteger(L, node->m_begin.m_line);
					lua_rawseti(L, -2, 2);
					lua_pushinteger(L, node->m_begin.m_col);
					lua_rawseti(L, -2, 3);
					for (int i = count, j = 4; i >= 1; i--, j++) {
						lua_pushvalue(L, -1 - i);
						lua_rawseti(L, -2, j);
					}
					lua_insert(L, -1 - count);
					lua_pop(L, count);
					break;
				}
			}
		};
		visit(info.node);
		return 1;
	} else {
		lua_pushnil(L);
		lua_pushlstring(L, info.error.c_str(), info.error.length());
		return 2;
	}
}

static const luaL_Reg yuelib[] = {
	{"to_lua", yuetolua},
	{"to_ast", yuetoast},
	{"version", nullptr},
	{"options", nullptr},
	{"load_stacktraceplus", nullptr},
	{nullptr, nullptr}};

YUE_API int luaopen_yue(lua_State* L) {
#if LUA_VERSION_NUM > 501
	luaL_newlib(L, yuelib); // yue
#else
	luaL_register(L, "yue", yuelib); // yue
#endif
	lua_pushlstring(L, &yue::version.front(), yue::version.size()); // yue version
	lua_setfield(L, -2, "version"); // yue["version"] = version, yue
	lua_createtable(L, 0, 0); // yue options
	lua_pushlstring(L, &yue::extension.front(), yue::extension.size()); // yue options ext
	lua_setfield(L, -2, "extension"); // options["extension"] = ext, yue options
	lua_pushliteral(L, LUA_DIRSEP);
	lua_setfield(L, -2, "dirsep"); // options["dirsep"] = dirsep, yue options
	lua_setfield(L, -2, "options"); // yue["options"] = options, yue
	lua_pushcfunction(L, init_stacktraceplus); // yue func1
	lua_setfield(L, -2, "load_stacktraceplus"); // yue["load_stacktraceplus"] = func1, yue
	lua_pushvalue(L, -1); // yue yue
	init_yuescript(L); // yue
	return 1;
}

} // extern "C"
