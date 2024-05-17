func((function()
	if cond then
		return 998
	else
		return "abc"
	end
end)())
func(valueA + (function()
	local _exp_0 = valueB
	if _exp_0 ~= nil then
		return _exp_0
	else
		return 123
	end
end)())
do
	(function(_arg_0, ...)
		local ok = _arg_0
		return print(select(3, ...))
	end)(func(1, 2, 3))
end
if (function()
	local _obj_0 = tb
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0.abc
		if _obj_1 ~= nil then
			local _obj_2 = _obj_1.call
			if _obj_2 ~= nil then
				return _obj_2(_obj_1, 123)
			end
			return nil
		end
		return nil
	end
	return nil
end)() then
	print("OK")
end
func((function()
	local _obj_0 = getmetatable(tb)
	return _obj_0["fn"](_obj_0, 123)
end)(), (function()
	local _obj_0 = getmetatable(tb)
	return _obj_0[1 + 1](_obj_0, "abc")
end)())
func((function()
	local _call_0 = tb
	local _call_1 = _call_0["end"](_call_0)
	return _call_1["🤣"](_call_1, 123)
end)())
local itemA = 1
local listA = { }
if (#listA > 0 and (function()
	for _index_0 = 1, #listA do
		if listA[_index_0] == itemA then
			return true
		end
	end
	return false
end)()) then
	print("itemA in listA")
end
if (itemB ~= nil) and (function()
	local _check_0 = listB
	local _val_0 = itemB
	for _index_0 = 1, #_check_0 do
		if _check_0[_index_0] == _val_0 then
			return true
		end
	end
	return false
end)() then
	print("itemB in listB")
