local a = {
	value = 123
};
local b = a.value;
local add = function(a, b)
	return a + b
end
local s = add(a.value, b)
print(s);
local Point = {};
Point.new = function(x, y)
local point = setmetatable({ }, {
	__index = Point
})
	point.x = x or 0
	point.y = y or 0
	return point
end
Point.move = function(self, dx, dy)
	self.x = self.x + dx
	self.y = self.y + dy
end
local p = Point.new(100, 100)
p:move(50, 50);
local filter = function(tab, handler)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #tab do
		local item = tab[_index_0]
		if handler(item) then
			_accum_0[_len_0] = item
			_len_0 = _len_0 + 1
		end
	end
	return _accum_0
end
local cond = function(item)
	return item ~= "a"
end
local res = filter({
	"a",
	"b",
	"c",
	"a"
}, cond)
for _index_0 = 1, #res do
	local s = res[_index_0]
	print(s)
end
