R"moonplus_codes(
--[[
Copyright (C) 2020 by Leaf Corcoran, modified by Li Jin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.]]

local moonp = require("moonp")
local concat, insert, remove = table.concat, table.insert, table.remove
local unpack = unpack or table.unpack
local lua = {
	loadstring = loadstring,
	load = load
}
local split, get_options, create_moonpath, moon_loader, load_text, moon_call, loadstring, loadfile, dofile, insert_loader, remove_loader, moon_require, find_modulepath
moonp.dirsep = "/"
moonp.moon_compiled = { }
moonp.file_exist = function(fname)
	local file = io.open(fname)
	if file then
		file:close()
		return true
	else
		return false
	end
end
moonp.read_file = function(fname)
	local file, err = io.open(fname)
	if not file then
		return nil, err
	end
	local text = assert(file:read("*a"))
	file:close()
	return text
end
split = function(str, delim)
	if str == "" then
		return { }
	end
	str = str .. delim
	local _accum_0 = { }
	local _len_0 = 1
	for m in str:gmatch("(.-)" .. delim) do
		_accum_0[_len_0] = m
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
get_options = function(...)
	local count = select("#", ...)
	local opts = select(count, ...)
	if type(opts) == "table" then
		return opts, unpack({
			...
		}, nil, count - 1)
	else
		return { }, ...
	end
end
create_moonpath = function(package_path)
	local extension = moonp.options.extension
	local moonpaths
	do
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = split(package_path, ";")
		for _index_0 = 1, #_list_0 do
			local path = _list_0[_index_0]
			local _continue_0 = false
			repeat
				local prefix = path:match("^(.-)%.lua$")
				if not prefix then
					_continue_0 = true
					break
				end
				_accum_0[_len_0] = prefix .. "." .. extension
				_len_0 = _len_0 + 1
				_continue_0 = true
			until true
			if not _continue_0 then
				break
			end
		end
		moonpaths = _accum_0
	end
	return concat(moonpaths, ";")
end
find_modulepath = function(name)
	if not package.moonpath then
		package.moonpath = create_moonpath(package.path)
	end
	local name_path = name:gsub("%.", moonp.dirsep)
	local file_exist, file_path
	for path in package.moonpath:gmatch("[^;]+") do
		file_path = path:gsub("?", name_path)
		file_exist = moonp.file_exist(file_path)
		if file_exist then
			break
		end
	end
	if file_exist then
		return file_path
	else
		return nil
	end
end
load_text = function(name)
	local file_path = find_modulepath(name)
	if file_path then
		return moonp.read_file(file_path), file_path
	end
	return nil, nil
end
moon_loader = function(name)
	local text, file_path = load_text(name)
	if text then
		local res, err = loadstring(text, file_path)
		if not res then
			error(file_path .. ": " .. err)
		end
		return res
	end
	return nil, "Could not find moonp file"
end
moon_call = function(f, ...)
	local args = {
		...
	}
	return xpcall((function()
		return f(unpack(args))
	end), function(err)
		return moonp.stp.stacktrace(err, 1)
	end)
end
loadstring = function(...)
	local options, str, chunk_name, mode, env = get_options(...)
	chunk_name = chunk_name or "=(moonplus.loadstring)"
	local code, err = moonp.to_lua(str, options)
	if not code then
		return nil, err
	end
	if chunk_name then
		moonp.moon_compiled["@" .. chunk_name] = code
	end
	return (lua.loadstring or lua.load)(code, chunk_name, unpack({
		mode,
		env
	}))
end
loadfile = function(fname, ...)
	local text = moonp.read_file(fname)
	return loadstring(text, tostring(fname), ...)
end
dofile = function(...)
	local f = assert(loadfile(...))
	return f()
end
insert_loader = function(pos)
	if pos == nil then
		pos = 2
	end
	if not package.moonpath then
		package.moonpath = create_moonpath(package.path)
	end
	local loaders = package.loaders or package.searchers
	for _index_0 = 1, #loaders do
		local loader = loaders[_index_0]
		if loader == moon_loader then
			return false
		end
	end
	insert(loaders, pos, moon_loader)
	return true
end
remove_loader = function()
	local loaders = package.loaders or package.searchers
	for i, loader in ipairs(loaders) do
		if loader == moon_loader then
			remove(loaders, i)
			return true
		end
	end
	return false
end
moon_require = function(name)
	insert_loader()
	local success, res = xpcall((function()
		return require(name)
	end), function(err)
		local msg = moonp.stp.stacktrace(err, 1)
		print(msg)
		return msg
	end)
	if success then
		return res
	else
		return nil
	end
end
setmetatable(moonp, {
	__index = function(self, key)
		if not (key == "stp") then
			return nil
		end
		local stp = rawget(moonp, "stp")
		if not stp then
			do
				local _with_0 = moonp.load_stacktraceplus()
				_with_0.dump_locals = false
				_with_0.simplified = true
				stp = _with_0
			end
			rawset(moonp, "stp", stp)
			rawset(moonp, "load_stacktraceplus", nil)
		end
		return stp
	end,
	__call = function(self, name)
		return self.require(name)
	end
})
for k, v in pairs({
	insert_loader = insert_loader,
	remove_loader = remove_loader,
	loader = moon_loader,
	dofile = dofile,
	loadfile = loadfile,
	loadstring = loadstring,
	create_moonpath = create_moonpath,
	find_modulepath = find_modulepath,
	pcall = moon_call,
	require = moon_require
}) do
	moonp[k] = v
end
)moonplus_codes";
