local _anon_func_0 = function(tb)
	return tb.func
end
local _anon_func_1 = function(tb)
	return tb.func()
end
local _anon_func_2 = function(tb)
	return tb.func()
end
local _anon_func_3 = function(tb)
	return tb.func()
end
local _anon_func_4 = function(tb)
	return tb:func(1, 2, 3)
end
local _anon_func_5 = function(tb)
	return tb.func(1)
end
local _anon_func_6 = function(tb)
	return tb.func(1)
end
local _anon_func_7 = function(a, b, c, tb)
	return tb.f(a, b, c)
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
		success, result = pcall(function()
			return func(1, 2, 3)
		end)
	end
	local tb = { }
	pcall(_anon_func_0, tb)
	pcall(_anon_func_1, tb)
	pcall(_anon_func_2, tb)
	pcall(_anon_func_3, tb)
	pcall(_anon_func_4, tb)
	pcall(_anon_func_5, tb)
	pcall(_anon_func_6, tb)
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
			local success, result = pcall(function()
				return func("abc", 123)
			end)
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
		pcall(function()
			return func(1, 2, 3)
		end)
		pcall(function()
			return func(1, 2, 3)
		end)
	end
	do
		x(function()
			local tb, a, b, c
			f = function()
				return pcall(_anon_func_7, a, b, c, tb)
			end
		end)
	end
	return nil
end
