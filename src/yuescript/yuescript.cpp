/* Copyright (c) 2024 Li Jin, dragon-fly@qq.com

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

#if LUA_VERSION_NUM > 501
#ifndef LUA_COMPAT_5_1
#ifndef lua_objlen
#define lua_objlen lua_rawlen
#endif // lua_objlen
#endif // LUA_COMPAT_5_1
#endif // LUA_VERSION_NUM

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

static void get_config(lua_State* L, yue::YueConfig& config) {
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
	lua_pushliteral(L, "reserve_comment");
	lua_gettable(L, -2);
	if (lua_isboolean(L, -1) != 0) {
		config.reserveComment = lua_toboolean(L, -1) != 0;
	}
	lua_pop(L, 1);
	lua_pushliteral(L, "space_over_tab");
	lua_gettable(L, -2);
	if (lua_isboolean(L, -1) != 0) {
		config.useSpaceOverTab = lua_toboolean(L, -1) != 0;
	}
	lua_pop(L, 1);
	lua_pushliteral(L, "options");
	lua_gettable(L, -2);
	if (lua_istable(L, -1) != 0) {
		lua_pushnil(L); // options startKey
		while (lua_next(L, -2) != 0) { // options key value
			size_t len = 0;
			auto pstr = lua_tolstring(L, -2, &len);
			std::string key{pstr, len};
			pstr = lua_tolstring(L, -1, &len);
			std::string value{pstr, len};
			config.options[key] = value;
			lua_pop(L, 1); // options key
		}
		lua_pop(L, 1); // options
	}
	lua_pop(L, 1);
}

static int yuetolua(lua_State* L) {
	size_t len = 0;
	auto input = luaL_checklstring(L, 1, &len);
	std::string_view codes(input, len);
	yue::YueConfig config;
	bool sameModule = false;
	if (lua_gettop(L) >= 2) {
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_pushvalue(L, 2);
		get_config(L, config);
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
		lua_pop(L, 2);
	}
	auto result = yue::YueCompiler(L, nullptr, sameModule).compile(codes, config);
	if (result.error) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, result.codes.c_str(), result.codes.size());
	}
	if (result.error) {
		const auto& msg = result.error.value().displayMessage;
		lua_pushlstring(L, msg.c_str(), msg.size());
	} else {
		lua_pushnil(L);
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

static int yueformat(lua_State* L) {
	size_t len = 0;
	auto input = luaL_checklstring(L, 1, &len);
	int tabSize = 0;
	if (!lua_isnoneornil(L, 2)) {
		tabSize = static_cast<int>(luaL_checkinteger(L, 2));
	}
	std::string_view codes(input, len);
	auto info = yue::YueParser::shared().parse<yue::File_t>(codes);
	if (info.error) {
		const auto& error = info.error.value();
		if (!info.codes) {
			lua_pushnil(L);
			lua_pushlstring(L, error.msg.c_str(), error.msg.size());
			return 2;
		}
		auto displayMessage = info.errorMessage(error.msg, error.line, error.col, 0);
		lua_pushnil(L);
		lua_pushlstring(L, displayMessage.c_str(), displayMessage.size());
		return 2;
	}
	yue::YueFormat formatter{};
	if (tabSize > 0) {
		formatter.spaceOverTab = true;
		formatter.tabSpaces = tabSize;
	} else {
		formatter.spaceOverTab = false;
	}
	auto result = formatter.toString(info.node.get());
	lua_pushlstring(L, result.c_str(), result.size());
	return 1;
}

static int yuecheck(lua_State* L) {
	size_t len = 0;
	auto input = luaL_checklstring(L, 1, &len);
	std::string_view codes(input, len);
	yue::YueConfig config;
	config.lintGlobalVariable = true;
	if (lua_gettop(L) >= 2) {
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_pushvalue(L, 2);
		get_config(L, config);
		lua_pop(L, 1);
	}
	auto result = yue::YueCompiler(L).compile(codes, config);
	lua_createtable(L, 0, 0);
	int i = 0;
	if (result.error) {
		const auto& error = result.error.value();
		lua_createtable(L, 4, 0);
		lua_pushliteral(L, "error");
		lua_rawseti(L, -2, 1);
		lua_pushlstring(L, error.msg.c_str(), error.msg.length());
		lua_rawseti(L, -2, 2);
		lua_pushinteger(L, error.line);
		lua_rawseti(L, -2, 3);
		lua_pushinteger(L, error.col);
		lua_rawseti(L, -2, 4);
		lua_rawseti(L, -2, ++i);
	}
	if (result.globals) {
		for (const auto& global : *result.globals) {
			lua_createtable(L, 4, 0);
			lua_pushliteral(L, "global");
			lua_rawseti(L, -2, 1);
			lua_pushlstring(L, global.name.c_str(), global.name.length());
			lua_rawseti(L, -2, 2);
			lua_pushinteger(L, global.line);
			lua_rawseti(L, -2, 3);
			lua_pushinteger(L, global.col);
			lua_rawseti(L, -2, 4);
			lua_rawseti(L, -2, ++i);
		}
	}
	if (result.error) {
		lua_pushboolean(L, 0);
		lua_insert(L, -2);
		return 2;
	} else {
		lua_pushboolean(L, 1);
		lua_insert(L, -2);
		lua_pushlstring(L, result.codes.c_str(), result.codes.length());
		return 3;
	}
}

struct yue_stack {
	int continuation;
	yue::ast_node* node;
	int i;
	std::unique_ptr<std::vector<yue::ast_node*>> children;
	bool hasSep;
};

static int yuetoast(lua_State* L) {
	size_t size = 0;
	const char* input = luaL_checklstring(L, 1, &size);
	int flattenLevel = 0;
	if (!lua_isnoneornil(L, 2)) {
		flattenLevel = static_cast<int>(luaL_checkinteger(L, 2));
		flattenLevel = std::max(std::min(2, flattenLevel), 0);
	}
	std::string_view ruleName;
	if (!lua_isnoneornil(L, 3)) {
		size_t nameSize = 0;
		if (auto name = luaL_checklstring(L, 3, &nameSize)) {
			ruleName = {name, nameSize};
		}
	}
	auto& yueParser = yue::YueParser::shared();
	auto info = ruleName.empty() ? yueParser.parse<yue::File_t>({input, size}) : yueParser.parse(ruleName, {input, size});
	if (!info.error) {
		lua_createtable(L, 0, 0);
		int tableIndex = lua_gettop(L);
		lua_createtable(L, 0, 0);
		int cacheIndex = lua_gettop(L);
		auto getName = [&](yue::ast_node* node) {
			int id = node->get_id();
			lua_rawgeti(L, cacheIndex, id);
			if (lua_isnil(L, -1) != 0) {
				lua_pop(L, 1);
				auto name = node->get_name();
				lua_pushlstring(L, &name.front(), name.length());
				lua_pushvalue(L, -1);
				lua_rawseti(L, cacheIndex, id);
			}
		};
		std::stack<yue_stack> stack;
		auto do_call = [&](yue::ast_node* node) {
			stack.push({0, node, 0, nullptr, false});
		};
		auto do_return = [&]() {
			stack.pop();
		};
		do_call(info.node);
		yue::YueFormat formatter{};
		while (!stack.empty()) {
			auto& current = stack.top();
			int continuation = current.continuation;
			auto node = current.node;
			if (auto comment = yue::ast_cast<yue::YueMultilineComment_t>(node)) {
				node = comment->inner.get();
			}
			switch (continuation) {
				case 0: {
					if (!current.children) {
						node->visit_child([&](yue::ast_node* child) {
							if (yue::ast_is<yue::Seperator_t>(child)) {
								current.hasSep = true;
								return false;
							}
							if (!current.children) {
								current.children = std::make_unique<std::vector<yue::ast_node*>>();
							}
							current.children->push_back(child);
							return false;
						});
					}
					current.i = 0;
					current.continuation = 1;
					break;
				}
				case 1: {
					if (current.children && current.i < static_cast<int>(current.children->size())) {
						do_call(current.children->at(current.i));
						current.continuation = 2;
						break;
					}
					current.continuation = 3;
					break;
				}
				case 2: {
					current.i++;
					current.continuation = 1;
					break;
				}
				case 3: {
					int count = current.children ? static_cast<int>(current.children->size()) : 0;
					switch (count) {
						case 0: {
							lua_createtable(L, 4, 0);
							getName(node);
							lua_rawseti(L, -2, 1);
							lua_pushinteger(L, node->m_begin.m_line);
							lua_rawseti(L, -2, 2);
							lua_pushinteger(L, node->m_begin.m_col);
							lua_rawseti(L, -2, 3);
							formatter.indent = 0;
							auto str = node->to_string(&formatter);
							yue::Utils::trim(str);
							lua_pushlstring(L, str.c_str(), str.length());
							lua_rawseti(L, -2, 4);
							lua_rawseti(L, tableIndex, lua_objlen(L, tableIndex) + 1);
							break;
						}
						case 1: {
							if (flattenLevel > 1 || (flattenLevel == 1 && !current.hasSep)) {
								lua_rawgeti(L, tableIndex, lua_objlen(L, tableIndex) + 1);
								getName(node);
								lua_rawseti(L, -2, 1);
								lua_pushinteger(L, node->m_begin.m_line);
								lua_rawseti(L, -2, 2);
								lua_pushinteger(L, node->m_begin.m_col);
								lua_rawseti(L, -2, 3);
								lua_pop(L, 1);
								break;
							}
							[[fallthrough]];
						}
						default: {
							auto len = lua_objlen(L, tableIndex);
							lua_createtable(L, count + 3, 0);
							getName(node);
							lua_rawseti(L, -2, 1);
							lua_pushinteger(L, node->m_begin.m_line);
							lua_rawseti(L, -2, 2);
							lua_pushinteger(L, node->m_begin.m_col);
							lua_rawseti(L, -2, 3);
							for (int i = count, j = 4; i >= 1; i--, j++) {
								lua_rawgeti(L, tableIndex, len - i + 1);
								lua_rawseti(L, -2, j);
							}
							for (int i = 1; i <= count; i++) {
								lua_pushnil(L);
								lua_rawseti(L, tableIndex, len);
								len--;
							}
							lua_rawseti(L, tableIndex, lua_objlen(L, tableIndex) + 1);
							break;
						}
					}
					do_return();
					break;
				}
			}
		}
		lua_rawgeti(L, tableIndex, 1);
		return 1;
	} else {
		lua_pushnil(L);
		const auto& err = info.error.value();
		auto displayMessage = info.errorMessage(err.msg, err.line, err.col);
		lua_pushlstring(L, displayMessage.c_str(), displayMessage.length());
		return 2;
	}
}

static int yueisast(lua_State* L) {
	size_t nameLen = 0;
	auto name = luaL_tolstring(L, 1, &nameLen);
	size_t codeLen = 0;
	auto code = luaL_tolstring(L, 2, &codeLen);
	bool result = yue::YueParser::shared().match({name, nameLen}, {code, codeLen});
	lua_pushboolean(L, result ? 1 : 0);
	return 1;
}

static const luaL_Reg yuelib[] = {
	{"to_lua", yuetolua},
	{"to_ast", yuetoast},
	{"is_ast", yueisast},
	{"check", yuecheck},
	{"format", yueformat},
	{"version", nullptr},
	{"options", nullptr},
	{"load_stacktraceplus", nullptr},
	{nullptr, nullptr}};

YUE_API int luaopen_yue(lua_State* L) {
#if LUA_VERSION_NUM > 501
	luaL_newlib(L, yuelib); // yue
#else
	lua_getglobal(L, "package"); // package
	lua_getfield(L, -1, "loaded"); // package loaded
	lua_createtable(L, 0, 0); // package loaded yue
	lua_pushvalue(L, -1); // package loaded yue yue
	lua_setfield(L, -3, "yue"); // loaded["yue"] = yue, package loaded yue
	luaL_register(L, nullptr, yuelib); // package loaded yue
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
