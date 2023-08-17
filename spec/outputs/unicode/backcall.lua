do
	_u6620_u5c04({
		1,
		2,
		3
	}, function(_u9879_u76ee)
		return _u9879_u76ee * 2
	end)
end
do
	_u6620_u5c04(function(_u9879_u76ee)
		return _u9879_u76ee * 2
	end, {
		1,
		2,
		3
	})
end
do
	_u8fc7_u6ee4(function(_u9879_u76ee)
		return _u9879_u76ee > 2
	end, (function()
		return _u6620_u5c04(function(_u9879_u76ee)
			return _u9879_u76ee * 2
		end, {
			1,
			2,
			3,
			4
		})
	end)())
end
do
	do
		local _obj_0 = _u7f51_u7edc
		if _obj_0 ~= nil then
			_obj_0["获取"]("测试地址", function(_u6570_u636e)
				local _call_0 = _u7f51_u9875_u4f53[".结果"]
				_call_0["网页"](_call_0, _u6570_u636e)
				return _u7f51_u7edc["请求"]("测试地址", _u6570_u636e, function(_u662f_u5426_u5904_u7406_u6210_u529f)
					local _call_1 = _u7f51_u9875_u4f53[".结果"]
					_call_1["添加"](_call_1, _u662f_u5426_u5904_u7406_u6210_u679c)
					return _u8bbe_u7f6e_u8d85_u65f6_u56de_u8c03(1000, function()
						return _u6253_u5370("完成")
					end)
				end)
			end)
		end
	end
end
do
	_u540c_u6b65_u72b6_u6001(function()
		return _u5f02_u6b65_u52a0_u8f7d("文件.yue", function(_u9519_u8bef_u4fe1_u606f, _u6570_u636e)
			if _u6570_u636e == nil then
				_u6570_u636e = "nil"
			end
			if _u9519_u8bef_u4fe1_u606f then
				_u6253_u5370(_u9519_u8bef_u4fe1_u606f)
				return
			end
			return _u5f02_u6b65_u7f16_u8bd1(_u6570_u636e, function(_u4ee3_u7801)
				local _u7ed3_u679c_u51fd_u6570 = _u52a0_u8f7d_u5b57_u7b26_u4e32(_u4ee3_u7801)
				return _u7ed3_u679c_u51fd_u6570()
			end)
		end)
	end)
end
do
	_u51fd_u65701(function()
		return _u51fd_u65702(function()
			do
				_u51fd_u65703(function()
					return _u51fd_u65704(function() end)
				end)
			end
			return _u51fd_u65705(function()
				return _u51fd_u65706(function()
					return _u51fd_u65707()
				end)
			end)
		end)
	end)
end
do
	local _u7ed3_u679c, _u6d88_u606f
	do
		do
			local _obj_0 = _u5f02_u6b65_u63a5_u6536("文件名.txt", function(_u6570_u636e)
				_u6253_u5370(_u6570_u636e)
				return _u5f02_u6b65_u5904_u7406(_u6570_u636e, function(_u4fe1_u606f)
					return _u68c0_u67e5(_u4fe1_u606f)
				end)
			end)
			_u7ed3_u679c, _u6d88_u606f = _obj_0["结果"], _obj_0["消息"]
		end
	end
	_u6253_u5370(_u7ed3_u679c, _u6d88_u606f)
	local _u603b_u5927_u5c0f = _u5408_u5e76(((function()
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = _u6587_u4ef6_u6570_u7ec4
		for _index_0 = 1, #_list_0 do
			local _u6587_u4ef6 = _list_0[_index_0]
			_accum_0[_len_0] = _u5f02_u6b65_u52a0_u8f7d(_u6587_u4ef6, function(_u6570_u636e)
				return _u52a0_u5165_u7f13_u5b58(_u6587_u4ef6, _u6570_u636e)
			end)
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)()), 0, function(_u5bf9_u8c61a, _u5bf9_u8c61b)
		return _u5bf9_u8c61a + _u5bf9_u8c61b
	end)
end
local _u5c5e_u6027A
do
	_u5c5e_u6027A = _u5c5e_u6027_u5b9a_u4e49(function(self)
		return self["_值"]
	end, function(self, _u503c)
		_u6253_u5370("之前值: " .. tostring(self["_值"]))
		_u6253_u5370("最新值: " .. tostring(_u503c))
		self["_值"] = _u503c
	end)
end
local _u5c5e_u6027B
do
	_u5c5e_u6027B = _u5c5e_u6027_u5b9a_u4e49(function(self)
		return self["_值"]
	end, function(self, _u503c)
		_u6253_u5370("之前值: " .. tostring(self["_值"]))
		_u6253_u5370("最新值: " .. tostring(_u503c))
		self["_值"] = _u503c
	end)
end
_u8b66_u544a("嗨")
local _u53d8_u91cfx, _u53d8_u91cfy, _u53d8_u91cfz
do
	_u53d8_u91cfx = (_u53d8_u91cfa) < -_u53d8_u91cfb
end
do
	_u53d8_u91cfx, _u53d8_u91cfy, _u53d8_u91cfz = _u53d8_u91cfb(function(_u53d8_u91cfa) end)
end
do
	_u53d8_u91cfx, _u53d8_u91cfy, _u53d8_u91cfz = _u53d8_u91cfb(function(_u53d8_u91cfa) end)
end
do
	_u53d8_u91cfx = _u53d8_u91cfa <= _u53d8_u91cfb
end
do
	_u53d8_u91cfx, _u53d8_u91cfy, _u53d8_u91cfz = _u53d8_u91cfb(function(self, _u53d8_u91cfa) end)
end
return nil
