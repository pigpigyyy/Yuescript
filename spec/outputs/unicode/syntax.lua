local _u53d8_u91cfa = 1 + 2 * 3 / 6
local _u4e00_u5806, _u53bb, _u8fd9_u91cc
_u53d8_u91cfa, _u4e00_u5806, _u53bb, _u8fd9_u91cc = _u53e6_u5916, _u4e16_u754c, nil, nil
_u51fd_u6570(_u53c2_u65701, _u53c2_u65702, _u53e6_u5916, _u53c2_u65703)
local _u6211_u4eec
_u8fd9_u91cc, _u6211_u4eec = function() end, _u662f_u7684
local _u8fd9_u4e2a, _u4e0d_u540c
_u8fd9_u4e2a, _u4e0d_u540c = function()
	return _u65b9_u6cd5
end, _u662f_u7684
_u7238_u7238()
_u7238_u7238(_u4e3b)
_u4f60_u597d(_u4e00, _u4e8c)();
(5 + 5)(_u4e16_u754c)
_u4e50_u8da3(_u53d8_u91cfa)(_u53d8_u91cfb)
_u4e50_u8da3(_u53d8_u91cfa)(_u53d8_u91cfb)
_u4e50_u8da3(_u53d8_u91cfa)(_u53d8_u91cfb, _u574f(_u4f60_u597d))
_u4f60_u597d(_u4e16_u754c(_u4f60_u5728_u8fd9_u91cc_u505a_u4ec0_u4e48))
_u4ec0_u4e48(_u8fd9_u4e2a)[3243](_u4e16_u754c, _u54ce_u5440)
_u6bdb_u8338_u8338[_u624b][_u662f](_u6076_u5fc3)(_u597d_u7684(_u597d_u7684[_u4e16_u754c]))
local _ = (_u5f97_u5230[_u67d0_u4e8b] + 5)[_u5e74]
local _u53d8_u91cfi, _u53d8_u91cfx = 200, 300
local _u662f_u7684 = (1 + 5) * 3
_u662f_u7684 = ((1 + 5) * 3) / 2
_u662f_u7684 = ((1 + 5) * 3) / 2 + _u53d8_u91cfi % 100
local _u54c7 = (1 + 2) * (3 + 4) * (4 + 5)
_ = function()
	if _u67d0_u4e8b then
		return 1, 2, 4
	end
	return _u6253_u5370("你好")
end
_ = function()
	if _u4f60_u597d then
		return "你好", "世界"
	else
		return _u4e0d, _u65b9_u6cd5
	end
end
_ = function()
	return 1, 2, 34
end
return 5 + function()
	return 4 + 2
end
return 5 + (function()
	return 4
end) + 2
_u6253_u5370(5 + function()
	_ = 34
	return _u597d(_u7403)
end)
_u67d0_u4e8b('其他', "是")
_u67d0_u4e8b('其他')
_u67d0_u4e8b("其他")
_ = _u67d0_u4e8b([[嘿]]) * 2
_ = _u67d0_u4e8b([======[嘿]======]) * 2
_ = _u67d0_u4e8b[ [======[嘿]======]] * 2
_, _ = _u67d0_u4e8b('其他'), 2
_, _ = _u67d0_u4e8b("其他"), 2
_, _ = _u67d0_u4e8b([[其他]]), 2
_, _ = _u67d0_u4e8b[ [[其他]]], 2
_u67d0_u4e8b('其他', 2)
_u67d0_u4e8b("其他", 2)
_u67d0_u4e8b([[其他]], 2)
_ = _u8fd9_u91cc(_u6211_u4eec)("去")[12123]
local _call_0 = _u5206_u88c2("abc xyz 123")
local _call_1 = _call_0["映射"](_call_0, "#")
_call_1["打印全部"](_call_1)
_ = f("")[_u53d8_u91cfa]
local _call_2 = f("")
_ = _call_2["变量b"](_call_2)
_ = f("")["变量c"]()
f(("")[_u53d8_u91cfa])
f((function()
	local _call_3 = ("")
	return _call_3["变量b"](_call_3)
end)())
f(("")["变量c"]())
local _call_3 = _u5217_u8868({
	"abc",
	"xyz",
	"123"
})
local _call_4 = _call_3["映射"](_call_3, "#")
_call_4["打印全部"](_call_4)
_ = f({ })[_u53d8_u91cfa]
local _call_5 = f({ })
_ = _call_5["变量b"](_call_5)
_ = f({ })["变量c"]()
local _u67d0_u4e8b = {
	["测试"] = 12323,
	["什么"] = function()
		return _u6253_u5370("你好 世界")
	end
}
_u6253_u5370(_u67d0_u4e8b["测试"])
local _u5929_u54ea = {
	["你好"] = "世界"
}
local _u6c29 = {
	["数字"] = 100,
	["世界"] = function(_u81ea_u5df1)
		_u6253_u5370(_u81ea_u5df1["数字"])
		return {
			["某事"] = function()
				return _u6253_u5370("嗨 从某事")
			end
		}
	end,
	["某事"] = function(_u81ea_u5df1, _u5b57_u7b26_u4e32)
		_u6253_u5370("字符串是", _u5b57_u7b26_u4e32)
		return {
			["世界"] = function(_u53d8_u91cfa, _u53d8_u91cfb)
				return _u6253_u5370("总和", _u53d8_u91cfa + _u53d8_u91cfb)
			end
		}
	end
}
_u67d0_u4e8b["什么"]()
_u6c29["世界"](_u6c29)["某事"]()
_u6c29["某事"](_u6c29, "200")["世界"](1, 2)
_u53d8_u91cfx = -434
_u53d8_u91cfx = -_u4f60_u597d(_u4e16_u754c(_u4e00(_u4e8c)))
local _u55e8 = -"herfef"
_u53d8_u91cfx = -(function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u53d8_u91cfx in _u53d8_u91cfx do
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)()
if _u9177 then
	_u6253_u5370("你好")
