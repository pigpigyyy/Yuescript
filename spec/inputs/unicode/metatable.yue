变量a = 关闭: true, <close>: => 打印 "离开作用域"
变量b = <add>: (左, 右)-> 右 - 左
变量c = 键1: true, :<add>, 键2: true
变量w = <[名称]>:123, <"新建">:(值)=> {值}
变量w.<>["新建"] 变量w.<>[名称]

do close _ = <close>: -> 打印 "离开作用域"

变量d, 变量e = 变量a.关闭, 变量a.<close>

变量f = 变量a\<close> 1
变量a.<add> = (x, y)-> x + y

do
	{:新, :<close>, <close>: 关闭A} = 变量a
	打印 新, close, 关闭A

do
	local *
	变量x, \
	{:新, :变量, :<close>, <close>: 关闭A}, \
	:num, :<add>, :<sub> \
	= 123, 变量a.变量b.变量c, 函数!

变量x.abc, 变量a.变量b.<> = 123, {}
函数!.<> = mt --, 额外
变量a, 变量b.变量c.<>, 变量d, 变量e = 1, mt, "abc", nil

相同 = 变量a.<>.__index == 变量a.<index>

--
变量a.<> = __index: 表
变量a.<>.__index = 表
变量a.<index> = 表
--

mt = 变量a.<>

表\函数 #列表
表\<"函数">列表
表\<"函数"> 列表

import "模块" as :<index>, <newindex>:设置函数

with 表
	打印 .<add>, .x\<index> "key"
	变量a = .<index>.<add>\<"新建"> 123
	变量b = t#.<close>.测试
	变量c = t #.<close> .测试

<>:mt = 变量a
变量a = <>:mt
变量a = <>:__index:mt

local index
<>:__index:索引 = 变量a
:<index> = 变量a

do <>:{新:构造, :更新} = 变量a
do {新:构造, :更新} = 变量a.<>

表 = {}
do
	变量f = 表\<"值#{x < y}">(123, ...)
	变量f 表\<'值'> 123, ...
	表\<[[
		值
		1
	]]>(123, ...)
	return 表\<["值" .. tostring x > y]>(123, ...)

do
	变量f = 表\<'值'>(123, ...)
	变量f 表\<'值'>(123, ...)
	表\<'值'>(123, ...)
	return 表\<'值'> 123, ...

do
	变量f = 表.<["值"]> 123, ...
	变量f = 表.<"值#{x < y}">(123, ...)
	变量f 表.<'值'> 123, ...
	表.<[[ 值
1]]>(123, ...)
	return 表.<["值" .. tostring x > y]>(123, ...)

nil

