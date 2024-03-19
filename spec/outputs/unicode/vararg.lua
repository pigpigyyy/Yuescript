local _anon_func_0 = function(_u53d8_u91cfa)
	_u53d8_u91cfa["函数"](_u53d8_u91cfa)
	return _u53d8_u91cfa
end
local _anon_func_1 = function(_u53d8_u91cfa, ...)
	_u53d8_u91cfa["函数"](_u53d8_u91cfa, ...)
	return _u53d8_u91cfa
end
local _anon_func_2 = function(_u9879_u76ee)
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = _u9879_u76ee[i]
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_3 = function(_u9879_u76ee, ...)
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = _u9879_u76ee[i](...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_4 = function(_u9879_u76ee)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #_u9879_u76ee do
		local _u7269_u54c1 = _u9879_u76ee[_index_0]
		_accum_0[_len_0] = _u7269_u54c1
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_5 = function(_u9879_u76ee, ...)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #_u9879_u76ee do
		local _u7269_u54c1 = _u9879_u76ee[_index_0]
		_accum_0[_len_0] = _u7269_u54c1(...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_6 = function(_u51fd_u6570, setmetatable)
	local _u53d8_u91cfA
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "变量A"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		local self = _class_0;
		_u51fd_u6570()
		_u53d8_u91cfA = _class_0
		return _class_0
	end
end
local _anon_func_7 = function(_u51fd_u6570, setmetatable, ...)
	local _u53d8_u91cfA
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "变量A"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		local self = _class_0;
		_u51fd_u6570(...)
		_u53d8_u91cfA = _class_0
		return _class_0
	end
end
local _anon_func_8 = function(_u8868, pairs)
	local _tbl_0 = { }
	for _u952e, _u503c in pairs(_u8868) do
		_tbl_0[_u952e] = _u503c
	end
	return _tbl_0
end
local _anon_func_9 = function(_u8868, pairs, ...)
	local _tbl_0 = { }
	for _u952e, _u503c in pairs(_u8868) do
		_tbl_0[_u952e] = _u503c(...)
	end
	return _tbl_0
end
local _anon_func_10 = function(_u9879_u76ee)
	local _tbl_0 = { }
	for _index_0 = 1, #_u9879_u76ee do
		local _u7269_u54c1 = _u9879_u76ee[_index_0]
		_tbl_0[_u7269_u54c1] = true
	end
	return _tbl_0
end
local _anon_func_11 = function(_u9879_u76ee, ...)
	local _tbl_0 = { }
	for _index_0 = 1, #_u9879_u76ee do
		local _u7269_u54c1 = _u9879_u76ee[_index_0]
		_tbl_0[_u7269_u54c1(...)] = true
	end
	return _tbl_0
end
local _anon_func_12 = function(_u51fd_u6570)
	do
		return _u51fd_u6570()
	end
end
local _anon_func_13 = function(_u51fd_u6570, ...)
	do
		return _u51fd_u6570(...)
	end
end
local _anon_func_14 = function(_u51fd_u6570)
	local _accum_0 = { }
	local _len_0 = 1
	while false do
		_accum_0[_len_0] = _u51fd_u6570()
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_15 = function(_u51fd_u6570, ...)
	local _accum_0 = { }
	local _len_0 = 1
	while false do
		_accum_0[_len_0] = _u51fd_u6570(...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_16 = function(_u51fd_u6570)
	if false then
		return _u51fd_u6570()
	end
end
local _anon_func_17 = function(_u51fd_u6570, ...)
	if false then
		return _u51fd_u6570(...)
	end
end
local _anon_func_18 = function(_u51fd_u6570)
	if not true then
		return _u51fd_u6570()
	end
end
local _anon_func_19 = function(_u51fd_u6570, ...)
	if not true then
		return _u51fd_u6570(...)
	end
end
local _anon_func_20 = function(_u51fd_u6570, _u53d8_u91cfx)
	if "abc" == _u53d8_u91cfx then
		return _u51fd_u6570()
	end
end
local _anon_func_21 = function(_u51fd_u6570, _u53d8_u91cfx, ...)
	if "abc" == _u53d8_u91cfx then
		return _u51fd_u6570(...)
	end
end
local _anon_func_22 = function(_u51fd_u6570)
	if _u51fd_u6570 ~= nil then
		return _u51fd_u6570()
	end
	return nil
end
local _anon_func_23 = function(_u51fd_u6570, ...)
	if _u51fd_u6570 ~= nil then
		return _u51fd_u6570(...)
	end
	return nil
end
local _anon_func_24 = function(_u6253_u5370, select, ...)
	do
		_u6253_u5370(select("#", ...))
		return _u6253_u5370(...)
	end
end
local _anon_func_25 = function(_u6253_u5370, ...)
	do
		return _u6253_u5370(...)
	end
end
local _anon_func_26 = function(_u53d8_u91cfx, _u8868, _u88682)
	if 1 == _u53d8_u91cfx then
		_u8868["变量x"] = 123
		return _u8868
	else
		return _u88682
	end
end
local _anon_func_27 = function(_u6253_u5370, ...)
	do
		return _u6253_u5370(...)
	end
end
local _anon_func_28 = function(_u6761_u4ef6)
	if _u6761_u4ef6 then
		return 3, 4, 5
	end
end
local _anon_func_29 = function(_u6253_u5370, _arg_0, ...)
	do
		local _u8868 = _arg_0
		return _u6253_u5370(...)
	end
end
local _u8fde_u63a5
_u8fde_u63a5 = function(...)
	_u5e26_u6709(_anon_func_0(_u53d8_u91cfa))
	_u5e26_u6709(_anon_func_1(_u53d8_u91cfa, ...))
	_u5217_u8868_u751f_u6210(_anon_func_2(_u9879_u76ee))
	_u5217_u8868_u751f_u6210(_anon_func_3(_u9879_u76ee, ...))
	_u5217_u8868_u751f_u6210(_anon_func_4(_u9879_u76ee))
	_u5217_u8868_u751f_u6210(_anon_func_5(_u9879_u76ee, ...))
	_u7c7b_u751f_u6210(_anon_func_6(_u51fd_u6570, setmetatable))
	_u7c7b_u751f_u6210(_anon_func_7(_u51fd_u6570, setmetatable, ...))
	_u8868_u751f_u6210(_anon_func_8(_u8868, pairs))
	_u8868_u751f_u6210(_anon_func_9(_u8868, pairs, ...))
	_u8868_u751f_u6210(_anon_func_10(_u9879_u76ee))
	_u8868_u751f_u6210(_anon_func_11(_u9879_u76ee, ...))
	_u505a_u64cd_u4f5c(_anon_func_12(_u51fd_u6570))
	_u505a_u64cd_u4f5c(_anon_func_13(_u51fd_u6570, ...))
	_u5f53_u64cd_u4f5c(_anon_func_14(_u51fd_u6570))
	_u5f53_u64cd_u4f5c(_anon_func_15(_u51fd_u6570, ...))
	_u5982_u679c_u64cd_u4f5c(_anon_func_16(_u51fd_u6570))
	_u5982_u679c_u64cd_u4f5c(_anon_func_17(_u51fd_u6570, ...))
	_u9664_u975e_u64cd_u4f5c(_anon_func_18(_u51fd_u6570))
	_u9664_u975e_u64cd_u4f5c(_anon_func_19(_u51fd_u6570, ...))
	_u5207_u6362_u64cd_u4f5c(_anon_func_20(_u51fd_u6570, _u53d8_u91cfx))
	_u5207_u6362_u64cd_u4f5c(_anon_func_21(_u51fd_u6570, _u53d8_u91cfx, ...))
	_u8868_u8fbe_u5f0f_u64cd_u4f5c(_anon_func_22(_u51fd_u6570))
	_u8868_u8fbe_u5f0f_u64cd_u4f5c(_anon_func_23(_u51fd_u6570, ...))
	_u5192_u53f7((function()
		local _base_0 = f()
		local _fn_0 = _base_0["函数"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end)())
	_u5192_u53f7((function(...)
		local _base_0 = f(...)
		local _fn_0 = _base_0["函数"]
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end)(...))
	local _
	_ = function()
		local _u5217_u8868 = {
			1,
			2,
			3,
			4,
			5
		}
		local _u51fd_u6570_u540d
		_u51fd_u6570_u540d = function(_u786e_u5b9a)
			return _u786e_u5b9a, table.unpack(_u5217_u8868)
		end
		return (function(_arg_0, ...)
			local _u786e_u5b9a = _arg_0
			_u6253_u5370(_u786e_u5b9a, ...)
			local _u591a_u53c2_u6570_u51fd_u6570
			_u591a_u53c2_u6570_u51fd_u6570 = function()
				return 10, nil, 20, nil, 30
			end
			return _anon_func_24(_u6253_u5370, select, _u591a_u53c2_u6570_u51fd_u6570())
		end)(_u51fd_u6570_u540d(true))
	end
	do
		_anon_func_25(_u6253_u5370, _anon_func_26(_u53d8_u91cfx, _u8868, _u88682))
	end
	do
		_anon_func_27(_u6253_u5370, 1, 2, _anon_func_28(_u6761_u4ef6))
	end
	do
		_anon_func_29(_u6253_u5370, {
			["名字"] = "abc",
			["值"] = 123
		})
	end
	return nil
end