end
if not _u9177 then
	_u6253_u5370("你好")
end
if not (1212 and 3434) then
	_u6253_u5370("你好")
end
for i = 1, 10 do
	_u6253_u5370("你好")
end
_u6253_u5370("疯子")
if _u4f60_u597d then
	_ = 343
end
if _u9177 then
	_u6253_u5370("什么")
end
while not (function()
		local _call_6 = _u9605_u8bfb_u8005
		return _call_6["结束"](_call_6)
	end)() do
	local _call_6 = _u9605_u8bfb_u8005
	_call_6["解析_行"](_call_6)
end
while not (function()
		local _call_6 = _u9605_u8bfb_u8005
		return _call_6["结束"](_call_6)
	end)() do
	local _call_6 = _u9605_u8bfb_u8005
	_call_6["解析_行"](_call_6)
end
(function(...)
	local _u53c2_u6570 = {
		...
	}
end)()
_u53d8_u91cfx = function(...)
	return dump({
		...
	})
end
_u53d8_u91cfx = not true
local _u53d8_u91cfy = not (5 + 5)
_u53d8_u91cfy = #"你好"
_u53d8_u91cfx = #{
	#{ },
	#{
		1
	},
	#{
		1,
		2
	}
}
_, _ = _u4f60_u597d, _u4e16_u754c
_u67d0_u4e8b["你好"](_u67d0_u4e8b, _u4ec0_u4e48)(_u53d8_u91cfa, _u53d8_u91cfb)
_u67d0_u4e8b["你好"](_u67d0_u4e8b, _u4ec0_u4e48)
local _call_6 = _u67d0_u4e8b["你好"]
_call_6["世界"](_call_6, _u53d8_u91cfa, _u53d8_u91cfb)
local _call_7 = _u67d0_u4e8b["你好"]
_call_7["世界"](_call_7, 1, 2, 3)(_u53d8_u91cfa, _u53d8_u91cfb)
_u53d8_u91cfx = 1232
_u53d8_u91cfx = _u53d8_u91cfx + (10 + 3)
local _u53d8_u91cfj = _u53d8_u91cfj - "你好"
_u53d8_u91cfy = _u53d8_u91cfy * 2
_u53d8_u91cfy = _u53d8_u91cfy / 100
_u53d8_u91cfy = _u53d8_u91cfy // 100
local _u53d8_u91cfm = _u53d8_u91cfm % 2
local _u4f60_u597d = _u4f60_u597d .. "世界"
self.__class["某事"] = self.__class["某事"] + 10
self["某事"] = self["某事"] + 10
local _update_0 = "你好"
_u53d8_u91cfa[_update_0] = _u53d8_u91cfa[_update_0] + 10
local _update_1 = "你好" .. tostring(tostring(ff))
_u53d8_u91cfa[_update_1] = _u53d8_u91cfa[_update_1] + 10
local _obj_0 = _u53d8_u91cfa[_u56db]
_obj_0["字段x"] = _obj_0["字段x"] + 10
_u53d8_u91cfa["变量b"] = _u53d8_u91cfa["变量b"] + 1
local _obj_1 = _u53d8_u91cfa["变量b"][1]["变量c"]
local _update_2 = 2 + 3
_obj_1[_update_2] = _obj_1[_update_2] + 1
do
	local _with_0 = _u8868
	local _obj_2 = _with_0["字段a"]
	_obj_2["变量c"] = _obj_2["变量c"] + 1
