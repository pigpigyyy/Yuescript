# Changelog

The implementation for the original Moonscript language 0.5.0 can be found in the `0.5.0` branch of Yuescript. The Moonscript with fixes and new features is in the main branch of Yuescript. Here are the changelogs for each Yuescript version.

## v0.17.10

### Added Features

* Added In-expression syntax to do range checking. In-expression can be used in switch statement as a new clause.
  ```moonscript
  a = 5
  if a in [1, 10] or a in (20, 30) or a in {77, 88} or a not in {100, 200}
    print "(1 <= a <= 10) or (20 < a < 30) or (a == 77 or a == 88) or not (a == 100 or a == 200)"
  
  switch a when not in [1, 10]
    print "not (1 <= a <= 10)"
  
  if item in list
  	print "item existed in a list"
  ```
* Added metamethod name checking for chain expression.
* Added feature to reformat the expressions passed as macro function arguments to the formated code strings.
* Added -r option to rewrite compiled Lua code output to match original Yuescript code line numbers.

### Fixed Issues

* Fixed a LuaJIT module loading issue.
* Fixed built-in $FILE macro does not escape backslash on Windows.
* Fixed more ambiguous Lua codes generation cases.
* Fixed syntax error should throw for invalid interpolation.
* Fixed an AST object life expired before accessing issue.

## v0.16.4

### Added Features

* Added -w option for Yuescript tool to recursively watch file changes under a target path and get codes recompiled.

* Added different metamethod name literals checking for different Lua target versions.

* Added checks for not using `break` statement in a for-loop scope.

* Added compiler option for preserving comments before statements.  Using command line tool:
  
  ```sh
  yue -c file.yue
  ```
  Using Lua lib:
  ```lua
  local yue = require("yue")
  print yue.to_lua([[
  -- a comment
  f = ->
  ]], {reserve_comment = true})
  ```

* Added `yue.check()` function to get compile errors in Lua table {{type, msg, line, col}}.

* Added support for extending script search paths using `yue.options.path`.

* Added new module export syntax for exporting items without creating local variables.
  ```moonscript
  export.<name> = "My module"
  export.<call> = (...) -> -- ...
  ```
  compiles to:
  ```lua
  local _module_0 = setmetatable({ }, { })
  getmetatable(_module_0).__name = "My module"
  getmetatable(_module_0).__call = function(...) end
  return _module_0
  ```

### Fixed Issues

* Reduced max parser call stack size.

* Refactored some syntax rules to speed up parsing.

* Fixed default value issue when doing metatable destructuring.

* Fixed a class syntax ambiguous issue when writting table block after a class declaration.

* Improve number literal syntax to support hexadecimal point values, positive decimal exponents and hexadecimal exponents.

* Prevented accessing scoped declared global variables multiple times when generating Lua codes.

* Fixed parser running exponentially slow issue when parsing nested expressions.

* Fixed exposing `yue` module as global variable by default.

* Fixed a stack overflow issue in `yue.to_ast()`.

* Fixed flatten level > 0 getting wrong AST issue in `yue.to_ast()` function.

* Fixed missing indent check for comments in the beginning of `in_block` syntax rule.

* Prevented `yue.loadfile()` throwing errors to match Lua `loadfile()` function behavior.

* Fixed `yue` module registering issue for Lua 5.1 and LuaJIT.

## v0.15.12

### Added Features

* yue.to_ast(): Reserve comment nodes followed by a statement in AST structures.
* Added `while` clause line decorator.
  ```moonscript
  reader\parse_line! until reader\eof!
  ```
  compiles to:
  ```lua
   while not reader:eof() do
     reader:parse_line()
   end
  ```
* Supported underscores in number literal. `1_000_000`, `0xFF_EF_06`
* Refactored some error messages with details instead of just "syntax error".
* Added chaining assignment. `a = b = c = 0`

### Fixed Issues

* Change metable accessing operator to `<>` instead of postfix `#`.
  ```moonscript
  <>: mt = tb
  mt = tb.<>
  a = <>: mt, value: 1
  b = :<add>, value: 2
  close _ = <close>: -> print "out of scope"
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
* Refactor `continue` keyword implementation with goto statement when targeting Lua version 5.2 and higher.
* Skip utf-8 bom in parser.
* Fixed classes don't inherits metamethods properly.

## v0.14.5

### Added Features

* Added -g option for Yuescript CMD tool.
* Added option `--target=5.1` to generate Lua 5.1 compatible codes.
* Added const destructuring and imported item is now const by default.
   ```moonscript
   const :width, :height = View.size
   const {:x = 0.0, :y = 0.0} = point
   import a from b
   -- report compiler error when doing width = nil or x = nil or a = nil
   ```
* Added left and right side item numbers check in assignments.
   ```moonscript
   a, b, c = 1 -- report error instead of implicitly assigning nil to extra variables
   a, b, c = f! -- expecting function call to return multiple values is accepted
   a = 1, 2 -- report error instead of ignoring extra values
   ```

### Fixed Issues

* Fix missing checks and issues related to doing destrucuring to if/switch expressions that returns values in multiple clauses.

## v0.13.4

### Added Features

* Added update syntax support for `var //= 5`.
* Added metamethod syntax support for class block.
   ```moonscript
   class Foo
     new: (x) => @x = x
     mul#: (y) => @x * y
     ["abc"]#: 123
     :add
     :add#
   ```
* Added support `with` block access with `[key]` syntax.
   ```moonscript
   with tb
     [1] = [2]\func!
     print [3]
     with [abc]
       [4] = 1
     [] = "abc"
   ```

* Added table pattern matching syntax.
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
         print "size #{width}, #{height}"
   ```

### Fixed Issues

* Fix `import X as {_}` generates invalid Lua code.
* Fix attribute syntax with line decorator compiles to unexpected codes.
   ```moonscript
   const a = 1 if true
   ```
   now compiles to:
   ```lua
   local a <const> = (function()
     if true then
       return 1
     end
   end)()
   ```
* Fix continue in `repeat` loop gives invalid code.
* Fix variables with attributes "const" and "close" should both get constant check in compiler.
* Fix ambiguous syntax starting with `[[`. Expressions starting with `[[` will now be always treated like raw strings other than nested list comprehensions.
* Fix module can export both macros and normal values. The macro exporting module can now only allow macro definition, macro importing and macro expansion in place for better compiler performance.

## v0.10.17

### Added Features

* Yuescript to AST function (`yue.to_ast`)
   ```moonscript
   yue.p yue.to_ast(
     "print 123"
     2 --[[ast flatten level, can only be 0, 1, 2]]
   )
   ```
   Prints:
   ```
   {
     [1] = "file"
     [2] = {
       [1] = "callable"
       [2] = "print"
     }
     [3] = {
       [1] = "invoke_args"
       [2] = "123"
     }
   }
   ```

## v0.10.16

### Fixed Issues

* Prevent text mode macros from inserting line numbers into generated code.
  ```moonscript
  macro text = (code) -> {
    :code
    type: "text"
  }
  
  $text[[#!yue -e]]
  nil
  ```
  Compiles to:
  ```
  #!yue -e
  return nil -- 8
  ```

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
