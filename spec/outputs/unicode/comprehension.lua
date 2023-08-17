local _u5217_u8868 = {
	1,
	2,
	3,
	4,
	5,
	6
}
local _u8f93_u51fa
do
	local _tbl_0 = { }
	for _u5143_u7d20 in _u5bf9_u8c61_u6570_u7ec4 do
		_tbl_0[_u5143_u7d20] = _u5143_u7d20 * 2
	end
	_u8f93_u51fa = _tbl_0
end
local _u5b57_u5178x = {
	["你好"] = "世界",
	["好吧"] = 2323
}
local _u590d_u5236
do
	local _tbl_0 = { }
	for _u952e, _u503c in pairs(_u5bf9_u8c61x) do
		if _u952e ~= "好吧" then
			_tbl_0[_u952e] = _u503c
		end
	end
	_u590d_u5236 = _tbl_0
end
local __u65e0_u6548_u53d8_u91cf
do
	local _tbl_0 = { }
	for _u5143_u7d20 in _u662f_u7684 do
		local _key_0, _val_0 = _u89e3_u5305(_u5143_u7d20)
		_tbl_0[_key_0] = _val_0
	end
	__u65e0_u6548_u53d8_u91cf = _tbl_0
end
do
	local _tbl_0 = { }
	local _list_0 = _u662f_u7684
	for _index_0 = 1, #_list_0 do
		local _u5143_u7d20 = _list_0[_index_0]
		local _key_0, _val_0 = _u89e3_u5305(_u5143_u7d20)
		_tbl_0[_key_0] = _val_0
	end
	__u65e0_u6548_u53d8_u91cf = _tbl_0
end
do
	local _tbl_0 = { }
	for _u5143_u7d20 in _u662f_u7684 do
		local _key_0, _val_0 = _u5143_u7d20_u5143_u7d20
		_tbl_0[_key_0] = _val_0
	end
	__u65e0_u6548_u53d8_u91cf = _tbl_0
end
do
	local _tbl_0 = { }
	local _list_0 = {
		{
			1,
			2
		},
		{
			3,
			4
		}
	}
	for _index_0 = 1, #_list_0 do
		local x_u5bf9_u8c61 = _list_0[_index_0]
		local _key_0, _val_0 = _u89e3_u5305((function()
			local _accum_0 = { }
			local _len_0 = 1
			for _u8ba1_u6570, _u5bf9_u8c61 in ipairs(x_u5bf9_u8c61) do
				_accum_0[_len_0] = _u5bf9_u8c61 * _u8ba1_u6570
				_len_0 = _len_0 + 1
			end
			return _accum_0
		end)())
		_tbl_0[_key_0] = _val_0
	end
	__u65e0_u6548_u53d8_u91cf = _tbl_0
end
local _u53d8_u91cf1
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		_accum_0[_len_0] = _u8ba1_u6570
		_len_0 = _len_0 + 1
	end
	_u53d8_u91cf1 = _accum_0
end
local _u53d8_u91cf2
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		if _u8ba1_u6570 % 2 == 1 then
			_accum_0[_len_0] = _u8ba1_u6570
			_len_0 = _len_0 + 1
		end
	end
	_u53d8_u91cf2 = _accum_0
end
local _u53d8_u91cfaa
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570x = 1, 10 do
		for _u8ba1_u6570y = 5, 14 do
			_accum_0[_len_0] = {
				_u8ba1_u6570x,
				_u8ba1_u6570y
			}
			_len_0 = _len_0 + 1
		end
	end
	_u53d8_u91cfaa = _accum_0
end
local _u53d8_u91cfbb
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u5143_u7d20 in _u5bf9_u8c61y do
		for _u8ba1_u6570i = 1, 10 do
			_accum_0[_len_0] = _u5bf9_u8c61y
			_len_0 = _len_0 + 1
		end
	end
	_u53d8_u91cfbb = _accum_0
end
local _u53d8_u91cfcc
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		for _u5143_u7d20 in _u5bf9_u8c61y do
			_accum_0[_len_0] = _u5bf9_u8c61y
			_len_0 = _len_0 + 1
		end
	end
	_u53d8_u91cfcc = _accum_0
