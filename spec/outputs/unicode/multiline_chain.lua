local _u53d8_u91cfx = _u53d8_u91cfa["变量b"]["变量c"]["变量d"]
local _call_0 = _u53d8_u91cfa["变量b"]
_u53d8_u91cfx["变量y"] = _call_0["变量c"](_call_0)
_u51fd_u6570(1, (function()
	local _call_1 = _u53c2_u65702["值"]
	return _call_1["获取"](_call_1)
end)(), (function()
	local _call_1 = _u53c2_u65703["值"]
	return _call_1["获取"](_call_1)
end)(), {
	1,
	((function()
		if _u53d8_u91cfx ~= nil then
			local _obj_0 = _u53d8_u91cfx["变量y"]
			if _obj_0 ~= nil then
				return _obj_0["变量z"]
			end
			return nil
		end
		return nil
	end)() ~= nil),
	3
})
local _u8868bb = {
	["键1"] = (function()
		local _call_1 = _u53d8_u91cfa
		return (_call_1["变量b"](_call_1, 123, {
			["字段A"] = _u5b57_u6bb5A
		}))["变量c"](1, 2, 3, {
			["字段B"] = _u5b57_u6bb5B
		})
	end)(),
	["键2"] = {
		["键w1"] = (function()
			local _call_1 = _u53d8_u91cfa()["变量b"]
			return _call_1["变量c"](_call_1)
		end)()
	}
}
local _u8868b
do
	local _obj_0 = _u51fd_u65702
	if _obj_0 ~= nil then
		_u8868b = _obj_0("abc", _u51fd_u65701({ })["变量a"])
	end
end
local _u51fd_u6570
_u51fd_u6570 = function()
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = _u503c_u96c6_u5408
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local _u53d8_u91cfa = _des_0[1]
		local _call_1 = _u53d8_u91cfa["变量b"]
		_accum_0[_len_0] = _call_1["变量c"](_call_1, 123)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_0 = function(_u53d8_u91cfa)
	do
		local _call_1 = _u53d8_u91cfa
		return (_call_1["变量b"](_call_1, 123))["变量c"]("abc")
	end
end
local _u51fd_u65701
_u51fd_u65701 = function()
	return _u53d8_u91cfx, _anon_func_0(_u53d8_u91cfa)
end
local _call_1 = _u539f_u59cb["转换"].root.gameObject
local _call_2 = _call_1["父节点"](_call_1)
local _call_3 = _call_2["后代节点"](_call_2)
local _call_4 = _call_3["选择启用"](_call_3)
local _call_5 = _call_4["选择可见"](_call_4)
local _call_6 = (_call_5["标签相等"](_call_5, "fx"))
local _call_7 = (_call_6["筛选"](_call_6, function(_u5143_u7d20)
	if _u5143_u7d20["是否目标"](_u5143_u7d20) then
		return false
	end
	local _call_7 = _u5143_u7d20["名称"]
	return _call_7["结束于"](_call_7, "(Clone)")
end))
local _u7ed3_u679c = _call_7["销毁"](_call_7)
local _call_8 = _u539f_u70b9["变换"]["根节点"]["游戏对象"]
local _call_9 = _call_8["父节点"](_call_8)
local _call_10 = _call_9["后代节点"](_call_9)
local _call_11 = _call_10["选择启用"](_call_10)
local _call_12 = _call_11["选择可见"](_call_11)
local _call_13 = (_call_12["标签相等"](_call_12, "fx"))
local _call_14 = (_call_13["筛选"](_call_13, function(_u5143_u7d20)
	local _call_14 = _u5143_u7d20["名称"]
	return _call_14["结束于"](_call_14, "(Clone)")
end))
_call_14["销毁"](_call_14)
do
	local _with_0 = _u9879_u76ee
	_with_0["项目字段A"] = 123
	local _call_15 = _with_0["调用方法"](_with_0)
	_call_15["连续调用"](_call_15)
	local _call_16 = _with_0["调用方法"](_with_0)
	_call_16["连续调用"](_call_16)
	_with_0["连续调用"](_with_0)
	do
		local _call_17 = _with_0["项目字段B"]
		local _exp_0 = _call_17["取值"](_call_17)
		if "Valid" == _exp_0 or _with_0["获取项目状态"](_with_0) == _exp_0 then
			local _call_18 = _with_0["项目方法A"](_with_0)
			_call_18["取值"](_call_18)
		else
			local _call_18 = _with_0["项目方法B"](_with_0)
			_call_18["取值"](_call_18)
		end
	end
	local _u53d8_u91cfa
	if _with_0["项目字段C"] then
		_u53d8_u91cfa = _with_0["项目字段D"]
	else
		_u53d8_u91cfa = _with_0["项目字段E"]
	end
	local _list_0 = _u503c_u96c6_u5408
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfv = _list_0[_index_0]
		_with_0["项目方法C"](_with_0, _u53d8_u91cfv)
	end
	for _u53d8_u91cfi = 1, _u8ba1_u6570_u5668 do
		_with_0["项目方法C"](_with_0, _u53d8_u91cfi)
	end
	if not _with_0["变量b"] then
		local _accum_0 = { }
		local _len_0 = 1
		while _with_0["项目字段D"] do
			local _call_17 = _with_0["下一个项目"](_with_0)
			_accum_0[_len_0] = _call_17["取得"](_call_17)
			_len_0 = _len_0 + 1
		end
		_with_0["变量c"] = _accum_0
	end
end
return nil
