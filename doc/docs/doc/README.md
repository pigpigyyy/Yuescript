---
sidebar: auto
---

<CompilerModal />

# Yuescript
<img src="/image/yuescript.svg" width="300" height="300" alt="logo"/>

## Introduction

Yuescript is a dynamic language that compiles to Lua. The codes written in Yuescript are expressive and extremely concise. And it is suitable for writing some changing application logic with more maintainable codes and runs in a Lua embeded environment such as games or website servers.

Yue (月) is the name of moon in Chinese and it's pronounced as [jyɛ].

### An Overview of Yuescript
```moonscript
-- import syntax
import "yue" as :p, :to_lua

-- implicit objects
inventory =
  equipment:
    * "sword"
    * "shield"
  items:
    * name: "potion"
      count: 10
    * name: "bread"
      count: 3

-- backcall
{1, 2, 3}
  |> map (x)-> x * 2
  |> filter (x)-> x > 4
  |> reduce 0, (a, b)-> a + b
  |> print

-- metatable manipulation
apple =
  size: 15
  index#: {color: 0x00ffff}
p apple.color, apple.#?, apple.index#

-- js-like export syntax
export yuescript = "月之脚本"
```
<YueDisplay>
<pre>
-- import syntax
import "yue" as :p, :to_lua

-- implicit objects
inventory =
  equipment:
    * "sword"
    * "shield"
  items:
    * name: "potion"
      count: 10
    * name: "bread"
      count: 3

-- backcall
{1, 2, 3}
  |> map (x)-> x * 2
  |> filter (x)-> x > 4
  |> reduce 0, (a, b)-> a + b
  |> print

-- metatable manipulation
apple =
  size: 15
  index#: {color: 0x00ffff}
p apple.color, apple.#?, apple.index#

-- js-like export syntax
export yuescript = "月之脚本"
</pre>
</YueDisplay>

## Installation

* **Lua Module**

