local _
_ = function()
	local _list_0 = _u7269_u54c1
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfx = _list_0[_index_0]
		_ = _u53d8_u91cfx
	end
end
_ = function()
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = _u7269_u54c1
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfx = _list_0[_index_0]
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
do
	local _list_0 = _u7269_u54c1
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfx = _list_0[_index_0]
		return _u53d8_u91cfx
	end
end
do
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = _u7269_u54c1
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfx = _list_0[_index_0]
		_accum_0[_len_0] = _u53d8_u91cfx
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
do
	local _tbl_0 = { }
	local _list_0 = _u7269_u54c1
	for _index_0 = 1, #_list_0 do
		local _u53d8_u91cfx, _u53d8_u91cfy = _list_0[_index_0]
		_tbl_0[_u53d8_u91cfx] = _u53d8_u91cfy
	end
	return _tbl_0
end
_ = function()
	if _u53d8_u91cfa then
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	elseif _u53d8_u91cfb then
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	else
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	end
end
do
	if _u53d8_u91cfa then
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	elseif _u53d8_u91cfb then
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	else
		if _u53d8_u91cfa then
			return _u53d8_u91cfa
		else
			return _u53d8_u91cfb
		end
	end
end
do
	return {
		["值"] = _u503c,
		["物品A"] = 123,
		["物品B"] = "abc"
	}
end
do
	local _tab_0 = {
		1,
		2
	}
	local _idx_0 = 1
	for _key_0, _value_0 in pairs(_u4e09) do
		if _idx_0 == _key_0 then
			_tab_0[#_tab_0 + 1] = _value_0
			_idx_0 = _idx_0 + 1
		else
			_tab_0[_key_0] = _value_0
		end
	end
	_tab_0[#_tab_0 + 1] = 4
	return _tab_0
end
_ = function()
	local _base_0 = _u53d8_u91cfa
	local _fn_0 = _base_0["变量b"]
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end
do
	local _base_0 = _u53d8_u91cfa
	local _fn_0 = _base_0["变量b"]
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end
