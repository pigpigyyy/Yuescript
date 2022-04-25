# Changelog

The implementation for the original Moonscript language 0.5.0 can be found in the `0.5.0` branch of Yuescript. The Moonscript with fixes and new features is in the main branch of Yuescript. Here are the changelogs for each Yuescript version.

## v0.10.15

### Fixed Issues

* Fix ambiguous syntax caused by argument table block.
* Fix an issue using import as table destructuring with meta methods.
* Fix a case combining the use of existential operator and metatable operator.
* Raise error when use existential operator in the left part of an assignment.
* Fix the way update assignment is implemented to reduce unnecessary table indexing.
* Fix more cases that global variables are not being cached.
* Fix Yuescript loader insertion order.
* Fix Yuescript not found error messages.

### Added Features

* Builtin macros `$FILE`, `$LINE` to get the source file names and code line numbers.
* Table appending operator.
   ```moonscript
   tb = {}
   tb[] = 1
   ```
   Compiles to:
   ```lua
   local tb = { }
   tb[#tb + 1] = 1
   ```
* Class mixing function by keyword `using`.
   ```moonscript
   class A using B
   -- If B is a Yuescript class object (which has a field named __base),
   -- only fields that are not meta method will be added to class object A
   -- If B is a plain table, all the fields will be added to class object A
   -- so that you can change the behavior of Yuescript class' meta method __index.
   ```
* Try catch syntax.
   ```moonscript
   try
     func 1, 2, 3
   catch err
     print yue.traceback err
   
   success, result = try func 1, 2, 3
   ```
   Compiles to:
   ```lua
   xpcall(function()
     return func(1, 2, 3)
   end, function(err)
     return print(yue.traceback(err))
   end)
   local success, result = pcall(func, 1, 2, 3)
   ```
* Add nil coalescing operator.
   ```moonscript
   local a, b, c
   a = b ?? c
   a ??= false
   ```
   Compiles to:
   ```lua
   local a, b, c
   if b ~= nil then
     a = b
   else
     a = c
   end
   if a == nil then
     a = false
   end
   ```

* Add a global variable "arg" for Yuescipt tool CLI execution.
* Add placeholder support for list destructuring.
   ```moonscript
   {_, b, _, d} = tb
   ```
   Compiles to:
   ```lua
   local b, d
   do
     local _obj_0 = tb
     b, d = _obj_0[2], _obj_0[4]
   end
   ```
* Add table spread syntax.
   ```moonscript
   copy = {...other}
   ```
   Compiles to:
   ```lua
   local copy
   do
     local _tab_0 = { }
     local _idx_0 = 1
     for _key_0, _value_0 in pairs(other) do
       if _idx_0 == _key_0 then
         _tab_0[#_tab_0 + 1] = _value_0
         _idx_0 = _idx_0 + 1
       else
         _tab_0[_key_0] = _value_0
       end
     end
     copy = _tab_0
   end
   ```

## v0.8.5

### Fixed Issues

* Simplify and extend import syntax.
* Report an error when destructuring nil item.

### Added Features

* Nil coalescing operator.
   ```moonscript
   local a, b, c, d
   a = b ?? c ?? d
   func a ?? {}
   
   a ??= false
   ```
   Compiles to:
   ```lua
   local a, b, c, d
   if b ~= nil then
     a = b
   else
     if c ~= nil then
       a = c
     else
       a = d
     end
   end
   func((function()
     if a ~= nil then
       return a
     else
       return { }
     end
   end)())
   if a == nil then
     a = false
   end
   ```

* New metatable syntax.
  ```moonscript
  #: mt = tb
  mt = tb.#
  a = #: mt, value: 1
  b = :add#, value: 2
  close _ = close#: -> print "out of scope"
  ```
  compiles to:
  ```lua
  local mt = getmetatable(tb)
  mt = getmetatable(tb)
  local a = setmetatable({
    value = 1
  }, mt)
  local b = setmetatable({
    value = 2
  }, {
    __add = add
  })
  local _ <close> = setmetatable({ }, {
    __close = function()
      return print("out of scope")
    end
  })
  ```

* Aliasing symbol :: with \ for writing colon chain items.
  ```moonscript
  tb\func1()::func2()
  ```
  compiles to:
  ```lua
  tb:func1():func2()
  ```

* Add until statement.
  ```moonscript
  a = 3
  until a == 0
    a -= 1
  ```
  compiles to:
  ```lua
  local a = 3
  while not (a == 0) do
    a = a - 1
  end
  ```

* Add existential check syntax for function argument as a shortcut feature to assign self field.
  ```moonscript
  tb\func1()::func2()
  ```
  compiles to:
  ```lua
  tb:func1():func2()
  ```

* Add default value support for destructure syntax.
  ```moonscript
  {:name = "nameless", :job = "jobless"} = person
  ```
  compiles to:
  ```lua
  local name, job
  do
    local _obj_0 = person
    name, job = _obj_0.name, _obj_0.job
  end
  if name == nil then
    name = "nameless"
  end
  if job == nil then
    job = "jobless"
  end
  ```

