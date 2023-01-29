rockspec_format = "3.0"
package = "Yuescript"
version = "dev-1"
source = {
	url = "git+https://github.com/pigpigyyy/yuescript"
}
description = {
	summary = "Yuescript is a Moonscript dialect.",
	detailed = [[
	Yuescript is a Moonscript dialect. It is derived from Moonscript language 0.5.0 and continuously adopting new features to be more up to date. ]],
	homepage = "https://github.com/pigpigyyy/yuescript",
	maintainer = "Li Jin <dragon-fly@qq.com>",
	labels = {"yuescript","cpp","transpiler","moonscript"},
	license = "MIT"
}
dependencies = {
	"lua >= 5.1",
}
build = {
	type = "cmake",
	variables = {
		LUA_INCDIR = "$(LUA_INCDIR)",
		LUA = "$(LUA)",
		CMAKE_BUILD_TYPE="Release"
	},
	install = {
		lib = {
			"build.luarocks/yue.so"
		},
		bin = {
			"build.luarocks/yue"
		}
	}
}
