# MoonPlus

![CI](https://github.com/pigpigyyy/MoonPlus/workflows/build-test/badge.svg)  
MoonPlus is a compiler with features from Moonscript language 0.5.0 and could be 2~4 times faster than the original Moonscript compiler. 

## Features

* No other dependencies needed except modified **parserlib** library from Achilleas Margaritis with some performance enhancement. **lpeg** library is no longer needed.
* Written in C++17.
* Support full Moonscript language features, generate the same Lua codes with original compiler.
* Reserve line numbers from source Moonscript codes in compiled Lua codes to help with debugging.

## Changes

* Add multi-line comment support.
* Add usage for symbol `\` to escape new line. Will compile codes:
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

* Add back call features support with new operator and syntax. For example:
```Moonscript
{1,2,3} \
  |> map((x)-> x * 2) \
  |> filter((x)-> x > 4) \
  |> reduce(0, (a,b)-> a + b) \
  |> print

do
  (data) <- http.get "ajaxtest"
  body[".result"]\html data
  (processed) <- http.get "ajaxprocess", data
  body[".result"]\append processed
  print "done"
```
&emsp;&emsp;compiles to:
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
    return http.get("ajaxprocess", data, function(processed)
      body[".result"]:append(processed)
      return print("done")
    end)
  end)
end
```

* Add existential operator support. Generate codes from:
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

* Add more features for `import` keyword. Will compile codes from:
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

* Add feature of `reusing variable` which helps generate reduced Lua codes. For example, MoonPlus will generate codes from:
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

## Standalone Compiler Usage

Test compiler with `make test`.
Run `moonc` complier in project folder with:
```shell
make
./moonc -h
```

## License
MIT
