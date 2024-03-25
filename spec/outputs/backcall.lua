do
	map({
		1,
		2,
		3
	}, function(x)
		return x * 2
	end)
end
do
	map(function(x)
		return x * 2
	end, {
		1,
		2,
		3
	})
end
do
	filter(function(x)
		return x > 2
	end, (function()
		return map(function(x)
			return x * 2
		end, {
			1,
			2,
			3,
			4
		})
	end)())
end
do
	local _obj_0 = http
	if _obj_0 ~= nil then
		_obj_0.get("ajaxtest", function(data)
			body[".result"]:html(data)
			return http.post("ajaxprocess", data, function(processed)
				body[".result"]:append(processed)
				return setTimeout(1000, function()
					return print("done")
				end)
			end)
		end)
	end
end
do
	syncStatus(function()
		return loadAsync("file.yue", function(err, data)
			if data == nil then
				data = "nil"
			end
			if err then
				print(err)
				return
			end
			return compileAsync(data, function(codes)
				local func = loadstring(codes)
				return func()
			end)
		end)
	end)
end
do
	f1(function()
		return f2(function()
			do
				f3(function()
					return f4(function() end)
				end)
			end
			return f5(function()
				return f6(function()
					return f7()
				end)
			end)
		end)
	end)
end
do
	local result, msg
	do
		local _obj_0 = receiveAsync("filename.txt", function(data)
			print(data)
			return processAsync(data, function(info)
				return check(info)
			end)
		end)
		result, msg = _obj_0.result, _obj_0.msg
	end
	print(result, msg)
	local totalSize = reduce(((function()
		local _accum_0 = { }
		local _len_0 = 1
		local _list_0 = files
		for _index_0 = 1, #_list_0 do
			local file = _list_0[_index_0]
			_accum_0[_len_0] = loadAsync(file, function(data)
				return addToCache(file, data)
			end)
			_len_0 = _len_0 + 1
		end
		return _accum_0
	end)()), 0, function(a, b)
		return a + b
	end)
end
local propA
do
	propA = property(function(self)
		return self._value
	end, function(self, value)
		print("old value: " .. tostring(self._value))
		print("new value: " .. tostring(value))
		self._value = value
	end)
end
local propB
do
	propB = property(function(self)
		return self._value
	end, function(self, value)
		print("old value: " .. tostring(self._value))
		print("new value: " .. tostring(value))
		self._value = value
	end)
end
alert("hi")
local x, y, z
do
	x = (a) < -b
end
do
	x, y, z = b(function(a) end)
end
do
	x, y, z = b(function(a) end)
end
do
	x = a <= b
end
do
	x, y, z = b(function(self, a) end)
end
return nil