end
func((function()
	local _tab_0 = { }
	local _idx_0 = #_tab_0 + 1
	for _index_0 = 1, #listA do
		local _value_0 = listA[_index_0]
		_tab_0[_idx_0] = _value_0
		_idx_0 = _idx_0 + 1
	end
	local _idx_1 = #_tab_0 + 1
	local _list_0 = listB
	for _index_0 = 1, #_list_0 do
		local _value_0 = _list_0[_index_0]
		_tab_0[_idx_1] = _value_0
		_idx_1 = _idx_1 + 1
	end
	return _tab_0
end)())
func((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)(), (function()
	local _accum_0 = { }
	local _len_0 = 1
	for k in pairs(tb) do
		_accum_0[_len_0] = k
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
func((function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i + 1
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
func((function()
	local _accum_0 = { }
	local _len_0 = 1
	for k, v in pairs(tb) do
		_accum_0[_len_0] = {
			k,
			v
		}
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
func((function()
	do
		local _class_0
		local _base_0 = { }
		if _base_0.__index == nil then
			_base_0.__index = _base_0
		end
		_class_0 = setmetatable({
			__init = function(self)
				self.value = 1
			end,
			__base = _base_0
		}, {
			__index = _base_0,
			__call = function(cls, ...)
				local _self_0 = setmetatable({ }, _base_0)
				cls.__init(_self_0, ...)
				return _self_0
			end
		})
		_base_0.__class = _class_0
		return _class_0
	end
end)())
func((function()
	local _with_0 = tb
	_with_0.field = 1
	_with_0:func("a")
	return _with_0
end)())
func((function()
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[tostring(k) .. "-post-fix"] = v * 2
	end
	return _tbl_0
end)())
func((function()
	print(123)
	return "abc"
end)())
do
	(function(_arg_0, ...)
		local success = _arg_0
		if success then
			return print(select('#', ...))
		end
	end)(pcall(function()
		local a = 1
		print(a + nil)
		return 1, 2, 3
	end))
end
local i = 1
func((function()
	local _accum_0 = { }
	local _len_0 = 1
	while cond do
		i = i + 1
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end)())
func((function()
	local _exp_0 = value
	if 1 == _exp_0 then
		return 'a'
	elseif 2 == _exp_0 then
		return 'b'
	end
end)())
local _anon_func_0 = function(cond)
	if cond then
		return 998
	else
		return "abc"
	end
end
local _anon_func_1 = function(valueB)
	if valueB ~= nil then
		return valueB
	else
		return 123
	end
end
local _anon_func_2 = function(print, select, _arg_0, ...)
	do
		local ok = _arg_0
		return print(select(3, ...))
	end
end
local _anon_func_3 = function(tb)
	if tb ~= nil then
		local _obj_0 = tb.abc
		if _obj_0 ~= nil then
			local _obj_1 = _obj_0.call
			if _obj_1 ~= nil then
				return _obj_1(_obj_0, 123)
			end
			return nil
		end
		return nil
	end
	return nil
end
local _anon_func_4 = function(getmetatable, tb)
	local _obj_0 = getmetatable(tb)
	return _obj_0["fn"](_obj_0, 123)
end
local _anon_func_5 = function(getmetatable, tb)
	local _obj_0 = getmetatable(tb)
	return _obj_0[1 + 1](_obj_0, "abc")
end
local _anon_func_6 = function(tb)
	do
		local _call_0 = tb
		local _call_1 = _call_0["end"](_call_0)
		return _call_1["🤣"](_call_1, 123)
	end
end
local _anon_func_7 = function(itemA, listA)
	for _index_0 = 1, #listA do
		if listA[_index_0] == itemA then
			return true
		end
	end
	return false
end
local _anon_func_8 = function(itemB, listB)
	for _index_0 = 1, #listB do
		if listB[_index_0] == itemB then
			return true
		end
	end
	return false
end
local _anon_func_9 = function(listA, listB, pairs)
	local _tab_0 = { }
	local _idx_0 = #_tab_0 + 1
	for _index_0 = 1, #listA do
		local _value_0 = listA[_index_0]
		_tab_0[_idx_0] = _value_0
		_idx_0 = _idx_0 + 1
	end
	local _idx_1 = #_tab_0 + 1
	for _index_0 = 1, #listB do
		local _value_0 = listB[_index_0]
		_tab_0[_idx_1] = _value_0
		_idx_1 = _idx_1 + 1
	end
	return _tab_0
end
local _anon_func_10 = function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_11 = function(pairs, tb)
	local _accum_0 = { }
	local _len_0 = 1
	for k in pairs(tb) do
		_accum_0[_len_0] = k
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_12 = function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i + 1
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_13 = function(pairs, tb)
	local _accum_0 = { }
	local _len_0 = 1
	for k, v in pairs(tb) do
		_accum_0[_len_0] = {
			k,
			v
		}
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_14 = function(setmetatable)
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self)
			self.value = 1
		end,
		__base = _base_0
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	return _class_0
end
local _anon_func_15 = function(tb)
	tb.field = 1
	tb:func("a")
	return tb
end
local _anon_func_16 = function(pairs, tb, tostring)
	local _tbl_0 = { }
	for k, v in pairs(tb) do
		_tbl_0[tostring(k) .. "-post-fix"] = v * 2
	end
	return _tbl_0
end
local _anon_func_17 = function(print)
	do
		print(123)
		return "abc"
	end
end
local _anon_func_18 = function(print, select, _arg_0, ...)
	do
		local success = _arg_0
		if success then
			return print(select('#', ...))
		end
	end
end
local _anon_func_19 = function(cond, i)
	local _accum_0 = { }
	local _len_0 = 1
	while cond do
		i = i + 1
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
local _anon_func_20 = function(value)
	if 1 == value then
		return 'a'
	elseif 2 == value then
		return 'b'
	end
end
GameEngine:onUpdate(function(deltaTime)
	func(_anon_func_0(cond))
	func(valueA + _anon_func_1(valueB))
	do
		_anon_func_2(print, select, func(1, 2, 3))
	end
	if _anon_func_3(tb) then
		print("OK")
	end
	func(_anon_func_4(getmetatable, tb), _anon_func_5(getmetatable, tb))
	func(_anon_func_6(tb))
	itemA = 1
	listA = { }
	if (#listA > 0 and _anon_func_7(itemA, listA)) then
		print("item in list")
	end
	if (itemB ~= nil) and _anon_func_8(itemB, listB) then
		print("item in list")
	end
	func(_anon_func_9(listA, listB, pairs))
	func(_anon_func_10(), _anon_func_11(pairs, tb))
	func(_anon_func_12())
	func(_anon_func_13(pairs, tb))
	func(_anon_func_14(setmetatable))
	func(_anon_func_15(tb))
	func(_anon_func_16(pairs, tb, tostring))
	func(_anon_func_17(print))
	do
		_anon_func_18(print, select, pcall(function()
			local a = 1
			print(a + nil)
			return 1, 2, 3
		end))
	end
	i = 1
	func(_anon_func_19(cond, i))
	return func(_anon_func_20(value))
end)
local _anon_func_21 = function(cond)
	if cond then
		return 998
	else
		return "abc"
	end
end
local _anon_func_22 = function(valueB)
	if valueB ~= nil then
		return valueB
	else
		return 123
	end
end
local _anon_func_23 = function(tb)
	if tb ~= nil then
		local _obj_0 = tb.abc
		if _obj_0 ~= nil then
			local _obj_1 = _obj_0.call
			if _obj_1 ~= nil then
				return _obj_1(_obj_0, 123)
			end
			return nil
		end
		return nil
	end
	return nil
end
local _anon_func_25 = function(itemA, listA)
	for _index_0 = 1, #listA do
		if listA[_index_0] == itemA then
			return true
		end
	end
	return false
end
local _anon_func_24 = function(itemA, listA, tb)
	do
		local _call_0 = tb
		local _call_1 = _call_0["end"](_call_0)
		return _call_1["🤣"](_call_1, 123 and (#listA > 0 and _anon_func_25(itemA, listA)))
	end
end
GameEngine:onEvent("SomeEvent", function()
	return func(value + (_anon_func_21(cond)) + (_anon_func_22(valueB)) > _anon_func_23(tb) + _anon_func_24(itemA, listA, tb))
end)
local _anon_func_26 = function(UpdateScoreText, tostring, value)
	if value > 200 then
		UpdateScoreText("Win: " .. tostring(value))
		return "done"
	else
		UpdateScoreText("Score: " .. tostring(value))
		return "continue"
	end
end
GameEngine:schedule(function(deltaTime)
	local value = 123
	return func(_anon_func_26(UpdateScoreText, tostring, value))
end)
GameEngine:schedule(function(deltaTime)
	local value = 123
	return func((function()
		if value > 200 then
			UpdateScoreText("Win: " .. tostring(value))
			return "done"
		else
			GameEngine:schedule(function(deltaTime)
				return UpdateScoreText("Score: " .. tostring(value))
			end)
			return "continue"
		end
	end)())
end)
local _anon_func_27 = function(char)
	local _obj_0 = char.stats.strength
	if _obj_0 ~= nil then
		return _obj_0:ref()
	end
	return nil
end
local _anon_func_28 = function(os, _arg_0, ...)
	do
		local ok = _arg_0
		if ok then
			return ...
		else
			return os.exit(1)
		end
	end
end
local _anon_func_29 = function(debug_env_after, debug_env_before, env, func)
	debug_env_before(env)
	func(env)
	return debug_env_after(env)
end
do
	local buff_strength
	buff_strength = function(char, item)
		local _obj_0 = item.buffer.strength
		if _obj_0 ~= nil then
			return _obj_0(_anon_func_27(char))
		end
		return nil
	end
	local debug_env_before, debug_env_after
	local exe_func
	exe_func = function(func, env)
		return _anon_func_28(os, xpcall(_anon_func_29, function(ex)
			error(ex)
			return ex
		end, debug_env_after, debug_env_before, env, func))
	end
end
