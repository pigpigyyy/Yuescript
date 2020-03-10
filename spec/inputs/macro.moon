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
	switch select "#",...
		when 0 then return ""
		when 1 then return ...
	ops = {...}
	last = ops[1]
	stmts = for i = 2,#ops
		stmt = "\tlocal _#{i} = #{last} |> #{ops[i]}"
		last = "_#{i}"
		stmt
	res = "do
#{table.concat stmts,"\n"}
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

