do
	local a, b
	do
		local _obj_0 = hello
		a, b = _obj_0[1], _obj_0[2]
	end
	local c
	do
		local _obj_0 = hello
		a, b, c = _obj_0[1][1], _obj_0[2], _obj_0[3][1]
	end
	local hello, world
	local _obj_0 = value
	hello, world = _obj_0.hello, _obj_0.world
end
do
	local no, thing
	do
		local _obj_0 = world
		no, thing = _obj_0.yes, _obj_0[1]
	end
	local a, b, c, d
	do
		local _obj_0 = yeah
		a, b, c, d = _obj_0.a, _obj_0.b, _obj_0.c, _obj_0.d
	end
	b = two
	a = one[1]
	c = nil
	b = one[1]
	local e = two
	d = one[1]
	local x = one
	local y = two[1]
	local xx, yy = 1, 2
	do
		local _obj_0 = {
			xx,
			yy
		}
		yy, xx = _obj_0[1], _obj_0[2]
	end
	local f, g
	do
		local _obj_0 = tbl
		a, b, c, d, e, f, g = _obj_0[1], _obj_0.b, _obj_0[2], _obj_0.d, _obj_0[3], _obj_0.f, _obj_0[4]
	end
	do
		c = nil
		local _obj_0 = tbl
		a, b = _obj_0.a, _obj_0.b
	end
	do
		a = tbl
		local _obj_0 = _
		b, c = _obj_0.b, _obj_0.c
	end
	do
		b = _
		a = tbl.a
		c = _.c
	end
end
do
	local futurists = {
		sculptor = "Umberto Boccioni",
		painter = "Vladimir Burliuk",
		poet = {
			name = "F.T. Marinetti",
			address = {
				"Via Roma 42R",
				"Bellagio, Italy 22021"
			}
		}
	}
	do
		local name, street, city = futurists.poet.name, futurists.poet.address[1], futurists.poet.address[2]
	end
	do
		local sculptor, painter, name, street, city = futurists.sculptor, futurists.painter, futurists.poet.name, futurists.poet.address[1], futurists.poet.address[2]
	end
end
do
	local c, e
	local _obj_0 = tb
	c, e = _obj_0.a.b, _obj_0.a.d
end
do
	local c, e, f
	local _obj_0 = tb
	c, e, f = _obj_0.a[1].b, _obj_0.a[2].d, _obj_0.a[3]
end
do
	self.world = x[1]
	do
		local _obj_0 = x
		a.b, c.y, func().z = _obj_0[1], _obj_0[2], _obj_0[3]
	end
	self.world = x.world
end
do
	local thing = {
		{
			1,
			2
		},
		{
			3,
			4
		}
	}
	for _index_0 = 1, #thing do
		local _des_0 = thing[_index_0]
		local x, y = _des_0[1], _des_0[2]
		print(x, y)
	end
	for _index_0 = 1, #thing do
		local _des_0 = thing[_index_0]
		local x, y = _des_0[1], _des_0[2]
		print(x, y)
	end
end
do
	local _with_0 = thing
	local a, b = _with_0[1], _with_0[2]
	print(a, b)
end
do
	local thing = nil
	if thing then
		local a = thing[1]
		print(a)
	else
		print("nothing")
	end
	local thang = {
		1,
		2
	}
	if thang then
		local a, b = thang[1], thang[2]
		print(a, b)
	end
	if thing then
		local a, b = thing[1], thing[2]
		print(a, b)
	else
		if thang then
			local c, d = thang[1], thang[2]
			print(c, d)
		else
			print("NO")
		end
	end
end
do
	local z = "yeah"
	local a, b, c = z[1], z[2], z[3]
end
do
	local a, b, c
	local _obj_0 = z
	a, b, c = _obj_0[1], _obj_0[2], _obj_0[3]
end
local _
_ = function(z)
	local a, b, c = z[1], z[2], z[3]
end
do
	local z = "oo"
	_ = function(k)
		local a, b, c = z[1], z[2], z[3]
	end
end
do
	local endVar = thing["function"]["end"]
end
do
	local a, b, c
	local _obj_0 = thing
	a, b, c = _obj_0["if"][1], _obj_0["if"][2], _obj_0["if"][3]
end
do
	local a, b
	if true then
		local _obj_0 = {
			a = "Hello",
			b = "World"
		}
		a, b = _obj_0.a, _obj_0.b
	end
	local days, hours, mins, secs
	do
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = {
			string.match("1 2 3 4", "(.+)%s(.+)%s(.+)%s(.+)")
		}
		for _index_0 = 1, #_list_0 do
			local a = _list_0[_index_0]
			_accum_0[_len_0] = tonumber(a)
			_len_0 = _len_0 + 1
		end
		days, hours, mins, secs = _accum_0[1], _accum_0[2], _accum_0[3], _accum_0[4]
	end
	local one, two, three
	do
		local _tbl_0 = { }
		for w in foo:gmatch("%S+") do
			_tbl_0[w] = true
		end
		one, two, three = _tbl_0.one, _tbl_0.two, _tbl_0.three
	end
	b = 123
	a = (a["if"](a, 123) + t).a
