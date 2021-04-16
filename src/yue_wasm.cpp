#include "yuescript/yue_compiler.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
int luaopen_yue(lua_State* L);
} // extern "C"

static void openlibs(void* state) {
	lua_State* L = static_cast<lua_State*>(state);
	luaL_openlibs(L);
	luaopen_yue(L);
}

#define YUE_ARGS nullptr,openlibs

#include <string_view>
using namespace std::string_view_literals;
#include <emscripten/bind.h>
using namespace emscripten;

struct YueResult
{
	std::string codes;
	std::string err;
};

YueResult tolua(const std::string& codes, bool reserveLineNumber = true, bool implicitReturnRoot = true, bool useSpaceOverTab = true) {
	yue::YueConfig config;
	config.reserveLineNumber = reserveLineNumber;
	config.implicitReturnRoot = implicitReturnRoot;
	config.useSpaceOverTab = useSpaceOverTab;
	auto result = yue::YueCompiler{YUE_ARGS}.compile(codes, config);
	return {result.codes, result.error};
}

std::string version() { return std::string(yue::version); }

#define _DEFER(code,line) std::shared_ptr<void> _defer_##line(nullptr, [&](auto){code;})
#define DEFER(code) _DEFER(code,__LINE__)

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
_G.__output = {}
_G.print = (...)->
	len = select "#", ...
	strs = {}
	for i = 1, len
		strs[#strs + 1] = tostring select i, ...
	_G.__output[#_G.__output + 1] = table.concat strs, "\n"
	)yuescript");
	std::string res2 = execStr(codes);
	std::string res1 = execStr(R"yuescript(
res = table.concat _G.__output, "\n"
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

