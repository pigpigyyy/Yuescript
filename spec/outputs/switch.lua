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
		local _type_0 = type(dict)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local first = dict[1]
			local one
			do
				local _obj_0 = dict[2]
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					one = _obj_0[1]
				end
			end
			local two
			do
				local _obj_0 = dict[2]
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					two = _obj_0[2]
				end
			end
			local three
			do
				local _obj_0 = dict[2]
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					three = _obj_0[3]
				end
			end
			local c
			do
				local _obj_0 = dict.a
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					do
						local _obj_1 = _obj_0.b
						local _type_2 = type(_obj_1)
						if "table" == _type_2 or "userdata" == _type_2 then
							c = _obj_1.c
						end
					end
				end
			end
			local z
			do
				local _obj_0 = dict.x
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					do
						local _obj_1 = _obj_0.y
						local _type_2 = type(_obj_1)
						if "table" == _type_2 or "userdata" == _type_2 then
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
			local _type_0 = type(item)
			local _tab_0 = "table" == _type_0 or "userdata" == _type_0
			local _match_0 = false
			if _tab_0 then
				local x = item.x
				local y = item.y
				if x ~= nil and y ~= nil then
					_match_0 = true
					print("Vec2 " .. tostring(x) .. ", " .. tostring(y))
				end
			end
			if not _match_0 then
				local _match_1 = false
				if _tab_0 then
					local width = item.width
					local height = item.height
					if width ~= nil and height ~= nil then
						_match_1 = true
						print("Size " .. tostring(width) .. ", " .. tostring(height))
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
								_match_2 = true
								if ClassA == cls then
									print("Object A")
								elseif ClassB == cls then
									print("Object B")
								end
							end
						end
						if not _match_2 then
							local _match_3 = false
							if _tab_0 then
								local mt = getmetatable(item)
								if mt ~= nil then
									_match_3 = true
									print("A table with metatable")
								end
							end
							if not _match_3 then
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
		local _type_0 = type(tb)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local a = tb.a
			local b = tb.b
			if a == nil then
				a = 1
			end
			if b == nil then
				b = 2
			end
			print(a, b)
		end
	end
	do
		local _type_0 = type(tb)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local a = tb.a
			local b = tb.b
			if b == nil then
				b = 2
			end
			if a ~= nil then
				print("partially matched", a, b)
			end
		end
	end
	do
		local _type_0 = type(tb)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		local _match_0 = false
		if _tab_0 then
			local a = tb.a
			local b = tb.b
			if a ~= nil and b ~= nil then
				_match_0 = true
				print(a, b)
			end
		end
		if not _match_0 then
			print("not matched")
		end
	end
end
do
	local tb = {
		x = "abc"
	}
	do
		local _type_0 = type(tb)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		local _match_0 = false
		if _tab_0 then
			local x = tb.x
			local y = tb.y
			if x ~= nil and y ~= nil then
				_match_0 = true
				print("x: " .. tostring(x) .. " with y: " .. tostring(y))
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
			local _type_0 = type(_exp_0)
			local _tab_0 = "table" == _type_0 or "userdata" == _type_0
			local _match_0 = false
			if _tab_0 then
				local x = _exp_0.x
				if x ~= nil then
					_match_0 = true
					matched = x
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
			local _type_0 = type(_exp_0)
			local _tab_0 = "table" == _type_0 or "userdata" == _type_0
			local _match_0 = false
			if _tab_0 then
				local a = _exp_0.a
				local b = _exp_0.b
				if a ~= nil and b ~= nil then
					_match_0 = true
					return tostring(a + b)
				end
			end
			if not _match_0 then
				if 1 == _exp_0 or 2 == _exp_0 or 3 == _exp_0 or 4 == _exp_0 or 5 == _exp_0 then
					return "number 1 - 5"
				else
					local _match_1 = false
					if _tab_0 then
						local matchAnyTable = _exp_0.matchAnyTable
						if matchAnyTable == nil then
							matchAnyTable = "fallback"
						end
						_match_1 = true
						return matchAnyTable
					end
					if not _match_1 then
						return "should not reach here unless it is not a table"
					end
				end
			end
		end
	end
end
do
	do
		local _exp_0 = y
		local _type_0 = type(_exp_0)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			local mt = (function()
				local _obj_0 = _exp_0.x
				if _obj_0 ~= nil then
					return getmetatable(_obj_0)
				end
				return nil
			end)()
			if mt ~= nil then
				print(mt)
			end
		end
	end
end
return nil
