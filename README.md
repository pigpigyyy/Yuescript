# MoonPlus

![CI](https://github.com/pigpigyyy/MoonPlus/workflows/build-test/badge.svg)  
MoonPlus is a compiler with features from [Moonscript language](https://github.com/leafo/moonscript) 0.5.0 and implementing new features to make Moonscript more up to date. 

Since original Moonscript has been used to write web framework [lapis](https://github.com/leafo/lapis) and run a few business web sites like [itch.io](https://itch.io) and [streak.club](https://streak.club) with some large code bases. The original language is getting too hard to adopt new language features for those may break the stablility for existing applications.

So MoonPlus is a new code base for pushing the language to go forward and may be a playground to try introducing new language syntax or programing paradigms to make Moonscript language more expressive and productive.

## Features

* No other dependencies needed except modified **parserlib** library from Achilleas Margaritis with some performance enhancement. **lpeg** library is no longer needed.
* Written in C++17.
* Support full Moonscript language features, generate the same Lua codes with original compiler.
* Reserve line numbers from Moonscript sources in compiled Lua codes to help with debugging.

## Installation

Install [luarocks](https://luarocks.org), a package manager for Lua modules. Then install Lua module with
```sh
> luarocks install moonplus
```

## Usage

```Lua
require("moonp")("main") -- require `main.moon`

local moonp = require("moonp")
print(moonp.to_lua[[
f = ->
  print "hello world"
f!
]])
```

## Changes

The original Moonscript language 0.5.0 support can be found in the `0.5.0` branch. Moonscript with new features is in the master branch. Here are the new features introduced in MoonPlus.

* Multi-line comment.
* Usage for symbol `\` to escape new line. Will compile codes:
```Moonscript
str = --[[
   This is a multi line comment.
   It's OK.
]] strA \ -- comment 1
   .. strB \ -- comment 2
   .. strC

func --[[ip]] "192.168.126.110", --[[port]] 3000
```
&emsp;&emsp;to:
```Lua
local str = strA .. strB .. strC
func("192.168.126.110", 3000)
```

* Back call features with new operator and syntax. For example:
```Moonscript
{1,2,3} \
  |> map((x)-> x * 2) \
  |> filter((x)-> x > 4) \
  |> reduce(0, (a,b)-> a + b) \
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

* Feature of `Reusing variable` which helps generate reduced Lua codes. For example, MoonPlus will generate codes from:
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

* Expression list appears at the middle of code block is not allowed. For codes like:
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
 local _ = "abc", 123 -- report error in MoonPlus
 return x + 1
end
```

This feature may lead to some silenced errors. For example:
```Moonscript
-- expected codes
tree\addChild with Node!
  \addChild subNode
  
-- in original Moonscript, codes will still run after adding a break
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
local _ -- report error in MoonPlus instead of creating
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

## Standalone Compiler Usage

Test compiler with `make test`.
Run `moonp` complier in project folder with:

```shell
make
./moonp -h
```

## License
MIT
