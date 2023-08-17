
class 你好
	new: (@测试, @世界) =>
		打印 "创建对象.."
	你好: =>
		打印 @测试, @世界
	__tostring: => "你好 世界"

对象x = 你好 1,2
对象x\你好()

打印 对象x

class 简单
	酷: => 打印 "酷"

class 伊克斯 extends 简单
	new: => 打印 "你好已创建"

x对象 = 伊克斯()
x对象\酷()


class 嗨
	new: (参数) =>
		打印 "初始化参数", 参数

	酷: (数值) =>
		打印 "数值", 数值


class 简单 extends 嗨
	new: => super "伙计"
	酷: => super 120302

x对象 = 简单()
x对象\酷()

打印 x对象.__class == 简单


class 好吧
	-- what is going on
	一些东西: 20323
	-- yeaha


class 好哒 extends 好吧
	一些东西: =>
		super 1,2,3,4
		super.一些东西 另一个自己, 1,2,3,4
		断言 super == 好吧


class 好
	不错: =>
		super\一些东西 1,2,3,4


class 什么
	一些东西: => 打印 "值:", @值

class 你好 extends 什么
	值: 2323
	一些东西: => super\一些东西

with 你好!
	对象x = \一些对象!
	打印 对象x
	对象x!

class 超级酷
	👋: =>
		super(1,2,3,4) 1,2,3,4
		super.一些东西 1,2,3,4
		_ = super.一些东西(1,2,3,4).世界
		super\好吧"世界".不错 哈, 哈, 哈
		_ = 一些东西.上级
		_ = super.上级.上级.上级
		_ = super\你好
		nil


-- selfing
变量x = @你好
变量x = @@你好

@你好 "世界"
@@你好 "世界"

@@一 @@二(4,5) @三, @四

变量xx = (@你好, @@世界, 酷) ->


-- class properties
class 一个类
	@好: 343
	蓝色: =>
	@你好: 3434, @世界: 23423
	绿色: =>
	@红色: =>


变量x = @
变量y = @@

@ 一些东西

@@ 一些东西

@ = @ + @ / @

@ = 343
@.你好 2,3,4

_ = 你好[@].世界


class 怪怪的
	_ = @你好
	if 一些东西
		打印 "你好世界"

	你好 = "世界"
	@另一 = "天"

	打印 "好" if 一些东西 -- this is briken


打印 "你好"

变量yyy = ->
	class 酷
		_ = nil


--

class 对象a.字段b.字段c.子类D
	_ = nil


class 对象a.字段b["你好"]
	_ = nil

class (-> require "moon")!.某个成员 extends 你好.世界
	_ = nil

--

类a = class
类b = class 一个类
类c = class 一个类 extends 你好
类d = class extends 世界

打印 (class 啥事).__name

--

global ^
class 一个类
	_ = nil


--

-- hoisting
class 一个类
	值 = 23
	{:插入} = 表
	new: => 打印 插入, 值 -- prints nil 23

--

class X类
	new: 嗨


--

class 酷 extends 东西
	当: =>
		{
			你好: -> super!
			世界: -> super.one
		}

-- 

class 奇怪 extends 东西
	当: 做点事 =>
		super!

---

class 喔哈 extends 东西
	@底部: ->
		super!
		_ = super.你好
		super\你好!
		super\你好


	@空间: 酷 {
		->
			super!
			_ = super.你好
			super\你好!
			super\你好
	}

do
	class 测试
		new: => @@如果 = true
		@做: => 1
		测试: => @@如果 and @@做!
	测试实例 = 测试!
	测试实例\测试!

do
	class 测试
		new: => @如果 = true
		做: => 1
		测试: => @如果 and @做!
	测试实例 = 测试!
	测试实例\测试!

class extends 青.应用
	"/": => json: { 状态: true }

class 类A using 类B, 类C, 类D, {值: 123}

class 示例
	@字段1 = 1
	@字段2 = @字段1 + 1

class 随便
	new: (参数) => @x字段 = 参数
	<mul>: (参数y) => @x字段 * 参数y
	<"任意名称">: 123
	:相加
	:<add>

nil
