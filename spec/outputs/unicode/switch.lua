do
	local _exp_0 = _u503c
	if "酷" == _exp_0 then
		_u6253_u5370("你好，世界")
	end
end
do
	local _exp_0 = _u503c
	if "酷" == _exp_0 then
		_u6253_u5370("你好，世界")
	else
		_u6253_u5370("好的，很棒")
	end
end
do
	local _exp_0 = _u503c
	if "酷" == _exp_0 then
		_u6253_u5370("你好，世界")
	elseif "是的" == _exp_0 then
		local _u53d8_u91cf_ = [[FFFF]] + [[MMMM]]
	elseif (2323 + 32434) == _exp_0 then
		_u6253_u5370("好的")
	else
		_u6253_u5370("好的，很棒")
	end
end
local _u8f93_u51fa
do
	local _exp_0 = _u503c
	if "酷" == _exp_0 then
		_u8f93_u51fa = _u6253_u5370("你好，世界")
	else
		_u8f93_u51fa = _u6253_u5370("好的，很棒")
	end
end
do
	local _exp_0 = _u503c
	if "酷" == _exp_0 then
		_u8f93_u51fa = _u53d8_u91cfx
	elseif "哦" == _exp_0 then
		_u8f93_u51fa = 34340
	else
		_u8f93_u51fa = error("这大大地失败了")
	end
end
do
	local _with_0 = _u4e1c_u897f
	local _exp_0 = _with_0["值"](_with_0)
	if _with_0["确定"] == _exp_0 then
		local _u53d8_u91cf_ = "世界"
	else
		local _u53d8_u91cf_ = "是的"
	end
end
_u4fee_u590d(_u8fd9_u4e2a)
call_func((function()
	local _exp_0 = _u67d0_u7269
	if 1 == _exp_0 then
		return "是"
	else
		return "否"
	end
end)())
do
	local _exp_0 = _u55e8
	if (_u4f60_u597d or _u4e16_u754c) == _exp_0 then
		local _u53d8_u91cf_ = _u7eff_u8272
	end
end
do
	local _exp_0 = _u55e8
	if "一个" == _exp_0 or "两个" == _exp_0 then
		_u6253_u5370("酷")
	elseif "爸爸" == _exp_0 then
		local _u53d8_u91cf_ = _u5426
	end
end
do
	local _exp_0 = _u55e8
	if (3 + 1) == _exp_0 or _u4f60_u597d() == _exp_0 or (function()
		return 4
	end)() == _exp_0 then
		local _u53d8_u91cf_ = _u9ec4_u8272
	else
		_u6253_u5370("酷")
	end
end
do
	local _u5b57_u5178 = {
		{ },
		{
			1,
			2,
			3
		},
		["变量a"] = {
			["变量b"] = {
				["变量c"] = 1
			}
		},
		["变量x"] = {
			["变量y"] = {
				["变量z"] = 1
			}
		}
	}
	local _type_0 = type(_u5b57_u5178)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	if _tab_0 then
		local _u7b2c_u4e00 = _u5b57_u5178[1]
		local _u4e00_u4e2a
		do
			local _obj_0 = _u5b57_u5178[2]
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				_u4e00_u4e2a = _obj_0[1]
			end
		end
		local _u4e24_u4e2a
		do
			local _obj_0 = _u5b57_u5178[2]
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				_u4e24_u4e2a = _obj_0[2]
			end
		end
		local _u4e09_u4e2a
		do
			local _obj_0 = _u5b57_u5178[2]
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				_u4e09_u4e2a = _obj_0[3]
			end
		end
		local _u53d8_u91cfc
		do
			local _obj_0 = _u5b57_u5178["变量a"]
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				do
					local _obj_1 = _obj_0["变量b"]
					local _type_2 = type(_obj_1)
					if "table" == _type_2 or "userdata" == _type_2 then
						_u53d8_u91cfc = _obj_1["变量c"]
					end
				end
			end
		end
		local _u53d8_u91cfz
		do
			local _obj_0 = _u5b57_u5178["变量x"]
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				do
					local _obj_1 = _obj_0["变量y"]
					local _type_2 = type(_obj_1)
					if "table" == _type_2 or "userdata" == _type_2 then
						_u53d8_u91cfz = _obj_1["变量z"]
					end
				end
			end
		end
		if _u7b2c_u4e00 ~= nil and _u4e00_u4e2a ~= nil and _u4e24_u4e2a ~= nil and _u4e09_u4e2a ~= nil and _u53d8_u91cfc ~= nil and _u53d8_u91cfz ~= nil then
			_u6253_u5370(_u7b2c_u4e00, _u4e00_u4e2a, _u4e24_u4e2a, _u4e09_u4e2a, _u53d8_u91cfc, _u53d8_u91cfz)
		end
	end
