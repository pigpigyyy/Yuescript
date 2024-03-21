local x
x = function()
	return print(what)
end
local _
_ = function() end
_ = function()
	return function()
		return function() end
	end
end
go(to(the(barn)))
open(function()
	return the(function()
		return door
	end)
end)
open(function()
	the(door)
	local hello
	hello = function()
		return my(func)
	end
end)
local h
h = function()
	return hi
end
eat(function() end, world);
(function() end)()
x = function(...) end
hello()
hello.world()
_ = hello().something
_ = what()["ofefe"]
what()(the()(heck()))
_ = function(a, b, c, d, e) end
_ = function(a, a, a, a, a)
	return print(a)
end
_ = function(x)
	if x == nil then
		x = 23023
	end
end
_ = function(x)
	if x == nil then
		x = function(y)
			if y == nil then
				y = function() end
			end
		end
	end
end
_ = function(x)
	if x == nil then
		if something then
			x = yeah
		else
			x = no
		end
	end
end
local something
something = function(hello, world)
	if hello == nil then
		hello = 100
	end
	if world == nil then
		world = function(x)
			if x == nil then
				x = [[yeah cool]]
			end
			return print("eat rice")
		end
	end
	return print(hello)
end
_ = function(self) end
_ = function(self, x, y) end
_ = function(self, x, y)
	self.x = x
	self.y = y
end
_ = function(self, x)
	if x == nil then
		x = 1
	end
end
_ = function(self, x, y, z)
	if x == nil then
		x = 1
	end
	if z == nil then
		z = "hello world"
	end
	self.x = x
	self.z = z
end
x(function()
	return
end)
y(function()
	return 1
end)
z(function()
	return 1, "hello", "world"
end)
k(function()
	if yes then
		return
	else
		return
	end
end)
_ = function()
	if something then
		return real_name
	end
end
d(function()
	return print("hello world")
end, 10)
d(1, 2, 3, 4, 5, 6, (function()
	if something then
		print("okay")
		return 10
	end
end)(), 10, 20)
f()()(what)(function()
	return print("srue")
end, 123)
x = function(a, b)
	return print("what")
end
local y
y = function(a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
local z
z = function(a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
local j
j = function(f, g, m, a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
y = function(a, b, ...)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
y = function(a, b, ...)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
local args
args = function(a, b)
	return print("what")
end
args = function(a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
args = function(a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
args = function(f, g, m, a, b)
	if a == nil then
		a = "hi"
	end
	if b == nil then
		b = 23
	end
	return print("what")
end
local self
self = function(n)
	if n == 0 then
		return 1
	end
	return n * self(n - 1)
end
do
	items.every(function(item)
		if item.field then
			local value = item.field.get("abc")
			if value then
				local _exp_0 = value:get()
				if 123 == _exp_0 then
					return false
				elseif 456 == _exp_0 then
					handle(item)
				end
			end
		end
		return true
	end)
	items.every(function(item)
		if item.field then
			local value = item.field.get("abc")
			if value then
				local _exp_0 = value:get()
				if 123 == _exp_0 then
					return false
				elseif 456 == _exp_0 then
					handle(item)
				end
			end
		end
		return true
	end)
	HttpServer:post("/login", function(req)
		do
			local _type_0 = type(req)
			local _tab_0 = "table" == _type_0 or "userdata" == _type_0
			if _tab_0 then
				local name = req.name
				local pwd = req.pwd
				if name ~= nil and pwd ~= nil then
					if name ~= "" then
						local user = DB:queryUser(name, pwd)
						if user then
							if user.status == "available" then
								return {
									success = true
								}
							end
						end
					end
				end
			end
		end
		return {
			success = false
		}
	end)
	local check
	check = function(num)
		return num
	end
	local func
	func = function()
		check(123)
	end
	print(func())
end
return nil
