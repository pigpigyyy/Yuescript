R"moonscript_codes(
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

import "moonp"
import concat, insert, remove from table
unpack = unpack or table.unpack
lua = :loadstring, :load

local *

dirsep = "/"

moonp.moon_compiled = {}

split = (str, delim) ->
	return {} if str == ""
	str ..= delim
	[m for m in str\gmatch("(.-)"..delim)]

get_options = (...) ->
	count = select "#", ...
	opts = select count, ...
	if type(opts) == "table"
		opts, unpack {...}, nil, count - 1
	else
		{}, ...

-- create moon path package from lua package path
create_moonpath = (package_path) ->
	moonpaths = for path in *split package_path, ";"
		prefix = path\match "^(.-)%.lua$"
		continue unless prefix
		prefix .. ".moon"
	concat moonpaths, ";"

moon_loader = (name) ->
	name_path = name\gsub "%.", dirsep

	local file, file_path
	for path in package.moonpath\gmatch "[^;]+"
		file_path = path\gsub "?", name_path
		file = io.open file_path
		break if file

	if file
		text = file\read "*a"
		file\close!
		res, err = loadstring text, "@#{file_path}"
		if not res
				error file_path .. ": " .. err

		return res

	return nil, "Could not find moon file"


loadstring = (...) ->
	options, str, chunk_name, mode, env = get_options ...
	chunk_name or= "=(moonscript.loadstring)"

	code, err = moonp.to_lua str, options
	unless code
		return nil, err

	moonp.moon_compiled[chunk_name] = code if chunk_name
	-- the unpack prevents us from passing nil
	(lua.loadstring or lua.load) code, chunk_name, unpack { mode, env }

loadfile = (fname, ...) ->
	file, err = io.open fname
	return nil, err unless file
	text = assert file\read "*a"
	file\close!
	loadstring text, "@#{fname}", ...

-- throws errros
dofile = (...) ->
	f = assert loadfile ...
	f!

insert_loader = (pos=2) ->
	if not package.moonpath
		package.moonpath = create_moonpath package.path

	loaders = package.loaders or package.searchers
	for loader in *loaders
		return false if loader == moon_loader

	insert loaders, pos, moon_loader
	true

remove_loader = ->
	loaders = package.loaders or package.searchers

	for i, loader in ipairs loaders
		if loader == moon_loader
			remove loaders, i
			return true

	false

moon_require = (name)->
	insert_loader!
	xpcall (-> require name), (err)->
		msg = moonp.stp.stacktrace err, 2
		print msg

setmetatable moonp, {
	__index: (key)=>
		return nil unless key == "stp"
		stp = rawget moonp,"stp"
		unless stp
			stp = with moonp.load_stacktraceplus!
				.dump_locals = false
				.simplified = true
			rawset moonp,"stp",stp
			rawset moonp,"load_stacktraceplus",nil
		stp
	__call: (name)=> @.require name
}

moonp[k] = v for k,v in pairs {
	:insert_loader, :remove_loader, :moon_loader, :dirsep,
	:dofile, :loadfile, :loadstring, :create_moonpath,
	require:moon_require
}
)moonscript_codes";
