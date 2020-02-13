rockspec_format = "3.0"
package = "MoonPlus"
version = "dev-1"
source = {
	url = "git+https://github.com/pigpigyyy/MoonPlus"
}
description = {
	summary = "MoonPlus is a compiler for Moonscript written in C++.",
	detailed = [[
	MoonPlus is a compiler with features from Moonscript language 0.5.0 and implementing new features to make Moonscript more up to date. ]],
	homepage = "https://github.com/pigpigyyy/MoonPlus",
	maintainer = "Li Jin <dragon-fly@qq.com>",
	labels = {"moonscript","cpp","transpiler"},
	license = "MIT"
}
dependencies = {
	"lua >= 5.1",
}
build = {
	type = "cmake",
	variables = {
		LUA_INCDIR="$(LUA_INCDIR)",
		LUA="$(LUA)",
	},
	install = {
		lib = {
			"build.luarocks/moonp.so"
		}
	}
}
