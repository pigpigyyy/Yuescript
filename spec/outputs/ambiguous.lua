local bind = grasp.bind;
(bind(stmt))({
	color = "Red"
})
local a = 'b'
local c = d;
(a(b))(c(d))
c = d.c;
(a(b))(c(d));
(c(d))(a(b))
local b
a, b = c, d;
(d(a))(c)
for i = 1, 10 do
	a = function() end
	do
		(print)(1)
	end
	a = f
	do
		(print)(2)
	end
	if cond then
		do
			(print)(3)
		end
	end
	::abc::;
	(print)(4)
	goto abc;
	(print)(5)
end
do
	print();
	(a)(b(1))
	print();
	(fn)(function() end)
end
do
	print()
	async_fn(function()
		print();
--[[a comment to insert]]
		(haha)()
		return nil
	end)
end
do
	do
		print(123)
	end
end
do
	f({
		v = v
	})
	local tb
	do
		local _accum_0 = { }
		local _len_0 = 1
		while f do
			_accum_0[_len_0] = {
				v = v
			}
			_len_0 = _len_0 + 1
		end
		tb = _accum_0
	end
	repeat
		print(v)
	until f({
			v = v
		})
	do
		local _with_0 = f
		local v = tb.v
		_with_0.x = 1
	end
	local x
	if f then
		x = {
			v = v
		}
	end
	do
		local _exp_0 = f({
			v = v
		})
		if f == _exp_0 then
			x = {
				v = v
			}
		end
	end
	local nums
	do
		local _accum_0 = { }
		local _len_0 = 1
		for num = 1, len do
			_accum_0[_len_0] = {
				num = num
			}
			_len_0 = _len_0 + 1
		end
		nums = _accum_0
	end
	local objects
	local _accum_0 = { }
	local _len_0 = 1
	local _list_0 = items
	for _index_0 = 1, #_list_0 do
		local item = _list_0[_index_0]
		_accum_0[_len_0] = {
			name = item
		}
		_len_0 = _len_0 + 1
	end
	objects = _accum_0
end
return nil
