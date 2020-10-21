$ ->
	package.moonpath = "?.mp;./spec/inputs/?.mp"

import "macro-teal" as {$}

$var a, "{string:number}", {value:123}
$var b, "number", a.value

$def add, "(a:number,b:number):number", (a, b)-> a + b

s = add(a.value, b)
print(s)

$record Point, [[
	x: number
	y: number
]]

$field Point, '.', new, "(x: number, y: number):Point", (x, y)->
	$var point, "Point", setmetatable {}, __index: Point
	point.x = x or 0
	point.y = y or 0
	point

$field Point, ":", move, "(dx: number, dy: number)", (dx, dy)=>
	@x += dx
	@y += dy

$var p, "Point", Point.new 100, 100

p\move 50, 50

