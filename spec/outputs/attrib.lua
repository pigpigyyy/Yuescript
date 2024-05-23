local a <const>, b <const>, c <const>, d <const> = 1, 2, 3, 4
do
	local a, b = setmetatable({ }, {
		__close = function(self)
			return print("closed")
		end
	})
	local _close_0 <close> = a
	local _close_1 <close> = b
	local c <const>, d <const> = 123, 'abc'
	close(a, b)
	const(c, d)
end
do
	local a <const> = f()
	local b, c, d
	local _obj_0, _obj_1 = f1()
	b, c = _obj_0[1], _obj_0[2]
	d = _obj_1[1]
end
do
	local a, b, c, d
	local _obj_0, _obj_1, _obj_2 = f()
	a = _obj_0
	b, c = _obj_1[1], _obj_1[2]
	d = _obj_2[1]
end
do
	local a, b
	local _obj_0 = {
		2,
		3
	}
	a, b = _obj_0[1], _obj_0[2]
end
do
	local v
	if flag then
		v = func()
	else
		v = setmetatable({ }, {
			__close = function(self) end
		})
	end
	local _close_0 <close> = v
	local f
	local _with_0 = io.open("file.txt")
	_with_0:write("Hello")
	f = _with_0
	local _close_1 <close> = f
end
do
	local a
	if true then
		a = 1
	end
	local b
	if not false then
		if x then
			b = 1
		end
	end
	local _close_0 <close> = b
	local c
	if true then
		local _exp_0 = x
		if "abc" == _exp_0 then
			c = 998
		end
	end
	local d
	if (function()
		if a ~= nil then
			return a
		else
			return b
		end
	end)() then
		d = {
			value = value
		}
	end
	local _close_1 <close> = d
end
do
	local _
	do
		local _with_0 = io.open("file.txt")
		_with_0:write("Hello")
		_ = _with_0
	end
	local _close_0 <close> = _
	local _ <close> = setmetatable({ }, {
		__close = function()
			return print("second")
		end
	})
	local _ <close> = setmetatable({ }, {
		__close = function()
			return print("first")
		end
	})
	print("third")
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
	local _ <close> = def(function()
		return print(3)
	end)
	local _ <close> = def(function()
		return print(2)
	end)
	local _ <close> = def(function()
		return print(1)
	end)
end