end
do
	local _u7269_u54c1 = {
		{
			["变量x"] = 100,
			["变量y"] = 200
		},
		{
			["宽度"] = 300,
			["高度"] = 400
		},
		false
	}
	for _index_0 = 1, #_u7269_u54c1 do
		local _u7269 = _u7269_u54c1[_index_0]
		local _type_0 = type(_u7269)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		local _match_0 = false
		if _tab_0 then
			local _u53d8_u91cfx = _u7269["变量x"]
			local _u53d8_u91cfy = _u7269["变量y"]
			if _u53d8_u91cfx ~= nil and _u53d8_u91cfy ~= nil then
				_match_0 = true
				_u6253_u5370("Vec2 " .. tostring(_u53d8_u91cfx) .. ", " .. tostring(_u53d8_u91cfy))
			end
		end
		if not _match_0 then
			local _match_1 = false
			if _tab_0 then
				local _u5bbd_u5ea6 = _u7269["宽度"]
				local _u9ad8_u5ea6 = _u7269["高度"]
				if _u5bbd_u5ea6 ~= nil and _u9ad8_u5ea6 ~= nil then
					_match_1 = true
					_u6253_u5370("Size " .. tostring(_u5bbd_u5ea6) .. ", " .. tostring(_u9ad8_u5ea6))
				end
			end
			if not _match_1 then
				if false == _u7269 then
					_u6253_u5370("没有")
				else
					local _match_2 = false
					if _tab_0 then
						local _u7c7b = _u7269["__类"]
						if _u7c7b ~= nil then
							_match_2 = true
							if _u7c7b_u522bA == _u7c7b then
								_u6253_u5370("对象 A")
							elseif _u7c7b_u522bB == _u7c7b then
								_u6253_u5370("对象 B")
							end
						end
					end
					if not _match_2 then
						local _match_3 = false
						if _tab_0 then
							local _u8868 = getmetatable(_u7269)
							if _u8868 ~= nil then
								_match_3 = true
								_u6253_u5370("带有元表的表")
							end
						end
						if not _match_3 then
							_u6253_u5370("物品不被接受！")
						end
					end
				end
			end
		end
	end
end
do
	local _u8868_u683c = { }
	do
		local _type_0 = type(_u8868_u683c)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local _u53d8_u91cfa = _u8868_u683c["变量a"]
			local _u53d8_u91cfb = _u8868_u683c["变量b"]
			if _u53d8_u91cfa == nil then
				_u53d8_u91cfa = 1
			end
			if _u53d8_u91cfb == nil then
				_u53d8_u91cfb = 2
			end
			_u6253_u5370(_u53d8_u91cfa, _u53d8_u91cfb)
		end
	end
	do
		local _type_0 = type(_u8868_u683c)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local _u53d8_u91cfa = _u8868_u683c["变量a"]
			local _u53d8_u91cfb = _u8868_u683c["变量b"]
			if _u53d8_u91cfb == nil then
				_u53d8_u91cfb = 2
			end
			if _u53d8_u91cfa ~= nil then
				_u6253_u5370("部分匹配", _u53d8_u91cfa, _u53d8_u91cfb)
			end
		end
	end
	local _type_0 = type(_u8868_u683c)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	local _match_0 = false
	if _tab_0 then
		local _u53d8_u91cfa = _u8868_u683c["变量a"]
		local _u53d8_u91cfb = _u8868_u683c["变量b"]
		if _u53d8_u91cfa ~= nil and _u53d8_u91cfb ~= nil then
			_match_0 = true
			_u6253_u5370(_u53d8_u91cfa, _u53d8_u91cfb)
		end
	end
	if not _match_0 then
		_u6253_u5370("没有匹配")
	end
