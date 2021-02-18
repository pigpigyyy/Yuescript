# Changelog

The implementation for original Moonscript language 0.5.0 can be found in the `0.5.0` branch of Yuescript. The Moonscript with fixes and new features is in the main branch of Yuescript. Here are the changelogs for each Yuescript version.


## v0.6.6

### Fixed Issues

* Simplify macro syntax. Macro function can either return a code string or a config table.

```moonscript
macro local = (var)-> "global *"

$local x
x = 1
y = 2
z = 3

macro local = (var)->
  {
    codes: "local #{var}"
    type: "lua"
    locals: {var}
  }

$local y
y = 1
```

* Change Yuescript file extension to '.yue' because some of the Moonscript syntax are no longer supported and some codes written in Yuescript syntax won't be accepted by Moonscript compiler.
* 

## v0.4.16

### Fixed Issues

* Remove support for escape new line symbol, binary operator expressions can now be written multiline without escape new line symbol.

* Fix an issue when extending class without name.

* Fix an issue when using return with export statement.

* Fix issues when declaring table key with Lua multiline string and indexing expressions with Lua multiline string.

* Make simple table and table block appear in the end of function arguments merged into one table.

  ```Moonscript
  -- function arguments end with simple table followed by table block
  another hello, one,
    two, three, four, yeah: man
    okay: yeah
    fine: alright
  ```

  ```Lua
  -- compiled in original Moonscript compiler
  -- get two tables as arguments
  another(hello, one, two, three, four, {
    yeah = man
  }, {
    okay = yeah,
    fine = alright
  })
  -- compiled in fixed Yuescript compiler
  -- get one table as argument
  another(hello, one, two, three, four, {
    yeah = man,
    okay = yeah,
    fine = alright
  })
  ```

  

### Added Features

* Add implicit objects support for table block syntax.

  ```Moonscript
  inventory =
    equipment:
      * "sword"
      * "shield"
    items:
      * name: "potion"
        count: 10
      * name: "bread"
        count: 3
  ```

  Compiles to:

  ```Lua
  local inventory = {
    equipment = {
      "sword",
      "shield"
    },
    items = {
      {
        name = "potion",
        count = 10
      },
      {
        name = "bread",
        count = 3
      }
    }
  }
  ```

  

* Add support for local variable declared with attribute 'close' and 'const' for Lua 5.4.

  ```moonscript
  close a = setmetatable {}, __close: =>
  const b = if var then 123 else 'abc'
  ```

  Compiles to:

  ```Lua
  local _var_0 = setmetatable({ }, {
    __close = function(self)
    end
  })
  local a <close> = _var_0
  local _var_1
  if var then
    _var_1 = 123
  else
    _var_1 = 'abc'
  end
  local b <const> = _var_1
  ```

  

## v0.4.0

### Fixed Issues

* Fix issues of unary and binary operator "~".
* Fix Moonscript issue 416: ambiguous Lua output in some cases.
* Fix errors when explicitly declaring global or local variable initialized with table block.
* Fix macro type mismatch issue.
* Fix line break issue in macro, disable macro declaration outside the root scope.
* Fix existential operator issue when used in operator-value list.
* Fix assignment with backcall expr not well handled issue.
* Fix destructure case assigning one table variable to an operator-value expression.
* Fix simple chain value with Lua keyword colon chain item.

### Added Features

