local _module_0 = { }
local p, to_lua
do
	local _obj_0 = require("yue")
	p, to_lua = _obj_0.p, _obj_0.to_lua
end
local inventory = {
	equipment = {
		"sword",
		"shield"
	},
	items = {
		{
			name = "potion",
			count = 10
		},
		{
			name = "bread",
			count = 3
		}
	}
}
print(reduce(filter(map({
	1,
	2,
	3
}, function(x)
	return x * 2
end), function(x)
	return x > 4
end), 0, function(a, b)
	return a + b
end))
local apple = setmetatable({
	size = 15,
}, {
	__index = {
		color = 0x00ffff
	}
})
if (getmetatable(apple) ~= nil) then
	p(apple.color, getmetatable(apple).__index)
end
local _ud83c_udf1b = "月之脚本"
_module_0["🌛"] = _ud83c_udf1b
return _module_0
local area = 6.2831853071796 * 5
print('hello world')
do
	assert(item ~= nil)
end
local value = item
if (f1() and f2() and f3()) then
	print("OK")
end
do
	local funcA
	funcA = function() end
end
local funcA
funcA = function()
	return "assign the Yue defined variable"
end
local function funcB() end
funcB = function()
	return "assign the Lua defined variable"
end
-- raw Lua codes insertion
if cond then
  print("output")
end
print("yuescript")
print(3)
print("Valid enum type:", "Static")
if tb ~= nil then
	tb:func()
end
if tb ~= nil then
	tb:func()
end
print(1 < 2 and 2 <= 2 and 2 < 3 and 3 == 3 and 3 > 2 and 2 >= 1 and 1 == 1 and 1 < 3 and 3 ~= 5)
local a = 5
print(1 <= a and a <= 10)
local v
v = function(x)
	print(x)
	return x
