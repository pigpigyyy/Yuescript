R"yuescript_codes(
--[[
Copyright (C) 2020 by Leaf Corcoran, modified by Li Jin, 2022

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

local yue = select(1, ...)
local concat, insert = table.concat, table.insert
local unpack = unpack or table.unpack
yue.yue_compiled = { }
if not (lovr and lovr.filesystem) then
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
else
	yue.file_exist = lovr.filesystem.isFile
	yue.read_file  = function(fname)
		contents, bytes = lovr.filesystem.read(fname)
		if contents == nil then
			return nil, 'File not found.'
		end
		return contents
	end
end
local function get_options(...)
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
local function find_modulepath(name)
	local suffix = "." .. yue.options.extension
	local dirsep = yue.options.dirsep
	local name_path = name:match("[\\/]") and name or name:gsub("%.", dirsep)
	local file_exist, file_path
	local tried = {}
	local paths = {package.path, yue.options.path}
	for i = 1, #paths do
		local yue_path = paths[i]
		for path in yue_path:gmatch("[^;]+") do
			file_path = path:gsub("?", name_path):gsub("%.lua$", suffix)
			file_exist = yue.file_exist(file_path)
			if file_exist then
				break
			else
				tried[#tried + 1] = file_path
			end
		end
	end
	if file_exist then
		return file_path
	else
		return nil, tried
	end
end
local yue_loadstring
local function yue_loader(name)
	local file_path, tried = yue.find_modulepath(name)
	if file_path then
		local text = yue.read_file(file_path)
		if text then
			local res, err = yue_loadstring(text, file_path)
			if not res then
				error(file_path .. ": " .. err)
			end
			return res
		else
			return "no file '" .. file_path .. "'"
		end
	end
	for i = 1, #tried do
		tried[i] = "no file '" .. tried[i] .. "'"
	end
	return concat(tried, "\n\t")
end
local function yue_call(f, ...)
	local args = {
		...
	}
	return xpcall((function()
		return f(unpack(args))
	end), function(err)
		return yue.traceback(err, 1)
	end)
end
yue_loadstring = function(...)
	local options, str, chunk_name, mode, env = get_options(...)
	chunk_name = chunk_name or "=(yuescript.loadstring)"
	options.module = chunk_name
	local code, err = yue.to_lua(str, options)
	if not code then
		return nil, err
	end
	if chunk_name then
		yue.yue_compiled["@" .. chunk_name] = code
	end
	return (loadstring or load)(code, chunk_name, unpack({
		mode,
		env
	}))
end
local function yue_loadfile(fname, ...)
	local text = yue.read_file(fname)
	return yue_loadstring(text, fname, ...)
end
local function yue_dofile(...)
	local f = assert(yue_loadfile(...))
	return f()
end
local function insert_loader(pos)
	if pos == nil then
		pos = 3
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
yue.options.dump_locals = false
yue.options.simplified = true
local load_stacktraceplus = yue.load_stacktraceplus
yue.load_stacktraceplus = nil
local stp
local function yue_traceback(err, level)
	if not stp then
		stp = load_stacktraceplus()
	end
	stp.dump_locals = yue.options.dump_locals
	stp.simplified = yue.options.simplified
	return stp.stacktrace(err, level)
end
local function yue_require(name)
	insert_loader()
	local success, res = xpcall(function()
		return require(name)
	end, function(err)
		return yue_traceback(err, 2)
	end)
	if success then
		return res
	else
		print(res)
		return nil
	end
end
setmetatable(yue, {
	__call = function(self, name)
		return self.require(name)
	end
})
local function dump(what)
	local seen = { }
	local _dump
	_dump = function(what, depth)
		depth = depth or 0
		local t = type(what)
		if "string" == t then
			return "\"" .. tostring(what) .. "\"\n"
		elseif "table" == t then
			if seen[what] then
				return "recursion(" .. tostring(what) .. ")...\n"
			end
			seen[what] = true
			depth = depth + 1
			local lines = {}
			for k, v in pairs(what) do
				insert(lines, ('\t'):rep(depth) .. "[" .. tostring(k) .. "] = " .. _dump(v, depth))
			end
			seen[what] = false
			return "{\n" .. concat(lines) .. ('\t'):rep(depth - 1) .. "}\n"
		else
			return tostring(what) .. "\n"
		end
	end
	return _dump(what)
end
local function p(...)
	local args = {...}
	for i = 1, #args do
		args[i] = dump(args[i])
	end
	print(concat(args))
end
yue.find_modulepath = find_modulepath
yue.insert_loader = insert_loader
yue.dofile = yue_dofile
yue.loadfile = yue_loadfile
yue.loadstring = yue_loadstring
yue.pcall = yue_call
yue.require = yue_require
yue.p = p
yue.traceback = yue_traceback
)yuescript_codes";
