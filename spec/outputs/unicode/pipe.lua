_u6bcf_u4e00_u4e2a({
	"abc",
	123,
	998
}, _u6253_u5370)
_u6253_u5370(_u5408_u5e76(_u8fc7_u6ee4(_u6620_u5c04({
	1,
	2,
	3
}, function(_u53d8_u91cfx)
	return _u53d8_u91cfx * 2
end), function(_u53d8_u91cfx)
	return _u53d8_u91cfx > 4
end), 0, function(_u53d8_u91cfa, _u53d8_u91cfb)
	return _u53d8_u91cfa + _u53d8_u91cfb
end))
_u6253_u5370(table.concat((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 0, 10 do
		_accum_0[_len_0] = tostring(i)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)(), ","))
local _u53d8_u91cfb = 1 + 2 + (_u6253_u5370(tostring(4), 1) or 123)
do
	local _u53d8_u91cfx = math.max(233, 998)
	if _u53d8_u91cfx then
		_u6253_u5370(_u53d8_u91cfx)
	end
end
do
	local _with_0
	do
		local _obj_0 = _u521b_u5efa
		if _obj_0 ~= nil then
			_with_0 = _obj_0(_u53d8_u91cfb, "new")
		end
	end
	_with_0.value = 123
	_u6253_u5370(_with_0["工作"](_with_0))
end
do
	local _obj_0 = _u51fd_u6570f
	if _obj_0 ~= nil then
		_obj_0(123)
	end
end
do
	local _obj_0 = _u51fd_u65702
	if _obj_0 ~= nil then
		_obj_0((function()
			local _obj_1 = _u51fd_u65701
			if _obj_1 ~= nil then
				return _obj_1("abc")
			end
			return nil
		end)())
	end
end
local _u53d8_u91cfc
do
	local _obj_0 = _u51fd_u65702
	if _obj_0 ~= nil then
		_u53d8_u91cfc = _obj_0((function()
			local _obj_1 = _u51fd_u65701
			if _obj_1 ~= nil then
				return _obj_1("abc")
			end
			return nil
		end)())
	end
end
local _u51fd_u6570f
_u51fd_u6570f = function()
	local _obj_0 = x.y
	if _obj_0 ~= nil then
		return _obj_0["如果"](_obj_0, _u53c2_u6570)
	end
	return nil
end
_u51fd_u65702(998, "abc", _u51fd_u65701(_u51fd_u65700(233)))
_u51fd_u65702(_u51fd_u65701(_u51fd_u65700(998, "abc", 233)))
_u51fd_u6570f(1, 2, 3, 4, 5)
_u51fd_u6570f(1, _u503c(2), 3, 4, 5)
_u51fd_u6570f(1, 2, _u6570_u7ec4[3], 4, 5)
local _u53d8_u91cfa = _u51fd_u65702(1, _u51fd_u65701(1, 2, 3, tonumber(table.concat({
	"1",
	"2",
	"3"
}, ""))), 3)
_u6253_u5370(_u6e32_u67d3(_u53d1_u51fa(_u89e3_u6790(_u63d0_u53d6(_u8bfb_u6587_u4ef6("example.txt"), _u8bed_u8a00, { }), _u8bed_u8a00))))
_u6253_u5370(_u6e32_u67d3(_u53d1_u51fa(_u89e3_u6790(_u63d0_u53d6(_u8bfb_u6587_u4ef6("example.txt"), _u8bed_u8a00, { }), _u8bed_u8a00))))
_u6253_u5370(_u6e32_u67d3(_u53d1_u51fa(_u89e3_u6790(_u63d0_u53d6(_u8bfb_u6587_u4ef6("example.txt"), _u8bed_u8a00, { }), _u8bed_u8a00))))
_u51fd_u6570f(not _u51fd_u6570(123))
do
	local _1 = abc(123, -_u51fd_u6570((function()
		local _call_0 = _u5217_u8868({
			"abc",
			"xyz",
			"123"
		})
		local _call_1 = _call_0["映射"](_call_0, "#")
		return _call_1["值"](_call_1)
	end)()), "x")
	_2, _3, _4 = 1, 2, _u51fd_u6570f(3)
	local _5
	_5 = _u51fd_u65704(_u51fd_u65703(_u51fd_u65702(_u51fd_u65701(_u53d8_u91cfv, 1), 2), 3), 4)
end
local _u53d8_u91cfx = _u51fd_u6570b(_u51fd_u6570a(123)) or _u51fd_u6570d(_u51fd_u6570c(456)) or (function()
	local _call_0 = _u51fd_u6570a["if"]
	return _call_0["那么"](_call_0, "abc")
end)() or (function()
	local _obj_0 = _u51fd_u6570a
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0["函数b"]
		local _obj_2 = _obj_1["函数c"]
		if _obj_2 ~= nil then
			return _obj_2(_obj_1, 123)
		end
		return nil
	end
	return nil
end)() or (function()
	local _base_0 = _u53d8_u91cfx
	local _fn_0 = _base_0["函数y"]
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)()
local _u53d8_u91cfx1 = 3 * _u51fd_u6570f(-4)
local _u53d8_u91cfx2 = 3 * _u51fd_u6570f(-2 ^ 2)
local _u53d8_u91cfy = 1 + _u51fd_u6570b(3, (_u51fd_u6570a ^ _u51fd_u6570c)(not #2)) * _u51fd_u65701(_u51fd_u6570f(4 ^ -123)) or 123
return nil