end
print((function()
	local _cond_0 = v(2)
	if not (v(1) < _cond_0) then
		return false
	else
		return _cond_0 <= v(3)
	end
end)())
print((function()
	local _cond_0 = v(2)
	if not (v(1) > _cond_0) then
		return false
	else
		return _cond_0 <= v(3)
	end
end)())
local tab = { }
tab[#tab + 1] = "Value"
local parts = {
	"shoulders",
	"knees"
}
local lyrics
do
	local _tab_0 = {
		"head"
	}
	local _idx_0 = 1
	for _key_0, _value_0 in pairs(parts) do
		if _idx_0 == _key_0 then
			_tab_0[#_tab_0 + 1] = _value_0
			_idx_0 = _idx_0 + 1
		else
			_tab_0[_key_0] = _value_0
		end
	end
	_tab_0[#_tab_0 + 1] = "and"
	_tab_0[#_tab_0 + 1] = "toes"
	lyrics = _tab_0
end
local copy
do
	local _tab_0 = { }
	local _idx_0 = 1
	for _key_0, _value_0 in pairs(other) do
		if _idx_0 == _key_0 then
			_tab_0[#_tab_0 + 1] = _value_0
			_idx_0 = _idx_0 + 1
		else
			_tab_0[_key_0] = _value_0
		end
	end
	copy = _tab_0
end
local a = {
	1,
	2,
	3,
	x = 1
}
local b = {
	4,
	5,
	y = 1
}
local merge
local _tab_0 = { }
local _idx_0 = 1
for _key_0, _value_0 in pairs(a) do
	if _idx_0 == _key_0 then
		_tab_0[#_tab_0 + 1] = _value_0
		_idx_0 = _idx_0 + 1
	else
		_tab_0[_key_0] = _value_0
	end
end
local _idx_1 = 1
for _key_0, _value_0 in pairs(b) do
	if _idx_1 == _key_0 then
		_tab_0[#_tab_0 + 1] = _value_0
		_idx_1 = _idx_1 + 1
	else
		_tab_0[_key_0] = _value_0
	end
end
merge = _tab_0
local mt = { }
local add
add = function(self, right)
	return setmetatable({
		value = self.value + right.value
	}, mt)
end
mt.__add = add
local a = setmetatable({
	value = 1
}, mt)
local b = setmetatable({
	value = 2
}, {
	__add = add
})
local c = setmetatable({
	value = 3
}, {
	__add = mt.__add
})
local d = a + b + c
print(d.value)
local _ <close> = setmetatable({ }, {
	__close = function()
		return print("out of scope")
	end
})
local tb = setmetatable({ }, {
	["value"] = 123
})
getmetatable(tb).__index = getmetatable(tb)
print(tb.value)
setmetatable(tb, {
	__index = {
		item = "hello"
	}
})
print(tb.item)
local item, new, close, getter
do
	local _obj_0 = tb
	item, new = _obj_0[1], _obj_0.new
	do
		local _obj_1 = getmetatable(_obj_0)
		close, getter = _obj_1.__close, _obj_1.__index
	end
end
print(item, new, close, getter)
do
	local _obj_0 = func
	if _obj_0 ~= nil then
		_obj_0()
	end
end
print((function()
	local _obj_0 = abc
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0["hello world"]
		if _obj_1 ~= nil then
			return _obj_1.xyz
		end
		return nil
	end
	return nil
end)())
local x
do
	local _obj_0 = tab
	if _obj_0 ~= nil then
		x = _obj_0.value
	end
end
local len = (function()
	local _obj_0 = utf8
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or (function()
	local _obj_0 = string
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or function(o)
	return #o
end
if print and (x ~= nil) then
	print(x)
end
local _with_0 = io.open("test.txt", "w")
if _with_0 ~= nil then
	_with_0:write("hello")
	_with_0:close()
end
print("hello")
print(1, 2)
print(1, 2, 3)
print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
local a, b, c, d
if b ~= nil then
	a = b
else
	if c ~= nil then
		a = c
	else
		a = d
	end
end
func((function()
	if a ~= nil then
		return a
	else
		return { }
	end
end)())
if a == nil then
	a = false
end
local list = {
	1,
	2,
	3
}
func({
	1,
	2,
	3
})
local tb = {
	name = "abc",
	values = {
		"a",
		"b",
		"c"
	},
	objects = {
		{
			name = "a",
			value = 1,
			func = function(self)
				return self.value + 1
			end,
			tb = {
				fieldA = 1
			}
		},
		{
			name = "b",
			value = 2,
			func = function(self)
				return self.value + 2
			end,
			tb = { }
		}
	}
}
do
	local insert, concat = table.insert, table.concat
	local C, Ct, Cmt
	do
		local _obj_0 = require("lpeg")
		C, Ct, Cmt = _obj_0.C, _obj_0.Ct, _obj_0.Cmt
	end
	local x, y, z
	do
		local _obj_0 = require('mymodule')
		x, y, z = _obj_0.x, _obj_0.y, _obj_0.z
	end
	local a, b, c
	local _obj_0 = require('module')
	a, b, c = _obj_0.a, _obj_0.b, _obj_0.c
end
do
	local module = require('module')
	local module_x = require('module_x')
	local d_a_s_h_e_s = require("d-a-s-h-e-s")
	local part = require("module.part")
end
do
	local PlayerModule = require("player")
	local C, Ct, Cmt
	do
		local _obj_0 = require("lpeg")
		C, Ct, Cmt = _obj_0.C, _obj_0.Ct, _obj_0.Cmt
	end
	local one, two, ch
	local _obj_0 = require("export")
	one, two, ch = _obj_0[1], _obj_0[2], _obj_0.Something.umm[1]
end
local _module_0 = { }
local a, b, c = 1, 2, 3
_module_0["a"], _module_0["b"], _module_0["c"] = a, b, c
local cool = "cat"
_module_0["cool"] = cool
local What
if this then
	What = "abc"
else
	What = "def"
end
_module_0["What"] = What
local y
y = function()
	local hallo = 3434
end
_module_0["y"] = y
local Something
local _class_0
local _base_0 = {
	umm = "cool"
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Something"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Something = _class_0
_module_0["Something"] = Something
return _module_0
local _module_0 = { }
local loadstring, tolua
do
	local _obj_0 = yue
	loadstring, tolua = _obj_0.loadstring, _obj_0.to_lua
end
_module_0["loadstring"], _module_0["tolua"] = loadstring, tolua
local fieldA = tb.itemA.fieldA
if fieldA == nil then
	fieldA = 'default'
end
_module_0["fieldA"] = fieldA
return _module_0
local _module_0 = setmetatable({ }, { })
_module_0.itemA = tb
getmetatable(_module_0).__index = items
_module_0["a-b-c"] = 123
return _module_0
local _module_0 = { }
local d, e, f = 3, 2, 1
_module_0[#_module_0 + 1] = d
_module_0[#_module_0 + 1] = e
_module_0[#_module_0 + 1] = f
if this then
	_module_0[#_module_0 + 1] = 123
else
	_module_0[#_module_0 + 1] = 456
end
local _with_0 = tmp
local j = 2000
_module_0[#_module_0 + 1] = _with_0
return _module_0
local _module_0 = nil
_module_0 = function()
	print("hello")
	return 123
end
return _module_0
local hello = "world"
local a, b, c = 1, 2, 3
hello = 123
local x = 1
x = x + 1
x = x - 1
x = x * 10
x = x / 10
x = x % 10
local s = s .. "world"
local arg = arg or "default value"
local a = 0
local b = 0
local c = 0
local d = 0
local e = 0
local x = f()
local y = x
local z = x
do
	local a
	a = 1
	local x, y, z
	print("forward declare all variables as locals")
	x = function()
		return 1 + y + z
	end
	y, z = 2, 3
	instance = Item:new()
end
do
	local X
	X = 1
	local B
	print("only forward declare upper case variables")
	local a = 1
	B = 2
end
do
	a = 1
	print("declare all variables as globals")
	local x
	x = function()
		return 1 + y + z
	end
	local y, z = 2, 3
end
do
	X = 1
	print("only declare upper case variables as globals")
	local a = 1
	local B = 2
	local Temp
	Temp = "a local value"
end
local thing = {
	1,
	2
}
local a, b = thing[1], thing[2]
print(a, b)
local obj = {
	hello = "world",
	day = "tuesday",
	length = 20
}
local hello, the_day = obj.hello, obj.day
print(hello, the_day)
local day = obj.day
local obj2 = {
	numbers = {
		1,
		2,
		3,
		4
	},
	properties = {
		color = "green",
		height = 13.5
	}
}
local first, second, color = obj2.numbers[1], obj2.numbers[2], obj2.properties.color
print(first, second, color)
local first, second, color
local _obj_0 = obj2
first, second, color = _obj_0.numbers[1], _obj_0.numbers[2], _obj_0.properties.color
local concat, insert
local _obj_0 = table
concat, insert = _obj_0.concat, _obj_0.insert
local mix, max, rand
local _obj_0 = math
mix, max, rand = _obj_0.mix, _obj_0.max, _obj_0.random
local name, job
local _obj_0 = person
name, job = _obj_0.name, _obj_0.job
if name == nil then
	name = "nameless"
end
if job == nil then
	job = "jobless"
end
local two, four
local _obj_0 = items
two, four = _obj_0[2], _obj_0[4]
local tuples = {
	{
		"hello",
		"world"
	},
	{
		"egg",
		"head"
	}
}
for _index_0 = 1, #tuples do
	local _des_0 = tuples[_index_0]
	local left, right = _des_0[1], _des_0[2]
	print(left, right)
end
local user = database.find_user("moon")
if user then
	print(user.name)
end
local hello = os.getenv("hello")
if hello then
	print("You have hello", hello)
else
	local world = os.getenv("world")
	if world then
		print("you have world", world)
	else
		print("nothing :(")
	end
end
do
	local success, result = pcall(function()
		return "get result without problems"
	end)
	if success then
		print(result)
	end
end
print("OK")
local list = {
	1,
	2,
	3,
	4,
	5
}
local fn
fn = function(ok)
	return ok, table.unpack(list)
end
(function(_arg_0, ...)
	local ok = _arg_0
	local count = select('#', ...)
	local first = select(1, ...)
	return print(ok, count, first)
end)(fn(true))
Rx.Observable.fromRange(1, 8):filter(function(x)
	return x % 2 == 0
end):concat(Rx.Observable.of('who do we appreciate')):map(function(value)
	return value .. '!'
end):subscribe(print)
local str = strA .. strB .. strC
func(3000, "192.168.1.1")
xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
local success, result = xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return yue.traceback(err)
end)
xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
success, result = pcall(function()
	return func(1, 2, 3)
end)
pcall(function()
	print("trying")
	return func(1, 2, 3)
end)
success, result = xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
if success then
	print(result)
end
local a <const> = 123
local _ <close> = setmetatable({ }, {
	__close = function()
		return print("Out of scope.")
	end
})
local a, b, c, d
local _obj_0 = tb
a, b, c, d = _obj_0.a, _obj_0.b, _obj_0[1], _obj_0[2]
local some_string = "Here is a string\n  that has a line break in it."
print("I am " .. tostring(math.random() * 100) .. "% sure.")
local integer = 1000000
local hex = 0xEFBBBF
local my_function
my_function = function() end
my_function()
local func_a
func_a = function()
	return print("hello world")
end
local func_b
func_b = function()
	local value = 100
	return print("The value:", value)
end
func_a()
func_b()
local sum
sum = function(x, y)
	return print("sum", x + y)
end
sum(10, 20)
print(sum(10, 20))
a(b(c("a", "b", "c")))
print("x:", sum(10, 20), "y:", sum(30, 40))
local sum
sum = function(x, y)
	return x + y
end
print("The sum is ", sum(10, 20))
local sum
sum = function(x, y)
	return x + y
end
local mystery
mystery = function(x, y)
	return x + y, x - y
end
local a, b = mystery(10, 20)
local func
func = function(self, num)
	return self.value + num
end
local my_function
my_function = function(name, height)
	if name == nil then
		name = "something"
	end
	if height == nil then
		height = 100
	end
	print("Hello I am", name)
	return print("My height is", height)
end
local some_args
some_args = function(x, y)
	if x == nil then
		x = 100
	end
	if y == nil then
		y = x + 1000
	end
	return print(x + y)
end
local a = x - 10
local b = x - 10
local c = x(-y)
local d = x - z
local x = func("hello") + 100
local y = func("hello" + 100)
my_func(5, 4, 3, 8, 9, 10)
cool_func(1, 2, 3, 4, 5, 6, 7, 8)
my_func(5, 6, 7, 6, another_func(6, 7, 8, 9, 1, 2), 5, 4)
local x = {
	1,
	2,
	3,
	4,
	a_func(4, 5, 5, 6),
	8,
	9,
	10
}
local y = {
	my_func(1, 2, 3, 4, 5),
	5,
	6,
	7
}
if func(1, 2, 3, "hello", "world") then
	print("hello")
	print("I am inside if")
end
if func(1, 2, 3, "hello", "world") then
	print("hello")
	print("I am inside if")
end
f(function()
	return print("hello")
end)
f(function(self)
	return print(self.value)
end)
map(function(x)
	return x * 2
end, {
	1,
	2,
	3
})
local result, msg
do
	result, msg = readAsync("filename.txt", function(data)
		print(data)
		return processAsync(data, function(info)
			return check(info)
		end)
	end)
end
print(result, msg)
local some_values = {
	1,
	2,
	3,
	4
}
local some_values = {
	name = "Bill",
	age = 200,
	["favorite food"] = "rice"
}
local profile = {
	height = "4 feet",
	shoe_size = 13,
	favorite_foods = {
		"ice cream",
		"donuts"
	}
}
local values = {
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	name = "superman",
	occupation = "crime fighting"
}
my_function({
	dance = "Tango",
	partner = "none"
})
local y = {
	type = "dog",
	legs = 4,
	tails = 1
}
local tbl = {
	["do"] = "something",
	["end"] = "hunger"
}
local hair = "golden"
local height = 200
local person = {
	hair = hair,
	height = height,
	shoe_size = 40
}
print_table({
	hair = hair,
	height = height
})
local t = {
	[1 + 2] = "hello",
	["hello world"] = true
}
local some_values = {
	1,
	2,
	3,
	4
}
local list_with_one_element = {
	1
}
local items = {
	1,
	2,
	3,
	4
}
local doubled
local _accum_0 = { }
local _len_0 = 1
for i, item in ipairs(items) do
	_accum_0[_len_0] = item * 2
	_len_0 = _len_0 + 1
end
doubled = _accum_0
local iter = ipairs(items)
local slice
local _accum_0 = { }
local _len_0 = 1
for i, item in iter do
	if i > 1 and i < 3 then
		_accum_0[_len_0] = item
		_len_0 = _len_0 + 1
	end
end
slice = _accum_0
local doubled
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item * 2
	_len_0 = _len_0 + 1
end
doubled = _accum_0
local x_coords = {
	4,
	5,
	6,
	7
}
local y_coords = {
	9,
	2,
	3
}
local points
local _accum_0 = { }
local _len_0 = 1
for _index_0 = 1, #x_coords do
	local x = x_coords[_index_0]
	for _index_1 = 1, #y_coords do
		local y = y_coords[_index_1]
		_accum_0[_len_0] = {
			x,
			y
		}
		_len_0 = _len_0 + 1
	end
end
points = _accum_0
local evens
local _accum_0 = { }
local _len_0 = 1
for i = 1, 100 do
	if i % 2 == 0 then
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
end
evens = _accum_0
local thing = {
	color = "red",
	name = "fast",
	width = 123
}
local thing_copy
local _tbl_0 = { }
for k, v in pairs(thing) do
	_tbl_0[k] = v
end
thing_copy = _tbl_0
local no_color
local _tbl_0 = { }
for k, v in pairs(thing) do
	if k ~= "color" then
		_tbl_0[k] = v
	end
end
no_color = _tbl_0
local numbers = {
	1,
	2,
	3,
	4
}
local sqrts
local _tbl_0 = { }
for _index_0 = 1, #numbers do
	local i = numbers[_index_0]
	_tbl_0[i] = math.sqrt(i)
end
sqrts = _tbl_0
local tuples = {
	{
		"hello",
		"world"
	},
	{
		"foo",
		"bar"
	}
}
local tbl
local _tbl_0 = { }
for _index_0 = 1, #tuples do
	local tuple = tuples[_index_0]
	local _key_0, _val_0 = unpack(tuple)
	_tbl_0[_key_0] = _val_0
end
tbl = _tbl_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
local _max_0 = 5
for _index_0 = 1, _max_0 < 0 and #_list_0 + _max_0 or _max_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 2, #_list_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 1, #_list_0, 2 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
for i = 10, 20 do
	print(i)
end
for k = 1, 15, 2 do
	print(k)
end
for key, value in pairs(object) do
	print(key, value)
end
local _list_0 = items
local _max_0 = 4
for _index_0 = 2, _max_0 < 0 and #_list_0 + _max_0 or _max_0 do
	local item = _list_0[_index_0]
	print(item)
end
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print(item)
end
for j = 1, 10, 3 do
	print(j)
end
local doubled_evens
local _accum_0 = { }
local _len_0 = 1
for i = 1, 20 do
	if i % 2 == 0 then
		_accum_0[_len_0] = i * 2
	else
		_accum_0[_len_0] = i
	end
	_len_0 = _len_0 + 1
end
doubled_evens = _accum_0
local func_a
func_a = function()
	for i = 1, 10 do
		print(i)
	end
end
local func_b
func_b = function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
print(func_a())
print(func_b())
local i = 10
repeat
	print(i)
	i = i - 1
until i == 0
local i = 10
while i > 0 do
	print(i)
	i = i - 1
end
while running == true do
	my_function()
end
local i = 10
while not (i == 0) do
	print(i)
	i = i - 1
end
while not (running == false) do
	my_function()
end
local i = 0
while i < 10 do
	i = i + 1
	if i % 2 == 0 then
		goto _continue_0
	end
	print(i)
	::_continue_0::
end
local my_numbers = {
	1,
	2,
	3,
	4,
	5,
	6
}
local odds
local _accum_0 = { }
local _len_0 = 1
for _index_0 = 1, #my_numbers do
	local x = my_numbers[_index_0]
	if x % 2 == 1 then
		goto _continue_0
	end
	_accum_0[_len_0] = x
	_len_0 = _len_0 + 1
	::_continue_0::
end
odds = _accum_0
local have_coins = false
if have_coins then
	print("Got coins")
else
	print("No coins")
end
local have_coins = false
if have_coins then
	print("Got coins")
else
	print("No coins")
end
local have_coins = false
print((function()
	if have_coins then
		return "Got coins"
	else
		return "No coins"
	end
end)())
local is_tall
is_tall = function(name)
	if name == "Rob" then
		return true
	else
		return false
	end
end
local message
if is_tall("Rob") then
	message = "I am very tall"
else
	message = "I am not so tall"
end
print(message)
if not (os.date("%A") == "Monday") then
	print("it is not Monday!")
end
if not (math.random() > 0.1) then
	print("You're lucky!")
end
local a = 5
if (1 == a or 3 == a or 5 == a or 7 == a) then
	print("checking equality with discrete values")
end
if (function()
	local _check_0 = list
	for _index_0 = 1, #_check_0 do
		if _check_0[_index_0] == a then
			return true
		end
	end
	return false
end)() then
	print("checking if `a` is in a list")
end
if not (math.random() > 0.1) then
	print("You're lucky!")
end
if name == "Rob" then
	print("hello world")
end
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print("item: ", item)
end
while game:isRunning() do
	game:update()
end
while not reader:eof() do
	reader:parse_line()
end
local name = "Dan"
if "Robert" == name then
	print("You are Robert")
elseif "Dan" == name or "Daniel" == name then
	print("Your name, it's Dan")
else
	print("I don't know about your name")
end
local b = 1
local next_number
if 1 == b then
	next_number = 2
elseif 2 == b then
	next_number = 3
else
	next_number = error("can't count that high!")
end
local msg
local _exp_0 = math.random(1, 5)
if 1 == _exp_0 then
	msg = "you are lucky"
elseif 2 == _exp_0 then
	msg = "you are almost lucky"
else
	msg = "not so lucky"
end
do
	local _exp_0 = math.random(1, 5)
	if 1 == _exp_0 then
		print("you are lucky")
	else
		print("not so lucky")
	end
end
local _exp_0 = math.random(1, 5)
if 1 == _exp_0 then
	print("you are lucky")
else
	print("not so lucky")
end
local items = {
	{
		x = 100,
		y = 200
	},
	{
		width = 300,
		height = 400
	}
}
for _index_0 = 1, #items do
	local item = items[_index_0]
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
		if _tab_0 then
			local width = item.width
			local height = item.height
			if width ~= nil and height ~= nil then
				print("size " .. tostring(width) .. ", " .. tostring(height))
			end
		end
	end
end
local item = { }
local x, y = item.pos.x, item.pos.y
if x == nil then
	x = 50
end
if y == nil then
	y = 200
end
local _type_0 = type(item)
local _tab_0 = "table" == _type_0 or "userdata" == _type_0
if _tab_0 then
	do
		local _obj_0 = item.pos
		local _type_1 = type(_obj_0)
		if "table" == _type_1 or "userdata" == _type_1 then
			x = _obj_0.x
		end
	end
	do
		local _obj_0 = item.pos
		local _type_1 = type(_obj_0)
		if "table" == _type_1 or "userdata" == _type_1 then
			y = _obj_0.y
		end
	end
	if x == nil then
		x = 50
	end
	if y == nil then
		y = 200
	end
	print("Vec2 " .. tostring(x) .. ", " .. tostring(y))
end
local Inventory
local _class_0
local _base_0 = {
	add_item = function(self, name)
		if self.items[name] then
			local _obj_0 = self.items
			_obj_0[name] = _obj_0[name] + 1
		else
			self.items[name] = 1
		end
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self)
		self.items = { }
	end,
	__base = _base_0,
	__name = "Inventory"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Inventory = _class_0
local inv = Inventory()
inv:add_item("t-shirt")
inv:add_item("pants")
local Person
do
	local _class_0
	local _base_0 = {
		clothes = { },
		give_item = function(self, name)
			return table.insert(self.clothes, name)
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Person"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Person = _class_0
end
local a = Person()
local b = Person()
a:give_item("pants")
b:give_item("shirt")
local _list_0 = a.clothes
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print(item)
end
local Person
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self)
		self.clothes = { }
	end,
	__base = _base_0,
	__name = "Person"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Person = _class_0
local BackPack
local _class_0
local _parent_0 = Inventory
local _base_0 = {
	size = 10,
	add_item = function(self, name)
		if #self.items > size then
			error("backpack is full")
		end
		return _class_0.__parent.__base.add_item(self, name)
	end
}
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "BackPack",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
BackPack = _class_0
local Shelf
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Shelf"
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
	self.__inherited = function(self, child)
		return print(self.__name, "was inherited by", child.__name)
	end
	Shelf = _class_0
end
local Cupboard
local _class_0
local _parent_0 = Shelf
local _base_0 = { }
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "Cupboard",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
Cupboard = _class_0
local MyClass
local _class_0
local _parent_0 = ParentClass
local _base_0 = {
	a_method = function(self)
		_class_0.__parent.__base.a_method(self, "hello", "world")
		_class_0.__parent.a_method(self, "hello", "world")
		_class_0.__parent.a_method(self, "hello", "world")
		return assert(_class_0.__parent == ParentClass)
	end
}
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "MyClass",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
MyClass = _class_0
local b = BackPack()
assert(b.__class == BackPack)
print(BackPack.size)
print(BackPack.__name)
local Things
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Things"
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
	self.some_func = function(self)
		return print("Hello from", self.__name)
	end
	Things = _class_0
end
Things:some_func()
assert(Things().some_func == nil)
local Counter
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self)
			self.__class.count = self.__class.count + 1
		end,
		__base = _base_0,
		__name = "Counter"
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
	self.count = 0
	Counter = _class_0
end
Counter()
Counter()
print(Counter.count)
self.__class:hello(1, 2, 3, 4)
local Things
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Things"
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
self.class_var = "hello world"
Things = _class_0
local MoreThings
local _class_0
local secret, log
local _base_0 = {
	some_method = function(self)
		return log("hello world: " .. secret)
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "MoreThings"
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
secret = 123
log = function(msg)
	return print("LOG:", msg)
end
MoreThings = _class_0
assert(self == self)
assert(self.__class == self.__class)
local some_instance_method
some_instance_method = function(self, ...)
	return self.__class(...)
end
local Something
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self, foo, bar, biz, baz)
			self.foo = foo
			self.bar = bar
			self.__class.biz = biz
			self.__class.baz = baz
		end,
		__base = _base_0,
		__name = "Something"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Something = _class_0
end
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self, foo, bar, biz, baz)
		self.foo = foo
		self.bar = bar
		self.__class.biz = biz
		self.__class.baz = baz
	end,
	__base = _base_0,
	__name = "Something"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Something = _class_0
local new
new = function(self, fieldA, fieldB)
	self.fieldA = fieldA
	self.fieldB = fieldB
	return self
end
local obj = new({ }, 123, "abc")
print(obj)
local x
local Bucket
local _class_0
local _base_0 = {
	drops = 0,
	add_drop = function(self)
		self.drops = self.drops + 1
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Bucket"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Bucket = _class_0
x = _class_0
local BigBucket
do
	local _class_0
	local _parent_0 = Bucket
	local _base_0 = {
		add_drop = function(self)
			self.drops = self.drops + 10
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "BigBucket",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	BigBucket = _class_0
end
assert(Bucket.__name == "BigBucket")
local x
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "x"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
x = _class_0
local MyIndex = {
	__index = {
		var = 1
	}
}
local X
do
	local _class_0
	local _base_0 = {
		func = function(self)
			return print(123)
		end
	}
	local _list_0 = {
		MyIndex
	}
	for _index_0 = 1, #_list_0 do
		local _item_0 = _list_0[_index_0]
		local _cls_0, _mixin_0 = (_item_0.__base ~= nil), _item_0.__base or _item_0
		for _key_0, _val_0 in pairs(_mixin_0) do
			if _base_0[_key_0] == nil and (not _cls_0 or not _key_0:match("^__")) then
				_base_0[_key_0] = _val_0
			end
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "X"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	X = _class_0
end
local x = X()
print(x.var)
local Y
do
	local _class_0
	local _base_0 = { }
	local _list_0 = {
		X
	}
	for _index_0 = 1, #_list_0 do
		local _item_0 = _list_0[_index_0]
		local _cls_0, _mixin_0 = (_item_0.__base ~= nil), _item_0.__base or _item_0
		for _key_0, _val_0 in pairs(_mixin_0) do
			if _base_0[_key_0] == nil and (not _cls_0 or not _key_0:match("^__")) then
				_base_0[_key_0] = _val_0
			end
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Y"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Y = _class_0
end
local y = Y()
y:func()
assert(y.__class.__parent ~= X)
local _with_0 = Person()
_with_0.name = "Oswald"
_with_0:add_relative(my_dad)
_with_0:save()
print(_with_0.name)
local file
local _with_0 = File("favorite_foods.txt")
_with_0:set_encoding("utf8")
file = _with_0
local create_person
create_person = function(name, relatives)
	local _with_0 = Person()
	_with_0.name = name
	for _index_0 = 1, #relatives do
		local relative = relatives[_index_0]
		_with_0:add_relative(relative)
	end
	return _with_0
end
local me = create_person("Leaf", {
	dad,
	mother,
	sister
})
local str = "Hello"
print("original:", str)
print("upper:", str:upper())
local _with_0 = tb
_with_0[1] = 1
print(_with_0[2])
do
	local _with_1 = _with_0[abc]
	_with_1[3] = _with_1[2]:func()
	_with_1["key-name"] = value
end
_with_0[#_with_0 + 1] = "abc"
do
	local var = "hello"
	print(var)
end
print(var)
local counter
do
	local i = 0
	counter = function()
		i = i + 1
		return i
	end
end
print(counter())
print(counter())
local tbl = {
	key = (function()
		print("assigning key!")
		return 1234
	end)()
}
local my_object = {
	value = 1000,
	write = function(self)
		return print("the value:", self.value)
	end
}
local run_callback
run_callback = function(func)
	print("running callback...")
	return func()
end
run_callback(my_object.write)
run_callback((function()
	local _base_0 = my_object
	local _fn_0 = _base_0.write
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
local i = 100
local my_func
my_func = function()
	i = 10
	while i > 0 do
		print(i)
		i = i - 1
	end
end
my_func()
print(i)
local i = 100
local my_func
my_func = function()
	local i = "hello"
end
my_func()
print(i)
local tmp = 1213
local i, k = 100, 50
local my_func
my_func = function(add)
	local tmp = tmp + add
	i = i + tmp
	k = k + tmp
end
my_func(22)
print(i, k)
local _module_0 = { }
local p, to_lua
do
	local _obj_0 = require("yue")
	p, to_lua = _obj_0.p, _obj_0.to_lua
end
local inventory = {
	equipment = {
		"sword",
		"shield"
	},
	items = {
		{
			name = "potion",
			count = 10
		},
		{
			name = "bread",
			count = 3
		}
	}
}
print(reduce(filter(map({
	1,
	2,
	3
}, function(x)
	return x * 2
end), function(x)
	return x > 4
end), 0, function(a, b)
	return a + b
end))
local apple = setmetatable({
	size = 15,
}, {
	__index = {
		color = 0x00ffff
	}
})
if (getmetatable(apple) ~= nil) then
	p(apple.color, getmetatable(apple).__index)
end
local _ud83c_udf1b = "月之脚本"
_module_0["🌛"] = _ud83c_udf1b
return _module_0
local area = 6.2831853071796 * 5
print('hello world')
do
	assert(item ~= nil)
end
local value = item
if (f1() and f2() and f3()) then
	print("OK")
end
do
	local funcA
	funcA = function() end
end
local funcA
funcA = function()
	return "assign the Yue defined variable"
end
local function funcB() end
funcB = function()
	return "assign the Lua defined variable"
end
-- raw Lua codes insertion
if cond then
  print("output")
end
print("yuescript")
print(3)
print("Valid enum type:", "Static")
if tb ~= nil then
	tb:func()
end
if tb ~= nil then
	tb:func()
end
print(1 < 2 and 2 <= 2 and 2 < 3 and 3 == 3 and 3 > 2 and 2 >= 1 and 1 == 1 and 1 < 3 and 3 ~= 5)
local a = 5
print(1 <= a and a <= 10)
local v
v = function(x)
	print(x)
	return x
end
print((function()
	local _cond_0 = v(2)
	if not (v(1) < _cond_0) then
		return false
	else
		return _cond_0 <= v(3)
	end
end)())
print((function()
	local _cond_0 = v(2)
	if not (v(1) > _cond_0) then
		return false
	else
		return _cond_0 <= v(3)
	end
end)())
local tab = { }
tab[#tab + 1] = "Value"
local parts = {
	"shoulders",
	"knees"
}
local lyrics
do
	local _tab_0 = {
		"head"
	}
	local _idx_0 = 1
	for _key_0, _value_0 in pairs(parts) do
		if _idx_0 == _key_0 then
			_tab_0[#_tab_0 + 1] = _value_0
			_idx_0 = _idx_0 + 1
		else
			_tab_0[_key_0] = _value_0
		end
	end
	_tab_0[#_tab_0 + 1] = "and"
	_tab_0[#_tab_0 + 1] = "toes"
	lyrics = _tab_0
end
local copy
do
	local _tab_0 = { }
	local _idx_0 = 1
	for _key_0, _value_0 in pairs(other) do
		if _idx_0 == _key_0 then
			_tab_0[#_tab_0 + 1] = _value_0
			_idx_0 = _idx_0 + 1
		else
			_tab_0[_key_0] = _value_0
		end
	end
	copy = _tab_0
end
local a = {
	1,
	2,
	3,
	x = 1
}
local b = {
	4,
	5,
	y = 1
}
local merge
local _tab_0 = { }
local _idx_0 = 1
for _key_0, _value_0 in pairs(a) do
	if _idx_0 == _key_0 then
		_tab_0[#_tab_0 + 1] = _value_0
		_idx_0 = _idx_0 + 1
	else
		_tab_0[_key_0] = _value_0
	end
end
local _idx_1 = 1
for _key_0, _value_0 in pairs(b) do
	if _idx_1 == _key_0 then
		_tab_0[#_tab_0 + 1] = _value_0
		_idx_1 = _idx_1 + 1
	else
		_tab_0[_key_0] = _value_0
	end
end
merge = _tab_0
local mt = { }
local add
add = function(self, right)
	return setmetatable({
		value = self.value + right.value
	}, mt)
end
mt.__add = add
local a = setmetatable({
	value = 1
}, mt)
local b = setmetatable({
	value = 2
}, {
	__add = add
})
local c = setmetatable({
	value = 3
}, {
	__add = mt.__add
})
local d = a + b + c
print(d.value)
local _ <close> = setmetatable({ }, {
	__close = function()
		return print("out of scope")
	end
})
local tb = setmetatable({ }, {
	["value"] = 123
})
getmetatable(tb).__index = getmetatable(tb)
print(tb.value)
setmetatable(tb, {
	__index = {
		item = "hello"
	}
})
print(tb.item)
local item, new, close, getter
do
	local _obj_0 = tb
	item, new = _obj_0[1], _obj_0.new
	do
		local _obj_1 = getmetatable(_obj_0)
		close, getter = _obj_1.__close, _obj_1.__index
	end
end
print(item, new, close, getter)
do
	local _obj_0 = func
	if _obj_0 ~= nil then
		_obj_0()
	end
end
print((function()
	local _obj_0 = abc
	if _obj_0 ~= nil then
		local _obj_1 = _obj_0["hello world"]
		if _obj_1 ~= nil then
			return _obj_1.xyz
		end
		return nil
	end
	return nil
end)())
local x
do
	local _obj_0 = tab
	if _obj_0 ~= nil then
		x = _obj_0.value
	end
end
local len = (function()
	local _obj_0 = utf8
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or (function()
	local _obj_0 = string
	if _obj_0 ~= nil then
		return _obj_0.len
	end
	return nil
end)() or function(o)
	return #o
end
if print and (x ~= nil) then
	print(x)
end
local _with_0 = io.open("test.txt", "w")
if _with_0 ~= nil then
	_with_0:write("hello")
	_with_0:close()
end
print("hello")
print(1, 2)
print(1, 2, 3)
print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
local a, b, c, d
if b ~= nil then
	a = b
else
	if c ~= nil then
		a = c
	else
		a = d
	end
end
func((function()
	if a ~= nil then
		return a
	else
		return { }
	end
end)())
if a == nil then
	a = false
end
local list = {
	1,
	2,
	3
}
func({
	1,
	2,
	3
})
local tb = {
	name = "abc",
	values = {
		"a",
		"b",
		"c"
	},
	objects = {
		{
			name = "a",
			value = 1,
			func = function(self)
				return self.value + 1
			end,
			tb = {
				fieldA = 1
			}
		},
		{
			name = "b",
			value = 2,
			func = function(self)
				return self.value + 2
			end,
			tb = { }
		}
	}
}
do
	local insert, concat = table.insert, table.concat
	local C, Ct, Cmt
	do
		local _obj_0 = require("lpeg")
		C, Ct, Cmt = _obj_0.C, _obj_0.Ct, _obj_0.Cmt
	end
	local x, y, z
	do
		local _obj_0 = require('mymodule')
		x, y, z = _obj_0.x, _obj_0.y, _obj_0.z
	end
	local a, b, c
	local _obj_0 = require('module')
	a, b, c = _obj_0.a, _obj_0.b, _obj_0.c
end
do
	local module = require('module')
	local module_x = require('module_x')
	local d_a_s_h_e_s = require("d-a-s-h-e-s")
	local part = require("module.part")
end
do
	local PlayerModule = require("player")
	local C, Ct, Cmt
	do
		local _obj_0 = require("lpeg")
		C, Ct, Cmt = _obj_0.C, _obj_0.Ct, _obj_0.Cmt
	end
	local one, two, ch
	local _obj_0 = require("export")
	one, two, ch = _obj_0[1], _obj_0[2], _obj_0.Something.umm[1]
end
local _module_0 = { }
local a, b, c = 1, 2, 3
_module_0["a"], _module_0["b"], _module_0["c"] = a, b, c
local cool = "cat"
_module_0["cool"] = cool
local What
if this then
	What = "abc"
else
	What = "def"
end
_module_0["What"] = What
local y
y = function()
	local hallo = 3434
end
_module_0["y"] = y
local Something
local _class_0
local _base_0 = {
	umm = "cool"
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Something"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Something = _class_0
_module_0["Something"] = Something
return _module_0
local _module_0 = { }
local loadstring, tolua
do
	local _obj_0 = yue
	loadstring, tolua = _obj_0.loadstring, _obj_0.to_lua
end
_module_0["loadstring"], _module_0["tolua"] = loadstring, tolua
local fieldA = tb.itemA.fieldA
if fieldA == nil then
	fieldA = 'default'
end
_module_0["fieldA"] = fieldA
return _module_0
local _module_0 = setmetatable({ }, { })
_module_0.itemA = tb
getmetatable(_module_0).__index = items
_module_0["a-b-c"] = 123
return _module_0
local _module_0 = { }
local d, e, f = 3, 2, 1
_module_0[#_module_0 + 1] = d
_module_0[#_module_0 + 1] = e
_module_0[#_module_0 + 1] = f
if this then
	_module_0[#_module_0 + 1] = 123
else
	_module_0[#_module_0 + 1] = 456
end
local _with_0 = tmp
local j = 2000
_module_0[#_module_0 + 1] = _with_0
return _module_0
local _module_0 = nil
_module_0 = function()
	print("hello")
	return 123
end
return _module_0
local hello = "world"
local a, b, c = 1, 2, 3
hello = 123
local x = 1
x = x + 1
x = x - 1
x = x * 10
x = x / 10
x = x % 10
local s = s .. "world"
local arg = arg or "default value"
local a = 0
local b = 0
local c = 0
local d = 0
local e = 0
local x = f()
local y = x
local z = x
do
	local a
	a = 1
	local x, y, z
	print("forward declare all variables as locals")
	x = function()
		return 1 + y + z
	end
	y, z = 2, 3
	instance = Item:new()
end
do
	local X
	X = 1
	local B
	print("only forward declare upper case variables")
	local a = 1
	B = 2
end
do
	a = 1
	print("declare all variables as globals")
	local x
	x = function()
		return 1 + y + z
	end
	local y, z = 2, 3
end
do
	X = 1
	print("only declare upper case variables as globals")
	local a = 1
	local B = 2
	local Temp
	Temp = "a local value"
end
local thing = {
	1,
	2
}
local a, b = thing[1], thing[2]
print(a, b)
local obj = {
	hello = "world",
	day = "tuesday",
	length = 20
}
local hello, the_day = obj.hello, obj.day
print(hello, the_day)
local day = obj.day
local obj2 = {
	numbers = {
		1,
		2,
		3,
		4
	},
	properties = {
		color = "green",
		height = 13.5
	}
}
local first, second = obj2.numbers[1], obj2.numbers[2]
print(first, second, color)
local first, second, color
local _obj_0 = obj2
first, second, color = _obj_0.numbers[1], _obj_0.numbers[2], _obj_0.properties.color
local concat, insert
local _obj_0 = table
concat, insert = _obj_0.concat, _obj_0.insert
local mix, max, rand
local _obj_0 = math
mix, max, rand = _obj_0.mix, _obj_0.max, _obj_0.random
local name, job
local _obj_0 = person
name, job = _obj_0.name, _obj_0.job
if name == nil then
	name = "nameless"
end
if job == nil then
	job = "jobless"
end
local two, four
local _obj_0 = items
two, four = _obj_0[2], _obj_0[4]
local tuples = {
	{
		"hello",
		"world"
	},
	{
		"egg",
		"head"
	}
}
for _index_0 = 1, #tuples do
	local _des_0 = tuples[_index_0]
	local left, right = _des_0[1], _des_0[2]
	print(left, right)
end
local user = database.find_user("moon")
if user then
	print(user.name)
end
local hello = os.getenv("hello")
if hello then
	print("You have hello", hello)
else
	local world = os.getenv("world")
	if world then
		print("you have world", world)
	else
		print("nothing :(")
	end
end
do
	local success, result = pcall(function()
		return "get result without problems"
	end)
	if success then
		print(result)
	end
end
print("OK")
local list = {
	1,
	2,
	3,
	4,
	5
}
local fn
fn = function(ok)
	return ok, table.unpack(list)
end
(function(_arg_0, ...)
	local ok = _arg_0
	local count = select('#', ...)
	local first = select(1, ...)
	return print(ok, count, first)
end)(fn(true))
Rx.Observable.fromRange(1, 8):filter(function(x)
	return x % 2 == 0
end):concat(Rx.Observable.of('who do we appreciate')):map(function(value)
	return value .. '!'
end):subscribe(print)
local str = strA .. strB .. strC
func(3000, "192.168.1.1")
xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
local success, result = xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return yue.traceback(err)
end)
xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
success, result = pcall(function()
	return func(1, 2, 3)
end)
pcall(function()
	print("trying")
	return func(1, 2, 3)
end)
success, result = xpcall(function()
	return func(1, 2, 3)
end, function(err)
	return print(yue.traceback(err))
end)
if success then
	print(result)
end
local a <const> = 123
local _ <close> = setmetatable({ }, {
	__close = function()
		return print("Out of scope.")
	end
})
local a, b, c, d
local _obj_0 = tb
a, b, c, d = _obj_0.a, _obj_0.b, _obj_0[1], _obj_0[2]
local some_string = "Here is a string\n  that has a line break in it."
print("I am " .. tostring(math.random() * 100) .. "% sure.")
local integer = 1000000
local hex = 0xEFBBBF
local my_function
my_function = function() end
my_function()
local func_a
func_a = function()
	return print("hello world")
end
local func_b
func_b = function()
	local value = 100
	return print("The value:", value)
end
func_a()
func_b()
local sum
sum = function(x, y)
	return print("sum", x + y)
end
sum(10, 20)
print(sum(10, 20))
a(b(c("a", "b", "c")))
print("x:", sum(10, 20), "y:", sum(30, 40))
local sum
sum = function(x, y)
	return x + y
end
print("The sum is ", sum(10, 20))
local sum
sum = function(x, y)
	return x + y
end
local mystery
mystery = function(x, y)
	return x + y, x - y
end
local a, b = mystery(10, 20)
local func
func = function(self, num)
	return self.value + num
end
local my_function
my_function = function(name, height)
	if name == nil then
		name = "something"
	end
	if height == nil then
		height = 100
	end
	print("Hello I am", name)
	return print("My height is", height)
end
local some_args
some_args = function(x, y)
	if x == nil then
		x = 100
	end
	if y == nil then
		y = x + 1000
	end
	return print(x + y)
end
local a = x - 10
local b = x - 10
local c = x(-y)
local d = x - z
local x = func("hello") + 100
local y = func("hello" + 100)
my_func(5, 4, 3, 8, 9, 10)
cool_func(1, 2, 3, 4, 5, 6, 7, 8)
my_func(5, 6, 7, 6, another_func(6, 7, 8, 9, 1, 2), 5, 4)
local x = {
	1,
	2,
	3,
	4,
	a_func(4, 5, 5, 6),
	8,
	9,
	10
}
local y = {
	my_func(1, 2, 3, 4, 5),
	5,
	6,
	7
}
if func(1, 2, 3, "hello", "world") then
	print("hello")
	print("I am inside if")
end
if func(1, 2, 3, "hello", "world") then
	print("hello")
	print("I am inside if")
end
f(function()
	return print("hello")
end)
f(function(self)
	return print(self.value)
end)
map(function(x)
	return x * 2
end, {
	1,
	2,
	3
})
local result, msg
do
	result, msg = readAsync("filename.txt", function(data)
		print(data)
		return processAsync(data, function(info)
			return check(info)
		end)
	end)
end
print(result, msg)
local some_values = {
	1,
	2,
	3,
	4
}
local some_values = {
	name = "Bill",
	age = 200,
	["favorite food"] = "rice"
}
local profile = {
	height = "4 feet",
	shoe_size = 13,
	favorite_foods = {
		"ice cream",
		"donuts"
	}
}
local values = {
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	name = "superman",
	occupation = "crime fighting"
}
my_function({
	dance = "Tango",
	partner = "none"
})
local y = {
	type = "dog",
	legs = 4,
	tails = 1
}
local tbl = {
	["do"] = "something",
	["end"] = "hunger"
}
local hair = "golden"
local height = 200
local person = {
	hair = hair,
	height = height,
	shoe_size = 40
}
print_table({
	hair = hair,
	height = height
})
local t = {
	[1 + 2] = "hello",
	["hello world"] = true
}
local some_values = {
	1,
	2,
	3,
	4
}
local list_with_one_element = {
	1
}
local items = {
	1,
	2,
	3,
	4
}
local doubled
local _accum_0 = { }
local _len_0 = 1
for i, item in ipairs(items) do
	_accum_0[_len_0] = item * 2
	_len_0 = _len_0 + 1
end
doubled = _accum_0
local iter = ipairs(items)
local slice
local _accum_0 = { }
local _len_0 = 1
for i, item in iter do
	if i > 1 and i < 3 then
		_accum_0[_len_0] = item
		_len_0 = _len_0 + 1
	end
end
slice = _accum_0
local doubled
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item * 2
	_len_0 = _len_0 + 1
end
doubled = _accum_0
local x_coords = {
	4,
	5,
	6,
	7
}
local y_coords = {
	9,
	2,
	3
}
local points
local _accum_0 = { }
local _len_0 = 1
for _index_0 = 1, #x_coords do
	local x = x_coords[_index_0]
	for _index_1 = 1, #y_coords do
		local y = y_coords[_index_1]
		_accum_0[_len_0] = {
			x,
			y
		}
		_len_0 = _len_0 + 1
	end
end
points = _accum_0
local evens
local _accum_0 = { }
local _len_0 = 1
for i = 1, 100 do
	if i % 2 == 0 then
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
end
evens = _accum_0
local thing = {
	color = "red",
	name = "fast",
	width = 123
}
local thing_copy
local _tbl_0 = { }
for k, v in pairs(thing) do
	_tbl_0[k] = v
end
thing_copy = _tbl_0
local no_color
local _tbl_0 = { }
for k, v in pairs(thing) do
	if k ~= "color" then
		_tbl_0[k] = v
	end
end
no_color = _tbl_0
local numbers = {
	1,
	2,
	3,
	4
}
local sqrts
local _tbl_0 = { }
for _index_0 = 1, #numbers do
	local i = numbers[_index_0]
	_tbl_0[i] = math.sqrt(i)
end
sqrts = _tbl_0
local tuples = {
	{
		"hello",
		"world"
	},
	{
		"foo",
		"bar"
	}
}
local tbl
local _tbl_0 = { }
for _index_0 = 1, #tuples do
	local tuple = tuples[_index_0]
	local _key_0, _val_0 = unpack(tuple)
	_tbl_0[_key_0] = _val_0
end
tbl = _tbl_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
local _max_0 = 5
for _index_0 = 1, _max_0 < 0 and #_list_0 + _max_0 or _max_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 2, #_list_0 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
local slice
local _accum_0 = { }
local _len_0 = 1
local _list_0 = items
for _index_0 = 1, #_list_0, 2 do
	local item = _list_0[_index_0]
	_accum_0[_len_0] = item
	_len_0 = _len_0 + 1
end
slice = _accum_0
for i = 10, 20 do
	print(i)
end
for k = 1, 15, 2 do
	print(k)
end
for key, value in pairs(object) do
	print(key, value)
end
local _list_0 = items
local _max_0 = 4
for _index_0 = 2, _max_0 < 0 and #_list_0 + _max_0 or _max_0 do
	local item = _list_0[_index_0]
	print(item)
end
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print(item)
end
for j = 1, 10, 3 do
	print(j)
end
local doubled_evens
local _accum_0 = { }
local _len_0 = 1
for i = 1, 20 do
	if i % 2 == 0 then
		_accum_0[_len_0] = i * 2
	else
		_accum_0[_len_0] = i
	end
	_len_0 = _len_0 + 1
end
doubled_evens = _accum_0
local func_a
func_a = function()
	for i = 1, 10 do
		print(i)
	end
end
local func_b
func_b = function()
	local _accum_0 = { }
	local _len_0 = 1
	for i = 1, 10 do
		_accum_0[_len_0] = i
		_len_0 = _len_0 + 1
	end
	return _accum_0
end
print(func_a())
print(func_b())
local i = 10
repeat
	print(i)
	i = i - 1
until i == 0
local i = 10
while i > 0 do
	print(i)
	i = i - 1
end
while running == true do
	my_function()
end
local i = 10
while not (i == 0) do
	print(i)
	i = i - 1
end
while not (running == false) do
	my_function()
end
local i = 0
while i < 10 do
	i = i + 1
	if i % 2 == 0 then
		goto _continue_0
	end
	print(i)
	::_continue_0::
end
local my_numbers = {
	1,
	2,
	3,
	4,
	5,
	6
}
local odds
local _accum_0 = { }
local _len_0 = 1
for _index_0 = 1, #my_numbers do
	local x = my_numbers[_index_0]
	if x % 2 == 1 then
		goto _continue_0
	end
	_accum_0[_len_0] = x
	_len_0 = _len_0 + 1
	::_continue_0::
end
odds = _accum_0
local have_coins = false
if have_coins then
	print("Got coins")
else
	print("No coins")
end
local have_coins = false
if have_coins then
	print("Got coins")
else
	print("No coins")
end
local have_coins = false
print((function()
	if have_coins then
		return "Got coins"
	else
		return "No coins"
	end
end)())
local is_tall
is_tall = function(name)
	if name == "Rob" then
		return true
	else
		return false
	end
end
local message
if is_tall("Rob") then
	message = "I am very tall"
else
	message = "I am not so tall"
end
print(message)
if not (os.date("%A") == "Monday") then
	print("it is not Monday!")
end
if not (math.random() > 0.1) then
	print("You're lucky!")
end
local a = 5
if (1 == a or 3 == a or 5 == a or 7 == a) then
	print("checking equality with discrete values")
end
if (function()
	local _check_0 = list
	for _index_0 = 1, #_check_0 do
		if _check_0[_index_0] == a then
			return true
		end
	end
	return false
end)() then
	print("checking if `a` is in a list")
end
if not (math.random() > 0.1) then
	print("You're lucky!")
end
if name == "Rob" then
	print("hello world")
end
local _list_0 = items
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print("item: ", item)
end
while game:isRunning() do
	game:update()
end
while not reader:eof() do
	reader:parse_line()
end
local name = "Dan"
if "Robert" == name then
	print("You are Robert")
elseif "Dan" == name or "Daniel" == name then
	print("Your name, it's Dan")
else
	print("I don't know about your name")
end
local b = 1
local next_number
if 1 == b then
	next_number = 2
elseif 2 == b then
	next_number = 3
else
	next_number = error("can't count that high!")
end
local msg
local _exp_0 = math.random(1, 5)
if 1 == _exp_0 then
	msg = "you are lucky"
elseif 2 == _exp_0 then
	msg = "you are almost lucky"
else
	msg = "not so lucky"
end
do
	local _exp_0 = math.random(1, 5)
	if 1 == _exp_0 then
		print("you are lucky")
	else
		print("not so lucky")
	end
end
local _exp_0 = math.random(1, 5)
if 1 == _exp_0 then
	print("you are lucky")
else
	print("not so lucky")
end
local items = {
	{
		x = 100,
		y = 200
	},
	{
		width = 300,
		height = 400
	}
}
for _index_0 = 1, #items do
	local item = items[_index_0]
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
		if _tab_0 then
			local width = item.width
			local height = item.height
			if width ~= nil and height ~= nil then
				print("size " .. tostring(width) .. ", " .. tostring(height))
			end
		end
	end
end
local item = { }
local x, y = item.pos.x, item.pos.y
if x == nil then
	x = 50
end
if y == nil then
	y = 200
end
local _type_0 = type(item)
local _tab_0 = "table" == _type_0 or "userdata" == _type_0
if _tab_0 then
	do
		local _obj_0 = item.pos
		local _type_1 = type(_obj_0)
		if "table" == _type_1 or "userdata" == _type_1 then
			x = _obj_0.x
		end
	end
	do
		local _obj_0 = item.pos
		local _type_1 = type(_obj_0)
		if "table" == _type_1 or "userdata" == _type_1 then
			y = _obj_0.y
		end
	end
	if x == nil then
		x = 50
	end
	if y == nil then
		y = 200
	end
	print("Vec2 " .. tostring(x) .. ", " .. tostring(y))
end
local Inventory
local _class_0
local _base_0 = {
	add_item = function(self, name)
		if self.items[name] then
			local _obj_0 = self.items
			_obj_0[name] = _obj_0[name] + 1
		else
			self.items[name] = 1
		end
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self)
		self.items = { }
	end,
	__base = _base_0,
	__name = "Inventory"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Inventory = _class_0
local inv = Inventory()
inv:add_item("t-shirt")
inv:add_item("pants")
local Person
do
	local _class_0
	local _base_0 = {
		clothes = { },
		give_item = function(self, name)
			return table.insert(self.clothes, name)
		end
	}
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Person"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Person = _class_0
end
local a = Person()
local b = Person()
a:give_item("pants")
b:give_item("shirt")
local _list_0 = a.clothes
for _index_0 = 1, #_list_0 do
	local item = _list_0[_index_0]
	print(item)
end
local Person
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self)
		self.clothes = { }
	end,
	__base = _base_0,
	__name = "Person"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Person = _class_0
local BackPack
local _class_0
local _parent_0 = Inventory
local _base_0 = {
	size = 10,
	add_item = function(self, name)
		if #self.items > size then
			error("backpack is full")
		end
		return _class_0.__parent.__base.add_item(self, name)
	end
}
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "BackPack",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
BackPack = _class_0
local Shelf
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Shelf"
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
	self.__inherited = function(self, child)
		return print(self.__name, "was inherited by", child.__name)
	end
	Shelf = _class_0
end
local Cupboard
local _class_0
local _parent_0 = Shelf
local _base_0 = { }
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "Cupboard",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
Cupboard = _class_0
local MyClass
local _class_0
local _parent_0 = ParentClass
local _base_0 = {
	a_method = function(self)
		_class_0.__parent.__base.a_method(self, "hello", "world")
		_class_0.__parent.a_method(self, "hello", "world")
		_class_0.__parent.a_method(self, "hello", "world")
		return assert(_class_0.__parent == ParentClass)
	end
}
for _key_0, _val_0 in pairs(_parent_0.__base) do
	if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
		_base_0[_key_0] = _val_0
	end
end
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
setmetatable(_base_0, _parent_0.__base)
_class_0 = setmetatable({
	__init = function(self, ...)
		return _class_0.__parent.__init(self, ...)
	end,
	__base = _base_0,
	__name = "MyClass",
	__parent = _parent_0
}, {
	__index = function(cls, name)
		local val = rawget(_base_0, name)
		if val == nil then
			local parent = rawget(cls, "__parent")
			if parent then
				return parent[name]
			end
		else
			return val
		end
	end,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
if _parent_0.__inherited then
	_parent_0.__inherited(_parent_0, _class_0)
end
MyClass = _class_0
local b = BackPack()
assert(b.__class == BackPack)
print(BackPack.size)
print(BackPack.__name)
local Things
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Things"
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
	self.some_func = function(self)
		return print("Hello from", self.__name)
	end
	Things = _class_0
end
Things:some_func()
assert(Things().some_func == nil)
local Counter
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self)
			self.__class.count = self.__class.count + 1
		end,
		__base = _base_0,
		__name = "Counter"
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
	self.count = 0
	Counter = _class_0
end
Counter()
Counter()
print(Counter.count)
self.__class:hello(1, 2, 3, 4)
local Things
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Things"
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
self.class_var = "hello world"
Things = _class_0
local MoreThings
local _class_0
local secret, log
local _base_0 = {
	some_method = function(self)
		return log("hello world: " .. secret)
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "MoreThings"
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
secret = 123
log = function(msg)
	return print("LOG:", msg)
end
MoreThings = _class_0
assert(self == self)
assert(self.__class == self.__class)
local some_instance_method
some_instance_method = function(self, ...)
	return self.__class(...)
end
local Something
do
	local _class_0
	local _base_0 = { }
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function(self, foo, bar, biz, baz)
			self.foo = foo
			self.bar = bar
			self.__class.biz = biz
			self.__class.baz = baz
		end,
		__base = _base_0,
		__name = "Something"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Something = _class_0
end
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function(self, foo, bar, biz, baz)
		self.foo = foo
		self.bar = bar
		self.__class.biz = biz
		self.__class.baz = baz
	end,
	__base = _base_0,
	__name = "Something"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Something = _class_0
local new
new = function(self, fieldA, fieldB)
	self.fieldA = fieldA
	self.fieldB = fieldB
	return self
end
local obj = new({ }, 123, "abc")
print(obj)
local x
local Bucket
local _class_0
local _base_0 = {
	drops = 0,
	add_drop = function(self)
		self.drops = self.drops + 1
	end
}
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "Bucket"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
Bucket = _class_0
x = _class_0
local BigBucket
do
	local _class_0
	local _parent_0 = Bucket
	local _base_0 = {
		add_drop = function(self)
			self.drops = self.drops + 10
		end
	}
	for _key_0, _val_0 in pairs(_parent_0.__base) do
		if _base_0[_key_0] == nil and _key_0:match("^__") and not (_key_0 == "__index" and _val_0 == _parent_0.__base) then
			_base_0[_key_0] = _val_0
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	setmetatable(_base_0, _parent_0.__base)
	_class_0 = setmetatable({
		__init = function(self, ...)
			return _class_0.__parent.__init(self, ...)
		end,
		__base = _base_0,
		__name = "BigBucket",
		__parent = _parent_0
	}, {
		__index = function(cls, name)
			local val = rawget(_base_0, name)
			if val == nil then
				local parent = rawget(cls, "__parent")
				if parent then
					return parent[name]
				end
			else
				return val
			end
		end,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	if _parent_0.__inherited then
		_parent_0.__inherited(_parent_0, _class_0)
	end
	BigBucket = _class_0
end
assert(Bucket.__name == "BigBucket")
local x
local _class_0
local _base_0 = { }
if _base_0.__index == nil then
	_base_0.__index = _base_0
end
_class_0 = setmetatable({
	__init = function() end,
	__base = _base_0,
	__name = "x"
}, {
	__index = _base_0,
	__call = function(cls, ...)
		local _self_0 = setmetatable({ }, _base_0)
		cls.__init(_self_0, ...)
		return _self_0
	end
})
_base_0.__class = _class_0
x = _class_0
local MyIndex = {
	__index = {
		var = 1
	}
}
local X
do
	local _class_0
	local _base_0 = {
		func = function(self)
			return print(123)
		end
	}
	local _list_0 = {
		MyIndex
	}
	for _index_0 = 1, #_list_0 do
		local _item_0 = _list_0[_index_0]
		local _cls_0, _mixin_0 = (_item_0.__base ~= nil), _item_0.__base or _item_0
		for _key_0, _val_0 in pairs(_mixin_0) do
			if _base_0[_key_0] == nil and (not _cls_0 or not _key_0:match("^__")) then
				_base_0[_key_0] = _val_0
			end
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "X"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	X = _class_0
end
local x = X()
print(x.var)
local Y
do
	local _class_0
	local _base_0 = { }
	local _list_0 = {
		X
	}
	for _index_0 = 1, #_list_0 do
		local _item_0 = _list_0[_index_0]
		local _cls_0, _mixin_0 = (_item_0.__base ~= nil), _item_0.__base or _item_0
		for _key_0, _val_0 in pairs(_mixin_0) do
			if _base_0[_key_0] == nil and (not _cls_0 or not _key_0:match("^__")) then
				_base_0[_key_0] = _val_0
			end
		end
	end
	if _base_0.__index == nil then
		_base_0.__index = _base_0
	end
	_class_0 = setmetatable({
		__init = function() end,
		__base = _base_0,
		__name = "Y"
	}, {
		__index = _base_0,
		__call = function(cls, ...)
			local _self_0 = setmetatable({ }, _base_0)
			cls.__init(_self_0, ...)
			return _self_0
		end
	})
	_base_0.__class = _class_0
	Y = _class_0
end
local y = Y()
y:func()
assert(y.__class.__parent ~= X)
local _with_0 = Person()
_with_0.name = "Oswald"
_with_0:add_relative(my_dad)
_with_0:save()
print(_with_0.name)
local file
local _with_0 = File("favorite_foods.txt")
_with_0:set_encoding("utf8")
file = _with_0
local create_person
create_person = function(name, relatives)
	local _with_0 = Person()
	_with_0.name = name
	for _index_0 = 1, #relatives do
		local relative = relatives[_index_0]
		_with_0:add_relative(relative)
	end
	return _with_0
end
local me = create_person("Leaf", {
	dad,
	mother,
	sister
})
local str = "Hello"
print("original:", str)
print("upper:", str:upper())
local _with_0 = tb
_with_0[1] = 1
print(_with_0[2])
do
	local _with_1 = _with_0[abc]
	_with_1[3] = _with_1[2]:func()
	_with_1["key-name"] = value
end
_with_0[#_with_0 + 1] = "abc"
do
	local var = "hello"
	print(var)
end
print(var)
local counter
do
	local i = 0
	counter = function()
		i = i + 1
		return i
	end
end
print(counter())
print(counter())
local tbl = {
	key = (function()
		print("assigning key!")
		return 1234
	end)()
}
local my_object = {
	value = 1000,
	write = function(self)
		return print("the value:", self.value)
	end
}
local run_callback
run_callback = function(func)
	print("running callback...")
	return func()
end
run_callback(my_object.write)
run_callback((function()
	local _base_0 = my_object
	local _fn_0 = _base_0.write
	return _fn_0 and function(...)
		return _fn_0(_base_0, ...)
	end
end)())
local i = 100
local my_func
my_func = function()
	i = 10
	while i > 0 do
		print(i)
		i = i - 1
	end
end
my_func()
print(i)
local i = 100
local my_func
my_func = function()
	local i = "hello"
end
my_func()
print(i)
local tmp = 1213
local i, k = 100, 50
local my_func
my_func = function(add)
	local tmp = tmp + add
	i = i + tmp
	k = k + tmp
end
my_func(22)
print(i, k)
