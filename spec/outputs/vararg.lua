local join
join = function(...)
	f_with((function()
		local _with_0 = a
		_with_0:func()
		return _with_0
	end)())
	f_with((function(...)
		local _with_0 = a
		_with_0:func(...)
		return _with_0
	end)(...))
	f_listcomp((function()
		local _accum_0 = { }
		local _len_0 = 1
		for i = 1, 10 do
			_accum_0[_len_0] = items[i]
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)())
	f_listcomp((function(...)
		local _accum_0 = { }
		local _len_0 = 1
		for i = 1, 10 do
			_accum_0[_len_0] = items[i](...)
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)(...))
	f_listcomp((function()
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = items
		for _index_0 = 1, #_list_0 do
			local item = _list_0[_index_0]
			_accum_0[_len_0] = item
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)())
	f_listcomp((function(...)
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = items
		for _index_0 = 1, #_list_0 do
			local item = _list_0[_index_0]
			_accum_0[_len_0] = item(...)
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)(...))
	f_class((function()
		local A
		do
			local _class_0
			local _base_0 = { }
			_base_0.__index = _base_0
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
	end)())
	f_class((function(...)
		local A
		do
			local _class_0
			local _base_0 = { }
			_base_0.__index = _base_0
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
	end)(...))
	f_tblcomp((function()
		local _tbl_0 = { }
		for k, v in pairs(tb) do
			_tbl_0[k] = v
		end
		return _tbl_0
	end)())
	f_tblcomp((function(...)
		local _tbl_0 = { }
		for k, v in pairs(tb) do
			_tbl_0[k] = v(...)
		end
		return _tbl_0
	end)(...))
	f_tblcomp((function()
		local _tbl_0 = { }
		local _list_0 = items
		for _index_0 = 1, #_list_0 do
			local item = _list_0[_index_0]
			_tbl_0[item] = true
		end
		return _tbl_0
	end)())
	f_tblcomp((function(...)
		local _tbl_0 = { }
		local _list_0 = items
		for _index_0 = 1, #_list_0 do
			local item = _list_0[_index_0]
			_tbl_0[item(...)] = true
		end
		return _tbl_0
	end)(...))
	f_do((function()
		return func()
	end)())
	f_do((function(...)
		return func(...)
	end)(...))
	f_while((function()
		local _accum_0 = { }
		local _len_0 = 1
		while false do
			_accum_0[_len_0] = func()
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)())
	f_while((function(...)
		local _accum_0 = { }
		local _len_0 = 1
		while false do
			_accum_0[_len_0] = func(...)
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)(...))
	f_if((function()
		if false then
			return func()
		end
	end)())
	f_if((function(...)
		if false then
			return func(...)
		end
	end)(...))
	f_unless((function()
		if not true then
			return func()
		end
	end)())
	f_unless((function(...)
		if not true then
			return func(...)
		end
	end)(...))
	f_switch((function()
		local _exp_0 = x
		if "abc" == _exp_0 then
			return func()
		end
	end)())
	f_switch((function(...)
		local _exp_0 = x
		if "abc" == _exp_0 then
			return func(...)
		end
	end)(...))
	f_eop((function()
		local _obj_0 = func
		if _obj_0 ~= nil then
			return _obj_0()
		end
		return nil
	end)())
	f_eop((function(...)
		local _obj_0 = func
		if _obj_0 ~= nil then
			return _obj_0(...)
		end
		return nil
	end)(...))
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
	return nil
end