end
do
	local name, job
	do
		local _obj_0 = person
		name, job = _obj_0.name, _obj_0.job
		if name == nil then
			name = "nameless"
		end
		if job == nil then
			job = "jobless"
		end
	end
	local request
	request = function(url, options)
		if options == nil then
			options = { }
		end
		local method, headers, parameters, payload, ok, err, final = options.method, options.headers, options.parameters, options.payload, options.ok, options.err, options.final
		if method == nil then
			method = "GET"
		end
		if headers == nil then
			headers = { }
		end
		if parameters == nil then
			parameters = { }
		end
		if payload == nil then
			payload = ""
		end
		local res = makeRequest(url, method, parameters, payload, ok, err, final)
		return res
	end
	local value1, key3
	do
		local _obj_0 = tb
		value1, key3 = _obj_0.key1.key2, _obj_0.key3
		if value1 == nil then
			value1 = 123
		end
		if key3 == nil then
			key3 = "abc"
		end
	end
	local mt, call, add
	do
		local _obj_0 = getmetatable(tb)
		mt, call, add = _obj_0, getmetatable(_obj_0).__call, getmetatable(_obj_0).__add
		if mt == nil then
			mt = {
				__index = {
					abc = 123
				}
			}
		end
		if call == nil then
			call = (function()
				return { }
			end)
		end
	end
	local mtx, y, zItem, index
	do
		local _obj_0 = tb
		mtx, y, zItem = getmetatable(_obj_0.x), _obj_0.y, _obj_0.z
		if mtx == nil then
			mtx = { }
		end
		index = getmetatable(_obj_0).__index
		if index == nil then
			index = function()
				return nil
			end
		end
	end
	local _tmp_0
	local _obj_0 = getmetatable(tb)
	_tmp_0 = _obj_0.func
	if _tmp_0 == nil then
		local _obj_1 = item
		if _obj_1 ~= nil then
			_tmp_0 = _obj_1.defVal
		end
	end
	a.b(function()
		return 123
	end).c = _tmp_0
end
do
	local mt, subFunc
	local _obj_0 = getmetatable(tb.x)
	mt, subFunc = _obj_0, _obj_0.__sub
	if mt == nil then
		mt = { }
	end
end
do
	local mt, subFunc
	local _obj_0 = tb
	mt, subFunc = getmetatable(_obj_0.x), getmetatable(_obj_0.x).__sub
	if mt == nil then
		mt = { }
	end
end
do
	local a, b
	do
		local _obj_0 = tb
		local _tmp_0
		a, b, _tmp_0 = _obj_0[1], _obj_0[2], _obj_0.c[1]
		if a == nil then
			a = 1
		end
		if b == nil then
			b = 2
		end
		if _tmp_0 == nil then
			_tmp_0 = 3
		end
		d.e = _tmp_0
	end
	local _list_0 = tuples
	for _index_0 = 1, #_list_0 do
		local _des_0 = _list_0[_index_0]
		local left, right = _des_0[1], _des_0[2]
		if left == nil then
			left = "null"
		end
		if right == nil then
			right = false
		end
		print(left, right)
	end
	local _list_1 = tuples
	for _index_0 = 1, #_list_1 do
		local _des_0 = _list_1[_index_0]
		local left, right = _des_0[1], _des_0[2]
		if left == nil then
			left = "null"
		end
		if right == nil then
			right = false
		end
		print(left, right)
	end
end
do
	local a, b
	local _obj_0 = tb
	a, b = _obj_0[2], _obj_0[4]
end
do
	local a, b
	local _obj_0 = tb
	a, b = _obj_0[1], _obj_0[3]
end
do
	local _obj_0 = x.x.x
	local _tmp_0, _tmp_1 = _obj_0.x, _obj_0.y
	if _tmp_0 == nil then
		_tmp_0 = 1
	end
	if _tmp_1 == nil then
		_tmp_1 = 2
	end
	a.b = _tmp_0
	a.c = _tmp_1
end
do
	local width, height
	do
		local _obj_0 = View.size
		width, height = _obj_0.width, _obj_0.height
	end
	local x, y
	local _obj_0 = point
	x, y = _obj_0.x, _obj_0.y
	if x == nil then
		x = 0.0
	end
	if y == nil then
		y = 0.0
	end
