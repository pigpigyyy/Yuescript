# MoonPlus

|macOS|Linux|Windows|
|:---:|:---:|:-----:|
|![CI](https://github.com/pigpigyyy/MoonPlus/workflows/build-test/badge.svg)|![CI](https://github.com/pigpigyyy/MoonPlus/workflows/build-test/badge.svg)|[![Build status](https://ci.appveyor.com/api/projects/status/4nbkye9mx9b3bf83/branch/master?svg=true)](https://ci.appveyor.com/project/pigpigyyy/moonplus/branch/master)|

MoonPlus is a compiler with features from [Moonscript language](https://github.com/leafo/moonscript) 0.5.0 and adopting new features to make Moonscript more up to date. 

Since original Moonscript has been used to write web framework [lapis](https://github.com/leafo/lapis) and run a few business web sites like [itch.io](https://itch.io) and [streak.club](https://streak.club) with some large code bases. The original language is getting too hard to adopt new features for those may break the stablility for existing applications.

So MoonPlus is a new code base for pushing the language to go forward and being a playground to try introducing new language syntax or programing paradigms to make Moonscript language more expressive and productive.



## Features

* No other dependencies needed except modified **parserlib** library from Achilleas Margaritis with some performance enhancement. **lpeg** library is no longer needed.
* Written in C++17.
* Support full Moonscript language features, generate the same Lua codes with original compiler.
* Reserve line numbers from Moonscript sources in compiled Lua codes to help with debugging.
* See other details in the [changelog](./CHANGELOG.md).



## Installation & Usage

* **Lua Module**

&emsp;&emsp;Build `moonp.so` file with

```sh
> make shared LUAI=/usr/local/include/lua LUAL=/usr/local/lib/lua
```

&emsp;&emsp;Then get the binary file from path `bin/shared/moonp.so`.

&emsp;&emsp;Or you can install [luarocks](https://luarocks.org), a package manager for Lua modules. Then install it as a Lua module with

```sh
> luarocks install moonplus
```

&emsp;&emsp;Then require the module in Lua:

```Lua
require("moonp")("main") -- require `main.mp`

local moonp = require("moonp")
local codes, err, globals = moonp.to_lua([[
f = ->
  print "hello world"
f!
]],{
  implicit_return_root = true,
  reserve_line_number = true,
  lint_global = true
})
```



* **Binary Tool**

&emsp;&emsp;Clone this repo, then build and install executable with:

```sh
> make install
```

&emsp;&emsp;Use MoonPlus tool with:

```sh
> moonp -h
Usage: moonp [options|files|directories] ...

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
&emsp;&emsp;Recursively compile every moon+ file with extension `.md` under current path:  `moonp .`  
&emsp;&emsp;Compile and save results to a target path:  `moonp -t /target/path/ .`  
&emsp;&emsp;Compile and reserve debug info:  `moonp -l .`  
&emsp;&emsp;Compile and generate minified codes:  `moonp -m .`  
&emsp;&emsp;Execute raw codes:  `moonp -e 'print 123'`  
&emsp;&emsp;Execute a moon+ file:  `moonp -e main.mp`



* **Docker Hub**
  Try moonp in another easy way: https://hub.docker.com/r/moonplus/moonplus



## Editor Support

* [Vim](https://github.com/pigpigyyy/MoonPlus-vim)



## License

MIT