end
do
	local _u8868_u683c = {
		["变量x"] = "abc"
	}
	local _type_0 = type(_u8868_u683c)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	local _match_0 = false
	if _tab_0 then
		local _u53d8_u91cfx = _u8868_u683c["变量x"]
		local _u53d8_u91cfy = _u8868_u683c["变量y"]
		if _u53d8_u91cfx ~= nil and _u53d8_u91cfy ~= nil then
			_match_0 = true
			_u6253_u5370("变量x: " .. tostring(_u53d8_u91cfx) .. " 和 变量y: " .. tostring(_u53d8_u91cfy))
		end
	end
	if not _match_0 then
		if _tab_0 then
			local _u53d8_u91cfx = _u8868_u683c["变量x"]
			if _u53d8_u91cfx ~= nil then
				_u6253_u5370("只有 变量x: " .. tostring(_u53d8_u91cfx))
			end
		end
	end
end
do
	local _u5339_u914d
	local _exp_0 = _u8868_u683c
	if 1 == _exp_0 then
		_u5339_u914d = "1"
	else
		local _type_0 = type(_exp_0)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		local _match_0 = false
		if _tab_0 then
			local _u53d8_u91cfx = _exp_0["变量x"]
			if _u53d8_u91cfx ~= nil then
				_match_0 = true
				_u5339_u914d = _u53d8_u91cfx
			end
		end
		if not _match_0 then
			if false == _exp_0 then
				_u5339_u914d = "false"
			else
				_u5339_u914d = nil
			end
		end
	end
end
do
	local _exp_0 = _u8868_u683c
	if nil == _exp_0 then
		return "无效"
	else
		local _type_0 = type(_exp_0)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		local _match_0 = false
		if _tab_0 then
			local _u53d8_u91cfa = _exp_0["变量a"]
			local _u53d8_u91cfb = _exp_0["变量b"]
			if _u53d8_u91cfa ~= nil and _u53d8_u91cfb ~= nil then
				_match_0 = true
				return tostring(_u53d8_u91cfa + _u53d8_u91cfb)
			end
		end
		if not _match_0 then
			if 1 == _exp_0 or 2 == _exp_0 or 3 == _exp_0 or 4 == _exp_0 or 5 == _exp_0 then
				return "数字 1 - 5"
			else
				local _match_1 = false
				if _tab_0 then
					local _u5339_u914d_u4efb_u4f55_u8868_u683c = _exp_0["匹配任何表格"]
					if _u5339_u914d_u4efb_u4f55_u8868_u683c == nil then
						_u5339_u914d_u4efb_u4f55_u8868_u683c = "后备"
					end
					_match_1 = true
					return _u5339_u914d_u4efb_u4f55_u8868_u683c
				end
				if not _match_1 then
					return "除非它不是一个表格，否则不应到达这里"
				end
			end
		end
	end
end
do
	local _exp_0 = _u53d8_u91cfy
	local _type_0 = type(_exp_0)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	if _tab_0 then
		local _u8868 = (function()
			local _obj_0 = _exp_0["变量x"]
			if _obj_0 ~= nil then
				return getmetatable(_obj_0)
			end
			return nil
		end)()
		if _u8868 ~= nil then
			_u6253_u5370(_u8868)
		end
	end
end
return nil
