assert(_u5bf9_u8c61 == nil)
local _u7ed3_u679c = (_u5bf9_u8c61 == nil)
if _u51fd_u65701() then
	_u6253_u5370("没问题")
end
if (_u51fd_u65701() and _u51fd_u65702() and _u51fd_u65703()) then
	_u6253_u5370("没问题")
end
local _u5bf9_u8c61
do
	local __u6e90_, __u76ee_u6807_
	do
		__u76ee_u6807_ = {
			["位置"] = { },
			["标签"] = (function()
				local _call_0 = _u6807_u7b7e
				return _call_0["转数字"](_call_0)
			end)()
		}
		do
			__u6e90_ = self
			__u76ee_u6807_["标识"] = __u6e90_["标识"]
			__u76ee_u6807_["连接们"] = __u6e90_["连接们"]
			__u76ee_u6807_["位置"]["x坐标"] = __u6e90_["位置"]["x坐标"]
			__u76ee_u6807_["位置"]["y坐标"] = __u6e90_["位置"]["y坐标"]
			__u76ee_u6807_["位置"]["z坐标"] = __u6e90_["位置"]["z坐标"]
		end
		_u5bf9_u8c61 = __u76ee_u6807_
	end
end
if (_u53d8_u91cfx == "🍎" or _u53d8_u91cfx == "🐷" or _u53d8_u91cfx == "🐶") then
	_u6253_u5370("存在")
end
local _list_0 = (function()
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = (function()
		local _accum_1 = { }
		local _len_1 = 1
		local _list_0 = {
			1,
			2,
			3
		}
		for _index_0 = 1, #_list_0 do
			local _ = _list_0[_index_0]
			_accum_1[_len_1] = _ * 2
			_len_1 = _len_1 + 1
		end
		return _accum_1
	end)()
	for _index_0 = 1, #_list_0 do
		local _ = _list_0[_index_0]
		if _ > 4 then
			_accum_0[_len_0] = _
			_len_0 = _len_0 + 1
		end
	end
	return _accum_0
end)()
for _index_0 = 1, #_list_0 do
	local _ = _list_0[_index_0]
	_u6253_u5370(_)
end
local _list_1 = (function()
	local _accum_0 = { }
	local _len_0 = 1
	local _list_1 = (function()
		local _accum_1 = { }
		local _len_1 = 1
		local _list_1 = {
			1,
			2,
			3
		}
		for _index_0 = 1, #_list_1 do
			local _ = _list_1[_index_0]
			_accum_1[_len_1] = _ * 2
			_len_1 = _len_1 + 1
		end
		return _accum_1
	end)()
	for _index_0 = 1, #_list_1 do
		local _ = _list_1[_index_0]
		if _ > 4 then
			_accum_0[_len_0] = _
			_len_0 = _len_0 + 1
		end
	end
	return _accum_0
end)()
for _index_0 = 1, #_list_1 do
	local _ = _list_1[_index_0]
	_u6253_u5370(_)
end
local _u503c
do
	local _2
	do
		local _accum_0 = { }
		local _len_0 = 1
		local _list_2 = {
			1,
			2,
			3
		}
		for _index_0 = 1, #_list_2 do
			local _ = _list_2[_index_0]
			_accum_0[_len_0] = _ * 2
			_len_0 = _len_0 + 1
		end
		_2 = _accum_0
	end
	local _3
	do
		local _accum_0 = { }
		local _len_0 = 1
		for _index_0 = 1, #_2 do
			local _ = _2[_index_0]
			if _ > 4 then
				_accum_0[_len_0] = _
				_len_0 = _len_0 + 1
			end
		end
		_3 = _accum_0
	end
	local _4
	if #_3 == 0 then
		_4 = 0
	else
		local _1 = 0
		for _index_0 = 1, #_3 do
			local _2 = _3[_index_0]
			_1 = _1 + _2
		end
		_4 = _1
	end
	_u503c = _4
end
local _call_0 = (1 + 2)
_call_0["调用"](_call_0, 123)
_u7ed3_u679c = (1 + 2)
local f
f = function(_u53c2_u6570x)
	return function(_u53c2_u6570y)
		return function(_u53c2_u6570z)
			return _u6253_u5370(_u53c2_u6570x, _u53c2_u6570y, _u53c2_u6570z)
		end
	end
end
do
	local _u53d8_u91cfa = 8
	do
		_u53d8_u91cfa = 1
		_u53d8_u91cfa = _u53d8_u91cfa + 1
	end
	_u53d8_u91cfa = _u53d8_u91cfa + (function()
		_u53d8_u91cfa = 1
		return _u53d8_u91cfa + 1
	end)()
	_u6253_u5370(_u53d8_u91cfa)
