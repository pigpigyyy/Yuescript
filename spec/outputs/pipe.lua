foreach({
	"abc",
	123,
	998
}, print)
print(reduce(filter(map({
	1,
	2,
	3
}, function(x)
	return x * 2
end), function(x)
	return x > 4
end), 0, function(a, b)
	return a + b
end))
print(table.concat((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 0, 10 do
		_accum_0[_len_0] = tostring(i)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)(), ","))
local b = 1 + 2 + (print(tostring(4), 1) or 123)
do
	local x = math.max(233, 998)
	if x then
		print(x)
	end
end
do
	local _with_0
	do
		local _obj_0 = create
		if _obj_0 ~= nil then
			_with_0 = _obj_0(b, "new")
		end
	end
	_with_0.value = 123
	print(_with_0:work())
end
do
	local _obj_0 = f
	if _obj_0 ~= nil then
		_obj_0(123)
	end
end
do
	local _obj_0 = f2
	if _obj_0 ~= nil then
		_obj_0((function()
			local _obj_1 = f1
			if _obj_1 ~= nil then
				return _obj_1("abc")
			end
			return nil
		end)())
	end
end
local c
do
	local _obj_0 = f2
	if _obj_0 ~= nil then
		c = _obj_0((function()
			local _obj_1 = f1
			if _obj_1 ~= nil then
				return _obj_1("abc")
			end
			return nil
		end)())
	end
end
local f
f = function()
	local _obj_0 = x.y
	if _obj_0 ~= nil then
		return _obj_0["if"](_obj_0, arg)
	end
	return nil
end
func2(998, "abc", func1(func0(233)))
func2(func1(func0(998, "abc", 233)))
f(1, 2, 3, 4, 5)
f(1, val(2), 3, 4, 5)
f(1, 2, arr[3], 4, 5)
local a = f2(1, f1(1, 2, 3, tonumber(table.concat({
	"1",
	"2",
	"3"
}, ""))), 3)
print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
f(not func(123))
do
	local _1 = abc(123, -func(list({
		"abc",
		"xyz",
		"123"
	}):map("#"):value()), "x")
	_2, _3, _4 = 1, 2, f(3)
	local _5
	_5 = f4(f3(f2(f1(v, 1), 2), 3), 4)
	local x <const> = z(y)
	local a <close> = c(b)
end
local x = b(a(123)) or d(c(456)) or (function()
	local _call_0 = a["if"]
	return _call_0["then"](_call_0, "abc")
end)() or (function()
	if a ~= nil then
		local _obj_0 = a.b
		local _obj_1 = _obj_0.c
		if _obj_1 ~= nil then
			return _obj_1(_obj_0, 123)
		end
		return nil
	end
	return nil
end)() or (function()
	local _base_0 = x
	local _fn_0 = _base_0.y
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)()
local x1 = 3 * f(-4)
local x2 = 3 * f(-2 ^ 2)
local y = 1 + b(3, (a ^ c)(not #2)) * f1(f(4 ^ -123)) or 123
return nil
