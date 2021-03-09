R"yuescript_codes(
--[[
Copyright (C) 2020 by Leaf Corcoran, modified by Li Jin, 2021

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

local yue = require("yue")
local concat, insert, remove = table.concat, table.insert, table.remove
local unpack = unpack or table.unpack
local lua = {
	loadstring = loadstring,
	load = load
}
local split, get_options, create_yuepath, yue_loader, load_text, yue_call, loadstring, loadfile, dofile, insert_loader, remove_loader, yue_require, find_modulepath
yue.dirsep = "/"
yue.yue_compiled = { }
yue.file_exist = function(fname)
	local file = io.open(fname)
	if file then
		file:close()
		return true
	else
		return false
	end
end
yue.read_file = function(fname)
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
	local tokens = { }
	for m in str:gmatch("(.-)" .. delim) do
		table.insert(tokens, m)
	end
	return tokens
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
create_yuepath = function(package_path)
	local extension = yue.options.extension
	local yuepaths = { }
	local tokens = split(package_path, ";")
	for i = 1, #tokens do
		local path = tokens[i]
		local prefix = path:match("^(.-)%.lua$")
		if prefix then
			table.insert(yuepaths, prefix .. "." .. extension)
		end
	end
	return concat(yuepaths, ";")
end
find_modulepath = function(name)
	if not package.yuepath then
		package.yuepath = create_yuepath(package.path)
	end
	local name_path = name:match("[\\/]") and name or name:gsub("%.", yue.dirsep)
	local file_exist, file_path
	for path in package.yuepath:gmatch("[^;]+") do
		file_path = path:gsub("?", name_path)
		file_exist = yue.file_exist(file_path)
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
		return yue.read_file(file_path), file_path
	end
	return nil, nil
end
yue_loader = function(name)
	local text, file_path = load_text(name)
	if text then
		local res, err = loadstring(text, file_path)
		if not res then
			error(file_path .. ": " .. err)
		end
		return res
	end
	return nil, "Could not find yue file"
end
yue_call = function(f, ...)
	local args = {
		...
	}
	return xpcall((function()
		return f(unpack(args))
	end), function(err)
		return yue.stp.stacktrace(err, 1)
	end)
end
loadstring = function(...)
	local options, str, chunk_name, mode, env = get_options(...)
	chunk_name = chunk_name or "=(yuescript.loadstring)"
	local code, err = yue.to_lua(str, options)
	if not code then
		return nil, err
	end
	if chunk_name then
		yue.yue_compiled["@" .. chunk_name] = code
	end
	return (lua.loadstring or lua.load)(code, chunk_name, unpack({
		mode,
		env
	}))
end
loadfile = function(fname, ...)
	local text = yue.read_file(fname)
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
	if not package.yuepath then
		package.yuepath = create_yuepath(package.path)
	end
	local loaders = package.loaders or package.searchers
	for i = 1, #loaders do
		local loader = loaders[i]
		if loader == yue_loader then
			return false
		end
	end
	insert(loaders, pos, yue_loader)
	return true
end
remove_loader = function()
	local loaders = package.loaders or package.searchers
	for i, loader in ipairs(loaders) do
		if loader == yue_loader then
			remove(loaders, i)
			return true
		end
	end
	return false
end
yue_require = function(name)
	insert_loader()
	local success, res = xpcall((function()
		return require(name)
	end), function(err)
		local msg = yue.stp.stacktrace(err, 1)
		print(msg)
		return msg
	end)
	if success then
		return res
	else
		return nil
	end
end
setmetatable(yue, {
	__index = function(self, key)
		if not (key == "stp") then
			return nil
		end
		local stp = rawget(yue, "stp")
		if not stp then
			stp = yue.load_stacktraceplus()
			stp.dump_locals = false
			stp.simplified = true
			rawset(yue, "stp", stp)
			rawset(yue, "load_stacktraceplus", nil)
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
	loader = yue_loader,
	dofile = dofile,
	loadfile = loadfile,
	loadstring = loadstring,
	create_yuepath = create_yuepath,
	find_modulepath = find_modulepath,
	pcall = yue_call,
	require = yue_require
}) do
	yue[k] = v
end
)yuescript_codes";
