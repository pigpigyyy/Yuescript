local _module_0 = { }
local a, b, c = 223, 343, 123
_module_0["a"], _module_0["b"], _module_0["c"] = a, b, c
local cool = "dad"
_module_0["cool"] = cool
local d, e, f = 3, 2, 1
_module_0[#_module_0 + 1] = d
_module_0[#_module_0 + 1] = e
_module_0[#_module_0 + 1] = f
local Something
do
	local _class_0
	local _base_0 = {
		umm = "cool"
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Something"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Something = _class_0
end
_module_0["Something"] = Something
if this then
	_module_0[#_module_0 + 1] = 232
else
	_module_0[#_module_0 + 1] = 4343
end
local What
if this then
	What = 232
else
	What = 4343
end
_module_0["What"] = What
local y
y = function()
	local hallo = 3434
end
_module_0["y"] = y
do
	local _with_0 = tmp
	local j = 2000
	_module_0[#_module_0 + 1] = _with_0
end
local cbVal
do
	local h = 100
	cbVal = f(function(x)
		return x(h)
	end)
end
_module_0["cbVal"] = cbVal
y = function()
	local h = 100
	local k = 100
end
_module_0["y"] = y
do
	local _exp_0 = h
	if 100 == _exp_0 or 150 == _exp_0 then
		_module_0[#_module_0 + 1] = 200
	elseif 200 == _exp_0 then
		_module_0[#_module_0 + 1] = 300
	else
		_module_0[#_module_0 + 1] = 0
	end
end
local Constant
do
	local _exp_0 = value
	if "good" == _exp_0 then
		Constant = 1
	elseif "better" == _exp_0 then
		Constant = 2
	elseif "best" == _exp_0 then
		Constant = 3
	end
end
_module_0["Constant"] = Constant
local item = func(123)
_module_0["item"] = item
_module_0[#_module_0 + 1] = x
f((function()
	if a then
		return b
	end
end)())
f((function()
	return 123
end)())
f((function()
	if b == a then
		return c
	end
end)())
f((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
f((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
f((function()
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[k] = v
	end
	return _tbl_0
end)())
f((function()
	local _accum_0 = { }
	local _len_0 = 1
	for k, v in pairs(tb) do
		_accum_0[_len_0] = k, v
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
f((function()
	local _accum_0 = { }
	local _len_0 = 1
	while a do
		_accum_0[_len_0] = true
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
f((function()
	a.b = 123
	return a
end)())
f((function()
	if a ~= nil then
		return a.b
	end
	return nil
end)())
f((function()
	local _base_0 = a
	local _fn_0 = _base_0.b
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
f((function()
	local A
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "A"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		A = _class_0
		return _class_0
	end
end)())
local _ = tostring((function()
	if a then
		return b
	end
end)())
_ = tostring((function()
	return 123
end)())
_ = tostring((function()
	if b == a then
		return c
	end
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[k] = v
	end
	return _tbl_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	for k, v in pairs(tb) do
		_accum_0[_len_0] = k, v
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	local _accum_0 = { }
	local _len_0 = 1
	while a do
		_accum_0[_len_0] = true
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
_ = tostring((function()
	a.b = 123
	return a
end)())
_ = tostring((function()
	if a ~= nil then
		return a.b
	end
	return nil
end)())
_ = tostring((function()
	local _base_0 = a
	local _fn_0 = _base_0.b
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
_ = tostring((function()
	local A
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function() end,
			__base = _base_0,
			__name = "A"
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		A = _class_0
		return _class_0
	end
end)())
local v1, v2, v3, v4, v5
v1 = 1
v2 = 2
_module_0["v2"] = v2
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "v4"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	v4 = _class_0
	v3 = _class_0
end
_module_0["v3"] = v3
v5 = 5
return _module_0
