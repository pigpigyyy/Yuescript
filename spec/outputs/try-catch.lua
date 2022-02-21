xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(err)
end)
xpcall(func, function(err)
	return print(err)
end, 1, 2, 3)
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
pcall((tb.func()))
pcall(tb.func, 1)
pcall(tb.func, 1)
if (xpcall(func, function(err)
	return print(err)
end, 1)) then
	print("OK")
end
if xpcall(func, function(err)
	return print(err)
end, 1) then
	print("OK")
end
do
	do
		local success, result = pcall(func, "abc", 123)
		if success then
			print(result)
		end
	end
	local success, result = xpcall(func, function(err)
		return print(err)
	end, "abc", 123)
	success, result = xpcall(func, function(err)
		return print(err)
	end, "abc", 123)
	if success then
		print(result)
	end
end
return nil
