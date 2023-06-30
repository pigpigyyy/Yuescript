f((function()
	local _val_0 = -a ^ 2
	return 1 == _val_0 or 2 == _val_0 or 3 == _val_0
end)())
local a, b = (function(...)
	local _val_0 = x(...)
	return not (1 <= _val_0 and _val_0 <= 3)
end)(...), 2
local d
do
	local _val_0 = (tb.x.y(...))
	d = not (1 <= _val_0 and _val_0 <= 3)
end
local has
do
	local _val_0 = "foo"
	has = "bar" == _val_0 or "foo" == _val_0
end
if (1 == a) and (2 == b or 3 == b or 4 == b) or (function()
	local _val_0 = c
	return 1 <= _val_0 and _val_0 <= 10
end)() then
	print(a, b, c)
end
do
	local _exp_0 = val
	if 1 == _exp_0 or 2 == _exp_0 or 3 == _exp_0 then
		print("1, 2, 3")
	elseif not (0 < _exp_0 and _exp_0 <= 100) then
		print("not (0 < val <= 100)")
	elseif (200 <= _exp_0 and _exp_0 < 300) then
		print("200 <= val < 300)")
	elseif not (333 == _exp_0 or 444 == _exp_0 or 555 == _exp_0) then
		print("not 333, 444 or 555")
	end
end
do
	local _val_0 = y
	return not (a < _val_0 and _val_0 < b)
end
do
	local exist
	do
		local _check_0 = list
		local _val_0 = item
		local _find_0 = false
		for _index_0 = 1, #_check_0 do
			local _item_0 = _check_0[_index_0]
			if _item_0 == _val_0 then
				_find_0 = true
				break
			end
		end
		exist = _find_0
	end
	check((function()
		local _check_0 = list
		local _val_0 = item
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == _val_0 then
				return true
			end
		end
		return false
	end)())
end
do
	local item = get()
	local exist = (function()
		local _check_0 = list
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == item then
				return true
			end
		end
		return false
	end)()
	check((function()
		local _check_0 = list
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == item then
				return true
			end
		end
		return false
	end)())
end
do
	local item = get()
	local list = {
		1,
		2,
		3
	}
	local exist = (#list > 0 and (function()
		for _index_0 = 1, #list do
			if list[_index_0] == item then
				return true
			end
		end
		return false
	end)())
	check((#list > 0 and (function()
		for _index_0 = 1, #list do
			if list[_index_0] == item then
				return true
			end
		end
		return false
	end)()))
	check((1 == item or 2 == item or 3 == item))
	check((function()
		local _check_0 = {
			[1] = 1,
			[2] = 2,
			[3] = 3
		}
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == item then
				return true
			end
		end
		return false
	end)())
end
return nil