&emsp;&emsp;Install [luarocks](https://luarocks.org), a package manager for Lua modules. Then install it as a Lua module and executable with:

```
> luarocks install yuescript
```

&emsp;&emsp;Or you can build `yue.so` file with:

```
> make shared LUAI=/usr/local/include/lua LUAL=/usr/local/lib/lua
```

&emsp;&emsp;Then get the binary file from path **bin/shared/yue.so**.

* **Binary Tool**

&emsp;&emsp;Clone this repo, then build and install executable with:
```
> make install
```

&emsp;&emsp;Build Yuescript tool without macro feature:
```
> make install NO_MACRO=true
```

&emsp;&emsp;Build Yuescript tool without built-in Lua binary:
```
> make install NO_LUA=true
```

## Usage

&emsp;&emsp;Require the Yuescript module in Lua:
```Lua
-- require `main.yue` in Lua
require("yue")("main")

-- use the Yuescript compiler in Lua
local yue = require("yue")
local codes, err, globals = yue.to_lua([[
f = ->
  print "hello world"
f!
]],{
  implicit_return_root = true,
  reserve_line_number = true,
  lint_global = true
})
```
&emsp;&emsp;Use Yuescript tool with:
```
> yue -h
Usage: yue [options|files|directories] ...

   -h       Print this message
   -e str   Execute a file or raw codes
   -t path  Specify where to place compiled files
   -o file  Write output to file
   -s       Use spaces in generated codes instead of tabs
   -m       Generate minified codes
   -p       Write output to standard out
   -b       Dump compile time (doesn't write output)
   -l       Write line numbers from source codes
   -v       Print version
   --       Read from standard in, print to standard out
            (Must be first and only argument)

   Execute without options to enter REPL, type symbol '$'
   in a single line to start/stop multi-line mode
```
&emsp;&emsp;Use cases:  
&emsp;&emsp;Recursively compile every Yuescript file with extension **.yue** under current path:  **yue .**  
&emsp;&emsp;Compile and save results to a target path:  **yue -t /target/path/ .**  
&emsp;&emsp;Compile and reserve debug info:  **yue -l .**  
&emsp;&emsp;Compile and generate minified codes:  **yue -m .**  
&emsp;&emsp;Execute raw codes:  **yue -e 'print 123'**  
&emsp;&emsp;Execute a Yuescript file:  **yue -e main.yue**

## Macro

### Common Usage
Macro function is used for evaluating a string in the compile time and insert the generated codes into final compilation.

```moonscript
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

-- the passed expressions are treated as strings
macro and = (...)-> "#{ table.concat {...}, ' and ' }"
if $and f1!, f2!, f3!
  print "OK"
```
<YueDisplay>
<pre>
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

-- the passed expressions are treated as strings
macro and = (...)-> "#{ table.concat {...}, ' and ' }"
if $and f1!, f2!, f3!
  print "OK"
</pre>
</YueDisplay>

### Insert Raw  Codes

A macro function can either return a Yuescript string or a config table containing Lua codes.
```moonscript
macro yueFunc = (var)-> "local #{var} = ->"
$yueFunc funcA
funcA = -> "assign the Yue defined variable"

-- take care and let Yuescript know the
-- local variables you declared in Lua code
macro luaFunc = (var)-> {
  codes: "local function #{var}() end"
  type: "lua"
  locals: {var}
}
$luaFunc funcB
funcB = -> "assign the Lua defined variable"

macro lua = (code)-> {
  :code
  type: "lua"
}

-- the raw string leading and ending symbols are auto trimed
$lua[==[
-- raw Lua codes insertion
if cond then
  print("output")
end
]==]
```
<YueDisplay>
<pre>
macro yueFunc = (var)-> "local #{var} = ->"
$yueFunc funcA
funcA = -> "assign the Yue defined variable"

-- take care and let Yuescript know the
-- local variables you declared in Lua codes
macro luaFunc = (var)-> {
  codes: "local function #{var}() end"
  type: "lua"
  locals: {var}
}
$luaFunc funcB
funcB = -> "assign the Lua defined variable"

macro lua = (code)-> {
  :code
  type: "lua"
}

-- the raw string leading and ending symbols are auto trimed
$lua[==[
-- raw Lua codes insertion
if cond then
  print("output")
end
]==]
</pre>
</YueDisplay>

### Export Macro

Macro functions can be exported from a module and get imported in another module. It is recommanded to export macro functions in a single file to speed up compilation.
```moonscript
-- file: utils.yue
export macro map = (items, action)-> "[#{action} for _ in *#{items}]"
export macro filter = (items, action)-> "[_ for _ in *#{items} when #{action}]"
export macro foreach = (items, action)-> "for _ in *#{items}
  #{action}"

-- file main.yue
import "utils" as {
  $, -- symbol to import all macros
  $foreach: $each -- rename macro $foreach to $each
}
{1, 2, 3} |> $map(_ * 2) |> $filter(_ > 4) |> $each print _
```
<YueDisplay>
<pre>
-- file: utils.yue
export macro map = (items, action)-> "[#{action} for _ in *#{items}]"
export macro filter = (items, action)-> "[_ for _ in *#{items} when #{action}]"
export macro foreach = (items, action)-> "for _ in *#{items}
  #{action}"

-- file main.yue
-- import function is not available in browser, try it in a real environment
--[[
import "utils" as {
  $, -- symbol to import all macros
  $foreach: $each -- rename macro $foreach to $each
}
{1, 2, 3} |> $map(_ * 2) |> $filter(_ > 4) |> $each print _
]]
</pre>
</YueDisplay>

## Special Operator

### Metatable

The **#** operator can be used as a shortcut for metatable manipulation.

* **Metatable Creation**  
Create normal table with  key **#** or metamethod key that ends with **#**.

```moonscript
mt = {}
add = (right)=> #: mt, value: @value + right.value
mt.__add = add

a = #: mt, value: 1
 -- set field with variable of the same name
b = :add#, value: 2
c = add#: mt.__add, value: 3

d = a + b + c
print d.value

close _ = close#: -> print "out of scope"
```
<YueDisplay>
<pre>
mt = {}
add = (right)=> #: mt, value: @value + right.value
mt.__add = add

a = #: mt, value: 1
 -- set field with variable of the same name
b = :add#, value: 2
c = add#: mt.__add, value: 3

d = a + b + c
print d.value

close _ = close#: -> print "out of scope"
</pre>
</YueDisplay>

* **Metatable Accessing**  
Accessing metatable with key **#** or metamethod key that ends with **#**.

```moonscript
-- create with metatable containing field "value"
tb = ["value"]#: 123
tb.index# = tb.#
print tb.value

tb.# = __index: {item: "hello"}
print tb.item
```
<YueDisplay>
<pre>
-- create with metatable containing field "value"
tb = ["value"]#: 123
tb.index# = tb.#
print tb.value

tb.# = __index: {item: "hello"}
print tb.item
</pre>
</YueDisplay>

* **Metatable Destructure**  
Destruct metatable with metamethod key that ends with **#**.

```moonscript
{item, :new, :close#, index#: getter} = tb
print item, new, close, getter
```
<YueDisplay>
<pre>
{item, :new, :close#, index#: getter} = tb
print item, new, close, getter
</pre>
</YueDisplay>

### Existence

The **?** operator can be used in a variety of contexts to check for existence.

```moonscript
func?!
print abc?["hello world"]?.xyz

x = tab?.value
len = utf8?.len or string?.len or (o)-> #o

if print and x?
  print x

with? io.open "test.txt", "w"
  \write "hello"
  \close!
```
<YueDisplay>
<pre>
func?!
print abc?["hello world"]?.xyz

x = tab?.value
len = utf8?.len or string?.len or (o)-> #o

if print and x?
  print x

with? io.open "test.txt", "w"
  \write "hello"
  \close!
</pre>
</YueDisplay>

### Piping

Instead of a series of nested function calls, you can pipe values with operator **|>**.
```moonscript
"hello" |> print
1 |> print 2 -- insert pipe item as the first argument
2 |> print 1, _, 3 -- pipe with a placeholder

-- pipe expression in multiline
readFile "example.txt"
  |> extract language, {}
  |> parse language
  |> emit
  |> render
  |> print
```
<YueDisplay>
<pre>
"hello" |> print
1 |> print 2 -- insert pipe item as the first argument
2 |> print 1, _, 3 -- pipe with a placeholder

-- pipe expression in multiline
readFile "example.txt"
  |> extract language, {}
  |> parse language
  |> emit
  |> render
  |> print
</pre>
</YueDisplay>

## Module

### Import

The import statement is a syntax sugar for requiring a module or help extracting items from an imported module.

```moonscript
-- used as table destructure
do
  import C, Ct, Cmt from require "lpeg"
  import insert, concat from table

-- shortcut for requring a module
do
  import 'module'
  import 'module_x'
  import "d-a-s-h-e-s"
  import "module.part"

-- requring module with aliasing or table destruction
do
  import "player" as PlayerModule
  import "lpeg" as :C, :Ct, :Cmt
  import "export" as {one, two, Something:{umm:{ch}}}
```
<YueDisplay>
<pre>
-- used as table destruction
do
  import C, Ct, Cmt from require "lpeg"
  import insert, concat from table

-- shortcut for requring a module
do
  import 'module'
  import 'module_x'
  import "d-a-s-h-e-s"
  import "module.part"

-- requring module with aliasing or table destruction
do
  import "player" as PlayerModule
  import "lpeg" as :C, :Ct, :Cmt
  import "export" as {one, two, Something:{umm:{ch}}}
</pre>
</YueDisplay>

### Export

The export statement offers a concise way to define modules.

* **Named Export**  
Named export will define a local variable as well as adding a field in the exported table.

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

* **Unnamed Export**  
Unnamed export will add the target item into the array part of the exported table.

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

* **Default Export**  
Using the **default** keyword in export statement to replace the exported table with any thing.

```moonscript
export default ->
  print "hello"
  123
```
<YueDisplay>
<pre>
export default ->
  print "hello"
  123
</pre>
</YueDisplay>