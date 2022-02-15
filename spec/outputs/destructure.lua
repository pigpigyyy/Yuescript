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
	do
		local _obj_0 = value
		hello, world = _obj_0.hello, _obj_0.world
	end
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
	local _ = two
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
		do
			local _obj_0 = tbl
			a, b = _obj_0.a, _obj_0.b
		end
	end
	do
		a = tbl
		b, c = _.b, _.c
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
	do
		local _obj_0 = tb
		c, e = _obj_0.a.b, _obj_0.a.d
	end
end
do
	local c, e, f
	do
		local _obj_0 = tb
		c, e, f = _obj_0.a[1].b, _obj_0.a[2].d, _obj_0.a[3]
	end
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
end
do
	do
		local _with_0 = thing
		local a, b = _with_0[1], _with_0[2]
		print(a, b)
	end
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
	do
		local _obj_0 = z
		a, b, c = _obj_0[1], _obj_0[2], _obj_0[3]
	end
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
	do
		local _obj_0 = thing
		a, b, c = _obj_0["if"][1], _obj_0["if"][2], _obj_0["if"][3]
	end
end
do
	local a, b
	if true then
		do
			local _obj_0 = {
				a = "Hello",
				b = "World"
			}
			a, b = _obj_0.a, _obj_0.b
		end
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
	end
	if name == nil then
		name = "nameless"
	end
	if job == nil then
		job = "jobless"
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
	end
	if value1 == nil then
		value1 = 123
	end
	if key3 == nil then
		key3 = "abc"
	end
	local mt, call, add
	do
		local _obj_0 = getmetatable(tb)
		mt, call, add = _obj_0, getmetatable(_obj_0).__call, getmetatable(_obj_0).__add
	end
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
	local _obj_0 = tb
	local mtx, y, zItem = getmetatable(_obj_0.x), _obj_0.y, _obj_0.z
	if mtx == nil then
		mtx = { }
	end
	local index = getmetatable(_obj_0).__index
	if index == nil then
		index = function()
			return nil
		end
	end
	local _obj_1 = getmetatable(tb).func
	if _obj_1 == nil then
		do
			local _obj_2 = item
			if _obj_2 ~= nil then
				_obj_1 = _obj_2.defVal
			end
		end
	end
	a.b(function()
			return 123
		end).c = _obj_1
end
do
	local mt, subFunc
	do
		local _obj_0 = getmetatable(tb.x)
		mt, subFunc = _obj_0, _obj_0.__sub
	end
	if mt == nil then
		mt = { }
	end
end
do
	local mt, subFunc
	do
		local _obj_0 = tb
		mt, subFunc = getmetatable(_obj_0.x), getmetatable(_obj_0.x).__sub
	end
	if mt == nil then
		mt = { }
	end
end
do
	local a, b, _obj_0
	do
		local _obj_1 = tb
		a, b, _obj_0 = _obj_1[1], _obj_1[2], _obj_1.c[1]
	end
	if a == nil then
		a = 1
	end
	if b == nil then
		b = 2
	end
	if _obj_0 == nil then
		_obj_0 = 3
	end
	d.e = _obj_0
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
end