end
local _u53d8_u91cfdd
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		if _u9177 then
			for _u5143_u7d20 in _u5bf9_u8c61y do
				if _u53d8_u91cfx > 3 then
					if _u53d8_u91cfc + 3 then
						_accum_0[_len_0] = _u5bf9_u8c61y
						_len_0 = _len_0 + 1
					end
				end
			end
		end
	end
	_u53d8_u91cfdd = _accum_0
end
do
	local _tbl_0 = { }
	for _u8ba1_u6570 = 1, 10 do
		_tbl_0["你好"] = "世界"
	end
	__u65e0_u6548_u53d8_u91cf = _tbl_0
end
local _u53d8_u91cfj
do
	local _accum_0 = { }
	local _len_0 = 1
	for _des_0 in _u5f88_u591a_u4e1c_u897f do
		local _u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c = _des_0[1], _des_0[2], _des_0[3]
		_accum_0[_len_0] = _u5143_u7d20a
		_len_0 = _len_0 + 1
	end
	_u53d8_u91cfj = _accum_0
end
local _u53d8_u91cfk
do
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = _u5f88_u591a_u4e1c_u897f
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local _u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c = _des_0[1], _des_0[2], _des_0[3]
		_accum_0[_len_0] = _u5143_u7d20a
		_len_0 = _len_0 + 1
	end
	_u53d8_u91cfk = _accum_0
end
local _u53d8_u91cfi
do
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = _u5f88_u591a_u4e1c_u897f
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local _u4f60_u597d, _u4e16_u754c = _des_0["你好"], _des_0["世界"]
		_accum_0[_len_0] = _u4f60_u597d
		_len_0 = _len_0 + 1
	end
	_u53d8_u91cfi = _accum_0
end
local _u53d8_u91cfhj
do
	local _tbl_0 = { }
	for _des_0 in _u5f88_u591a_u4e1c_u897f do
		local _u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c = _des_0[1], _des_0[2], _des_0[3]
		_tbl_0[_u5143_u7d20a] = _u5143_u7d20c
	end
	_u53d8_u91cfhj = _tbl_0
end
local _u53d8_u91cfhk
do
	local _tbl_0 = { }
	local _list_0 = _u5f88_u591a_u4e1c_u897f
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local _u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c = _des_0[1], _des_0[2], _des_0[3]
		_tbl_0[_u5143_u7d20a] = _u5143_u7d20c
	end
	_u53d8_u91cfhk = _tbl_0
end
local _u53d8_u91cfhi
do
	local _tbl_0 = { }
	local _list_0 = _u5f88_u591a_u4e1c_u897f
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local _u4f60_u597d, _u4e16_u754c = _des_0["你好"], _des_0["世界"]
		_tbl_0[_u4f60_u597d] = _u4e16_u754c
	end
	_u53d8_u91cfhi = _tbl_0
end
for _des_0 in _u5f88_u591a_u4e1c_u897f do
	local _u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c = _des_0[1], _des_0[2], _des_0[3]
	_u6ca1_u9519(_u5143_u7d20a, _u5143_u7d20b, _u5143_u7d20c)
end
do
	local _accum_0 = { }
	local _len_0 = 1
	local _max_0 = 3 + 4
	for _index_0 = 1 + 2, _max_0 < 0 and #_u5217_u8868 + _max_0 or _max_0 do
		local _u9879_u76ee = _u5217_u8868[_index_0]
		_accum_0[_len_0] = _u9879_u76ee
		_len_0 = _len_0 + 1
	end
	__u65e0_u6548_u53d8_u91cf = _accum_0
end
do
	local _accum_0 = { }
	local _len_0 = 1
	local _max_0 = 2 - _u4e1c_u897f[4]
	for _index_0 = _u4f60_u597d() * 4, _max_0 < 0 and #_u5217_u8868 + _max_0 or _max_0 do
		local _u9879_u76ee = _u5217_u8868[_index_0]
		_accum_0[_len_0] = _u9879_u76ee
		_len_0 = _len_0 + 1
	end
	__u65e0_u6548_u53d8_u91cf = _accum_0
end
do
	local _accum_0 = { }
	local _len_0 = 1
	for _u9879_u76ee in _u5217_u8868 do
		if _u9879_u76ee ~= nil then
			_accum_0[_len_0] = _u9879_u76ee["调用"](_u9879_u76ee, 123)
		end
		_len_0 = _len_0 + 1
	end
	_u5217_u8868 = _accum_0
end
return nil
