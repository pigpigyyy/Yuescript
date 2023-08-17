do
	local _obj_0 = _u51fd_u65701
	if _obj_0 ~= nil then
		_obj_0()
	end
end
do
	local _obj_0 = _u51fd_u65702
	if _obj_0 ~= nil then
		_obj_0("参数0", 123)
	end
end
local _u53d8_u91cfx
do
	local _obj_0 = _u8868
	if _obj_0 ~= nil then
		_u53d8_u91cfx = _obj_0["值"]
	end
end
_u6253_u5370((function()
	local _obj_0 = _u5bf9_u8c61abc
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0["你好 世界"]
		if _obj_1 ~= nil then
			return _obj_1["字段xyz"]
		end
		return nil
	end
	return nil
end)())
if _u6253_u5370 and (_u53d8_u91cfx ~= nil) then
	_u6253_u5370(_u53d8_u91cfx)
end
do
	local _obj_0 = self
	if _obj_0 ~= nil then
		_obj_0["函数"](_obj_0, 998)
	end
end
do
	local _with_0
	do
		local _obj_0 = _u5bf9_u8c61abc
		if _obj_0 ~= nil then
			do
				local _obj_1 = _obj_0()
				local _obj_2 = _obj_1["函数"]
				if _obj_2 ~= nil then
					_with_0 = _obj_2(_obj_1)
				end
			end
		end
	end
	if (function()
		local _obj_0 = _with_0["函数p"]
		if _obj_0 ~= nil then
			return _obj_0(_with_0, "abc")
		end
		return nil
	end)() then
		return 123
	end
end
do
	local _des_0
	do
		local _obj_0 = _u5bf9_u8c61a
		if _obj_0 ~= nil then
			do
				local _obj_1 = _obj_0["如果"]
				if _obj_1 ~= nil then
					do
						local _obj_2 = _obj_1["然后"]
						if _obj_2 ~= nil then
							do
								local _obj_3 = _obj_2(_obj_1, 123)
								if _obj_3 ~= nil then
									_des_0 = _obj_3((function()
										local _obj_4 = self
										if _obj_4 ~= nil then
											return _obj_4["方法"](_obj_4, 998)
										end
										return nil
									end)())
								end
							end
						end
					end
				end
			end
		end
	end
	if _des_0 then
		local x_u5b57_u6bb5 = _des_0["x字段"]
		_u6253_u5370(x_u5b57_u6bb5)
	end
end
local _u7ed3_u679c = ((function()
	local _call_0 = _u5bf9_u8c61b["方法"]
	local _obj_0 = _call_0["执行"](_call_0)
	local _obj_1 = _obj_0["当"]
	if _obj_1 ~= nil then
		local _call_1 = _obj_1(_obj_0, "没问题")
		local _base_0 = _call_1["如果"](_call_1, "默认", 998)
		local _fn_0 = _base_0["函数"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)() ~= nil)
_u6253_u5370(_u7ed3_u679c)
local _u81ea_u6211_u4e3a_u4e2d_u5fc3
if (_u601d_u60f3 ~= nil) and not (_u4e16_u754c ~= nil) then
	_u81ea_u6211_u4e3a_u4e2d_u5fc3 = true
end
local _u901f_u5ea6 = 0
_u901f_u5ea6 = _u901f_u5ea6 or 15
local _u811a_u5370 = _u91ce_u4eba or "熊"
local _u4e13_u4e1a = '计算机科学'
if not (_u4e13_u4e1a ~= nil) then
	_u9009_u4fee_u8bfe_u7a0b('葡萄酒入门')
end
if (_u7a97_u4f53 ~= nil) then
	local _u73af_u5883 = '浏览器（有可能）'
end
local _u90ae_u7f16
do
	local _obj_0 = _u62bd_u5956["中奖者"]
	if _obj_0 ~= nil then
		do
			local _obj_1 = _obj_0()["地址"]
			if _obj_1 ~= nil then
				_u90ae_u7f16 = _obj_1["邮政编码"]
			end
		end
	end
end
local _u957f_u5ea6 = (function()
	local _obj_0 = utf8
	if _obj_0 ~= nil then
		return _obj_0["长度"]
	end
	return nil
end)() or (function()
	local _obj_0 = _u5b57_u7b26_u4e32
	if _obj_0 ~= nil then
		return _obj_0["长度"]
	end
	return nil
end)() or function(_u5bf9_u8c61)
	return #_u5bf9_u8c61
end
local _u53d8_u91cfa
do
	local _obj_0 = _u88681
	if _obj_0 ~= nil then
		do
			local _obj_1 = _obj_0["结束"]
			if _obj_1 ~= nil then
				_u53d8_u91cfa = _obj_1(_obj_0, 123 + (function()
					local _obj_2 = _u88682
					if _obj_2 ~= nil then
						return _obj_2["然后"](_obj_2, 456)
					end
					return nil
				end)())
			end
		end
	end
end
local _u53d8_u91cfb = ((function()
	local _obj_0 = _u88681
	if _obj_0 ~= nil then
		local _base_0 = _obj_0
		local _fn_0 = _base_0["结束"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)() ~= nil) or (function()
	local _obj_0 = _u88682
	if _obj_0 ~= nil then
		local _base_0 = _obj_0
		local _fn_0 = _base_0["然后"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end
	return nil
end)()
do
	local _with_0 = io.open("测试.txt", "w")
	if _with_0 ~= nil then
		_with_0:write("你好")
		_with_0:close()
	end
end
do
	local _obj_0 = _u8868
	if _obj_0 ~= nil then
		do
			local _obj_1 = getmetatable(_obj_0).__call
			if _obj_1 ~= nil then
				_obj_1(123)
			end
		end
	end
end
do
	local _with_0
	do
		local _obj_0 = getmetatable(_u8868)
		if _obj_0 ~= nil then
			_with_0 = getmetatable(_obj_0).__index
		end
	end
	if _with_0 ~= nil then
		_with_0["字段a"] = 1
	end
end
return nil
