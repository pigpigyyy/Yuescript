do
	local _exp_0 = value
	if "cool" == _exp_0 then
		print("hello world")
	end
end
do
	local _exp_0 = value
	if "cool" == _exp_0 then
		print("hello world")
	else
		print("okay rad")
	end
end
do
	local _exp_0 = value
	if "cool" == _exp_0 then
		print("hello world")
	elseif "yeah" == _exp_0 then
		local _ = [[FFFF]] + [[MMMM]]
	elseif (2323 + 32434) == _exp_0 then
		print("okay")
	else
		print("okay rad")
	end
end
local out
do
	local _exp_0 = value
	if "cool" == _exp_0 then
		out = print("hello world")
	else
		out = print("okay rad")
	end
end
do
	local _exp_0 = value
	if "cool" == _exp_0 then
		out = xxxx
	elseif "umm" == _exp_0 then
		out = 34340
	else
		out = error("this failed big time")
	end
end
do
	local _with_0 = something
	do
		local _exp_0 = _with_0:value()
		if _with_0.okay == _exp_0 then
			local _ = "world"
		else
			local _ = "yesh"
		end
	end
end
fix(this)
call_func((function()
	local _exp_0 = something
	if 1 == _exp_0 then
		return "yes"
	else
		return "no"
	end
end)())
do
	local _exp_0 = hi
	if (hello or world) == _exp_0 then
		local _ = greene
	end
end
do
	local _exp_0 = hi
	if "one" == _exp_0 or "two" == _exp_0 then
		print("cool")
	elseif "dad" == _exp_0 then
		local _ = no
	end
end
do
	local _exp_0 = hi
	if (3 + 1) == _exp_0 or hello() == _exp_0 or (function()
		return 4
	end)() == _exp_0 then
		local _ = yello
	else
		print("cool")
	end
end
do
	local dict = {
		{ },
		{
			1,
			2,
			3
		},
		a = {
			b = {
				c = 1
			}
		},
		x = {
			y = {
				z = 1
			}
		}
	}
	do
		local _tab_0 = "table" == type(dict)
		if _tab_0 then
			local first = dict[1]
			local one
			do
				local _obj_0 = dict[2]
				if _obj_0 ~= nil then
					one = _obj_0[1]
				end
			end
			local two
			do
				local _obj_0 = dict[2]
				if _obj_0 ~= nil then
					two = _obj_0[2]
				end
			end
			local three
			do
				local _obj_0 = dict[2]
				if _obj_0 ~= nil then
					three = _obj_0[3]
				end
			end
			local c
			do
				local _obj_0 = dict.a
				if _obj_0 ~= nil then
					do
						local _obj_1 = _obj_0.b
						if _obj_1 ~= nil then
							c = _obj_1.c
						end
					end
				end
			end
			local z
			do
				local _obj_0 = dict.x
				if _obj_0 ~= nil then
					do
						local _obj_1 = _obj_0.y
						if _obj_1 ~= nil then
							z = _obj_1.z
						end
					end
				end
			end
			if first ~= nil and one ~= nil and two ~= nil and three ~= nil and c ~= nil and z ~= nil then
				print(first, one, two, three, c, z)
			end
		end
	end
end
do
	local items = {
		{
			x = 100,
			y = 200
		},
		{
			width = 300,
			height = 400
		},
		false
	}
	for _index_0 = 1, #items do
		local item = items[_index_0]
		do
			local _tab_0 = "table" == type(item)
			local _match_0 = false
			if _tab_0 then
				local x = item.x
				local y = item.y
				if x ~= nil and y ~= nil then
					print("Vec2 " .. tostring(x) .. ", " .. tostring(y))
					_match_0 = true
				end
			end
			if not _match_0 then
				local _match_1 = false
				if _tab_0 then
					local width = item.width
					local height = item.height
					if width ~= nil and height ~= nil then
						print("Size " .. tostring(width) .. ", " .. tostring(height))
						_match_1 = true
					end
				end
				if not _match_1 then
					if false == item then
						print("None")
					else
						local _match_2 = false
						if _tab_0 then
							local cls = item.__class
							if cls ~= nil then
								if ClassA == cls then
									print("Object A")
								elseif ClassB == cls then
									print("Object B")
								end
								_match_2 = true
							end
						end
						if not _match_2 then
							if _tab_0 then
								local mt = getmetatable(item)
								if mt ~= nil then
									print("A table with metatable")
								end
							else
								print("item not accepted!")
							end
						end
					end
				end
			end
		end
	end
end
do
	local tb = { }
	do
		local _tab_0 = "table" == type(tb)
		if _tab_0 then
			local a = tb.a
			local b = tb.b
			if a == nil then
				a = 1
			end
			if b == nil then
				b = 2
			end
			if a ~= nil and b ~= nil then
				print(a, b)
			end
		end
	end
end
do
	local tb = {
		x = "abc"
	}
	do
		local _tab_0 = "table" == type(tb)
		local _match_0 = false
		if _tab_0 then
			local x = tb.x
			local y = tb.y
			if x ~= nil and y ~= nil then
				print("x: " .. tostring(x) .. " with y: " .. tostring(y))
				_match_0 = true
			end
		end
		if not _match_0 then
			if _tab_0 then
				local x = tb.x
				if x ~= nil then
					print("x: " .. tostring(x) .. " only")
				end
			end
		end
	end
end
do
	local matched
	do
		local _exp_0 = tb
		if 1 == _exp_0 then
			matched = "1"
		else
			local _tab_0 = "table" == type(_exp_0)
			local _match_0 = false
			if _tab_0 then
				local x = _exp_0.x
				if x ~= nil then
					matched = x
					_match_0 = true
				end
			end
			if not _match_0 then
				if false == _exp_0 then
					matched = "false"
				else
					matched = nil
				end
			end
		end
	end
end
do
	local _exp_0 = tb
	if nil == _exp_0 then
		return "invalid"
	else
		do
			local _tab_0 = "table" == type(_exp_0)
			local _match_0 = false
			if _tab_0 then
				local a = _exp_0.a
				local b = _exp_0.b
				if a ~= nil and b ~= nil then
					return tostring(a + b)
					_match_0 = true
				end
			end
			if not _match_0 then
				if 1 == _exp_0 or 2 == _exp_0 or 3 == _exp_0 or 4 == _exp_0 or 5 == _exp_0 then
					return "number 1 - 5"
				else
					if _tab_0 then
						local alwaysMatch = _exp_0.alwaysMatch
						if alwaysMatch == nil then
							alwaysMatch = "fallback"
						end
						if alwaysMatch ~= nil then
							return alwaysMatch
						end
					else
						return "should not reach here"
					end
				end
			end
		end
	end
end
return nil
