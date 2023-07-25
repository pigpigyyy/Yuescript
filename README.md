# YueScript

<img src="doc/docs/.vuepress/public/image/yuescript.svg" width="300" height="300" alt="logo"/>

[![Ubuntu](https://github.com/pigpigyyy/Yuescript/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/pigpigyyy/Yuescript/actions/workflows/ubuntu.yml) [![Windows](https://github.com/pigpigyyy/Yuescript/actions/workflows/windows.yml/badge.svg)](https://github.com/pigpigyyy/Yuescript/actions/workflows/windows.yml) [![macOS](https://github.com/pigpigyyy/Yuescript/actions/workflows/macos.yml/badge.svg)](https://github.com/pigpigyyy/Yuescript/actions/workflows/macos.yml) [![Discord Badge](https://img.shields.io/discord/844031511208001577?color=5865F2&label=Discord&logo=discord&logoColor=white&style=flat-square)](https://discord.gg/cRJ2VAm2NV)

Yuescript is a Moonscript dialect. It is derived from [Moonscript language](https://github.com/leafo/moonscript) 0.5.0 and continuously adopting new features to be more up to date. 

Moonscript is a language that compiles to Lua. Since original Moonscript has been used to write web framework [lapis](https://github.com/leafo/lapis) and run a few business web sites like [itch.io](https://itch.io) and [streak.club](https://streak.club) with some large code bases. The original language is getting too hard to adopt new features for those may break the stablility for existing applications.

So Yuescript is a new code base for pushing the language to go forward and being a playground to try introducing new language syntax or programing paradigms to make Moonscript language more expressive and productive.

Yue (月) is the name of moon in Chinese and it's pronounced as [jyɛ].



## Features

* No other dependencies needed except modified [parserlib](https://github.com/axilmar/parserlib) library from Achilleas Margaritis with some performance enhancement. **lpeg** library is no longer needed.
* Written in C++17.
* Support most of the features from Moonscript language. Generate Lua codes in the same way like the original compiler.
* Reserve line numbers from source file in the compiled Lua codes to help debugging.
* More features like macro, existential operator, pipe operator, Javascript-like export syntax and etc.
* See other details in the [changelog](./CHANGELOG.md). Find document [here](http://yuescript.org).



## Installation & Usage

* **Lua Module**

&emsp;&emsp;Build `yue.so` file with

```sh
> make shared LUAI=/usr/local/include/lua LUAL=/usr/local/lib/lua
```

&emsp;&emsp;Then get the binary file from path `bin/shared/yue.so`.

&emsp;&emsp;Or you can install [luarocks](https://luarocks.org), a package manager for Lua modules. Then install it as a Lua module with

```sh
> luarocks install yuescript
```

&emsp;&emsp;Then require the Yuescript module in Lua:

```Lua
require("yue")("main") -- require `main.yue`

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



* **Binary Tool**

&emsp;&emsp;Clone this repo, then build and install executable with:
```sh
> make install
```

&emsp;&emsp;Build Yuescript tool without macro feature:
```sh
> make install NO_MACRO=true
```

&emsp;&emsp;Build Yuescript tool without built-in Lua binary:
```sh
> make install NO_LUA=true
```

&emsp;&emsp;Use Yuescript tool with:

```sh
> yue -h
Usage: yue [options|files|directories] ...

   -h       Print this message
   -e str   Execute a file or raw codes
   -m       Generate minified codes
   -r       Rewrite output to match original line numbers
   -t path  Specify where to place compiled files
   -o file  Write output to file
   -s       Use spaces in generated codes instead of tabs
   -p       Write output to standard out
   -b       Dump compile time (doesn't write output)
   -g       Dump global variables used in NAME LINE COLUMN
   -l       Write line numbers from source codes
   -c       Reserve comments before statement from source codes
   -w path  Watch changes and compile every file under directory
   -v       Print version
   --       Read from standard in, print to standard out
            (Must be first and only argument)

   --target=version  Specify the Lua version that codes will be generated to
                     (version can only be 5.1, 5.2, 5.3 or 5.4)
   --path=path_str   Append an extra Lua search path string to package.path

   Execute without options to enter REPL, type symbol '$'
   in a single line to start/stop multi-line mode
```
&emsp;&emsp;Use cases:  
&emsp;&emsp;Recursively compile every Yuescript file with extension `.yue` under current path:  `yue .`  
&emsp;&emsp;Compile and save results to a target path:  `yue -t /target/path/ .`  
&emsp;&emsp;Compile and reserve debug info:  `yue -l .`  
&emsp;&emsp;Compile and generate minified codes:  `yue -m .`  
&emsp;&emsp;Execute raw codes:  `yue -e 'print 123'`  
&emsp;&emsp;Execute a Yuescript file:  `yue -e main.yue`



## Editor Support

* [Vim](https://github.com/pigpigyyy/Yuescript-vim)
* [ZeroBraneStudio](https://github.com/pkulchenko/ZeroBraneStudio/issues/1134) (Syntax highlighting)
* [Visual Studio Code](https://github.com/pigpigyyy/yuescript-vscode)



## License

MIT
