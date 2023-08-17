
你好 = [变量x*2 for _, 变量x in ipairs{1,2,3,4}]

物品 = {1,2,3,4,5,6}

_ = [变量z for 变量z in ipairs 物品 when 变量z > 4]

半径 = [{变量a} for 变量a in ipairs {
	 1,2,3,4,5,6,
} when 好数字 变量a]

_ = [变量z for 变量z in 物品 for 变量j in 列表 when 变量z > 4]

require "实用"

倾倒 = (变量x) -> 打印 实用.倾倒 变量x

范围 = (数量) ->
	变量i = 0
	return coroutine.wrap ->
		while 变量i < 数量
			coroutine.yield 变量i
			变量i = 变量i + 1

倾倒 [变量x for 变量x in 范围 10]
倾倒 [{变量x, 变量y} for 变量x in 范围 5 when 变量x > 2 for 变量y in 范围 5]

东西 = [变量x + 变量y for 变量x in 范围 10 when 变量x > 5 for 变量y in 范围 10 when 变量y > 7]

打印 变量x, 变量y for 变量x in ipairs{1,2,4} for 变量y in ipairs{1,2,3} when 变量x != 2

打印 "你好", 变量x for 变量x in 物品

_ = [变量x for 变量x in 变量x]
变量x = [变量x for 变量x in 变量x]

打印 变量x, 变量y for 变量x in ipairs{1,2,4} for 变量y in ipairs{1,2,3} when 变量x != 2

双倍 = [变量x*2 for 变量x in *物品]

打印 变量x for 变量x in *双倍

切 = [变量x for 变量x in *物品 when 变量x > 3]

你好 = [变量x + 变量y for 变量x in *物品 for 变量y in *物品]

打印 变量z for 变量z in *你好

-- 切片
变量x = {1, 2, 3, 4, 5, 6, 7}
打印 变量y for 变量y in *变量x[2,-5,2]
打印 变量y for 变量y in *变量x[,3]
打印 变量y for 变量y in *变量x[2,]
打印 变量y for 变量y in *变量x[,,2]
打印 变量y for 变量y in *变量x[2,,2]

变量a, 变量b, 变量c = 1, 5, 2
打印 变量y for 变量y in *变量x[变量a,变量b,变量c]

正常 = (你好) ->
	[变量x for 变量x in 嗯哼]

测试 = 变量x 1,2,3,4,5
打印 事情 for 事情 in *测试

-> 变量a = 变量b for 行 in *行们

