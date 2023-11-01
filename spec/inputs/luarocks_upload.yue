luarocksKey = ...
local version
with? io.open "src/yuescript/yue_compiler.cpp"
	codes = \read "*a"
	\close!
	version = codes\match "const std::string_view version = \"(.-)\"sv;"
unless version?
	print "failed to get version!"
	os.exit 1

rockspec = "rockspec_format = '3.0'
package = 'Yuescript'
version = '#{version}-1'
source = {
	url = 'git+https://github.com/pigpigyyy/yuescript'
}
description = {
	summary = 'Yuescript is a Moonscript dialect.',
	detailed = [[
	Yuescript is a Moonscript dialect. It is derived from Moonscript language 0.5.0 and continuously adopting new features to be more up to date. ]],
	homepage = 'https://github.com/pigpigyyy/yuescript',
	maintainer = 'Li Jin <dragon-fly@qq.com>',
	labels = {'yuescript','cpp','transpiler','moonscript'},
	license = 'MIT'
}
dependencies = {
	'lua >= 5.1',
}
build = {
	type = 'cmake',
	variables = {
		LUA='$(LUA)',
		LUA_INCDIR='$(LUA_INCDIR)',
		CMAKE_BUILD_TYPE='Release'
	},
	install = {
		lib = {
			'build.luarocks/yue.so'
		},
		bin = {
			'build.luarocks/yue'
		}
	}
}"

specFile = "yuescript-#{version}-1.rockspec"
with? io.open specFile, "w+"
	\write rockspec
	\close!

result = io.popen("luarocks upload --api-key #{luarocksKey} #{specFile}")\read '*a'
unless result\match "Done:"
	print result
	os.exit 1

