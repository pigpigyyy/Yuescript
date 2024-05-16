local _anon_func_0 = function(func, print)
	print("trying")
	return func(1, 2, 3)
end
local _anon_func_1 = function(tb)
	return tb.func
end
local _anon_func_2 = function(tb)
	return tb.func()
end
local _anon_func_3 = function(tb)
	return tb.func()
end
local _anon_func_4 = function(tb)
	return tb.func()
end
local _anon_func_5 = function(tb)
	return tb:func(1, 2, 3)
end
local _anon_func_6 = function(tb)
	return tb.func(1)
end
local _anon_func_7 = function(tb)
	return tb.func(1)
end
local f
f = function()
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
	pcall(_anon_func_0, func, print)
	do
		local success, result = xpcall(function()
			return func(1, 2, 3)
		end, function(err)
			return print(err)
		end)
		success, result = pcall(func, 1, 2, 3)
	end
	pcall(_anon_func_1, tb)
	pcall(_anon_func_2, tb)
	pcall(_anon_func_3, tb)
	pcall(_anon_func_4, tb)
	pcall(_anon_func_5, tb)
	pcall(_anon_func_6, tb)
	pcall(_anon_func_7, tb)
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
	do
		pcall(func, 1, 2, 3)
		pcall(func, 1, 2, 3)
	end
	return nil
end
