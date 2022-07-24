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
	local _obj_0 = a
	local new = _obj_0.new
	local close, closeA
	do
		local _obj_1 = getmetatable(_obj_0)
		close, closeA = _obj_1.__close, _obj_1.__close
	end
	print(new, close, closeA)
end
do
	local x, new, var, close, closeA, num, add, sub
	local _obj_0, _obj_1
	x, _obj_0, _obj_1 = 123, a.b.c, func()
	new, var = _obj_0.new, _obj_0.var
	do
		local _obj_2 = getmetatable(_obj_0)
		close, closeA = _obj_2.__close, _obj_2.__close
	end
	num = _obj_1.num
	do
		local _obj_2 = getmetatable(_obj_1)
		add, sub = _obj_2.__add, _obj_2.__sub
	end
end
setmetatable(a.b, { })
x.abc = 123
setmetatable(func(), mt)
setmetatable(b.c, mt)
a, d, e = 1, "abc"
local is_same = getmetatable(a).__index == getmetatable(a).__index
setmetatable(a, {
	__index = tb
})
getmetatable(a).__index = tb
getmetatable(a).__index = tb
local mt = getmetatable(a)
tb:func(#list)
getmetatable(tb):__func(list)
getmetatable(tb):__func(list)
local index, setFunc
do
	local _obj_0 = getmetatable(require("module"))
	index, setFunc = _obj_0.__index, _obj_0.__newindex
end
do
	local _with_0 = tb
	print(getmetatable(_with_0).__add, getmetatable(_with_0.x):__index("key"))
	a = getmetatable(getmetatable(getmetatable(_with_0).__index).__add):__new(123)
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
	do
		local _obj_0 = getmetatable(a)
		ctor, update = _obj_0.new, _obj_0.update
	end
end
do
	local ctor, update
	do
		local _obj_0 = getmetatable(a)
		ctor, update = _obj_0.new, _obj_0.update
	end
end
return nil