## v0.6.7

### Fixed Issues

* Simplify macro syntax. A macro function can either return a Yuescript string or a config table containing Lua codes.

  ```moonscript
  macro localFunc = (var)-> "local #{var} = ->"
  $localFunc f1
  f1 = -> "another function"

  -- or

  macro localFunc = (var)->
    {
      codes: "local function #{var}() end"
      type: "lua"
      locals: {var}
    }
  $localFunc f2
  f2 = -> "another function"
  ```
  Compiles to:
  ```Lua
  local f1
  f1 = function() end
  f1 = function()
    return "another function"
  end
  local function f2() end
  f2 = function()
    return "another function"
  end
  ```

* Change the Yuescript file extension to '.yue' because some of the Moonscript syntax is no longer supported and some codes written in Yuescript syntax won't be accepted by the Moonscript compiler.
* Disable the use of local and global statements with wildcard operators.
* Change the backcall operator syntax, extra parentheses for multiline chains are no longer needed.
  ```moonscript
  readFile "example.txt"
    |> extract language, {}
    |> parse language
    |> emit
    |> render
    |> print
  ```
  Compiles to:
  ```Lua
  return print(render(emit(parse(extract(readFile("example.txt"), language, { }), language))))
  ```


### Added Features

* Supporting multiline chaining function call syntax.
  ```Moonscript
  result = origin
    .transform.root
    .gameObject
    \Parents!
    \Descendants!
    \SelectEnable!
    \SelectVisible!
    \TagEqual "fx"
    \Where (x)->
      if x\IsTargeted!
        return false
      x.name\EndsWith "(Clone)"
    \Destroy!
  
  origin.transform.root.gameObject
    \Parents!\Descendants!
    \SelectEnable!
    \SelectVisible!
    \TagEqual "fx"
    \Where (x)-> x.name\EndsWith "(Clone)"
    \Destroy!
  ```
  Compiles to:
  ```Lua
  local result = origin.transform.root.gameObject:Parents():Descendants():SelectEnable():SelectVisible():TagEqual("fx"):Where(function(x)
    if x:IsTargeted() then
      return false
    end
    return x.name:EndsWith("(Clone)")
  end):Destroy()
  return origin.transform.root.gameObject:Parents():Descendants():SelectEnable():  SelectVisible():TagEqual("fx"):Where(function(x)
    return x.name:EndsWith("(Clone)")
  end):Destroy()
  ```



## v0.4.16

### Fixed Issues

* Remove support for escape newline symbol, binary operator expressions can now be written multiline without escape newline symbol.

* Fix an issue when extending class without a name.

* Fix an issue when using return with an export statement.

* Fix issues when declaring table key with Lua multiline string and indexing expressions with Lua multiline string.

* Make simple table and table block appear at the end of function arguments merged into one table.

  ```Moonscript
  -- function arguments end with a simple table followed by table block
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
   local a <close> = setmetatable({ }, {
     __close = function(self) end
  })
  local b <const> = (function()
    if var then
      return 123
    else
      return 'abc'
    end
  end)()
   ```

  

## v0.4.0

### Fixed Issues

* Fix issues of the unary and binary operator "~".
* Fix Moonscript issue 416: ambiguous Lua output in some cases.
* Fix errors when explicitly declaring global or local variable initialized with table block.
* Fix macro type mismatch issue.
* Fix line break issue in macro, disable macro declaration outside the root scope.
* Fix existential operator issue when used in an operator-valued list.
* Fix assignment with backcall expressions in not well-handled cases.
* Fix destructure case assigning one table variable to an operator-valued expression.
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



### Changed Behaviors

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

* Fix some cases when using backcall with an assignment.
* Fix a case that complier crashes with an empty code body.
* Force forward declaration of a variable for assigning local function.

From the original Moonscript compiler:

* [#390](https://github.com/leafo/moonscript/issues/390) Many lists are not allowed to be multiline

  

### Added Features

* Allow value lists in for and local statement to be multiline.
* `yue` now compiles source files in multiple threads to speed up compilation.
* Add placeholder support for backcall operator.
* Add placeholder support for backcall statement.
* Add fat arrow support for backcall statement.
* Add option to compile Yuescript as a Lua C lib. Got Yuescript released to `Luarocks`.
* Move old `export` statement functions to `global` statement to match the `local` statement.
* Change `export` statement behavior to support module management.  Moon codes with `export` statements can not explicitly return values in the root scope. And codes with `export default` can export only one value as the module content. Use cases:
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
  will be compiled to:
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
  to:
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
  to:
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
  to:
  ```Lua
  local _call_3 = c["repeat"]["if"]
  local _call_4 = _call_3["then"](_call_3, "xyz")
  _call_4["else"](_call_4, res)
  ```



### Changed Behaviors

* Left part of update statement will now accept only one assignable expression.
* Expression list appears in the middle of the code block is not allowed. For codes like:

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

  The original Moonscript will compile these codes to:

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

* Reusing variables which help generate reduced Lua codes. 
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

  to:

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

  instead of:

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
