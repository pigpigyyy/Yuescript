xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(err)
end)
xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(err)
end)
pcall(function()
	print("trying")
	return func(1, 2, 3)
end)
do
	local success, result = xpcall(function()
		return func(1, 2, 3)
	end, function(err)
		return print(err)
	end)
	success, result = pcall(func, 1, 2, 3)
end
pcall(tb.func)
pcall(tb.func)
pcall(tb.func)
pcall((tb.func))
pcall(((function()
	local _base_0 = tb
	local _fn_0 = _base_0.func
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)()), 1, 2, 3)
pcall(tb.func, 1)
pcall(tb.func, 1)
if (xpcall(function()
	return func(1)
end, function(err)
	return print(err)
end)) then
	print("OK")
end
if xpcall(function()
	return func(1)
end, function(err)
	return print(err)
end) then
	print("OK")
end
do
	do
		local success, result = pcall(func, "abc", 123)
		if success then
			print(result)
		end
	end
	local success, result = xpcall(function()
		return func("abc", 123)
	end, function(err)
		return print(err)
	end)
	success, result = xpcall(function()
		return func("abc", 123)
	end, function(err)
		return print(err)
	end)
	if success then
		print(result)
	end
end
return nil
