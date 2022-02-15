do
	local _obj_0 = f1
	if _obj_0 ~= nil then
		_obj_0()
	end
end
do
	local _obj_0 = f2
	if _obj_0 ~= nil then
		_obj_0("arg0", 123)
	end
end
local x
do
	local _obj_0 = tab
	if _obj_0 ~= nil then
		x = _obj_0.value
	end
end
print((function()
	local _obj_0 = abc
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0["hello world"]
		if _obj_1 ~= nil then
			return _obj_1.xyz
		end
		return nil
	end
	return nil
end)())
if print and (x ~= nil) then
	print(x)
end
do
	local _obj_0 = self
	if _obj_0 ~= nil then
		_obj_0:func(998)
	end
end
do
	local _with_0
	do
		local _obj_0 = abc
		if _obj_0 ~= nil then
			do
				local _obj_1 = _obj_0()
				local _obj_2 = _obj_1.func
				if _obj_2 ~= nil then
					_with_0 = _obj_2(_obj_1)
				end
			end
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
	do
		local _obj_0 = a
		if _obj_0 ~= nil then
			do
				local _obj_1 = _obj_0["if"]
				if _obj_1 ~= nil then
					do
						local _obj_2 = _obj_1["then"]
						if _obj_2 ~= nil then
							do
								local _obj_3 = _obj_2(_obj_1, 123)
								if _obj_3 ~= nil then
									_des_0 = _obj_3((function()
										local _obj_4 = self
										if _obj_4 ~= nil then
											return _obj_4["function"](_obj_4, 998)
										end
										return nil
									end)())
								end
							end
						end
					end
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
do
	local _obj_0 = lottery.drawWinner
	if _obj_0 ~= nil then
		do
			local _obj_1 = _obj_0().address
			if _obj_1 ~= nil then
				zip = _obj_1.zipcode
			end
		end
	end
end
local len = (function()
	local _obj_0 = utf8
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or (function()
	local _obj_0 = string
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or function(o)
	return #o
end
local a
do
	local _obj_0 = tb1
	if _obj_0 ~= nil then
		do
			local _obj_1 = _obj_0["end"]
			if _obj_1 ~= nil then
				a = _obj_1(_obj_0, 123 + (function()
					local _obj_2 = tb2
					if _obj_2 ~= nil then
						return _obj_2["then"](_obj_2, 456)
					end
					return nil
				end)())
			end
		end
	end
end
local b = ((function()
	local _obj_0 = tb1
	if _obj_0 ~= nil then
		local _base_0 = _obj_0
		local _fn_0 = _base_0["end"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)() ~= nil) or (function()
	local _obj_0 = tb2
	if _obj_0 ~= nil then
		local _base_0 = _obj_0
		local _fn_0 = _base_0["then"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)()
do
	local _with_0 = io.open("test.txt", "w")
	if _with_0 ~= nil then
		_with_0:write("hello")
		_with_0:close()
	end
end
do
	local _obj_0 = tb
	if _obj_0 ~= nil then
		do
			local _obj_1 = getmetatable(_obj_0).__a
			if _obj_1 ~= nil then
				_obj_1(123)
			end
		end
	end
end
do
	local _with_0
	do
		local _obj_0 = getmetatable(tb)
		if _obj_0 ~= nil then
			_with_0 = getmetatable(_obj_0).__index
		end
	end
	if _with_0 ~= nil then
		_with_0.a = 1
	end
end
return nil
