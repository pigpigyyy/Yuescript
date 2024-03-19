local a, b, c, d = 1, 2, 3, 4
do
	local a, b = setmetatable({ }, {
		__close = function(self)
			return print("closed")
		end
	})
	local _close_0 = assert(getmetatable(a).__close)
	local _close_1 = assert(getmetatable(b).__close);
	(function(_arg_0, ...)
		local _ok_0 = _arg_0
		_close_1(b)
		_close_0(a)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end)(pcall(function()
		local c, d = 123, 'abc'
		close(a, b)
		return const(c, d)
	end))
end
do
	local a = f()
	local b, c, d
	do
		local _obj_0, _obj_1 = f1()
		b, c = _obj_0[1], _obj_0[2]
		d = _obj_1[1]
	end
end
do
	local a, b, c, d
	do
		local _obj_0, _obj_1, _obj_2 = f()
		a = _obj_0
		b, c = _obj_1[1], _obj_1[2]
		d = _obj_2[1]
	end
end
do
	local a, b
	do
		local _obj_0 = {
			2,
			3
		}
		a, b = _obj_0[1], _obj_0[2]
	end
end
local _anon_func_0 = function(_close_1, error, f, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_1(f)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
local _anon_func_1 = function(io)
	local _with_0 = io.open("file.txt")
	_with_0:write("Hello")
	return _with_0
end
local _anon_func_2 = function()
	do
	end
end
do
	local v = (function()
		if flag then
			return func()
		else
			return setmetatable({ }, {
				__close = function(self) end
			})
		end
	end)()
	local _close_0 = assert(getmetatable(v).__close);
	(function(_arg_0, ...)
		local _ok_0 = _arg_0
		_close_0(v)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end)(pcall(function()
		local f = _anon_func_1(io)
		local _close_1 = assert(getmetatable(f).__close)
		return _anon_func_0(_close_1, error, f, pcall(_anon_func_2))
	end))
end
local _anon_func_3 = function(_close_1, d, error, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_1(d)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
local _anon_func_4 = function(x)
	if true then
		if "abc" == x then
			return 998
		end
	end
end
local _anon_func_6 = function(a, b)
	if a ~= nil then
		return a
	else
		return b
	end
end
local _anon_func_5 = function(a, b)
	if _anon_func_6(a, b) then
		return {
			value = value
		}
	end
end
local _anon_func_7 = function()
	do
	end
end
do
	local a = (function()
		if true then
			return 1
		end
	end)()
	local b = (function()
		if not false then
			if x then
				return 1
			end
		end
	end)()
	local _close_0 = assert(getmetatable(b).__close);
	(function(_arg_0, ...)
		local _ok_0 = _arg_0
		_close_0(b)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end)(pcall(function()
		local c = _anon_func_4(x)
		local d = _anon_func_5(a, b)
		local _close_1 = assert(getmetatable(d).__close)
		return _anon_func_3(_close_1, d, error, pcall(_anon_func_7))
	end))
end
local _anon_func_8 = function(_, _close_1, error, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_1(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
local _anon_func_9 = function(_, _close_2, error, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_2(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
do
	local _ = (function()
		local _with_0 = io.open("file.txt")
		_with_0:write("Hello")
		return _with_0
	end)()
	local _close_0 = assert(getmetatable(_).__close);
	(function(_arg_0, ...)
		local _ok_0 = _arg_0
		_close_0(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end)(pcall(function()
		local _ = setmetatable({ }, {
			__close = function()
				return print("second")
			end
		})
		local _close_1 = assert(getmetatable(_).__close)
		return _anon_func_8(_, _close_1, error, pcall(function()
			local _ = setmetatable({ }, {
				__close = function()
					return print("first")
				end
			})
			local _close_2 = assert(getmetatable(_).__close)
			return _anon_func_9(_, _close_2, error, pcall(print, "third"))
		end))
	end))
end
local _defers = setmetatable({ }, {
	__close = function(self)
		self[#self]()
		self[#self] = nil
	end
})
local def
def = function(item)
	_defers[#_defers + 1] = item
	return _defers
end
local _anon_func_10 = function(_, _close_1, error, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_1(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
local _anon_func_11 = function(_, _close_2, error, _arg_0, ...)
	do
		local _ok_0 = _arg_0
		_close_2(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end
end
local _anon_func_12 = function()
	do
	end
end
do
	local _ = def(function()
		return print(3)
	end)
	local _close_0 = assert(getmetatable(_).__close)
	return (function(_arg_0, ...)
		local _ok_0 = _arg_0
		_close_0(_)
		if _ok_0 then
			return ...
		else
			return error(...)
		end
	end)(pcall(function()
		local _ = def(function()
			return print(2)
		end)
		local _close_1 = assert(getmetatable(_).__close)
		return _anon_func_10(_, _close_1, error, pcall(function()
			local _ = def(function()
				return print(1)
			end)
			local _close_2 = assert(getmetatable(_).__close)
			return _anon_func_11(_, _close_2, error, pcall(_anon_func_12))
		end))
	end))
end