* Change operator precedence to
  * [1] ^
  * [2] unary operators (not, #, -, ~)
  * [3] |>
  * [4] *, /, //, %, ...
  
* Add existential operator support for `with` statement.

  ```Moonscript
  with? io.open "test.txt", "w"
    \write "hello"
    \close!
  ```

  Compiles to:

  ```Lua
  local _with_0 = io.open("test.txt", "w")
  if _with_0 ~= nil then
    _with_0:write("hello")
    _with_0:close()
  end
  ```

* Add repeat until statement support.

* Allow implicitly returning block macro.

* Add support for macro system expanding to Lua codes directly.

* Add goto statement support.

* Add variadic arguments declaration check.

* `yue` now supports recursively traversing any directory and compiling any moon file in the path.

* `yue` now supports REPL functions for Moonscript.

* Add `useSpaceOverTab` function to `yue`.

* Add Lua codes minify function to `yue`.



### Changed Hehaviors

* Remove else clause support from if-else line decorator syntax which makes users confused.



## v0.3.0

### Added Features

* Add macro functions.
```Moonscript
-- file 'macro.mp'
export macro config = (debugging = true)->
  global debugMode = debugging == "true"
  ""

export macro asserts = (cond)->
  debugMode and "assert #{cond}" or ""

export macro assert = (cond)->
  debugMode and "assert #{cond}" or "#{cond}"

$config!

-- file 'main.mp'
import 'macro' as {:$config, :$assert, :$asserts}

macro and = (...)-> "#{ table.concat {...}, ' and ' }"

$asserts item ~= nil
$config false
value = $assert item

if $and f1!, f2!, f3!
  print "OK"
```
 Compiles to:
```Lua
-- file 'macro.mp'
local _module_0 = { }
return _module_0

-- file 'main.mp'
assert(item ~= nil)
local value = item
if (f1() and f2() and f3()) then
  print("OK")
end
```



## v0.2.0

### Fixed Issues

* Fix some cases when using backcall with assignment.
* Fix a case that complier crashes with empty code body.
* Force forward declaration of variable for assigning local function.

From original Moonscript compiler:

* [#390](https://github.com/leafo/moonscript/issues/390) Many lists are not allowed to be multiline

  

### Added Features

* Allow value lists in for and local statement to be multiline.
* `yue` now compiles source files in multiple threads to speed up compilation.
* Add placeholder support for backcall operator.
* Add placeholder support for backcall statement.
* Add fat arrow support for backcall statement.
* Add option to compile Yuescript as a Lua C lib. Got Yuescript released to `Luarocks`.
* Move old `export` statement functions to `global` statement to match the `local` statement.
* Change `export` statement behavier to support module management.  Moon codes with `export` statement can not explicit return values in root scope. And codes with `export default` can export only one value as the module content. Use cases:
```Moonscript
-- file 'Config.mp'
export default {flag:1, value:"x"}

-- file 'Utils.mp'
export map = (items, func)-> [func item for item in *items]
export filter = (items, func)-> [item for item in *items when func item]

-- file 'main.mp'
import 'Config' as {:flag, :value}
import 'Utils' as {:map, :filter}
```
Compiles to:
```Lua
-- file 'Config.mp'
local _module_0 = nil
_module_0 = {
  flag = 1,
  value = "x"
}
return _module_0

-- file 'Utils.mp'
local _module_0 = { }
local map
map = function(items, func)
  local _accum_0 = { }
  local _len_0 = 1
  for _index_0 = 1, #items do
    local item = items[_index_0]
    _accum_0[_len_0] = func(item)
    _len_0 = _len_0 + 1
  end
  return _accum_0
end
_module_0["map"] = map
local filter
filter = function(items, func)
  local _accum_0 = { }
  local _len_0 = 1
  for _index_0 = 1, #items do
    local item = items[_index_0]
    if func(item) then
      _accum_0[_len_0] = item
      _len_0 = _len_0 + 1
    end
  end
  return _accum_0
end
_module_0["filter"] = filter
return _module_0

-- file 'main.mp'
local flag, value
do
  local _obj_0 = require('Config')
  flag, value = _obj_0.flag, _obj_0.value
end
local map, filter
do
  local _obj_0 = require('Utils')
  map, filter = _obj_0.map, _obj_0.filter
end
```



## v0.1.0

### Fixed Issues

Fix issues in original Moonscript compiler:
* [#122](https://github.com/leafo/moonscript/issues/122) use of ? symbol
* [#155](https://github.com/leafo/moonscript/issues/151) wish moon could support: function?()
* [#156](https://github.com/leafo/moonscript/issues/156) About the 'import' statement
* [#375](https://github.com/leafo/moonscript/issues/375) assignment in line decorators
* [#384](https://github.com/leafo/moonscript/issues/384) The @ and @@ accessors do not escape Lua keywords



### Added Features

* Multi-line comment support.

* Back call features with new operator and syntax. For example:
```Moonscript
{1,2,3}
  |> map((x)-> x * 2)
  |> filter((x)-> x > 4)
  |> reduce(0, (a,b)-> a + b)
  |> print

do
  (data) <- http.get "ajaxtest"
  body[".result"]\html data
  (processed) <- http.post "ajaxprocess", data
  body[".result"]\append processed
  print "done"
```
&emsp;&emsp;will be compiled to:
```Lua
print(reduce(filter(map({
  1,
  2,
  3
}, function(x)
  return x * 2
end), function(x)
  return x > 4
end), 0, function(a, b)
  return a + b
end))
do
  http.get("ajaxtest", function(data)
    body[".result"]:html(data)
    return http.post("ajaxprocess", data, function(processed)
      body[".result"]:append(processed)
      return print("done")
    end)
  end)
end
```

* Existential operator support. Generate codes from:
```Moonscript
func?!

x = tab?.value

print abc?["hello world"]?.xyz

if print and x?
  print x
```
&emsp;&emsp;to:
```Lua
if func ~= nil then
  func()
end
local x
if tab ~= nil then
  x = tab.value
end
print((function()
  if abc ~= nil then
    local _obj_0 = abc["hello world"]
    if _obj_0 ~= nil then
      return _obj_0.xyz
    end
    return nil
  end
  return nil
end)())
if print and (x ~= nil) then
  print(x)
end
```

* More usages for `import` keyword. Will compile codes from:
```Moonscript
import 'module'
import "module.part"
import "d-a-s-h-e-s"
import "player" as Player
import "lpeg" as {:C, :Ct, :Cmt}
```
&emsp;&emsp;to:
```Lua
local module = require('module')
local part = require("module.part")
local d_a_s_h_e_s = require("d-a-s-h-e-s")
local Player = require("player")
local C, Ct, Cmt
do
  local _obj_0 = require("lpeg")
  C, Ct, Cmt = _obj_0.C, _obj_0.Ct, _obj_0.Cmt
end
```

* Can do slash call with Lua keywords. Generate codes from:
```Moonscript
c.repeat.if\then("xyz")\else res
```
&emsp;&emsp;to:
```Lua
local _call_3 = c["repeat"]["if"]
local _call_4 = _call_3["then"](_call_3, "xyz")
_call_4["else"](_call_4, res)
```



### Changed Hehaviors

* Left part of update statement will now accept only one assignable expression.
* Expression list appears in the middle of code block is not allowed. For codes like:

```Moonscript
-- Moonscript 0.5.0
f = (x)->
  "abc",123 -- valid meaningless codes
  x + 1
```

in original Moonscript compiles to:

```Lua
local f
f = function(x)
 local _ = "abc", 123 -- report error in Yuescript
 return x + 1
end
```

This feature may lead to some silenced errors. For example:

```Moonscript
-- expected codes
tree\addChild with Node!
  \addChild subNode
  
-- in original Moonscript, codes will still run
-- after adding a break by mistake
tree\addChild
with Node!
   \addChild subNode
```

the original Moonscript will compile these codes to:

```Lua
-- expected codes
tree:addChild((function()
  do
    local _with_0 = Node()
    _with_0:addChild(subNode)
    return _with_0
  end
end)())

-- codes added with a break will still run
local _ -- report error in Yuescript instead of creating
do      -- an anonymous function to bind the object method
  local _base_0 = tree
  local _fn_0 = _base_0.addChild
  _ = function(...)
    return _fn_0(_base_0, ...)
  end
end
do
  local _with_0 = Node()
  _with_0:addChild(subNode)
end
```

* Reusing variables which helps generate reduced Lua codes. 
  For example, Yuescript will generate codes from:

```Moonscript
with leaf
  .world 1,2,3

with leaf
  g = .what.is.this
  print g

for x in *something
  print x
```

&emsp;&emsp;to:

```Lua
leaf.world(1, 2, 3)
do
  local g = leaf.what.is.this
  print(g)
end
for _index_0 = 1, #something do
  local x = something[_index_0]
  print(x)
end
```

&emsp;&emsp;instead of:

```lua
do
  local _with_0 = leaf
  _with_0.world(1, 2, 3)
end
do
  local _with_0 = leaf
  local g = _with_0.what.is.this
end
local _list_0 = something
for _index_0 = 1, #_list_0 do
  local x = _list_0[_index_0]
  print(x)
end
```



## v0.0.1

Implemented full features from Moonscript 0.5.0.
