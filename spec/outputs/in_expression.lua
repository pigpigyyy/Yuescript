f((function()
	local _val_0 = -a ^ 2
	return 1 == _val_0 or 2 == _val_0 or 3 == _val_0
end)())
local has
do
	local _val_0 = "foo"
	has = "bar" == _val_0 or "foo" == _val_0
end
if (1 == a) and (function()
	local _val_0 = b
	return 2 == _val_0 or 3 == _val_0 or 4 == _val_0
end)() then
	print(a, b, c)
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
	local exist
	do
		local _check_0 = list
		local _find_0 = false
		for _index_0 = 1, #_check_0 do
			local _item_0 = _check_0[_index_0]
			if _item_0 == item then
				_find_0 = true
				break
			end
		end
		exist = _find_0
	end
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
	local not_exist
	do
		local _find_0 = false
		for _index_0 = 1, #list do
			local _item_0 = list[_index_0]
			if _item_0 == item then
				_find_0 = true
				break
			end
		end
		not_exist = not _find_0
	end
	check((#list > 0 and (function()
		for _index_0 = 1, #list do
			if list[_index_0] == item then
				return true
			end
		end
		return false
	end)()))
	check((1 == item or 2 == item or 3 == item))
	check((function(...)
		local _check_0 = {
			[1] = 1,
			[2] = 2,
			[3] = 3
		}
		local _val_0 = item(...)
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == _val_0 then
				return true
			end
		end
		return false
	end)(...))
end
do
	check(function()
		local _check_0 = tb
		local _val_0 = x
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == _val_0 then
				return true
			end
		end
		return false
	end)
	check(function()
		local _check_0 = tb
		local _val_0 = x
		for _index_0 = 1, #_check_0 do
			if _check_0[_index_0] == _val_0 then
				return false
			end
		end
		return true
	end)
	local x, tb
	check(function()
		for _index_0 = 1, #tb do
			if tb[_index_0] == x then
				return true
			end
		end
		return false
	end)
	check(function()
		for _index_0 = 1, #tb do
			if tb[_index_0] == x then
				return false
			end
		end
		return true
	end)
end
return nil
