---
sidebar: auto
title: 参考手册
---

# 月之脚本

<img src="/image/yuescript.svg" width="300px" height="300px" alt="logo"/>

## 介绍

月之脚本（Yuescript）是一种动态语言，可以编译为Lua。它是[Moonscript](https://github.com/leafo/moonscript)的方言。用月之脚本编写的代码既有表现力又非常简洁。它适合编写一些更易于维护的代码，并在嵌入 Lua 的环境中运行，如游戏或网站服务器。

Yue（月）是中文中“月亮”的名称。

### 月之脚本概览
```moonscript
-- 导入语法
import "yue" as :p, :to_lua

-- 隐式对象
inventory =
  equipment:
    * "sword"
    * "shield"
  items:
    * name: "potion"
      count: 10
    * name: "bread"
      count: 3

-- 管道操作符
[1, 2, 3]
  |> map (x)-> x * 2
  |> filter (x)-> x > 4
  |> reduce 0, (a, b)-> a + b
  |> print

-- 元表操作
apple =
  size: 15
  <index>: {color: 0x00ffff}
p apple.color, apple.<index> if apple.<>?

-- 类似js的导出语法
export 🌛 = "月之脚本"
```
<YueDisplay>
<pre>
-- 导入语法
import "yue" as :p, :to_lua

-- 隐式对象
inventory =
  equipment:
    * "sword"
    * "shield"
  items:
    * name: "potion"
      count: 10
    * name: "bread"
      count: 3

-- 管道操作符
[1, 2, 3]
  |> map (x)-> x * 2
  |> filter (x)-> x > 4
  |> reduce 0, (a, b)-> a + b
  |> print

-- 元表操作
apple =
  size: 15
  &lt;index&gt;: {color: 0x00ffff}
p apple.color, apple.&lt;index&gt; if apple.&lt;&gt;?

-- 类似js的导出语法
export 🌛 = "月之脚本"
</pre>
</YueDisplay>

## 安装

* **Lua 模块**

&emsp;安装 [luarocks](https://luarocks.org)，一个Lua模块的包管理器。然后作为Lua模块和可执行文件安装它：

```
> luarocks install yuescript
```

&emsp;或者你可以自己构建 `yue.so` 文件：

```
> make shared LUAI=/usr/local/include/lua LUAL=/usr/local/lib/lua
```

&emsp;然后从路径 **bin/shared/yue.so** 获取二进制文件。

* **二进制工具**

&emsp;克隆项目仓库，然后构建并安装可执行文件：
```
> make install
```

&emsp;构建不带宏功能的月之脚本编译工具：
```
> make install NO_MACRO=true
```

&emsp;构建不带内置Lua二进制文件的月之脚本编译工具：
```
> make install NO_LUA=true
```

## 使用方法

### Lua 模块

在Lua中使用月之脚本模块：

* **用法 1**  
在Lua中引入 "你的脚本入口文件.yue"。
```Lua
require("yue")("你的脚本入口文件")
```
当你在同一路径下把 "你的脚本入口文件.yue" 编译成了 "你的脚本入口文件.lua" 时，仍然可以使用这个代码加载 .lua 代码文件。在其余的月之脚本文件中，只需正常使用 **require** 或 **import**进行脚本引用即可。错误消息中的代码行号也会被正确处理。

* **用法 2**  
手动引入月之脚本模块并重写错误消息来帮助调试。
```lua
local yue = require("yue")
local success, result = xpcall(function()
  yue.require("yuescript_module_name")
end, function(err)
  return yue.traceback(err)
end)
```

* **用法 3**  
在Lua中使用月之脚本编译器功能。
```lua
local yue = require("yue")
local codes, err, globals = yue.to_lua([[
f = ->
  print "hello world"
f!
]],{
  implicit_return_root = true,
  reserve_line_number = true,
  lint_global = true,
  space_over_tab = false,
  options = {
    target = "5.4",
    path = "/script"
  }
})
```

### 月之脚本编译工具

使用月之脚本编译工具：
```
使用命令: yue [选项|文件|目录] ...

   -h       打印此消息
   -e str   执行一个文件或一段原始代码
   -m       生成压缩后的代码
   -r       重写输出的Lua代码以匹配原始代码中的行号
   -t path  指定放置编译结果文件的位置
   -o file  将输出写到指定的文件中
   -s       在生成的代码中使用空格代替制表符
   -p       将输出写入标准输出
   -b       输出编译时间（不写输出）
   -g       以“名称 行号 列号”的形式输出代码中使用的全局变量
   -l       在输出的每一行代码的末尾写上原代码的行号
   -c       在输出的代码中保留语句前的注释
   -w path  监测目录下的文件更改并重新编译生成目录下的文件
   -v       打印版本号
   --       从标准输入读取原始代码，打印到编译结果到标准输出
            (必须是第一个且是唯一的参数)

   --target=version  指定编译器将生成的Lua代码版本号
                     (版本号只能是 5.1, 5.2, 5.3 或 5.4)
   --path=path_str   将额外的Lua搜索路径字符串追加到package.path

   不添加任何选项执行命令可以进入REPL模式，
   在单行输入符号 '$' 并换行后，可以开始或是停止多行输入模式
```
&emsp;&emsp;使用案例：  
&emsp;&emsp;递归编译当前路径下扩展名为 **.yue** 的每个月之脚本文件： **yue .**  
&emsp;&emsp;编译并将结果保存到目标路径： **yue -t /target/path/ .**  
&emsp;&emsp;编译并保留调试信息： **yue -l .**  
&emsp;&emsp;编译并生成压缩代码： **yue -m .**  
&emsp;&emsp;直接执行代码： **yue -e 'print 123'**  
&emsp;&emsp;执行一个月之脚本文件： **yue -e main.yue**

## 宏

### 常见用法

宏函数用于在编译时执行一段代码来生成新的代码，并将生成的代码插入到最终编译结果中。

```moonscript
macro PI2 = -> math.pi * 2
area = $PI2 * 5

macro HELLO = -> "'你好 世界'"
print $HELLO

macro config = (debugging)->
  global debugMode = debugging == "true"
  ""

macro asserts = (cond)->
  debugMode and "assert #{cond}" or ""

macro assert = (cond)->
  debugMode and "assert #{cond}" or "#{cond}"

$config true
$asserts item ~= nil

$config false
value = $assert item

-- 宏函数参数传递的表达式会被转换为字符串
macro and = (...)-> "#{ table.concat {...}, ' and ' }"
if $and f1!, f2!, f3!
  print "OK"
```
<YueDisplay>
<pre>
macro PI2 = -> math.pi * 2
area = $PI2 * 5

macro HELLO = -> "'你好 世界'"
print $HELLO

macro config = (debugging)->
  global debugMode = debugging == "true"
  ""

macro asserts = (cond)->
  debugMode and "assert #{cond}" or ""

macro assert = (cond)->
  debugMode and "assert #{cond}" or "#{cond}"

$config true
$asserts item ~= nil

$config false
value = $assert item

-- 宏函数参数传递的表达式会被转换为字符串
macro and = (...)-> "#{ table.concat {...}, ' and ' }"
if $and f1!, f2!, f3!
  print "OK"
</pre>
</YueDisplay>

### 直接插入代码

宏函数可以返回一个包含月之脚本代码的字符串，或是一个包含Lua代码字符串的配置表。
```moonscript
macro yueFunc = (var)-> "local #{var} = ->"
$yueFunc funcA
funcA = -> "访问月之脚本定义的变量"

-- 让月之脚本知道你在Lua代码中声明的局部变量
macro luaFunc = (var)-> {
  code: "local function #{var}() end"
  type: "lua"
  locals: {var}
}
$luaFunc funcB
funcB = -> "访问Lua代码里定义的变量"

macro lua = (code)-> {
  :code
  type: "lua"
}

-- raw字符串的开始和结束符号会自动被去除了再传入宏函数
$lua[==[
-- 插入原始Lua代码
if cond then
  print("输出")
end
]==]
```
<YueDisplay>
<pre>
macro yueFunc = (var)-> "local #{var} = ->"
$yueFunc funcA
funcA = -> "访问月之脚本定义的变量"

-- 让月之脚本知道你在Lua代码中声明的局部变量
macro luaFunc = (var)-> {
  code: "local function #{var}() end"
  type: "lua"
  locals: {var}
}
$luaFunc funcB
funcB = -> "访问Lua代码里定义的变量"

macro lua = (code)-> {
  :code
  type: "lua"
}

-- raw字符串的开始和结束符号会自动被去除了再传入宏函数
$lua[==[
-- 插入原始Lua代码
if cond then
  print("输出")
end
]==]
</pre>
</YueDisplay>

### 导出宏

宏函数可以从一个模块中导出，并在另一个模块中导入。您必须将导出的宏函数放在一个单独的文件中使用，而且只有宏定义、宏导入和宏展开可以放入这个宏导出模块中。
```moonscript
-- 文件: utils.yue
export macro map = (items, action)-> "[#{action} for _ in *#{items}]"
export macro filter = (items, action)-> "[_ for _ in *#{items} when #{action}]"
export macro foreach = (items, action)-> "for _ in *#{items}
  #{action}"

-- 文件 main.yue
import "utils" as {
  $, -- 表示导入所有宏的符号
  $foreach: $each -- 重命名宏 $foreach 为 $each
}
[1, 2, 3] |> $map(_ * 2) |> $filter(_ > 4) |> $each print _
```
<YueDisplay>
<pre>
-- 文件: utils.yue
export macro map = (items, action)-> "[#{action} for _ in *#{items}]"
export macro filter = (items, action)-> "[_ for _ in *#{items} when #{action}]"
export macro foreach = (items, action)-> "for _ in *#{items}
  #{action}"
-- 文件 main.yue
-- 在浏览器中不支持import函数，请在真实环境中尝试
--[[
import "utils" as {
  $, -- 表示导入所有宏的符号
  $foreach: $each -- 重命名宏 $foreach 为 $each
}
[1, 2, 3] |> $map(_ * 2) |> $filter(_ > 4) |> $each print _
]]
</pre>
</YueDisplay>

### 内置宏

月之脚本中有一些内置可以直接使用的宏，但你可以通过声明相同名称的宏来覆盖它们。
```moonscript
print $FILE -- 获取当前模块名称的字符串
print $LINE -- 获取当前代码行数：2
```
<YueDisplay>
<pre>
print $FILE -- 获取当前模块名称的字符串
print $LINE -- 获取当前代码行数：2
</pre>
</YueDisplay>

## 操作符

Lua的所有二元和一元操作符在月之脚本中都是可用的。此外，**!=** 符号是 **~=** 的别名，而 **\\** 或 **::** 均可用于编写链式函数调用，如写作 `tb\func!` 或 `tb::func!`。此外月之脚本还提供了一些其他特殊的操作符，以编写更具表达力的代码。

```moonscript
tb\func! if tb ~= nil
tb::func! if tb != nil
```
<YueDisplay>
<pre>
tb\func! if tb ~= nil
tb::func! if tb != nil
</pre>
</YueDisplay>

### 链式比较

您可以在月之脚本中进行比较表达式的链式书写：

```moonscript
print 1 < 2 <= 2 < 3 == 3 > 2 >= 1 == 1 < 3 != 5
-- 输出：true

a = 5
print 1 <= a <= 10
-- 输出：true
```
<YueDisplay>
<pre>
print 1 < 2 <= 2 < 3 == 3 > 2 >= 1 == 1 < 3 != 5
-- 输出：true

a = 5
print 1 <= a <= 10
-- 输出：true
</pre>
</YueDisplay>

可以注意一下链式比较表达式的求值行为：

```moonscript
v = (x)->
	print x
	x

print v(1) < v(2) <= v(3)
--[[
	输出：
	2
	1
	3
	true
]]

print v(1) > v(2) <= v(3)
--[[
	输出：
	2
	1
	false
]]
```
<YueDisplay>
<pre>
v = (x)->
	print x
	x

print v(1) < v(2) <= v(3)
--[[
	输出：
	2
	1
	3
	true
]]

print v(1) > v(2) <= v(3)
--[[
	输出：
	2
	1
	false
]]
</pre>
</YueDisplay>

在上面的例子里，中间的表达式`v(2)`仅被计算一次，如果把表达式写成`v(1) < v(2) and v(2) <= v(3)`的方式，中间的`v(2)`才会被计算两次。在链式比较中，求值的顺序往往是未定义的。所以强烈建议不要在链式比较中使用具有副作用（比如做打印操作）的表达式。如果需要使用有副作用的函数，应明确使用短路 `and` 运算符来做连接。

### 表追加

**[] =** 操作符用于向Lua表的最后插入值。

```moonscript
tab = []
tab[] = "Value"
```
<YueDisplay>
<pre>
tab = []
tab[] = "Value"
</pre>
</YueDisplay>

### 表扩展

您可以使用前置 `...` 操作符在Lua表中插入数组表或哈希表。

```moonscript
parts =
	* "shoulders"
	* "knees"
lyrics =
	* "head"
	* ...parts
	* "and"
	* "toes"

copy = {...other}

a = {1, 2, 3, x: 1}
b = {4, 5, y: 1}
merge = {...a, ...b}
```
<YueDisplay>
<pre>
parts =
	* "shoulders"
	* "knees"
lyrics =
	* "head"
	* ...parts
	* "and"
	* "toes"

copy = {...other}

a = {1, 2, 3, x: 1}
b = {4, 5, y: 1}
merge = {...a, ...b}
</pre>
</YueDisplay>

### 元表

**<>** 操作符可提供元表操作的快捷方式。

* **元表创建**  
使用空括号 **<>** 或被 **<>** 包围的元方法键创建普通的Lua表。

```moonscript
mt = {}
add = (right)=> <>: mt, value: @value + right.value
mt.__add = add

a = <>: mt, value: 1
-- 使用与临时变量名相同的字段名，将临时变量赋值给元表
b = :<add>, value: 2
c = <add>: mt.__add, value: 3

d = a + b + c
print d.value

close _ = <close>: -> print "超出范围"
```
<YueDisplay>
<pre>
mt = {}
add = (right)=> &lt;&gt;: mt, value: @value + right.value
mt.__add = add

a = &lt;&gt;: mt, value: 1
-- 使用与临时变量名相同的字段名，将临时变量赋值给元表
b = :&lt;add&gt;, value: 2
c = &lt;add&gt;: mt.__add, value: 3

d = a + b + c
print d.value

close _ = &lt;close&gt;: -> print "超出范围"
</pre>
</YueDisplay>

* **元表访问**  
使用 **<>** 或被 **<>** 包围的元方法名或在 **<>** 中编写某些表达式来访问元表。

```moonscript
-- 使用包含字段 "value" 的元表创建
tb = <"value">: 123
tb.<index> = tb.<>
print tb.value

tb.<> = __index: {item: "hello"}
print tb.item
```
<YueDisplay>

<pre>
-- 使用包含字段 "value" 的元表创建
tb = &lt;"value"&gt;: 123
tb.&lt;index&gt; = tb.&lt;&gt;
print tb.value
tb.&lt;&gt; = __index: {item: "hello"}
print tb.item
</pre>
</YueDisplay>

* **元表解构**  
使用被 **<>** 包围的元方法键解构元表。

```moonscript
{item, :new, :<close>, <index>: getter} = tb
print item, new, close, getter
```
<YueDisplay>
<pre>
{item, :new, :&lt;close&gt;, &lt;index&gt;: getter} = tb
print item, new, close, getter
</pre>
</YueDisplay>

### 存在性

**?** 运算符可以在多种上下文中用来检查存在性。

```moonscript
func?!
print abc?["你好 世界"]?.xyz

x = tab?.value
len = utf8?.len or string?.len or (o)-> #o

if print and x?
  print x

with? io.open "test.txt", "w"
  \write "你好"
  \close!
```
<YueDisplay>
<pre>
func?!
print abc?["你好 世界"]?.xyz

x = tab?.value
len = utf8?.len or string?.len or (o)-> #o

if print and x?
  print x

with? io.open "test.txt", "w"
  \write "你好"
  \close!
</pre>
</YueDisplay>

### 管道

与其使用一系列嵌套的函数调用，您还可以考虑使用运算符 **|>** 来传递值。

```moonscript
"你好" |> print
1 |> print 2 -- 将管道项作为第一个参数插入
2 |> print 1, _, 3 -- 带有占位符的管道

-- 多行的管道表达式
readFile "example.txt"
  |> extract language, {}
  |> parse language
  |> emit
  |> render
  |> print
```
<YueDisplay>
<pre>
"你好" |> print
1 |> print 2 -- 将管道项作为第一个参数插入
2 |> print 1, _, 3 -- 带有占位符的管道
-- 多行的管道表达式
readFile "example.txt"
  |> extract language, {}
  |> parse language
  |> emit
  |> render
  |> print
</pre>
</YueDisplay>

### 空值合并

如果其左操作数不是**nil**，则nil合并运算符 **??** 返回其左操作数的值；否则，它将计算右操作数并返回其结果。如果左操作数计算结果为非nil的值，**??** 运算符将不再计算其右操作数。
```moonscript
local a, b, c, d
a = b ?? c ?? d
func a ?? {}

a ??= false
```
<YueDisplay>
<pre>
local a, b, c, d
a = b ?? c ?? d
func a ?? {}
a ??= false
</pre>
</YueDisplay>

### 隐式对象

您可以在表格块内使用符号 **\*** 开始编写一系列隐式结构。如果您正在创建隐式对象，对象的字段必须具有相同的缩进。
```moonscript
list =
  * 1
  * 2
  * 3

func
  * 1
  * 2
  * 3

tb =
  name: "abc"

  values:
    * "a"
    * "b"
    * "c"

  objects:
    * name: "a"
      value: 1
      func: => @value + 1
      tb:
        fieldA: 1

    * name: "b"
      value: 2
      func: => @value + 2
      tb: { }

```
<YueDisplay>
<pre>
list =
  * 1
  * 2
  * 3

func
  * 1
  * 2
  * 3

tb =
  name: "abc"

  values:
    * "a"
    * "b"
    * "c"

  objects:
    * name: "a"
      value: 1
      func: => @value + 1
      tb:
        fieldA: 1

    * name: "b"
      value: 2
      func: => @value + 2
      tb: { }
</pre>
</YueDisplay>

## 模块

### 导入

导入语句是一个语法糖，用于需要引入一个模块或者从已导入的模块中提取子项目。从模块导入的变量默认为不可修改的常量。

```moonscript
-- 用作表解构
do
  import insert, concat from table
  -- 当给 insert, concat 变量赋值时，编译器会报告错误
  import C, Ct, Cmt from require "lpeg"
  -- 快捷写法引入模块的子项
  import x, y, z from 'mymodule'
  -- 使用Python风格的导入
  from 'module' import a, b, c

-- 快捷地导入一个模块
do
  import 'module'
  import 'module_x'
  import "d-a-s-h-e-s"
  import "module.part"

-- 导入模块后起一个别名使用，或是进行导入模块表的解构
do
  import "player" as PlayerModule
  import "lpeg" as :C, :Ct, :Cmt
  import "export" as {one, two, Something:{umm:{ch}}}
```
<YueDisplay>
<pre>
-- 用作表解构
do
  import insert, concat from table
  -- 当给 insert, concat 变量赋值时，编译器会报告错误
  import C, Ct, Cmt from require "lpeg"
  -- 快捷写法引入模块的子项
  import x, y, z from 'mymodule'
  -- 使用Python风格的导入
  from 'module' import a, b, c

-- 快捷地导入一个模块
do
  import 'module'
  import 'module_x'
  import "d-a-s-h-e-s"
  import "module.part"

-- 导入模块后起一个别名使用，或是进行导入模块表的解构
do
  import "player" as PlayerModule
  import "lpeg" as :C, :Ct, :Cmt
  import "export" as {one, two, Something:{umm:{ch}}}
</pre>
</YueDisplay>

### 导出

导出语句提供了一种简洁的方式来定义当前的模块。

* **命名导出**  
带命名的导出将定义一个局部变量，并在导出的表中添加一个同名的字段。

```moonscript
export a, b, c = 1, 2, 3
export cool = "cat"

export What = if this
  "abc"
else
  "def"

export y = ->
  hallo = 3434

export class Something
  umm: "cool"
```
<YueDisplay>
<pre>
export a, b, c = 1, 2, 3
export cool = "cat"

export What = if this
  "abc"
else
  "def"

export y = ->
  hallo = 3434

export class Something
  umm: "cool"
</pre>
</YueDisplay>

使用解构进行命名导出。

```moonscript
export :loadstring, to_lua: tolua = yue
export {itemA: {:fieldA = '默认值'}} = tb
```
<YueDisplay>
<pre>
export :loadstring, to_lua: tolua = yue
export {itemA: {:fieldA = '默认值'}} = tb
</pre>
</YueDisplay>

从模块导出命名项目时，可以不用创建局部变量。

```moonscript
export.itemA = tb
export.<index> = items
export["a-b-c"] = 123
```
<YueDisplay>
<pre>
export.itemA = tb
export.&lt;index&gt; = items
export["a-b-c"] = 123
</pre>
</YueDisplay>

* **未命名导出**  
未命名导出会将要导出的目标项目添加到导出表的数组部分。

```moonscript
d, e, f = 3, 2, 1
export d, e, f

export if this
  123
else
  456

export with tmp
  j = 2000
```
<YueDisplay>
<pre>
d, e, f = 3, 2, 1
export d, e, f

export if this
  123
else
  456

export with tmp
  j = 2000
</pre>
</YueDisplay>

* **默认导出**  
在导出语句中使用 **default** 关键字，来替换导出的表为一个目标的对象。

```moonscript
export default ->
  print "你好"
  123
```
<YueDisplay>
<pre>
export default ->
  print "你好"
  123
</pre>
</YueDisplay>

## 赋值

月之脚本中定义的变量是动态类型的，并默认为局部变量。但你可以通过**local**和**global**声明来改变声明变量的作用范围。

```moonscript
hello = "world"
a, b, c = 1, 2, 3
hello = 123 -- 访问现有的变量
```
<YueDisplay>
<pre>
hello = "world"
a, b, c = 1, 2, 3
hello = 123 -- 访问现有的变量
</pre>
</YueDisplay>

### 执行更新

你可以使用各式二进制运算符执行更新赋值。
```moonscript
x = 1
x += 1
x -= 1
x *= 10
x /= 10
x %= 10
s ..= "world" -- 如果执行更新的局部变量不存在，将新建一个局部变量
arg or= "默认值"
```
<YueDisplay>
<pre>
x = 1
x += 1
x -= 1
x *= 10
x /= 10
x %= 10
s ..= "world" -- 如果执行更新的局部变量不存在，将新建一个局部变量
arg or= "默认值"
</pre>
</YueDisplay>

### 链式赋值

你可以进行链式赋值，将多个项目赋予相同的值。
```moonscript
a = b = c = d = e = 0
x = y = z = f!
```
<YueDisplay>
<pre>
a = b = c = d = e = 0
x = y = z = f!
</pre>
</YueDisplay>

### 显式声明局部变量
```moonscript
do
  local a = 1
  local *
  print "预先声明后续所有变量为局部变量"
  x = -> 1 + y + z
  y, z = 2, 3
  global instance = Item\new!

do
  local X = 1
  local ^
  print "只预先声明后续大写的变量为局部变量"
  a = 1
  B = 2
```
<YueDisplay>
<pre>
do
  local a = 1
  local *
  print "预先声明后续所有变量为局部变量"
  x = -> 1 + y + z
  y, z = 2, 3
  global instance = Item\new!

do
  local X = 1
  local ^
  print "只预先声明后续大写的变量为局部变量"
  a = 1
  B = 2
</pre>
</YueDisplay>

### 显式声明全局变量
```moonscript
do
  global a = 1
  global *
  print "预先声明所有变量为全局变量"
  x = -> 1 + y + z
  y, z = 2, 3

do
  global x = 1
  global ^
  print "只预先声明大写的变量为全局变量"
  a = 1
  B = 2
  local Temp = "一个局部值"
```
<YueDisplay>
<pre>
do
  global a = 1
  global *
  print "预先声明所有变量为全局变量"
  x = -> 1 + y + z
  y, z = 2, 3

do
  global x = 1
  global ^
  print "只预先声明大写的变量为全局变量"
  a = 1
  B = 2
  local Temp = "一个局部值"
</pre>
</YueDisplay>

## 解构赋值

解构赋值是一种快速从Lua表中按名称或基于数组中的位置提取值的方法。

通常当你看到一个字面量的Lua表，比如{1,2,3}，它位于赋值的右侧，因为它是一个值。解构赋值语句的写法就是交换了字面量Lua表的角色，并将其放在赋值语句的左侧。

最好是通过示例来解释。以下是如何从表格中解包前两个值的方法：

```moonscript
thing = [1, 2]

[a, b] = thing
print a, b
```
<YueDisplay>

<pre>
thing = [1, 2]

[a, b] = thing
print a, b
</pre>
</YueDisplay>

在解构表格字面量中，键代表从右侧读取的键，值代表读取的值将被赋予的名称。

```moonscript
obj = {
  hello: "world"
  day: "tuesday"
  length: 20
}

{hello: hello, day: the_day} = obj
print hello, the_day

:day = obj -- 可以不带大括号进行简单的解构
```
<YueDisplay>
<pre>
obj = {
  hello: "world"
  day: "tuesday"
  length: 20
}

{hello: hello, day: the_day} = obj
print hello, the_day

:day = obj -- 可以不带大括号进行简单的解构
</pre>
</YueDisplay>

这也适用于嵌套的数据结构：

```moonscript
obj2 = {
  numbers: [1,2,3,4]
  properties: {
    color: "green"
    height: 13.5
  }
}

{numbers: [first, second]} = obj2
print first, second, color
```
<YueDisplay>
<pre>
obj2 = {
  numbers: [1,2,3,4]
  properties: {
    color: "green"
    height: 13.5
  }
}

{numbers: [first, second]} = obj2
print first, second, color
</pre>
</YueDisplay>

如果解构语句很复杂，也可以任意将其分散在几行中。稍微复杂一些的示例：

```moonscript
{
  numbers: [first, second]
  properties: {
    color: color
  }
} = obj2
```
<YueDisplay>
<pre>
{
  numbers: [first, second]
  properties: {
    color: color
  }
} = obj2
</pre>
</YueDisplay>

有时候我们会需要从Lua表中提取值并将它们赋给与键同名的局部变量。为了避免编写重复代码，我们可以使用 **:** 前缀操作符：

```moonscript
{:concat, :insert} = table
```
<YueDisplay>
<pre>
{:concat, :insert} = table
</pre>
</YueDisplay>

这样的用法与导入语法有些相似。但我们可以通过混合语法重命名我们想要提取的字段：

```moonscript
{:mix, :max, random: rand} = math
```
<YueDisplay>
<pre>
{:mix, :max, random: rand} = math
</pre>
</YueDisplay>

在进行解构时，您可以指定默认值，如：

```moonscript
{:name = "nameless", :job = "jobless"} = person
```
<YueDisplay>
<pre>
{:name = "nameless", :job = "jobless"} = person
</pre>
</YueDisplay>

在进行列表解构时，您可以使用`_`作为占位符：

```moonscript
[_, two, _, four] = items
```
<YueDisplay>
<pre>
{_, two, _, four} = items
</pre>
</YueDisplay>

### 在其它地方的解构

解构也可以出现在其它隐式进行赋值的地方。一个例子是用在for循环：

```moonscript
tuples = [
  ["hello", "world"]
  ["egg", "head"]
]

for [left, right] in *tuples
  print left, right
```
<YueDisplay>
<pre>
tuples = [
  ["hello", "world"]
  ["egg", "head"]
]

for [left, right] in *tuples
  print left, right
</pre>
</YueDisplay>

我们知道数组表中的每个元素都是一个两项的元组，所以我们可以直接在for语句的名称子句中使用解构来解包它。

## If 赋值

`if` 和 `elseif` 代码块可以在条件表达式的位置进行赋值。在代码执行到要计算条件时，会首先进行赋值计算，并使用赋与的值作为分支判断的条件。赋值的变量仅在条件分支的代码块内有效，这意味着如果值不是真值，那么它就不会被用到。注意，你必须使用“海象运算符” `:=` 而不是 `=` 来做赋值。

```moonscript
if user := database.find_user "moon"
  print user.name
```
<YueDisplay>
<pre>
if user := database.find_user "moon"
  print user.name
</pre>
</YueDisplay>

```moonscript
if hello := os.getenv "hello"
  print "你有 hello", hello
elseif world := os.getenv "world"
  print "你有 world", world
else
  print "什么都没有 :("
```
<YueDisplay>
<pre>
if hello := os.getenv "hello"
  print "你有 hello", hello
elseif world := os.getenv "world"
  print "你有 world", world
else
  print "什么都没有 :("
</pre>
</YueDisplay>

使用多个返回值的 If 赋值。只有第一个值会被检查，其他值都有同样的作用域。
```moonscript
if success, result := pcall -> "无报错地获取结果"
  print result -- 变量 result 是有作用域的
print "好的"
```
<YueDisplay>
<pre>
if success, result := pcall -> "无报错地获取结果"
  print result -- 变量 result 是有作用域的
print "好的"
</pre>
</YueDisplay>

## 可变参数赋值

您可以将函数返回的结果赋值给一个可变参数符号 `...`。然后使用Lua的方式访问其内容。
```moonscript
list = [1, 2, 3, 4, 5]
fn = (ok) -> ok, table.unpack list
ok, ... = fn true
count = select '#', ...
first = select 1, ...
print ok, count, first
```
<YueDisplay>
<pre>
list = [1, 2, 3, 4, 5]
fn = (ok) -> ok, table.unpack list
ok, ... = fn true
count = select '#', ...
first = select 1, ...
print ok, count, first
</pre>
</YueDisplay>

## 空白

月之脚本是一个对空白敏感的语言。您必须在相同的缩进中使用空格 **' '** 或制表符 **'\t'** 来编写一些代码块，如函数体、值列表和一些控制块。包含不同空白的表达式可能意味着不同的事情。制表符被视为4个空格，但最好不要混合使用空格和制表符。

### 多行链式调用

你可以使用相同的缩进来编写多行链式函数调用。
```moonscript
Rx.Observable
  .fromRange 1, 8
  \filter (x)-> x % 2 == 0
  \concat Rx.Observable.of 'who do we appreciate'
  \map (value)-> value .. '!'
  \subscribe print
```
<YueDisplay>
<pre>
Rx.Observable
  .fromRange 1, 8
  \filter (x)-> x % 2 == 0
  \concat Rx.Observable.of 'who do we appreciate'
  \map (value)-> value .. '!'
  \subscribe print
</pre>
</YueDisplay>

## 注释

```moonscript
-- 我是一个注释

str = --[[
这是一个多行注释。
没问题。
]] strA \ -- 注释 1
  .. strB \ -- 注释 2
  .. strC

func --[[端口]] 3000, --[[ip]] "192.168.1.1"
```
<YueDisplay>
<pre>
-- 我是一个注释

str = --[[
这是一个多行注释。
没问题。
]] strA \ -- 注释 1
  .. strB \ -- 注释 2
  .. strC

func --[[端口]] 3000, --[[ip]] "192.168.1.1"
</pre>
</YueDisplay>

## 错误处理

用于统一进行Lua错误处理的便捷语法。

```moonscript
try
  func 1, 2, 3
catch err
  print yue.traceback err

success, result = try
  func 1, 2, 3
catch err
  yue.traceback err

try func 1, 2, 3
catch err
  print yue.traceback err

success, result = try func 1, 2, 3

try
  print "尝试中"
  func 1, 2, 3

-- 使用if赋值模式
if success, result := try func 1, 2, 3
catch err
    print yue.traceback err
  print result
```
<YueDisplay>
<pre>
try
  func 1, 2, 3
catch err
  print yue.traceback err

success, result = try
  func 1, 2, 3
catch err
  yue.traceback err

try func 1, 2, 3
catch err
  print yue.traceback err

success, result = try func 1, 2, 3

try
  print "尝试中"
  func 1, 2, 3

-- 使用if赋值模式
if success, result := try func 1, 2, 3
catch err
    print yue.traceback err
  print result
</pre>
</YueDisplay>

## 属性

月之脚本现在提供了Lua 5.4新增的叫做属性的语法支持。在月之脚本编译到的Lua目标版本低于5.4时，你仍然可以同时使用`const`和`close`的属性声明语法，并获得常量检查和作用域回调的功能。

```moonscript
const a = 123
close _ = <close>: -> print "超出范围。"
```
<YueDisplay>
<pre>
const a = 123
close _ = &lt;close&gt;: -> print "超出范围。"
</pre>
</YueDisplay>

你可以对进行解构得到的变量标记为常量。

```moonscript
const {:a, :b, c, d} = tb
-- a = 1
```
<YueDisplay>
<pre>
const {:a, :b, c, d} = tb
-- a = 1
</pre>
</YueDisplay>

## 字面量

Lua中的所有基本字面量都可以在月之脚本中使用。包括数字、字符串、布尔值和**nil**。

但与Lua不同的是，单引号和双引号字符串内部允许有换行：

```moonscript
some_string = "这是一个字符串
  并包括一个换行。"

-- 使用#{}语法可以将表达式插入到字符串字面量中。
-- 字符串插值只在双引号字符串中可用。
print "我有#{math.random! * 100}%的把握。"
```
<YueDisplay>
<pre>
some_string = "这是一个字符串
  并包括一个换行。"

-- 使用#{}语法可以将表达式插入到字符串字面量中。
-- 字符串插值只在双引号字符串中可用。
print "我有#{math.random! * 100}%的把握。"
</pre>
</YueDisplay>

### 数字字面量

您可以在数字字面量中使用下划线来增加可读性。

```moonscript
integer = 1_000_000
hex = 0xEF_BB_BF
```
<YueDisplay>

<pre>
integer = 1_000_000
hex = 0xEF_BB_BF
</pre>
</YueDisplay>

## 函数字面量

所有函数都是使用月之脚本的函数表达式创建的。一个简单的函数可以用箭头表示为：**->**。

```moonscript
my_function = ->
my_function() -- 调用空函数
```
<YueDisplay>
<pre>
my_function = ->
my_function() -- 调用空函数
</pre>
</YueDisplay>

函数体可以是紧跟在箭头后的一个语句，或者是在后面的行上使用同样缩进的一系列语句：

```moonscript
func_a = -> print "你好，世界"

func_b = ->
  value = 100
  print "这个值是：", value
```
<YueDisplay>
<pre>
func_a = -> print "你好，世界"

func_b = ->
  value = 100
  print "这个值是：", value
</pre>
</YueDisplay>

如果一个函数没有参数，可以使用 **\!** 操作符调用它，而不是空括号。使用 **\!** 调用没有参数的函数是推荐的写法。

```moonscript
func_a!
func_b()
```
<YueDisplay>
<pre>
func_a!
func_b()
</pre>
</YueDisplay>

带有参数的函数可以通过在箭头前加上括号中的参数名列表来进行创建：

```moonscript
sum = (x, y)-> print "数字的和", x + y
```
<YueDisplay>
<pre>
sum = (x, y)-> print "数字的和", x + y
</pre>
</YueDisplay>

函数可以通过在函数名后列出参数来调用。当对函数做嵌套的调用时，后面列出的参数会应用于左侧最近的函数。

```moonscript
sum 10, 20
print sum 10, 20

a b c "a", "b", "c"
```
<YueDisplay>
<pre>
sum 10, 20
print sum 10, 20

a b c "a", "b", "c"
</pre>
</YueDisplay>

为了避免在调用函数时产生歧义，也可以使用括号将参数括起来。比如在以下的例子中是必需的，这样才能确保参数被传入到正确的函数。

```moonscript
print "x:", sum(10, 20), "y:", sum(30, 40)
```
<YueDisplay>
<pre>
print "x:", sum(10, 20), "y:", sum(30, 40)
</pre>
</YueDisplay>

注意：函数名与开始括号之间不能有任何空格。

函数会将函数体中的最后一个语句强制转换为返回语句，这被称作隐式返回：

```moonscript
sum = (x, y)-> x + y
print "数字的和是", sum 10, 20
```
<YueDisplay>
<pre>
sum = (x, y) -> x + y
print "数字的和是", sum 10, 20
</pre>
</YueDisplay>

如果您需要做显式返回，可以使用return关键字：

```moonscript
sum = (x, y)-> return x + y
```
<YueDisplay>
<pre>
sum = (x, y)-> return x + y
</pre>
</YueDisplay>

就像在Lua中一样，函数可以返回多个值。最后一个语句必须是由逗号分隔的值列表：

```moonscript
mystery = (x, y)-> x + y, x - y
a, b = mystery 10, 20
```
<YueDisplay>
<pre>
mystery = (x, y)-> x + y, x - y
a, b = mystery 10, 20
</pre>
</YueDisplay>

### 粗箭头

因为在Lua中调用方法时，经常习惯将对象作为第一个参数传入，所以月之脚本提供了一种特殊的语法来创建自动包含self参数的函数。

```moonscript
func = (num)=> @value + num
```
<YueDisplay>
<pre>
func = (num)=> @value + num
</pre>
</YueDisplay>

### 参数默认值

可以为函数的参数提供默认值。如果参数的值为nil，则确定该参数为空。任何具有默认值的nil参数在函数体运行之前都会被替换。

```moonscript
my_function = (name = "某物", height = 100)->
  print "你好，我是", name
  print "我的高度是", height
```
<YueDisplay>
<pre>
my_function = (name = "某物", height = 100)->
  print "你好，我是", name
  print "我的高度是", height
</pre>
</YueDisplay>

函数参数的默认值表达式在函数体中会按参数声明的顺序进行计算。因此，在默认值的表达式中可以访问先前声明的参数。

```moonscript
some_args = (x = 100, y = x + 1000)->
  print x + y
```
<YueDisplay>
<pre>
some_args = (x = 100, y = x + 1000)->
  print x + y
</pre>
</YueDisplay>

### 多行参数

当调用接收大量参数的函数时，将参数列表分成多行是很方便的。由于月之脚本语言对空白字符的敏感性，做参数列表的分割时务必要小心。

如果要将参数列表写到下一行，那么当前行必须以逗号结束。并且下一行的缩进必须比当前的缩进多。一旦做了参数的缩进，所有其他参数列表的行必须保持相同的缩进级别，以成为参数列表的一部分。

```moonscript
my_func 5, 4, 3,
  8, 9, 10

cool_func 1, 2,
  3, 4,
  5, 6,
  7, 8
```
<YueDisplay>
<pre>
my_func 5, 4, 3,
  8, 9, 10

cool_func 1, 2,
  3, 4,
  5, 6,
  7, 8
</pre>
</YueDisplay>

这种调用方式可以做嵌套。并通过缩进级别来确定参数属于哪一个函数。

```moonscript
my_func 5, 6, 7,
  6, another_func 6, 7, 8,
    9, 1, 2,
  5, 4
```
<YueDisplay>
<pre>
my_func 5, 6, 7,
  6, another_func 6, 7, 8,
    9, 1, 2,
  5, 4
</pre>
</YueDisplay>

因为Lua表也使用逗号作为分隔符，这种缩进语法有助于让值成为参数列表的一部分，而不是Lua表的一部分。

```moonscript
x = [
  1, 2, 3, 4, a_func 4, 5,
    5, 6,
  8, 9, 10
]
```
<YueDisplay>
<pre>
x = [
  1, 2, 3, 4, a_func 4, 5,
    5, 6,
  8, 9, 10
]
</pre>
</YueDisplay>

有个不常见的写法可以注意一下，如果我们将在后面使用较低的缩进，我们可以为函数参数提供更深的缩进来区分列表的归属。

```moonscript
y = [ my_func 1, 2, 3,
   4, 5,
  5, 6, 7
]
```
<YueDisplay>
<pre>
y = [ my_func 1, 2, 3,
   4, 5,
  5, 6, 7
]
</pre>
</YueDisplay>

对于其它有代码块跟随的语句，比如条件语句，也可以通过小心安排缩进来做类似的事。比如我们可以通过调整缩进级别来控制一些值归属于哪个语句：

```moonscript
if func 1, 2, 3,
  "你好",
  "世界"
    print "你好"
    print "我在if内部"

if func 1, 2, 3,
    "你好",
    "世界"
  print "hello"
  print "我在if内部"
```
<YueDisplay>
<pre>
if func 1, 2, 3,
  "你好",
  "世界"
    print "你好"
    print "我在if内部"

if func 1, 2, 3,
    "你好",
    "世界"
  print "你好"
  print "我在if内部"
</pre>
</YueDisplay>

## 反向回调

反向回调用于减少函数回调的嵌套。它们使用指向左侧的箭头，并且默认会被定义为传入后续函数调用的最后一个参数。它的语法大部分与常规箭头函数相同，只是它指向另一方向，并且后续的函数体不需要进行缩进。

```moonscript
<- f
print "hello"
```
<YueDisplay>
<pre>
<- f
print "hello"
</pre>
</YueDisplay>

月之脚本也提供了粗箭头反向回调函数。

```moonscript
<= f
print @value
```
<YueDisplay>
<pre>
<= f
print @value
</pre>
</YueDisplay>

您可以通过一个占位符指定回调函数的传参位置。

```moonscript
(x) <- map _, [1, 2, 3]
x * 2
```
<YueDisplay>
<pre>
(x) <- map _, [1, 2, 3]
x * 2
</pre>
</YueDisplay>

如果您希望在反向回调处理后继续编写更多其它的代码，您可以使用do语句将不归属反向回调的代码分开。

```moonscript
result, msg = do
  (data) <- readAsync "文件名.txt"
  print data
  (info) <- processAsync data
  check info
print result, msg
```
<YueDisplay>
<pre>
result, msg = do
  (data) <- readAsync "文件名.txt"
  print data
  (info) <- processAsync data
  check info
print result, msg
</pre>
</YueDisplay>

## 表格字面量

和Lua一样，表格可以通过花括号进行定义。

```moonscript
some_values = [1, 2, 3, 4]
```
<YueDisplay>
<pre>
some_values = [1, 2, 3, 4]
</pre>
</YueDisplay>

但与Lua不同的是，给表格中的键赋值是用 **:**（而不是 **=**）。

```moonscript
some_values = {
  name: "Bill",
  age: 200,
  ["favorite food"]: "rice"
}
```
<YueDisplay>
<pre>
some_values = {
  name: "Bill",
  age: 200,
  ["favorite food"]: "rice"
}
</pre>
</YueDisplay>

如果只分配一个键值对的表格，可以省略花括号。

```moonscript
profile =
  height: "4英尺",
  shoe_size: 13,
  favorite_foods: ["冰淇淋", "甜甜圈"]
```
<YueDisplay>
<pre>
profile =
  height: "4英尺",
  shoe_size: 13,
  favorite_foods: ["冰淇淋", "甜甜圈"]
</pre>
</YueDisplay>

可以使用换行符而不使用逗号（或两者都用）来分隔表格中的值：

```moonscript
values = {
  1, 2, 3, 4
  5, 6, 7, 8
  name: "超人"
  occupation: "打击犯罪"
}
```
<YueDisplay>
<pre>
values = {
  1, 2, 3, 4
  5, 6, 7, 8
  name: "超人"
  occupation: "打击犯罪"
}
</pre>
</YueDisplay>

创建单行表格字面量时，也可以省略花括号：

```moonscript
my_function dance: "探戈", partner: "无"

y = type: "狗", legs: 4, tails: 1
```
<YueDisplay>
<pre>
my_function dance: "探戈", partner: "无"

y = type: "狗", legs: 4, tails: 1
</pre>
</YueDisplay>

表格字面量的键可以使用Lua语言的关键字，而无需转义：

```moonscript
tbl = {
  do: "某事"
  end: "饥饿"
}
```
<YueDisplay>
<pre>
tbl = {
  do: "某事"
  end: "饥饿"
}
</pre>
</YueDisplay>

如果你要构造一个由变量组成的表，并希望键与变量名相同，那么可以使用 **:** 前缀操作符：

```moonscript
hair = "金色"
height = 200
person = { :hair, :height, shoe_size: 40 }

print_table :hair, :height
```
<YueDisplay>
<pre>
hair = "金色"
height = 200
person = { :hair, :height, shoe_size: 40 }

print_table :hair, :height
</pre>
</YueDisplay>

如果你希望表中字段的键是某个表达式的结果，那么可以用 **[ ]** 包裹它，就像在Lua中一样。如果键中有任何特殊字符，也可以直接使用字符串字面量作为键，省略方括号。

```moonscript
t = {
  [1 + 2]: "你好"
  "你好 世界": true
}
```
<YueDisplay>
<pre>
t = {
  [1 + 2]: "你好"
  "你好 世界": true
}
</pre>
</YueDisplay>

Lua的表同时具有数组部分和哈希部分，但有时候你会希望在书写Lua表时，对Lua表做数组和哈希不同用法的语义区分。然后你可以用 **[ ]** 而不是 **{ }** 来编写表示数组的 Lua 表，并且不允许在数组 Lua 表中写入任何键值对。

```moonscript
some_values = [ 1, 2, 3, 4 ]
list_with_one_element = [ 1, ]
```
<YueDisplay>
<pre>
some_values = [ 1, 2, 3, 4 ]
list_with_one_element = [ 1, ]
</pre>
</YueDisplay>

## 推导式

推导式为我们提供了一种便捷的语法，通过遍历现有对象并对其值应用表达式来构造出新的表格。月之脚本有两种推导式：列表推导式和表格推导式。它们最终都是产生Lua表格；列表推导式将值累积到类似数组的表格中，而表格推导式允许您在每次遍历时设置新表格的键和值。

### 列表推导式

以下操作创建了一个items表的副本，但所有包含的值都翻倍了。

```moonscript
items = [1, 2, 3, 4]
doubled = [item * 2 for i, item in ipairs items]
```
<YueDisplay>
<pre>
items = [1, 2, 3, 4]
doubled = [item * 2 for i, item in ipairs items]
</pre>
</YueDisplay>

可以使用when子句筛选新表中包含的项目：

```moonscript
iter = ipairs items
slice = [item for i, item in iter when i > 1 and i < 3]
```
<YueDisplay>
<pre>
iter = ipairs items
slice = [item for i, item in iter when i > 1 and i < 3]
</pre>
</YueDisplay>

因为我们常常需要迭代数值索引表的值，所以引入了 **\*** 操作符来做语法简化。doubled示例可以重写为：

```moonscript
doubled = [item * 2 for item in *items]
```
<YueDisplay>
<pre>
doubled = [item * 2 for item in *items]
</pre>
</YueDisplay>

for和when子句可以根据需要进行链式操作。唯一的要求是推导式中至少要有一个for子句。

使用多个for子句与使用多重循环的效果相同：

```moonscript
x_coords = [4, 5, 6, 7]
y_coords = [9, 2, 3]

points = [ [x, y] for x in *x_coords \
for y in *y_coords]
```
<YueDisplay>
<pre>
x_coords = [4, 5, 6, 7]
y_coords = [9, 2, 3]

points = [ [x, y] for x in *x_coords \
for y in *y_coords]
</pre>
</YueDisplay>

在推导式中也可以使用简单的数值for循环：

```moonscript
evens = [i for i = 1, 100 when i % 2 == 0]
```
<YueDisplay>
<pre>
evens = [i for i = 1, 100 when i % 2 == 0]
</pre>
</YueDisplay>

### 表格推导式

表格推导式和列表推导式的语法非常相似，只是要使用 **{** 和 **}** 并从每次迭代中取两个值。

以下示例生成了表格thing的副本：

```moonscript
thing = {
  color: "red"
  name: "fast"
  width: 123
}

thing_copy = {k, v for k, v in pairs thing}
```
<YueDisplay>
<pre>
thing = {
  color: "red"
  name: "fast"
  width: 123
}

thing_copy = {k, v for k, v in pairs thing}
</pre>
</YueDisplay>

```moonscript
no_color = {k, v for k, v in pairs thing when k != "color"}
```
<YueDisplay>
<pre>
no_color = {k, v for k, v in pairs thing when k != "color"}
</pre>
</YueDisplay>

**\*** 操作符在表格推导式中能使用。在下面的例子里，我们为几个数字创建了一个平方根查找表。

```moonscript
numbers = [1, 2, 3, 4]
sqrts = {i, math.sqrt i for i in *numbers}
```
<YueDisplay>
<pre>
numbers = [1, 2, 3, 4]
sqrts = {i, math.sqrt i for i in *numbers}
</pre>
</YueDisplay>

表格推导式中的键值元组也可以来自单个表达式，在这种情况下，表达式在计算后应返回两个值。第一个用作键，第二个用作值：

在下面的示例中，我们将一些数组转换为一个表，其中每个数组里的第一项是键，第二项是值。

```moonscript
tuples = [ ["hello", "world"], ["foo", "bar"]]
tbl = {unpack tuple for tuple in *tuples}
```
<YueDisplay>
<pre>
tuples = [ ["hello", "world"], ["foo", "bar"]]
tbl = {unpack tuple for tuple in *tuples}
</pre>
</YueDisplay>

### 切片

当使用 **\*** 操作符时，月之脚本还提供了一种特殊的语法来限制要遍历的列表范围。这个语法也相当于在for循环中设置迭代边界和步长。

下面的案例中，我们在切片中设置最小和最大边界，取索引在1到5之间（包括1和5）的所有项目：

```moonscript
slice = [item for item in *items[1, 5]]
```
<YueDisplay>
<pre>
slice = [item for item in *items[1, 5]]
</pre>
</YueDisplay>

切片的任意参数都可以省略，并会使用默认值。在如下示例中，如果省略了最大索引边界，它默认为表的长度。使下面的代码取除第一个元素之外的所有元素：

```moonscript
slice = [item for item in *items[2,]]
```
<YueDisplay>
<pre>
slice = [item for item in *items[2,]]
</pre>
</YueDisplay>

如果省略了最小边界，便默认会设置为1。这里我们只提供一个步长，并留下其他边界为空。这样会使得代码取出所有奇数索引的项目：(1, 3, 5, …)

```moonscript
slice = [item for item in *items[,,2]]
```
<YueDisplay>

<pre>
slice = [item for item in *items[,,2]]
</pre>
</YueDisplay>

## for 循环

Lua中有两种for循环形式，数字型和通用型：

```moonscript
for i = 10, 20
  print i

for k = 1, 15, 2 -- 提供了一个遍历的步长
  print k

for key, value in pairs object
  print key, value
```
<YueDisplay>
<pre>
for i = 10, 20
  print i

for k = 1, 15, 2 -- 提供了一个遍历的步长
  print k

for key, value in pairs object
  print key, value
</pre>
</YueDisplay>

可以使用切片和 **\*** 操作符，就像在列表推导中一样：

```moonscript
for item in *items[2, 4]
  print item
```
<YueDisplay>
<pre>
for item in *items[2, 4]
  print item
</pre>
</YueDisplay>

当代码语句只有一行时，循环语句也都可以写作更短的语法：

```moonscript
for item in *items do print item

for j = 1, 10, 3 do print j
```
<YueDisplay>
<pre>
for item in *items do print item

for j = 1, 10, 3 do print j
</pre>
</YueDisplay>

for循环也可以用作表达式。for循环主体中的最后一条语句会被强制转换为一个返回值的表达式，并会将表达式计算结果的值追加到一个作为结果的数组表中。

将每个偶数加倍：

```moonscript
doubled_evens = for i = 1, 20
  if i % 2 == 0
    i * 2
  else
    i
```
<YueDisplay>
<pre>
doubled_evens = for i = 1, 20
  if i % 2 == 0
    i * 2
  else
    i
</pre>
</YueDisplay>

您还可以结合for循环表达式与continue语句来过滤值。

注意出现在函数体末尾的for循环，不会被当作是一个表达式，并将循环结果累积到一个列表中作为返回值（相反，函数将返回nil）。如果要函数末尾的循环转换为列表表达式，可以使用返回语句加for循环表达式。

```moonscript
func_a = -> for i = 1, 10 do print i
func_b = -> return for i = 1, 10 do i

print func_a! -- 打印 nil
print func_b! -- 打印 table 对象
```
<YueDisplay>
<pre>
func_a = -> for i = 1, 10 do print i
func_b = -> return for i = 1, 10 do i

print func_a! -- 打印 nil
print func_b! -- 打印 table 对象
</pre>
</YueDisplay>

这样做是为了避免在不需要返回循环结果的函数，创建无效的返回值表格。

## repeat 循环

repeat循环是从Lua语言中搬过来的相似语法：

```moonscript
i = 10
repeat
  print i
  i -= 1
until i == 0
```
<YueDisplay>
<pre>
i = 10
repeat
  print i
  i -= 1
until i == 0
</pre>
</YueDisplay>

## while 循环

在月之脚本中的while循环有四种写法：

```moonscript
i = 10
while i > 0
  print i
  i -= 1

while running == true do my_function!
```
<YueDisplay>
<pre>
i = 10
while i > 0
  print i
  i -= 1

while running == true do my_function!
</pre>
</YueDisplay>

```moonscript
i = 10
until i == 0
  print i
  i -= 1

until running == false do my_function!
```
<YueDisplay>
<pre>
i = 10
until i == 0
  print i
  i -= 1
until running == false do my_function!
</pre>
</YueDisplay>

像for循环的语法一样，while循环也可以作为一个表达式使用。为了使函数返回while循环的累积列表值，必须明确使用返回语句返回while循环表达式。

## 继续

继续语句可以用来跳出当前的循环迭代。

```moonscript
i = 0
while i < 10
  i += 1
  continue if i % 2 == 0
  print i
```
<YueDisplay>
<pre>
i = 0
while i < 10
  i += 1
  continue if i % 2 == 0
  print i
</pre>
</YueDisplay>

继续语句也可以与各种循环表达式一起使用，以防止当前的循环迭代结果累积到结果列表中。以下示例将数组表过滤为仅包含偶数的数组：

```moonscript
my_numbers = [1, 2, 3, 4, 5, 6]
odds = for x in *my_numbers
  continue if x % 2 == 1
  x
```
<YueDisplay>
<pre>
my_numbers = [1, 2, 3, 4, 5, 6]
odds = for x in *my_numbers
  continue if x % 2 == 1
  x
</pre>
</YueDisplay>

## 条件语句

```moonscript
have_coins = false
if have_coins
  print "有硬币"
else
  print "没有硬币"
```
<YueDisplay>
<pre>
have_coins = false
if have_coins
  print "有硬币"
else
  print "没有硬币"
</pre>
</YueDisplay>

对于简单的语句，也可以使用简短的语法：

```moonscript
have_coins = false
if have_coins then print "有硬币" else print "没有硬币"
```
<YueDisplay>
<pre>
have_coins = false
if have_coins then print "有硬币" else print "没有硬币"
</pre>
</YueDisplay>

因为if语句可以用作表达式，所以也可以这样写：

```moonscript
have_coins = false
print if have_coins then "有硬币" else "没有硬币"
```
<YueDisplay>
<pre>
have_coins = false
print if have_coins then "有硬币" else "没有硬币"
</pre>
</YueDisplay>

条件语句也可以作为表达式用在返回语句和赋值语句中：

```moonscript
is_tall = (name) ->
  if name == "Rob"
    true
  else
    false

message = if is_tall "Rob"
  "我很高"
else
  "我不是很高"

print message -- 打印: 我很高
```
<YueDisplay>
<pre>
is_tall = (name) ->
  if name == "Rob"
    true
  else
    false

message = if is_tall "Rob"
  "我很高"
else
  "我不是很高"

print message -- 打印: 我很高
</pre>
</YueDisplay>

if的反义词是unless（相当于if not，如果 vs 除非）：

```moonscript
unless os.date("%A") == "Monday"
  print "今天不是星期一！"
```
<YueDisplay>
<pre>
unless os.date("%A") == "Monday"
  print "今天不是星期一！"
</pre>
</YueDisplay>

```moonscript
print "你真幸运！" unless math.random! > 0.1
```
<YueDisplay>
<pre>
print "你真幸运！" unless math.random! > 0.1
</pre>
</YueDisplay>

### 范围表达式

您可以使用范围表达式来编写进行范围检查的代码。

```moonscript
a = 5

if a in [1, 3, 5, 7]
  print "检查离散值的相等性"

if a in list
  print "检查`a`是否在列表中"
```
<YueDisplay>
<pre>
a = 5

if a in [1, 3, 5, 7]
  print "检查离散值的相等性"

if a in list
  print "检查`a`是否在列表中"
</pre>
</YueDisplay>

```moonscript
print "你很幸运!" unless math.random! > 0.1
```
<YueDisplay>
<pre>
print "你很幸运!" unless math.random! > 0.1
</pre>
</YueDisplay>

## 代码行修饰符

为了方便编写代码，循环语句和if语句可以应用于单行代码语句的末尾：

```moonscript
print "你好，世界" if name == "Rob"
```
<YueDisplay>
<pre>
print "你好，世界" if name == "Rob"
</pre>
</YueDisplay>

修饰for循环的示例：

```moonscript
print "项目: ", item for item in *items
```
<YueDisplay>
<pre>
print "项目: ", item for item in *items
</pre>
</YueDisplay>

修饰while循环的示例：

```moonscript
game\update! while game\isRunning!

reader\parse_line! until reader\eof!
```
<YueDisplay>
<pre>
game\update! while game\isRunning!

reader\parse_line! until reader\eof!
</pre>
</YueDisplay>

## switch 语句

switch语句是为了简化检查一系列相同值的if语句而提供的简写语法。要注意用于比较检查的目标值只会计算一次。和if语句一样，switch语句在最后可以接一个else代码块来处理没有匹配的情况。在生成的Lua代码中，进行比较是使用==操作符完成的。

```moonscript
name = "Dan"
switch name
  when "Robert"
    print "你是Robert"
  when "Dan", "Daniel"
    print "你的名字是Dan"
  else
    print "我不知道你的名字"
```
<YueDisplay>
<pre>
name = "Dan"
switch name
  when "Robert"
    print "你是Robert"
  when "Dan", "Daniel"
    print "你的名字是Dan"
  else
    print "我不知道你的名字"
</pre>
</YueDisplay>

switch语句的when子句中可以通过使用逗号分隔的列表来匹配多个值。

switch语句也可以作为表达式使用，下面我们可以将switch语句返回的结果分配给一个变量：

```moonscript
b = 1
next_number = switch b
  when 1
    2
  when 2
    3
  else
    error "数字数得太大了！"
```
<YueDisplay>
<pre>
b = 1
next_number = switch b
  when 1
    2
  when 2
    3
  else
    error "数字数得太大了！"
</pre>
</YueDisplay>

我们可以使用then关键字在when子句的同一行上编写处理代码。else代码块的后续代码中要写在同一行上不需要额外的关键字。

```moonscript
msg = switch math.random(1, 5)
  when 1 then "你很幸运"
  when 2 then "你差点很幸运"
  else "不太幸运"
```
<YueDisplay>
<pre>
msg = switch math.random(1, 5)
  when 1 then "你很幸运"
  when 2 then "你差点很幸运"
  else "不太幸运"
</pre>
</YueDisplay>

如果在编写switch语句时希望少写一个缩进，那么你可以把第一个when子句放在switch开始语句的第一行，然后后续的子语句就都可以都少写一个缩进。

```moonscript
switch math.random(1, 5)
  when 1
    print "你很幸运" -- 两个缩进级别
  else
    print "不太幸运"

switch math.random(1, 5) when 1
  print "你很幸运" -- 一个缩进级别
else
  print "不太幸运"
```
<YueDisplay>
<pre>
switch math.random(1, 5)
  when 1
    print "你很幸运" -- 两个缩进级别
  else
    print "不太幸运"

switch math.random(1, 5) when 1
  print "你很幸运" -- 一个缩进级别
else
  print "不太幸运"
</pre>
</YueDisplay>

值得注意的是，在生成Lua代码时，我们要做检查的目标变量会放在==表达式的右侧。当您希望给when子句的比较对象定义一个\_\_eq元方法来重载判断逻辑时，可能会有用。

### 表格匹配

在switch的when子句中，如果期待检查目标是一个表格，且可以通过特定的结构进行解构并获得非nil值，那么你可以尝试使用表格匹配的语法。

```moonscript
items =
  * x: 100
    y: 200
  * width: 300
    height: 400

for item in *items
  switch item
    when :x, :y
      print "Vec2 #{x}, #{y}"
    when :width, :height
      print "尺寸 #{width}, #{height}"
```
<YueDisplay>
<pre>
items =
  * x: 100
    y: 200
  * width: 300
    height: 400

for item in *items
  switch item
    when :x, :y
      print "Vec2 #{x}, #{y}"
    when :width, :height
      print "尺寸 #{width}, #{height}"
</pre>
</YueDisplay>

你可以使用默认值来选择性地解构表格的某些字段。

```moonscript
item = {}

{pos: {:x = 50, :y = 200}} = item -- 获取错误：尝试索引nil值（字段'pos'）

switch item
  when {pos: {:x = 50, :y = 200}}
    print "Vec2 #{x}, #{y}" -- 表格解构仍然会通过
```
<YueDisplay>
<pre>
item = {}

{pos: {:x = 50, :y = 200}} = item -- 获取错误：尝试索引nil值（字段'pos'）

switch item
  when {pos: {:x = 50, :y = 200}}
    print "Vec2 #{x}, #{y}" -- 表格解构仍然会通过
</pre>
</YueDisplay>

## 面向对象编程

在以下的示例中，月之脚本生成的Lua代码可能看起来会很复杂。所以最好主要关注月之脚本代码层面的意义，然后如果您想知道关于面向对象功能的实现细节，再查看Lua代码。

一个简单的类：

```moonscript
class Inventory
  new: =>
    @items = {}

  add_item: (name)=>
    if @items[name]
      @items[name] += 1
    else
      @items[name] = 1
```
<YueDisplay>
<pre>
class Inventory
  new: =>
    @items = {}

  add_item: (name)=>
    if @items[name]
      @items[name] += 1
    else
      @items[name] = 1
</pre>
</YueDisplay>

在月之脚本中采用面向对象的编程方式时，通常会使用类声明语句结合Lua表格字面量来做类定义。这个类的定义包含了它的所有方法和属性。在这种结构中，键名为“new”的成员扮演了一个重要的角色，是作为构造函数来使用。

值得注意的是，类中的方法都采用了粗箭头函数语法。当在类的实例上调用方法时，该实例会自动作为第一个参数被传入，因此粗箭头函数用于生成一个名为“self”的参数。

此外，“@”前缀在变量名上起到了简化作用，代表“self”。例如，`@items` 就等同于 `self.items`。

为了创建类的一个新实例，可以将类名当作一个函数来调用，这样就可以生成并返回一个新的实例。

```moonscript
inv = Inventory!
inv\add_item "t-shirt"
inv\add_item "pants"
```
<YueDisplay>

<pre>
inv = Inventory!
inv\add_item "t-shirt"
inv\add_item "pants"
</pre>
</YueDisplay>

在月之脚本的类中，由于需要将类的实例作为参数传入到调用的方法中，因此使用了 **\\** 操作符做类的成员函数调用。

需要特别注意的是，类的所有属性在其实例之间是共享的。这对于函数类型的成员属性通常不会造成问题，但对于其他类型的属性，可能会导致意外的结果。

例如，在下面的示例中，clothes属性在所有实例之间共享。因此，对这个属性在一个实例中的修改，将会影响到其他所有实例。

```moonscript
class Person
  clothes: []
  give_item: (name)=>
    table.insert @clothes, name

a = Person!
b = Person!

a\give_item "pants"
b\give_item "shirt"

-- 会同时打印出裤子和衬衫
print item for item in *a.clothes
```
<YueDisplay>
<pre>
class Person
  clothes: []
  give_item: (name)=>
    table.insert @clothes, name

a = Person!
b = Person!

a\give_item "pants"
b\give_item "shirt"

-- 会同时打印出裤子和衬衫
print item for item in *a.clothes
</pre>
</YueDisplay>

避免这个问题的正确方法是在构造函数中创建对象的可变状态：

```moonscript
class Person
  new: =>
    @clothes = []
```
<YueDisplay>
<pre>
class Person
  new: =>
    @clothes = []
</pre>
</YueDisplay>

### 继承

`extends`关键字可以在类声明中使用，以继承另一个类的属性和方法。

```moonscript
class BackPack extends Inventory
  size: 10
  add_item: (name)=>
    if #@items > size then error "背包已满"
    super name
```
<YueDisplay>
<pre>
class BackPack extends Inventory
  size: 10
  add_item: (name)=>
    if #@items > size then error "背包已满"
    super name
</pre>
</YueDisplay>


在这一部分，我们对月之脚本中的`Inventory`类进行了扩展，加入了对可以携带物品数量的限制。

在这个特定的例子中，子类并没有定义自己的构造函数。因此，当创建一个新的实例时，系统会默认调用父类的构造函数。但如果我们在子类中定义了构造函数，我们可以利用`super`方法来调用并执行父类的构造函数。

此外，当一个类继承自另一个类时，它会尝试调用父类上的`__inherited`方法（如果这个方法存在的话），以此来向父类发送通知。这个`__inherited`函数接受两个参数：被继承的父类和继承的子类。

```moonscript
class Shelf
  @__inherited: (child)=>
    print @__name, "被", child.__name, "继承"

-- 将打印: Shelf 被 Cupboard 继承
class Cupboard extends Shelf
```
<YueDisplay>
<pre>
class Shelf
  @__inherited: (child)=>
    print @__name, "被", child.__name, "继承"

-- 将打印: Shelf 被 Cupboard 继承
class Cupboard extends Shelf
</pre>
</YueDisplay>

### super 关键字

`super`是一个特别的关键字，它有两种不同的使用方式：既可以当作一个对象来看待，也可以像调用函数那样使用。它仅在类的内部使用时具有特殊的功能。

当`super`被作为一个函数调用时，它将调用父类中与之同名的函数。此时，当前的`self`会自动作为第一个参数传递，正如上面提到的继承示例所展示的那样。

在将`super`当作普通值使用时，它实际上是对父类对象的引用。通过这种方式，我们可以访问父类中可能被子类覆盖的值，就像访问任何普通对象一样。

此外，当使用`\`操作符与`super`一起使用时，`self`将被插入为第一个参数，而不是使用`super`本身的值。而在使用`.`操作符来检索函数时，则会返回父类中的原始函数。

下面是一些使用`super`的不同方法的示例：

```moonscript
class MyClass extends ParentClass
  a_method: =>
    -- 以下效果相同：
    super "你好", "世界"
    super\a_method "你好", "世界"
    super.a_method self, "你好", "世界"

    -- super 作为值等于父类：
    assert super == ParentClass
```
<YueDisplay>
<pre>
class MyClass extends ParentClass
  a_method: =>
    -- 以下效果相同：
    super "你好", "世界"
    super\a_method "你好", "世界"
    super.a_method self, "你好", "世界"

    -- super 作为值等于父类：
    assert super == ParentClass
</pre>
</YueDisplay>

**super** 也可以用在函数存根的左侧。唯一的主要区别是，生成的函数不是绑定到 super 的值，而是绑定到 self。

### 类型

每个类的实例都带有它的类型。这存储在特殊的 \_\_class 属性中。此属性会保存类对象。类对象是我们用来构建新实例的对象。我们还可以索引类对象以检索类方法和属性。

```moonscript
b = BackPack!
assert b.__class == BackPack

print BackPack.size -- 打印 10
```
<YueDisplay>
<pre>
b = BackPack!
assert b.__class == BackPack

print BackPack.size -- 打印 10
</pre>
</YueDisplay>

### 类对象


在月之脚本中，当我们编写类的定义语句时，实际上是在创建一个类对象。这个类对象被保存在一个与该类同名的变量中。

类对象具有函数的特性，可以被调用来创建新的实例。这正是我们在之前示例中所展示的创建类实例的方式。

一个类由两个表构成：类表本身和一个基表。基表作为所有实例的元表。在类声明中列出的所有属性都存放在基表中。

如果在类对象的元表中找不到某个属性，系统会从基表中检索该属性。这就意味着我们可以直接从类本身访问到其方法和属性。

需要特别注意的是，对类对象的赋值并不会影响到基表，因此这不是向实例添加新方法的正确方式。相反，需要直接修改基表。关于这点，可以参考下面的“__base”字段。

此外，类对象包含几个特殊的属性：当类被声明时，类的名称会作为一个字符串存储在类对象的“__name”字段中。

```moonscript
print BackPack.__name -- 打印 Backpack
```
<YueDisplay>
<pre>
print BackPack.__name -- 打印 Backpack
</pre>
</YueDisplay>

基础对象被保存在一个名为 `__base` 的特殊表中。我们可以编辑这个表，以便为那些已经创建出来的实例和还未创建的实例增加新的功能。

另外，如果一个类是从另一个类派生而来的，那么其父类对象则会被存储在名为 `__parent` 的地方。这种机制允许在类之间实现继承和功能扩展。

### 类变量

我们可以直接在类对象中创建变量，而不是在类的基对象中，通过在类声明中的属性名前使用 @。

```moonscript
class Things
  @some_func: => print "Hello from", @__name

Things\some_func!

-- 类变量在实例中不可见
assert Things().some_func == nil
```
<YueDisplay>
<pre>
class Things
  @some_func: => print "Hello from", @__name

Things\some_func!

-- 类变量在实例中不可见
assert Things().some_func == nil
</pre>
</YueDisplay>

在表达式中，我们可以使用 @@ 来访问存储在 `self.__class` 中的值。因此，`@@hello` 是 `self.__class.hello` 的简写。

```moonscript
class Counter
  @count: 0

  new: =>
    @@count += 1

Counter!
Counter!

print Counter.count -- 输出 2
```
<YueDisplay>
<pre>
class Counter
  @count: 0

  new: =>
    @@count += 1

Counter!
Counter!

print Counter.count -- 输出 2
</pre>
</YueDisplay>

@@ 的调用语义与 @ 类似。调用 @@ 时，会使用 Lua 的冒号语法将类作为第一个参数传入。

```moonscript
@@hello 1,2,3,4
```
<YueDisplay>
<pre>
@@hello 1,2,3,4
</pre>
</YueDisplay>

### 类声明语句

在类声明的主体中，除了键/值对外，我们还可以编写普通的表达式。在这种类声明体中的普通代码的上下文中，self等于类对象，而不是实例对象。

以下是创建类变量的另一种方法：

```moonscript
class Things
  @class_var = "hello world"
```
<YueDisplay>
<pre>
class Things
  @class_var = "hello world"
</pre>
</YueDisplay>

这些表达式会在所有属性被添加到类的基对象后执行。

在类的主体中声明的所有变量都会限制作用域只在类声明的范围。这对于放置只有类方法可以访问的私有值或辅助函数很方便：

```moonscript
class MoreThings
  secret = 123
  log = (msg)-> print "LOG:", msg

  some_method: =>
    log "hello world: " .. secret
```
<YueDisplay>
<pre>
class MoreThings
  secret = 123
  log = (msg)-> print "LOG:", msg

  some_method: =>
    log "hello world: " .. secret
</pre>
</YueDisplay>

### @ 和 @@ 值

当@和@@前缀在一个名字前时，它们分别代表在self和self.\_\_class中访问的那个名字。

如果它们单独使用，它们是self和self.\_\_class的别名。

```moonscript
assert @ == self
assert @@ == self.__class
```
<YueDisplay>
<pre>
assert @ == self
assert @@ == self.__class
</pre>
</YueDisplay>

例如，使用@@从实例方法快速创建同一类的新实例的方法：

```moonscript
some_instance_method = (...)=> @@ ...
```
<YueDisplay>
<pre>
some_instance_method = (...)=> @@ ...
</pre>
</YueDisplay>

### 构造属性提升

为了减少编写简单值对象定义的代码。你可以这样简单写一个类：

```moonscript
class Something
  new: (@foo, @bar, @@biz, @@baz) =>

-- 这是以下声明的简写形式

class Something
  new: (foo, bar, biz, baz) =>
    @foo = foo
    @bar = bar
    @@biz = biz
    @@baz = baz
```
<YueDisplay>
<pre>
class Something
  new: (@foo, @bar, @@biz, @@baz) =>

-- 这是以下声明的简写形式

class Something
  new: (foo, bar, biz, baz) =>
    @foo = foo
    @bar = bar
    @@biz = biz
    @@baz = baz
</pre>
</YueDisplay>

你也可以使用这种语法为一个函数初始化传入对象的字段。

```moonscript
new = (@fieldA, @fieldB)=> @
obj = new {}, 123, "abc"
print obj
```
<YueDisplay>
<pre>
new = (@fieldA, @fieldB)=> @
obj = new {}, 123, "abc"
print obj
</pre>
</YueDisplay>

### 类表达式

类声明的语法也可以作为一个表达式使用，可以赋值给一个变量或者被返回语句返回。

```moonscript
x = class Bucket
  drops: 0
  add_drop: => @drops += 1
```
<YueDisplay>
<pre>
x = class Bucket
  drops: 0
  add_drop: => @drops += 1
</pre>
</YueDisplay>

### 匿名类

声明类时可以省略名称。如果类的表达式不在赋值语句中，\_\_name属性将为nil。如果出现在赋值语句中，赋值操作左侧的名称将代替nil。

```moonscript
BigBucket = class extends Bucket
  add_drop: => @drops += 10

assert Bucket.__name == "BigBucket"
```
<YueDisplay>
<pre>
BigBucket = class extends Bucket
  add_drop: => @drops += 10

assert Bucket.__name == "BigBucket"
</pre>
</YueDisplay>

你甚至可以省略掉主体，这意味着你可以这样写一个空白的匿名类：

```moonscript
x = class
```
<YueDisplay>
<pre>
x = class
</pre>
</YueDisplay>

### 类混合

您可以通过使用 `using` 关键字来实现类混合。这意味着您可以从一个普通 Lua 表格或已定义的类对象中，复制函数到您创建的新类中。当您使用普通 Lua 表格进行类混合时，您有机会用自己的实现来重写类的索引方法（例如元方法 `__index`）。然而，当您从一个类对象做混合时，需要注意的是该类对象的元方法将不会被复制到新类。

```moonscript
MyIndex = __index: var: 1

class X using MyIndex
  func: =>
    print 123

x = X!
print x.var

class Y using X

y = Y!
y\func!

assert y.__class.__parent ~= X -- X 不是 Y 的父类
```
<YueDisplay>
<pre>
MyIndex = __index: var: 1

class X using MyIndex
  func: =>
    print 123

x = X!
print x.var

class Y using X

y = Y!
y\func!

assert y.__class.__parent ~= X -- X 不是 Y 的父类
</pre>
</YueDisplay>

## with 语句

在编写Lua代码时，我们在创建对象后的常见操作是立即调用这个对象一系列操作函数并设置一系列属性。

这导致在代码中多次重复引用对象的名称，增加了不必要的文本噪音。一个常见的解决方案是在创建对象时，在构造函数传入一个表，该表包含要覆盖设置的键和值的集合。这样做的缺点是该对象的构造函数必须支持这种初始化形式。

with块有助于简化编写这样的代码。在with块内，我们可以使用以.或\开头的特殊语句，这些语句代表我们正在使用的对象的操作。

例如，我们可以这样处理一个新创建的对象：

```moonscript
with Person!
  .name = "Oswald"
  \add_relative my_dad
  \save!
  print .name
```
<YueDisplay>
<pre>
with Person!
  .name = "Oswald"
  \add_relative my_dad
  \save!
  print .name
</pre>
</YueDisplay>

with语句也可以用作一个表达式，并返回它的代码块正在处理的对象。

```moonscript
file = with File "favorite_foods.txt"
  \set_encoding "utf8"
```
<YueDisplay>
<pre>
file = with File "favorite_foods.txt"
  \set_encoding "utf8"
</pre>
</YueDisplay>

或者…

```moonscript
create_person = (name,  relatives)->
  with Person!
    .name = name
    \add_relative relative for relative in *relatives

me = create_person "Leaf", [dad, mother, sister]
```
<YueDisplay>
<pre>
create_person = (name,  relatives)->
  with Person!
    .name = name
    \add_relative relative for relative in *relatives

me = create_person "Leaf", [dad, mother, sister]
</pre>
</YueDisplay>

在此用法中，with可以被视为K组合子（k-combinator）的一种特殊形式。

如果您想给表达式另外起一个名称的话，with语句中的表达式也可以是一个赋值语句。

```moonscript
with str = "你好"
  print "原始:", str
  print "大写:", \upper!
```
<YueDisplay>
<pre>
with str = "你好"
  print "原始:", str
  print "大写:", \upper!
</pre>
</YueDisplay>

在with语句中可以使用`[]`访问特殊键。

```moonscript
with tb
  [1] = 1
  print [2]
  with [abc]
    [3] = [2]\func!
    ["key-name"] = value
  [] = "abc" -- 追加到 "tb"
```
<YueDisplay>
<pre>
with tb
  [1] = 1
  print [2]
  with [abc]
    [3] = [2]\func!
    ["key-name"] = value
  [] = "abc" -- 追加到 "tb"
</pre>
</YueDisplay>

## do 语句

当用作语句时，do语句的作用就像在Lua中差不多。

```moonscript
do
  var = "hello"
  print var
print var -- 这里是nil
```
<YueDisplay>
<pre>
do
  var = "hello"
  print var
print var -- 这里是nil
</pre>
</YueDisplay>

月之脚本的 **do** 也可以用作表达式。允许您将多行代码的处理合并为一个表达式，并将do语句代码块的最后一个语句作为表达式返回的结果。

```moonscript
counter = do
  i = 0
  ->
    i += 1
    i

print counter!
print counter!
```
<YueDisplay>
<pre>
counter = do
  i = 0
  ->
    i += 1
    i

print counter!
print counter!
</pre>
</YueDisplay>

```moonscript
tbl = {
  key: do
    print "分配键值!"
    1234
}
```
<YueDisplay>
<pre>
tbl = {
  key: do
    print "分配键值!"
    1234
}
</pre>
</YueDisplay>

## 函数存根

在编程中，将对象的方法作为函数类型的值进行传递是一种常见做法，尤其是在将实例方法作为回调函数传递给其他函数的情形中。当目标函数需要将该对象作为其第一个参数时，我们需要找到一种方式将对象和函数绑定在一起，以便能够正确地调用该函数。

函数存根（stub）语法提供了一种便捷的方法来创建一个新的闭包函数，这个函数将对象和原函数绑定在一起。这样，当调用这个新创建的函数时，它会在正确的对象上下文中执行原有的函数。

这种语法类似于使用 \ 操作符调用实例方法的方式，区别在于，这里不需要在 \ 操作符后面附加参数列表。

```moonscript
my_object = {
  value: 1000
  write: => print "值为:", @value
}

run_callback = (func)->
  print "运行回调..."
  func!

-- 这样写不起作用：
-- 函数没有引用my_object
run_callback my_object.write

-- 函数存根语法
-- 让我们把对象捆绑到一个新函数中
run_callback my_object\write
```
<YueDisplay>
<pre>
my_object = {
  value: 1000
  write: => print "值为:", @value
}

run_callback = (func)->
  print "运行回调..."
  func!

-- 这样写不起作用：
-- 函数没有引用my_object
run_callback my_object.write

-- 函数存根语法
-- 让我们把对象捆绑到一个新函数中
run_callback my_object\write
</pre>
</YueDisplay>

## 使用 using 语句：防止破坏性赋值

Lua 的变量作用域是降低代码复杂度的重要工具。然而，随着代码量的增加，维护这些变量可能变得更加困难。比如，看看下面的代码片段：

```moonscript
i = 100

-- 许多代码行...

my_func = ->
  i = 10
  while i > 0
    print i
    i -= 1

my_func!

print i -- 将打印 0
```
<YueDisplay>
<pre>
i = 100

-- 许多代码行...

my_func = ->
  i = 10
  while i > 0
    print i
    i -= 1

my_func!

print i -- 将打印 0
</pre>
</YueDisplay>

在 `my_func` 中，我们不小心覆盖了变量 `i` 的值。虽然在这个例子中这个问题很明显，但在一个庞大的或者是由多人共同维护的代码库中，很难追踪每个变量的声明情况。

如果我们可以明确指出哪些变量是我们想在当前作用域内修改的，并且防止我们不小心更改了其他作用域中同名的变量，那将大有裨益。

`using` 语句就是为此而生。`using nil` 确保函数内部的赋值不会意外地影响到外部作用域的变量。我们只需将 `using` 子句放在函数的参数列表之后；若函数没有参数，则直接放在括号内即可。

```moonscript
i = 100

my_func = (using nil)->
  i = "hello" -- 这里创建了一个新的局部变量

my_func!
print i -- 打印 100，i 没有受到影响
```
<YueDisplay>
<pre>
i = 100

my_func = (using nil)->
  i = "hello" -- 这里创建了一个新的局部变量

my_func!
print i -- 打印 100，i 没有受到影响
</pre>
</YueDisplay>

using子句中可以填写多个用逗号分隔名称。指定可以访问和修改的外部变量的名称：

```moonscript
tmp = 1213
i, k = 100, 50

my_func = (add using k, i)->
  tmp = tmp + add -- 创建了一个新的局部tmp
  i += tmp
  k += tmp

my_func(22)
print i, k -- 这些已经被更新
```
<YueDisplay>
<pre>
tmp = 1213
i, k = 100, 50

my_func = (add using k, i)->
  tmp = tmp + add -- 创建了一个新的局部tmp
  i += tmp
  k += tmp

my_func(22)
print i, k -- 这些已经被更新
</pre>
</YueDisplay>

## MIT 许可证

版权 (c) 2024 李瑾

特此免费授予任何获得本软件副本和相关文档文件（下称“软件”）的人不受限制地处置该软件的权利，包括不受限制地使用、复制、修改、合并、发布、分发、转授许可和/或出售该软件副本，以及再授权被配发了本软件的人如上的权利，须在下列条件下：  
上述版权声明和本许可声明应包含在该软件的所有副本或实质成分中。  
本软件是“如此”提供的，没有任何形式的明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和不侵权的保证。在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论这些追责来自合同、侵权或其它行为中，还是产生于、源于或有关于本软件以及本软件的使用或其它处置。

<CompilerModal />
