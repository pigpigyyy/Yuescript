macro block init = ->
	with require "moonp"
		package.moonpath = "?.moon;./spec/inputs/?.moon"
	""

$init!

import "macro_export" as {$myconfig:$config, :$showMacro, :$asserts, :$assert}

$asserts item == nil

$myconfig false

v = $assert item == nil

macro expr and = (...)->
	values = [value for value in *{...}]
	$showMacro "and", "#{ table.concat values, " and " }"

if $and f1
	print "OK"

if $and f1,f2,f3
	print "OK"

macro expr map = (items,action)->
	$showMacro "map", "[#{action} for _ in *#{items}]"

macro expr filter = (items,action)->
	$showMacro "filter", "[_ for _ in *#{items} when #{action}]"

macro expr reduce = (items,def,action)->
	$showMacro "reduce", "if ##{items} == 0
	#{def}
else
	_1 = #{def}
	for _2 in *#{items}
		_1 = #{action}
	_1"

macro block foreach = (items,action)->
	$showMacro "foreach", "for _ in *#{items}
	#{action}"

macro expr pipe = (...)->
	switch select "#", ...
		when 0 then return ""
		when 1 then return ...
	ops = {...}
	last = ops[1]
	stmts = for i = 2,#ops
		stmt = "\tlocal _#{i} = #{last} |> #{ops[i]}"
		last = "_#{i}"
		stmt
	res = "do
#{table.concat stmts, "\n"}
	#{last}"
	$showMacro "pipe", res

{1,2,3} |> $map(_ * 2) |> $filter(_ > 4) |> $foreach print _

$foreach $filter($map({1,2,3}, _ * 2), _ > 4), print _

val = $pipe(
	{1, 2, 3}
	$map(_ * 2)
	$filter(_ > 4)
	$reduce(0, _1 + _2)
)

macro expr plus = (a, b)-> "#{a} + #{b}"

$plus(1,2)\call 123

macro expr curry = (...)->
	args = {...}
	len = #args
	body = args[len]
	def = table.concat ["(#{args[i]})->" for i = 1, len - 1]
	"#{def}\n#{body\gsub "^do%s*\n",""}"

f = $curry x,y,z,do
	print x,y,z

macro expr get_inner = (var)-> "do
	a = 1
	a + 1"

macro expr get_inner_hygienic = (var)-> "(->
	local a = 1
	a + 1)!"

do
	a = 8
	a = $get_inner!
	a += $get_inner!
	print a

do
	a = 8
	a = $get_inner_hygienic!
	a += $get_inner_hygienic!
	print a

macro lua lua = (codes)-> "#{codes}"

x = 0

$lua [[
local function f(a)
	return a + 1
end
x = x + f(3)
]]

print x

macro lua def = (fname, ...)->
	args = {...}
	last = table.remove args
	$showMacro "def", "local function #{fname}(#{table.concat args, ', '})
	#{last}
end"

sel = (a, b, c)-> if a then b else c

$def sel, a, b, c, [[
	if a then
		return b
	else
		return c
	end
]]

$def dummy,[[
]]

nil

