# MoonPlus

![CI](https://github.com/pigpigyyy/MoonPlus/workflows/build-test/badge.svg)  
MoonPlus is a compiler for Moonscript language 0.5.0 written in C++ which could be 2~4 times faster than the original Moonscript compiler written in Moonscript.

## Features

* No other dependencies needed except modified **parserlib** library from Achilleas Margaritis with some performance enhancement. **lpeg** library is no longer needed.
* Written in C++17.
* Support full Moonscript language features, generate the same Lua codes with original compiler.
* Reserve line numbers from source Moonscript codes in compiled Lua codes to help with debugging.

## Minor Changes

* Can do slash call with Lua keyword. Generate codes from:
```Moonscript
c.repeat.if\then("xyz")\else res
```
&emsp;&emsp;to:
```Moonscript
local _call_3 = c["repeat"]["if"]
local _call_4 = _call_3["then"](_call_3, "xyz")
_call_4["else"](_call_4, res)
```

* Add a compiler flag `reuseVariable` which can help generate reduced Lua codes. For example, when set `reuseVariable` to `true`, MoonPlus will generate codes from:
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
```lua
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
