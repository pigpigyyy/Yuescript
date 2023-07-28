local a:{string:number} = {
	value = 123
}
local b:number = a.value
local function add(a:number, b:number):number

	return a + b

end
local s = add(a.value, b)
print(s)
local record Point
	x: number
	y: number
end
function Point.new(x:number, y:number):Point
local point:Point = setmetatable({ }, {
	__index = Point
})
	point.x = x or 0
	point.y = y or 0
	return point

end
function Point:move(dx:number, dy:number)
	self.x = self.x + dx
	self.y = self.y + dy

end
local p:Point = Point.new(100, 100)
p:move(50, 50)
local function filter(tab:{string}, handler:function(item:string):boolean):{string}

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
local function cond(item:string):boolean

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
