local a = setmetatable({
	close = true,
}, {
	__close = function(self)
		return print("out of scope")
	end
})
local b = setmetatable({ }, {
	__add = function(left, right)
		return right - left
	end
})
local c = setmetatable({
	key1 = true,
	key2 = true
}, {
	__add = add
})
local w = setmetatable({ }, {
	[name] = 123,
	["new"] = function(self, val)
		return {
			val
		}
	end
})
getmetatable(w)["new"](getmetatable(w)[name])
do
	local _ <close> = setmetatable({ }, {
		__close = function()
			return print("out of scope")
		end
	})
end
local d, e = a.close, getmetatable(a).__close
local f = getmetatable(a):__close(1)
getmetatable(a).__add = function(x, y)
	return x + y
end
do
	local new = a.new
	local close, closeA
	do
		local _obj_0 = getmetatable(a)
		close, closeA = _obj_0.__close, _obj_0.__close
	end
	print(new, close, closeA)
end
do
	local x, new, var, close, closeA, num, add, sub
	x = 123
	local _obj_0 = a.b.c
	new, var = _obj_0.new, _obj_0.var
	local _obj_1 = getmetatable(_obj_0)
	close, closeA = _obj_1.__close, _obj_1.__close
	_obj_1 = func()
	num = _obj_1.num
	local _obj_2 = getmetatable(_obj_1)
	add, sub = _obj_2.__add, _obj_2.__sub
end
x.abc = 123
setmetatable(a.b, { })
setmetatable(func(), mt)
a = 1
setmetatable(b.c, mt)
d, e = "abc", nil
local is_same = getmetatable(a).__index == getmetatable(a).__index
setmetatable(a, {
	__index = tb
})
getmetatable(a).__index = tb
getmetatable(a).__index = tb
local mt = getmetatable(a)
tb:func(#list)
do
	local _obj_0 = getmetatable(tb)
	_obj_0["func"](_obj_0, list)
end
do
	local _obj_0 = getmetatable(tb)
	_obj_0["func"](_obj_0, list)
end
local index, setFunc
do
	local _obj_0 = getmetatable(require("module"))
	index, setFunc = _obj_0.__index, _obj_0.__newindex
end
do
	local _with_0 = tb
	print(getmetatable(_with_0).__add, getmetatable(_with_0.x):__index("key"))
	do
		local _obj_0 = getmetatable(getmetatable(getmetatable(_with_0).__index).__add)
		a = _obj_0["new"](_obj_0, 123)
	end
	b = t(#getmetatable(_with_0).__close.test)
	c = t(#getmetatable(_with_0).__close(_with_0.test))
end
mt = getmetatable(a)
a = setmetatable({ }, mt)
a = setmetatable({ }, {
	__index = mt
})
local index
index = getmetatable(a).__index
index = getmetatable(a).__index
do
	local ctor, update
	local _obj_0 = getmetatable(a)
	ctor, update = _obj_0.new, _obj_0.update
end
do
	local ctor, update
	local _obj_0 = getmetatable(a)
	ctor, update = _obj_0.new, _obj_0.update
end
local tb = { }
do
	do
		local _obj_0 = getmetatable(tb)
		f = _obj_0["value" .. tostring(x < y)](_obj_0, 123, ...)
	end
	f((function(...)
		local _obj_0 = getmetatable(tb)
		return _obj_0['value'](_obj_0, 123, ...)
	end)(...))
	do
		local _obj_0 = getmetatable(tb)
		_obj_0[ [[		value
		1
	]]](_obj_0, 123, ...)
	end
	local _obj_0 = getmetatable(tb)
	return _obj_0["value" .. tostring(x > y)](_obj_0, 123, ...)
end
do
	do
		local _obj_0 = getmetatable(tb)
		f = _obj_0['value'](_obj_0, 123, ...)
	end
	f((function(...)
		local _obj_0 = getmetatable(tb)
		return _obj_0['value'](_obj_0, 123, ...)
	end)(...))
	do
		local _obj_0 = getmetatable(tb)
		_obj_0['value'](_obj_0, 123, ...)
	end
	local _obj_0 = getmetatable(tb)
	return _obj_0['value'](_obj_0, 123, ...)
end
do
	f = getmetatable(tb)["value"](123, ...)
	f = getmetatable(tb)["value" .. tostring(x < y)](123, ...)
	f(getmetatable(tb)['value'](123, ...))
	getmetatable(tb)[ [[ value
1]]](123, ...)
	return getmetatable(tb)["value" .. tostring(x > y)](123, ...)
end
return nil