end
do
	local x1, x2, x3, d, e
	local b
	do
		local _obj_0, _obj_1, _obj_2, _obj_3, _obj_4 = f()
		do
			local _obj_5 = a
			_obj_5[#_obj_5 + 1] = _obj_0
		end
		setmetatable(c, _obj_4)
		x1, x2, x3, d, e = 1, 2, 3, _obj_1, _obj_3
		b = _obj_2[1]
	end
	local y1, y4
	local y2, y3
	local _obj_0, _obj_1 = f2()
	y1, y4 = f1(), _obj_1
	y2, y3 = _obj_0.y2, _obj_0.y3
end
do
	local v1, v2, v3, v4
	local _obj_0 = tb
	local _tmp_0, _tmp_1 = 1 + 1, self.x
	v1, v2, v3, v4 = _obj_0[ [["abc"]]], _obj_0[_tmp_0][1], _obj_0[_tmp_0][2], _obj_0[_tmp_1]
	if v1 == nil then
		v1 = 111
	end
	if v2 == nil then
		v2 = 222
	end
	if v3 == nil then
		v3 = 333
	end
	if v4 == nil then
		v4 = 444
	end
	local v5, v6, v7
	local _obj_1 = tb2
	local _tmp_2, _tmp_3 = func(), func2()
	v5, v6, v7 = _obj_1['x-y-z'], _obj_1[_tmp_2][_tmp_3], _obj_1[_tmp_2][1]
end
do
	local value, value_meta
	local _obj_0 = tb
	value = _obj_0[name]
	value_meta = getmetatable(_obj_0)[name]
end
do
	local tostring, add
	do
		local _obj_0 = getmetatable(tb)
		tostring, add = _obj_0.__tostring, _obj_0.__add
		if tostring == nil then
			tostring = (function()
				return "name"
			end)
		end
	end
	local _exp_0 = tb
	local _type_0 = type(_exp_0)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	if _tab_0 then
		local name, meta_field
		do
			local _obj_0 = getmetatable(_exp_0)
			name = _obj_0.__name
			meta_field = _obj_0["123"]
			if name == nil then
				name = "item"
			end
		end
		if meta_field ~= nil then
			print(name, meta_field)
		end
	end
end
do
	local tb = { }
	local _tmp_0 = c()
	local v2 = tb[_tmp_0]
	local v1
	local _obj_0 = getmetatable(tb)
	local _tmp_1 = a + b
	v1 = _obj_0[_tmp_1]
end
do
	local add, field
	do
		local _obj_0 = tb
		add, field = getmetatable(_obj_0.a).__add, getmetatable(_obj_0.b)[fieldName]
		if add == nil then
			add = addFunc
		end
		if field == nil then
			field = 123
		end
	end
	local meta_field, abc, def
	do
		local _obj_0 = getmetatable(tb)
		meta_field, abc, def = _obj_0["abc"], _obj_0[ [[any string]]], _obj_0['str']
		if meta_field == nil then
			meta_field = "def"
		end
		if abc == nil then
			abc = 123
		end
		if def == nil then
			def = { }
		end
	end
	do
		local _exp_0 = tb
		local _type_0 = type(_exp_0)
		local _tab_0 = "table" == _type_0 or "userdata" == _type_0
		if _tab_0 then
			do
				local _obj_0 = _exp_0.a
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					do
						local _obj_1 = getmetatable(_obj_0)
						local _type_2 = type(_obj_1)
						if "table" == _type_2 or "userdata" == _type_2 then
							add = _obj_1.__add
						end
					end
				end
			end
			do
				local _obj_0 = _exp_0.b
				local _type_1 = type(_obj_0)
				if "table" == _type_1 or "userdata" == _type_1 then
					do
						local _obj_1 = getmetatable(_obj_0)
						local _type_2 = type(_obj_1)
						if "table" == _type_2 or "userdata" == _type_2 then
							field = _obj_1[fieldName]
						end
					end
				end
			end
			if add == nil then
				add = addFunc
			end
			if field == nil then
				field = 123
			end
			print(add, field)
		end
	end
	local _exp_0 = tb
	local _type_0 = type(_exp_0)
	local _tab_0 = "table" == _type_0 or "userdata" == _type_0
	if _tab_0 then
		do
			local _obj_0 = _exp_0.c
			local _type_1 = type(_obj_0)
			if "table" == _type_1 or "userdata" == _type_1 then
				do
					local _obj_1 = getmetatable(_obj_0)
					local _type_2 = type(_obj_1)
					if "table" == _type_2 or "userdata" == _type_2 then
						meta_field = _obj_1["abc"]
					end
				end
			end
		end
		if meta_field == nil then
			meta_field = "def"
		end
		do
			local _obj_0 = getmetatable(_exp_0)
			do
				local _obj_1 = _obj_0[ [[any string]]]
				local _type_1 = type(_obj_1)
				if "table" == _type_1 or "userdata" == _type_1 then
					abc = _obj_1.d
				end
			end
			do
				local _obj_1 = _obj_0['str']
				local _type_1 = type(_obj_1)
				if "table" == _type_1 or "userdata" == _type_1 then
					def = _obj_1.e
				end
			end
			if abc == nil then
				abc = 123
			end
			if def == nil then
				def = { }
			end
		end
		print(meta_field, abc, def)
	end
end
return nil