end
do
	local _u53d8_u91cfa = 8
	_u53d8_u91cfa = (function()
		local _u53d8_u91cfa
		_u53d8_u91cfa = 1
		return _u53d8_u91cfa + 1
	end)()
	_u53d8_u91cfa = _u53d8_u91cfa + (function()
		local _u53d8_u91cfa
		_u53d8_u91cfa = 1
		return _u53d8_u91cfa + 1
	end)()
	_u6253_u5370(_u53d8_u91cfa)
end
local _u53d8_u91cfx = 0
local function f(a)
	return a + 1
end
x = x + f(3)
function tb:func()
	print(123)
end
_u6253_u5370(_u53d8_u91cfx)
local sel
sel = function(_u53c2_u6570a, _u53c2_u6570b, _u53c2_u6570c)
	if _u53c2_u6570a then
		return _u53c2_u6570b
	else
		return _u53c2_u6570c
	end
end
local function sel(a, b, c)
	if a then
		return b
	else
		return c
	end
end
local function dummy()
	
end
-- 这有个注释
local _ = require('下划线')
local _call_1 = (_({
	1,
	2,
	3,
	4,
	-2,
	3
}))
_call_1 = _call_1["链"](_call_1, _call_1, _call_1, _call_1, _call_1, _call_1, _call_1, _call_1)
_call_1 = _call_1["映射"](_call_1, _call_1, _call_1, _call_1, function(self)
	return self * 2
end)
_call_1 = _call_1["过滤"](_call_1, _call_1, function(self)
	return self > 3
end)
local _u7ed3_u679ca = _call_1["取值"](_call_1)
local _call_2 = (_({
	1,
	2,
	3,
	4,
	-2,
	3
}))
_call_2 = _call_2["链"](_call_2, _call_2, _call_2, _call_2, _call_2, _call_2, _call_2, _call_2)
_call_2 = _call_2["映射"](_call_2, _call_2, _call_2, _call_2, function(self)
	return self * 2
end)
_call_2 = _call_2["过滤"](_call_2, _call_2, function(self)
	return self > 3
end)
_call_2["每一个"](_call_2, function(self)
	return _u6253_u5370(self)
end)
local _call_3 = _u539f_u70b9["变换"]["根节点"]["游戏对象"]
_call_3 = _call_3["父节点"](_call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3)
_call_3 = _call_3["后代"](_call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3)
_call_3 = _call_3["选择启用"](_call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3)
_call_3 = _call_3["选择可见"](_call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3, _call_3)
_call_3 = _call_3["标签等于"](_call_3, _call_3, _call_3, _call_3, "fx")
_call_3 = _call_3["其中"](_call_3, _call_3, function(x)
	local _call_4 = x["名称"]
	return _call_4["结尾为"](_call_4, _call_4, "(克隆)")
end)
_u7ed3_u679c = _call_3["摧毁"](_call_3)
do
	local _1
	local _call_4 = _u539f_u70b9["变换"]["根节点"]["游戏对象"]
	_1 = _call_4["父节点"](_call_4)
	local _2
	_2 = _1["后代"](_1)
	local _3
	_3 = _2["选择启用"](_2)
	local _4
	_4 = _3["选择可见"](_3)
	local _5
	_5 = _4["标签等于"](_4, "fx")
	local _6
	_6 = _5["其中"](_5, function(x)
		local _call_5 = x["名称"]
		return _call_5["结尾为"](_call_5, "(克隆)")
	end)
	_6["摧毁"](_6)
end
local _call_0 = _u539f_u70b9["变换"]["根节点"]["游戏对象"]
_call_0["父节点"](_call_0):_u540e_u4ee3():_u9009_u62e9_u542f_u7528():_u9009_u62e9_u53ef_u89c1():_u6807_u7b7e_u7b49_u4e8e("fx"):_u5176_u4e2d(function(x)
	local _call_0 = x["名称"]
	return _call_0["结尾为"](_call_0, "(克隆)")
end):_u6467_u6bc1()
_u6253_u5370((setmetatable({
	'abc',
	["字段a"] = 123,
}, {
	__call = function(self)
		return 998
	end
}))[1], (setmetatable({
	'abc',
	["字段a"] = 123,
}, {
	__call = function(self)
		return 998
	end
}))["字段a"], (setmetatable({
	'abc',
	["字段a"] = 123,
}, {
	__call = function(self)
		return 998
	end
}))(), setmetatable({
	'abc',
	["字段a"] = 123,
}, {
	__call = function(self)
		return 998
	end
}))
_u6253_u5370("当前代码行数: " .. tostring(268))
-- 待实现
do
	_u6253_u5370(1)
end
local _1
_1 = function()
	_u6253_u5370(1)
	local _accum_0 = { }
	local _len_0 = 1
	while false do
		break
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
_u6253_u5370('abc')
return 123