end
do
	local _obj_2 = _u8868
	_obj_2[#_obj_2 + 1] = 10
end
do
	local _obj_2 = _u53d8_u91cfa["变量b"]["变量c"]
	_obj_2[#_obj_2 + 1] = 1
end
if _u6761_u4ef6v then
	_u53d8_u91cfx[#_u53d8_u91cfx + 1] = 1
else
	_u53d8_u91cfx[#_u53d8_u91cfx + 1] = 2
end
do
	local _with_0 = _u8868
	do
		local _obj_2 = _with_0["变量b"]["变量c"]
		do
			local _with_1 = _u5411_u91cf
			_with_1["字段x"] = 1
			_with_1["字段y"] = 2
			_obj_2[#_obj_2 + 1] = _with_1
		end
	end
end
do
	local _call_8 = _u5bf9_u8c61a["变量b"]["变量c"]["变量d"]
	local _obj_2 = _call_8["函数"](_call_8)["变量g"]
	_obj_2[#_obj_2 + 1] = 1
end
local _u8868 = { }
_u8868[#_u8868 + 1] = 1
_u8868[#_u8868 + 1] = 2
_u8868[#_u8868 + 1] = 3
_u53d8_u91cfx = 0
local _list_0 = _u503c
for _index_0 = 1, #_list_0 do
	local _u53d8_u91cfv = _list_0[_index_0]
	if ntype(_u53d8_u91cfv) == "函数定义" then
		_ = _u53d8_u91cfx + 1
	end
end
_u4f60_u597d = {
	["某物"] = _u4e16_u754c,
	["如果"] = "你好",
	["否则"] = 3434,
	["函数"] = "好的",
	["好的"] = 230203
}
_u8868 = {
	["执行"] = _u53d8_u91cfb,
	_u6267_u884c({
		["变量b"] = _u53d8_u91cfb
	})
}
div({
	["类"] = "酷"
})
_ = 5 + _u4ec0_u4e48(_u65e0_u804a)
_u4ec0_u4e48(_u65e0_u804a + 5)
_ = 5 - _u4ec0_u4e48(_u65e0_u804a)
_u4ec0_u4e48(_u65e0_u804a - 5)
_u53d8_u91cfx = _u4f60_u597d - _u4e16_u754c - _u67d0_u7269
local _anon_func_0 = function(_u4ec0_u4e48)
	do
		local _call_8 = _u4ec0_u4e48
		return _call_8["酷"](_call_8, 100)
	end
end
(function(_u67d0_u7269)
	if _u67d0_u7269 == nil then
		_u67d0_u7269 = _u4e0e(_anon_func_0(_u4ec0_u4e48))
	end
	return _u6253_u5370(_u67d0_u7269)
end)()
if _u67d0_u7269 then
	_ = 03589
else
	_ = 3434
end
if _u67d0_u7269 then
	_ = _u662f_u7684
elseif "嗯嗯嗯" then
	_u6253_u5370("酷")
else
	_ = _u597d_u7684
end
_u53d8_u91cfx = _u4e0d_u662f_u67d0_u7269
_u53d8_u91cfy = _u5982_u679c_u67d0_u7269
local _u53d8_u91cfz = _u53d8_u91cfx(_u4e0e(_u53d8_u91cfb))
_u53d8_u91cfz = _u53d8_u91cfx(_u4e0eb)
while 10 > _u67d0_u7269({
		["某物"] = "世界"
	}) do
	_u6253_u5370("是的")
end
_u53d8_u91cfx = {
	["好的"] = _u786e_u5b9a
}
_u662f_u7684({
	["好的"] = _u7537_u4eba,
	["确定"] = _u5148_u751f
})
_u4f60_u597d("无逗号", {
	["是的"] = _u7238_u7238,
	["另一个"] = _u4e16_u754c
})
_u4f60_u597d("逗号", {
	["某物"] = _u4f60_u597d__u4e16_u754c,
	["骂人"] = _u4f60
})
_u53e6_u4e00_u4e2a(_u4f60_u597d, _u4e00_u4e2a, _u4e8c, _u4e09, _u56db, {
	["是的"] = _u7537_u4eba,
	["好的"] = _u662f_u7684,
	["好"] = _u597d_u7684
})
_u53e6_u4e00_u4e2a(_u4f60_u597d, _u4e00_u4e2a, _u4e8c, _u4e09, _u56db, {
	["是的"] = _u7537_u4eba,
	["好的"] = _u662f_u7684,
	{
		["好"] = _u597d_u7684,
		["好的"] = 1
	},
	{
		["好"] = _u597d_u7684,
		["好的"] = 2
	}
})
_u53e6_u4e00_u4e2a(_u4f60_u597d, _u4e00_u4e2a, _u4e8c, _u4e09, _u56db, {
	["是的"] = _u7537_u4eba,
	["好的"] = _u662f_u7684
})
_u53e6_u4e00_u4e2a(_u4f60_u597d, _u4e00_u4e2a, _u4e8c, _u4e09, _u56db, {
	["是的"] = _u7537_u4eba({
		["好的"] = _u662f_u7684
	})
})
_u963f_u8d3e_u514b_u65af(_u5730_u5740, function(_u6570_u636e)
	return _u5904_u7406(_u6570_u636e)
end, function(_u9519_u8bef)
	return _u6253_u5370(_u9519_u8bef)
end)
_u53d8_u91cfa = _u53d8_u91cfa + (3 - 5)
_u53d8_u91cfa = _u53d8_u91cfa * (3 + 5)
_u53d8_u91cfa = _u53d8_u91cfa * 3
_u53d8_u91cfa = _u53d8_u91cfa >> 3
_u53d8_u91cfa = _u53d8_u91cfa << 3
_u53d8_u91cfa = _u53d8_u91cfa / _u51fd_u6570("酷")
_u53d8_u91cfx["然后"] = "你好"
_u53d8_u91cfx["当"]["真"] = "你好"
_u53d8_u91cfx["当"]["真"] = "你好"
_u53d8_u91cfx = _u53d8_u91cfx or "你好"
_u53d8_u91cfx = _u53d8_u91cfx and "你好"
_u53d8_u91cfz = _u53d8_u91cfa - _u53d8_u91cfb
_u53d8_u91cfz = _u53d8_u91cfa(-_u53d8_u91cfb)
_u53d8_u91cfz = _u53d8_u91cfa - _u53d8_u91cfb
_u53d8_u91cfz = _u53d8_u91cfa - _u53d8_u91cfb
local _u5b57_u7b26_u4e32 = _u5b57_u7b26_u4e32A .. _u5b57_u7b26_u4e32B .. _u5b57_u7b26_u4e32C
_u51fd_u6570(3000, "192.168.1.1")
local f
f = function()
	return _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc, _u53d8_u91cfd, _u53d8_u91cfe, _u53d8_u91cff
end
f = function()
	return _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc, _u53d8_u91cfd, _u53d8_u91cfe, _u53d8_u91cff
end
do
	local _with_0 = _u5bf9_u8c61
	_u8c03_u7528(_with_0["函数"](_with_0), 123, "abc")
end
_u8c03_u7528A(_u8c03_u7528B(_u8c03_u7528C(123)))
_u8c03_u7528A(_u8c03_u7528B(_u8c03_u7528C(123)))
local _u53d8_u91cfv = {
	_u53d8_u91cfa(-1),
	_u53d8_u91cfa(-1),
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa - 1,
	_u53d8_u91cfa(~1),
	_u53d8_u91cfa(~1),
	_u53d8_u91cfa ~ 1,
	_u53d8_u91cfa ~ 1,
	_u53d8_u91cfa ~ 1,
	_u53d8_u91cfa ~ 1,
	_u53d8_u91cfa ~ 1,
	_u53d8_u91cfa ~ 1
}
do
	_u53d8_u91cfa = 1 + 2 * 3 / 4
	local _u53d8_u91cf_1 = f1(-1 + 2 + 3)
	local _u53d8_u91cf_2 = f1 - 1 + 2 + 3
	local f2
	f2 = function(_u53d8_u91cfx)
		return _u6253_u5370(_u53d8_u91cfx + 1)
	end
	_u53d8_u91cfa = f2()
	f2(-1)
	_u53d8_u91cfa = f2() - f2(1)
	local _u53d8_u91cf_3, _u53d8_u91cf_4
	_u53d8_u91cf_1, _u53d8_u91cf_2, _u53d8_u91cf_3, _u53d8_u91cf_4 = 1, f(2, 3, f(4, 4))
end
do
	f = function(_u53d8_u91cfa, _u53d8_u91cfb)
		return _u53d8_u91cfa + _u53d8_u91cfb
	end
	f(_u53d8_u91cfa, _u53d8_u91cfb)
	f()
end
return nil
