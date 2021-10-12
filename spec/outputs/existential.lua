if f1 ~= nil then
	f1()
end
if f2 ~= nil then
	f2("arg0", 123)
end
local x
if tab ~= nil then
	x = tab.value
end
print((function()
	if abc ~= nil then
		local _obj_0 = abc["hello world"]
		if _obj_0 ~= nil then
			return _obj_0.xyz
		end
		return nil
	end
	return nil
end)())
if print and (x ~= nil) then
	print(x)
end
if self ~= nil then
	self:func(998)
end
do
	local _with_0
	if abc ~= nil then
		local _obj_0 = abc()
		local _obj_1 = _obj_0.func
		if _obj_1 ~= nil then
			_with_0 = _obj_1(_obj_0)
		end
	end
	if (function()
		local _obj_0 = _with_0.p
		if _obj_0 ~= nil then
			return _obj_0(_with_0, "abc")
		end
		return nil
	end)() then
		return 123
	end
end
do
	local _des_0
	if a ~= nil then
		local _obj_0 = a["if"]
		if _obj_0 ~= nil then
			local _obj_1 = _obj_0["then"]
			if _obj_1 ~= nil then
				local _obj_2 = _obj_1(_obj_0, 123)
				if _obj_2 ~= nil then
					_des_0 = _obj_2((function()
						if self ~= nil then
							return self["function"](self, 998)
						end
						return nil
					end)())
				end
			end
		end
	end
	if _des_0 then
		x = _des_0.x
		print(x)
	end
end
local res = ((function()
	local _call_0 = b["function"]
	local _obj_0 = _call_0["do"](_call_0)
	local _obj_1 = _obj_0["while"]
	if _obj_1 ~= nil then
		local _call_1 = _obj_1(_obj_0, "OK")
		local _base_0 = _call_1["if"](_call_1, "def", 998)
		local _fn_0 = _base_0.f
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)() ~= nil)
print(res)
local solipsism
if (mind ~= nil) and not (world ~= nil) then
	solipsism = true
end
local speed = 0
speed = speed or 15
local footprints = yeti or "bear"
local major = 'Computer Science'
if not (major ~= nil) then
	signUpForClass('Introduction to Wines')
end
if (window ~= nil) then
	local environment = 'browser (probably)'
end
local zip
local _obj_0 = lottery.drawWinner
if _obj_0 ~= nil then
	local _obj_1 = _obj_0().address
	if _obj_1 ~= nil then
		zip = _obj_1.zipcode
	end
end
local len = (function()
	if utf8 ~= nil then
		return utf8.len
	end
	return nil
end)() or (function()
	if string ~= nil then
		return string.len
	end
	return nil
end)() or function(o)
	return #o
end
local a
if tb1 ~= nil then
	local _obj_1 = tb1["end"]
	if _obj_1 ~= nil then
		a = _obj_1(tb1, 123 + (function()
			if tb2 ~= nil then
				return tb2["then"](tb2, 456)
			end
			return nil
		end)())
	end
end
local b = ((function()
	if tb1 ~= nil then
		local _base_0 = tb1
		local _fn_0 = _base_0["end"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)() ~= nil) or (function()
	if tb2 ~= nil then
		local _base_0 = tb2
		local _fn_0 = _base_0["then"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)()
local _with_0 = io.open("test.txt", "w")
if _with_0 ~= nil then
	_with_0:write("hello")
	_with_0:close()
end
return _with_0
