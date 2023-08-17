switch 值
	when "酷"
		打印 "你好，世界"

switch 值
	when "酷"
		打印 "你好，世界"
	else
		打印 "好的，很棒"

switch 值
	when "酷"
		打印 "你好，世界"
	when "是的"
		变量_ = [[FFFF]] + [[MMMM]]
	when 2323 + 32434
		打印 "好的"
	else
		打印 "好的，很棒"

输出 = switch 值
	when "酷" then 打印 "你好，世界"
	else 打印 "好的，很棒"

输出 = switch 值
	when "酷" then 变量x
	when "哦" then 34340
	else error "这大大地失败了"

with 东西
	switch \值!
		when .确定
			变量_ = "世界"
		else
			变量_ = "是的"

修复 这个
call_func switch 某物
	when 1 then "是"
	else "否"

--

switch 嗨
	when 你好 or 世界
		变量_ = 绿色

--

switch 嗨
	when "一个", "两个"
		打印 "酷"
	when "爸爸"
		变量_ = 否

switch 嗨
	when 3+1, 你好!, (-> 4)!
		变量_ = 黄色
	else
		打印 "酷"

do
	字典 = {
		{}
		{1, 2, 3}
		变量a: 变量b: 变量c: 1
		变量x: 变量y: 变量z: 1
	}

	switch 字典
		when {
				第一
				{一个, 两个, 三个}
				变量a: 变量b: :变量c
				变量x: 变量y: :变量z
			}
			打印 第一, 一个, 两个, 三个, 变量c, 变量z

do
	物品 =
		* 变量x: 100
			变量y: 200
		* 宽度: 300
			高度: 400
		* false

	for 物 in *物品
		switch 物
			when :变量x, :变量y
				打印 "Vec2 #{变量x}, #{变量y}"
			when :宽度, :高度
				打印 "Size #{宽度}, #{高度}"
			when false
				打印 "没有"
			when __类: 类
				switch 类
					when 类别A
						打印 "对象 A"
					when 类别B
						打印 "对象 B"
			when <>: 表
				打印 "带有元表的表"
			else
				打印 "物品不被接受！"

do
	表格 = {}

	switch 表格
		when {:变量a = 1, :变量b = 2}
			打印 变量a, 变量b

	switch 表格
		when {:变量a, :变量b = 2}
			打印 "部分匹配", 变量a, 变量b

	switch 表格
		when {:变量a, :变量b}
			打印 变量a, 变量b
		else
			打印 "没有匹配"

do
	表格 = 变量x: "abc"
	switch 表格
		when :变量x, :变量y
			打印 "变量x: #{变量x} 和 变量y: #{变量y}"
		when :变量x
			打印 "只有 变量x: #{变量x}"

do
	匹配 = switch 表格
		when 1
			"1"
		when :变量x
			变量x
		when false
			"false"
		else
			nil

do
	return switch 表格
		when nil
			"无效"
		when :变量a, :变量b
			"#{变量a + 变量b}"
		when 1, 2, 3, 4, 5
			"数字 1 - 5"
		when {:匹配任何表格 = "后备"}
			匹配任何表格
		else
			"除非它不是一个表格，否则不应到达这里"

do
	switch 变量y
		when {变量x: <>: 表}
			打印 表

nil

