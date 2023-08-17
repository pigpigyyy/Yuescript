local _module_0 = setmetatable({ }, { })
local _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc = 223, 343, 123
_module_0["变量a"], _module_0["变量b"], _module_0["变量c"] = _u53d8_u91cfa, _u53d8_u91cfb, _u53d8_u91cfc
local _u9177 = "🧧"
_module_0["酷"] = _u9177
local _u53d8_u91cfd, _u53d8_u91cfe, _u53d8_u91cff = 3, 2, 1
_module_0[#_module_0 + 1] = _u53d8_u91cfd
_module_0[#_module_0 + 1] = _u53d8_u91cfe
_module_0[#_module_0 + 1] = _u53d8_u91cff
local _u67d0_u7c7b
do
	local _class_0
	local _base_0 = {
		["嗯"] = "酷"
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "某类"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u67d0_u7c7b = _class_0
end
_module_0["_u67d0_u7c7b"] = _u67d0_u7c7b
if _u8fd9_u4e2a then
	_module_0[#_module_0 + 1] = 232
else
	_module_0[#_module_0 + 1] = 4343
end
local _u4ec0_u4e48
if _u8fd9_u4e2a then
	_u4ec0_u4e48 = 232
else
	_u4ec0_u4e48 = 4343
end
_module_0["什么"] = _u4ec0_u4e48
local _u53d8_u91cfy
_u53d8_u91cfy = function()
	local _u4f60_u597d = 3434
end
_module_0["变量y"] = _u53d8_u91cfy
do
	local _with_0 = _u4e34_u65f6
	local _u53d8_u91cfj = 2000
	_module_0[#_module_0 + 1] = _with_0
end
local cb_u7684_u503c
do
	local _u53d8_u91cfh = 100
	cb_u7684_u503c = _u51fd_u6570(function(_u53c2_u6570)
		return _u53c2_u6570(h_u53d8_u91cf)
	end)
end
_module_0["cb的值"] = cb_u7684_u503c
local _u4e2b_u4e2b
_u4e2b_u4e2b = function()
	local _u53d8_u91cfh = 100
	local _u53d8_u91cfk = 100
end
_module_0["丫丫"] = _u4e2b_u4e2b
do
	local _exp_0 = _u53d8_u91cfh
	if 100 == _exp_0 or 150 == _exp_0 then
		_module_0[#_module_0 + 1] = 200
	elseif 200 == _exp_0 then
		_module_0[#_module_0 + 1] = 300
	else
		_module_0[#_module_0 + 1] = 0
	end
end
local _u5e38_u91cf
do
	local _exp_0 = _u503c
	if "好" == _exp_0 then
		_u5e38_u91cf = 1
	elseif "更好" == _exp_0 then
		_u5e38_u91cf = 2
	elseif "最好" == _exp_0 then
		_u5e38_u91cf = 3
	end
end
_module_0["常量"] = _u5e38_u91cf
local _u9879_u76ee = _u51fd_u6570(123)
_module_0["项目"] = _u9879_u76ee
_module_0[#_module_0 + 1] = _u53d8_u91cfx
_u51fd_u6570((function()
	if _u6761_u4ef6a then
		return _u503cb
	end
end)())
_u51fd_u6570((function()
	return 123
end)())
_u51fd_u6570((function()
	local _exp_0 = _u5bf9_u8c61a
	if _u53d8_u91cfb == _exp_0 then
		return _u503cc
	end
end)())
_u51fd_u6570((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		_accum_0[_len_0] = _u8ba1_u6570
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_u51fd_u6570((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		_accum_0[_len_0] = _u8ba1_u6570
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_u51fd_u6570((function()
	local _tbl_0 = { }
	for _u952e, _u503c in pairs(_u8868) do
		_tbl_0[_u952e] = _u503c
	end
	return _tbl_0
end)())
_u51fd_u6570((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u952e, _u503c in pairs(_u8868) do
		_accum_0[_len_0] = {
			_u952e,
			_u503c
		}
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_u51fd_u6570((function()
	local _accum_0 = { }
	local _len_0 = 1
	while _u6761_u4ef6a do
		_accum_0[_len_0] = true
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_u51fd_u6570((function()
	local _with_0 = _u6761_u4ef6a
	_with_0["字段b"] = 123
	return _with_0
end)())
_u51fd_u6570((function()
	local _obj_0 = _u5bf9_u8c61a
	if _obj_0 ~= nil then
		return _obj_0["字段b"]
	end
	return nil
end)())
_u51fd_u6570((function()
	local _base_0 = _u5bf9_u8c61a
	local _fn_0 = _base_0["字段b"]
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
_u51fd_u6570((function()
	local A_u7c7b
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "A类"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		A_u7c7b = _class_0
		return _class_0
	end
end)())
local _ = tostring((function()
	if _u6761_u4ef6a then
		return _u503cb
	end
end)())
_ = tostring((function()
	return 123
end)())
_ = tostring((function()
	local _exp_0 = _u5bf9_u8c61a
	if _u53d8_u91cfb == _exp_0 then
		return _u503cc
	end
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		_accum_0[_len_0] = _u8ba1_u6570
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u8ba1_u6570 = 1, 10 do
		_accum_0[_len_0] = _u8ba1_u6570
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _tbl_0 = { }
	for _u952e, _u503c in pairs(_u8868) do
		_tbl_0[_u952e] = _u503c
	end
	return _tbl_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for _u952e, _u503c in pairs(_u8868) do
		_accum_0[_len_0] = {
			_u952e,
			_u503c
		}
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	while _u6761_u4ef6a do
		_accum_0[_len_0] = true
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _with_0 = _u5bf9_u8c61a
	_with_0["字段b"] = 123
	return _with_0
end)())
_ = tostring((function()
	local _obj_0 = _u5bf9_u8c61a
	if _obj_0 ~= nil then
		return _obj_0["字段b"]
	end
	return nil
end)())
_ = tostring((function()
	local _base_0 = _u5bf9_u8c61a
	local _fn_0 = _base_0["字段b"]
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
_ = tostring((function()
	local A_u7c7b
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "A类"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		A_u7c7b = _class_0
		return _class_0
	end
end)())
local _u53d8_u91cf1, _u53d8_u91cf2, _u53d8_u91cf3, _u7c7b4, _u53d8_u91cf5
_u53d8_u91cf1 = 1
_u53d8_u91cf2 = 2
_module_0["变量2"] = _u53d8_u91cf2
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "类4"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	_u7c7b4 = _class_0
	_u53d8_u91cf3 = _class_0
end
_module_0["变量3"] = _u53d8_u91cf3
_u53d8_u91cf5 = 5
getmetatable(_module_0)["🀄️"] = 1
getmetatable(_module_0).__name = "导出"
getmetatable(_module_0).__call = function(self)
	return { }
end
_module_0["导出-任意-名称"] = 123
return _module_0
