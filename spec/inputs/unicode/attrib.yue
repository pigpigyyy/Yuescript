const 常量a, 常量b, 常量c, 常量d = 1, 2, 3, 4

do
	close 关闭变量a, 关闭变量b = setmetatable {},__close:=> 打印 "已关闭"
	const 变量c, 常量d = 123, 'abc'

	close 常量a, 常量b
	const 常量c, 常量d

do
	const 常量a, {元素b, 元素c}, {元素d} = 函数!, 函数1!

do
	const 常量a, {元素b, 元素c}, {元素d} = 函数!

do
	close 关闭变量v = if 标记为真
		函数调用!
	else
		<close>: =>

	close 关闭变量f = with io.open "文件.txt"
		\write "你好"

do
	const 常量a = 1 if true
	close 关闭变量b = (if 条件x then 1) unless false
	const 常量c = (switch 条件x
		when "abc" then 998) if true
	close 关闭变量d =
		:数值 if 条件a ?? 条件b

do
	close _无效变量 = with io.open "文件.txt"
		\write "你好"
	
	close _无效变量 = <close>: -> 打印 "第二"

	close _无效变量 = <close>: -> 打印 "第一"

_延迟对象数组 = <close>: =>
	@[#@]!
	@[#@] = nil

延迟 = (项目)->
	_延迟对象数组[] = 项目
	_延迟对象数组

do
	close _无效变量 = 延迟 -> 打印 3
	close _无效变量 = 延迟 -> 打印 2
	close _无效变量 = 延迟 -> 打印 1

