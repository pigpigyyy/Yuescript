local luarocksKey = ...
local version
do
	local _with_0 = io.open("src/yuescript/yue_compiler.cpp")
	if _with_0 ~= nil then
		local codes = _with_0:read("*a")
		_with_0:close()
		version = codes:match("const std::string_view version = \"(.-)\"sv;")
	end
end
if not (version ~= nil) then
	print("failed to get version!")
	os.exit(1)
end
local rockspec = "rockspec_format = '3.0'\npackage = 'Yuescript'\nversion = '" .. tostring(version) .. "-1'\nsource = {\n	url = 'git+https://github.com/pigpigyyy/yuescript'\n}\ndescription = {\n	summary = 'Yuescript is a Moonscript dialect.',\n	detailed = [[\n	Yuescript is a Moonscript dialect. It is derived from Moonscript language 0.5.0 and continuously adopting new features to be more up to date. ]],\n	homepage = 'https://github.com/pigpigyyy/yuescript',\n	maintainer = 'Li Jin <dragon-fly@qq.com>',\n	labels = {'yuescript','cpp','transpiler','moonscript'},\n	license = 'MIT'\n}\ndependencies = {\n	'lua >= 5.1',\n}\nbuild = {\n	type = 'cmake',\n	variables = {\n		LUA='$(LUA)',\n		LUA_INCDIR='$(LUA_INCDIR)',\n		CMAKE_BUILD_TYPE='Release'\n	},\n	install = {\n		lib = {\n			'build.luarocks/yue.so'\n		},\n		bin = {\n			'build.luarocks/yue'\n		}\n	}\n}"
local specFile = "yuescript-" .. tostring(version) .. "-1.rockspec"
do
	local _with_0 = io.open(specFile, "w+")
	if _with_0 ~= nil then
		_with_0:write(rockspec)
		_with_0:close()
	end
end
local result = io.popen("luarocks upload --api-key " .. tostring(luarocksKey) .. " " .. tostring(specFile)):read('*a')
if not result:match("Done:") then
	print(result)
	return os.exit(1)
end
