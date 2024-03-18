local _anon_func_0 = function(a)
	a:func()
	return a
end
local _anon_func_1 = function(a, ...)
	a:func(...)
	return a
end
local _anon_func_2 = function(items)
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = items[i]
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_3 = function(items, ...)
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = items[i](...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_4 = function(items)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #items do
		local item = items[_index_0]
		_accum_0[_len_0] = item
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_5 = function(items, ...)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #items do
		local item = items[_index_0]
		_accum_0[_len_0] = item(...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_6 = function(setmetatable, func)
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
		local self = _class_0;
		func()
		A = _class_0
		return _class_0
	end
end
local _anon_func_7 = function(setmetatable, func, ...)
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
		local self = _class_0;
		func(...)
		A = _class_0
		return _class_0
	end
end
local _anon_func_8 = function(pairs, tb)
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[k] = v
	end
	return _tbl_0
end
local _anon_func_9 = function(pairs, tb, ...)
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[k] = v(...)
	end
	return _tbl_0
end
local _anon_func_10 = function(items)
	local _tbl_0 = { }
	for _index_0 = 1, #items do
		local item = items[_index_0]
		_tbl_0[item] = true
	end
	return _tbl_0
end
local _anon_func_11 = function(items, ...)
	local _tbl_0 = { }
	for _index_0 = 1, #items do
		local item = items[_index_0]
		_tbl_0[item(...)] = true
	end
	return _tbl_0
end
local _anon_func_12 = function(func)
	do
		return func()
	end
end
local _anon_func_13 = function(func, ...)
	do
		return func(...)
	end
end
local _anon_func_14 = function(func)
	local _accum_0 = { }
	local _len_0 = 1
	while false do
		_accum_0[_len_0] = func()
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_15 = function(func, ...)
	local _accum_0 = { }
	local _len_0 = 1
	while false do
		_accum_0[_len_0] = func(...)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_16 = function(func)
	if false then
		return func()
	end
end
local _anon_func_17 = function(func, ...)
	if false then
		return func(...)
	end
end
local _anon_func_18 = function(func)
	if not true then
		return func()
	end
end
local _anon_func_19 = function(func, ...)
	if not true then
		return func(...)
	end
end
local _anon_func_20 = function(x, func)
	if "abc" == x then
		return func()
	end
end
local _anon_func_21 = function(x, func, ...)
	if "abc" == x then
		return func(...)
	end
end
local _anon_func_22 = function(func)
	if func ~= nil then
		return func()
	end
	return nil
end
local _anon_func_23 = function(func, ...)
	if func ~= nil then
		return func(...)
	end
	return nil
end
local _anon_func_24 = function(select, print, ...)
	do
		print(select("#", ...))
		return print(...)
	end
end
local _anon_func_25 = function(print, ...)
	do
		return print(...)
	end
end
local _anon_func_26 = function(x, tb, tb2)
	if 1 == x then
		tb.x = 123
		return tb
	else
		return tb2
	end
end
local _anon_func_27 = function(print, ...)
	do
		return print(...)
	end
end
local _anon_func_28 = function(cond)
	if cond then
		return 3, 4, 5
	end
end
local _anon_func_29 = function(print, _arg_0, ...)
	do
		local tb = _arg_0
		return print(...)
	end
end
local join
join = function(...)
	f_with(_anon_func_0(a))
	f_with(_anon_func_1(a, ...))
	f_listcomp(_anon_func_2(items))
	f_listcomp(_anon_func_3(items, ...))
	f_listcomp(_anon_func_4(items))
	f_listcomp(_anon_func_5(items, ...))
	f_class(_anon_func_6(setmetatable, func))
	f_class(_anon_func_7(setmetatable, func, ...))
	f_tblcomp(_anon_func_8(pairs, tb))
	f_tblcomp(_anon_func_9(pairs, tb, ...))
	f_tblcomp(_anon_func_10(items))
	f_tblcomp(_anon_func_11(items, ...))
	f_do(_anon_func_12(func))
	f_do(_anon_func_13(func, ...))
	f_while(_anon_func_14(func))
	f_while(_anon_func_15(func, ...))
	f_if(_anon_func_16(func))
	f_if(_anon_func_17(func, ...))
	f_unless(_anon_func_18(func))
	f_unless(_anon_func_19(func, ...))
	f_switch(_anon_func_20(x, func))
	f_switch(_anon_func_21(x, func, ...))
	f_eop(_anon_func_22(func))
	f_eop(_anon_func_23(func, ...))
	f_colon((function()
		local _base_0 = f()
		local _fn_0 = _base_0.func
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end)())
	f_colon((function(...)
		local _base_0 = f(...)
		local _fn_0 = _base_0.func
		return _fn_0 and function(...)
			return _fn_0(_base_0, ...)
		end
	end)(...))
	local _
	_ = function()
		local list = {
			1,
			2,
			3,
			4,
			5
		}
		local fn
		fn = function(ok)
			return ok, table.unpack(list)
		end
		return (function(_arg_0, ...)
			local ok = _arg_0
			print(ok, ...)
			local fn_many_args
			fn_many_args = function()
				return 10, nil, 20, nil, 30
			end
			return _anon_func_24(select, print, fn_many_args())
		end)(fn(true))
	end
	do
		_anon_func_25(print, _anon_func_26(x, tb, tb2))
	end
	do
		_anon_func_27(print, 1, 2, _anon_func_28(cond))
	end
	do
		_anon_func_29(print, {
			name = "abc",
			value = 123
		})
	end
	return nil
end
