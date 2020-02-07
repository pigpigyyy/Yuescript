package = "MoonPlus"
version = "0.1-1"
source = {
	url = "git+https://github.com/pigpigyyy/MoonPlus"
}
description = {
	summary = "MoonPlus is a compiler for Moonscript written in C++.",
	detailed = [[
	MoonPlus is a compiler with features from Moonscript language 0.5.0 and more advanced features. could be faster than the original Moonscript compiler. ]],
	homepage = "https://github.com/pigpigyyy/MoonPlus",
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
