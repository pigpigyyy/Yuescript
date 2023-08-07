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
	end)(pcall(function(...)
		local c, d = 123, 'abc'
		close(a, b)
		return const(c, d)
	end, ...))
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
	end)(pcall(function(...)
		local f = (function()
			local _with_0 = io.open("file.txt")
			_with_0:write("Hello")
			return _with_0
		end)()
		local _close_1 = assert(getmetatable(f).__close)
		return (function(_arg_0, ...)
			local _ok_0 = _arg_0
			_close_1(f)
			if _ok_0 then
				return ...
			else
				return error(...)
			end
		end)(pcall(function(...) end, ...))
	end, ...))
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
	end)(pcall(function(...)
		local c = (function()
			if true then
				local _exp_0 = x
				if "abc" == _exp_0 then
					return 998
				end
			end
		end)()
		local d = (function()
			if (function()
				if a ~= nil then
					return a
				else
					return b
				end
			end)() then
				return {
					value = value
				}
			end
		end)()
		local _close_1 = assert(getmetatable(d).__close)
		return (function(_arg_0, ...)
			local _ok_0 = _arg_0
			_close_1(d)
			if _ok_0 then
				return ...
			else
				return error(...)
			end
		end)(pcall(function(...) end, ...))
	end, ...))
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
	end)(pcall(function(...)
		local _ = setmetatable({ }, {
			__close = function()
				return print("second")
			end
		})
		local _close_1 = assert(getmetatable(_).__close)
		return (function(_arg_0, ...)
			local _ok_0 = _arg_0
			_close_1(_)
			if _ok_0 then
				return ...
			else
				return error(...)
			end
		end)(pcall(function(...)
			local _ = setmetatable({ }, {
				__close = function()
					return print("first")
				end
			})
			local _close_2 = assert(getmetatable(_).__close)
			return (function(_arg_0, ...)
				local _ok_0 = _arg_0
				_close_2(_)
				if _ok_0 then
					return ...
				else
					return error(...)
				end
			end)(pcall(function(...) end, ...))
		end, ...))
	end, ...))
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
	end)(pcall(function(...)
		local _ = def(function()
			return print(2)
		end)
		local _close_1 = assert(getmetatable(_).__close)
		return (function(_arg_0, ...)
			local _ok_0 = _arg_0
			_close_1(_)
			if _ok_0 then
				return ...
			else
				return error(...)
			end
		end)(pcall(function(...)
			local _ = def(function()
				return print(1)
			end)
			local _close_2 = assert(getmetatable(_).__close)
			return (function(_arg_0, ...)
				local _ok_0 = _arg_0
				_close_2(_)
				if _ok_0 then
					return ...
				else
					return error(...)
				end
			end)(pcall(function(...) end, ...))
		end, ...))
	end, ...))
end
