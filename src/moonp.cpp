/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <future>
#include "MoonP/moon_compiler.h"
#include "MoonP/moon_parser.h"

#ifndef LIBMOONP

int main(int narg, const char** args) {
	const char* help =
"Usage: moonp [options|files] ...\n\n"
"    -h          Print this message\n"
"    -t path     Specify where to place compiled files\n"
"    -o file     Write output to file\n"
"    -p          Write output to standard out\n"
"    -b          Dump compile time (doesn't write output)\n"
"    -l          Write line numbers from source codes\n"
"    -v          Print version\n";
	if (narg == 0) {
		std::cout << help;
		return 0;
	}
	MoonP::MoonConfig config;
	config.reserveLineNumber = false;
	bool writeToFile = true;
	bool dumpCompileTime = false;
	std::string targetPath;
	std::string resultFile;
	std::list<std::string> files;
	for (int i = 1; i < narg; ++i) {
		std::string arg = args[i];
		if (arg == "-l"sv) {
			config.reserveLineNumber = true;
		} else if (arg == "-p"sv) {
			writeToFile = false;
		} else if (arg == "-t"sv) {
			++i;
			if (i < narg) {
				targetPath = args[i];
			} else {
				std::cout << help;
				return 1;
			}
		} else if (arg == "-b"sv) {
			dumpCompileTime = true;
		} else if (arg == "-h"sv) {
			std::cout << help;
			return 0;
		} else if (arg == "-v"sv) {
			std::cout << "Moonscript version: " << MoonP::moonScriptVersion() << '\n';
			return 0;
		} else if (arg == "-o"sv) {
			++i;
			if (i < narg) {
				resultFile = args[i];
			} else {
				std::cout << help;
				return 1;
			}
		} else {
			files.push_back(arg);
		}
	}
	if (files.empty()) {
		std::cout << help;
		return 0;
	}
	if (!resultFile.empty() && files.size() > 1) {
		std::cout << "Error: -o can not be used with multiple input files.\n";
		std::cout << help;
	}
	if (!targetPath.empty() && targetPath.back() != '/' && targetPath.back() != '\\') {
		targetPath.append("/");
	}
	std::list<std::future<std::result_of_t<std::decay_t<int()>()>>> results;
	for (const auto& file : files) {
		auto task = std::async(std::launch::async, [=]() {
			std::ifstream input(file, input.in);
			if (input) {
				std::string s(
					(std::istreambuf_iterator<char>(input)),
					std::istreambuf_iterator<char>());
				if (dumpCompileTime) {
					auto start = std::chrono::high_resolution_clock::now();
					auto result = MoonP::moonCompile(s, config);
					auto end = std::chrono::high_resolution_clock::now();
					if (!std::get<0>(result).empty()) {
						std::chrono::duration<double> diff = end - start;
						start = std::chrono::high_resolution_clock::now();
						MoonP::MoonParser{}.parse<MoonP::File_t>(s);
						end = std::chrono::high_resolution_clock::now();
						std::chrono::duration<double> parseDiff = end - start;
						std::cout << file << " \n";
						std::cout << "Parse time:     " << std::setprecision(5) << parseDiff.count() * 1000 << " ms\n";
						std::cout << "Compile time:   " << std::setprecision(5) << (diff.count() - parseDiff.count()) * 1000 << " ms\n\n";
						return 0;
					} else {
						std::cout << "Fail to compile: " << file << ".\n";
						std::cout << std::get<1>(result) << '\n';
						return 1;
					}
				}
				auto result = MoonP::moonCompile(s, config);
				if (!std::get<0>(result).empty()) {
					if (!writeToFile) {
						std::cout << std::get<0>(result) << '\n';
						return 1;
					} else {
						std::string targetFile;
						if (resultFile.empty()) {
							targetFile = file;
							size_t pos = file.rfind('.');
							if (pos != std::string::npos) {
								targetFile = file.substr(0, pos) + ".lua";
							}
							if (!targetPath.empty()) {
								std::string name;
								pos = targetFile.find_last_of("/\\");
								if (pos == std::string::npos) {
									name = targetFile;
								} else {
									name = targetFile.substr(pos + 1);
								}
								targetFile = targetPath + name;
							}
						} else {
							targetFile = resultFile;
						}
						std::ofstream output(targetFile, output.trunc | output.out);
						if (output) {
							const auto& codes = std::get<0>(result);
							output.write(codes.c_str(), codes.size());
							std::cout << "Built " << file << '\n';
							return 0;
						} else {
							std::cout << "Fail to write file: " << targetFile << ".\n";
							return 1;
						}
					}
				} else {
					std::cout << "Fail to compile: " << file << ".\n";
					std::cout << std::get<1>(result) << '\n';
					return 1;
				}
			} else {
				std::cout << "Fail to read file: " << file << ".\n";
				return 1;
			}
		});
		results.push_back(std::move(task));
	}
	int ret = 0;
	for (auto& result : results) {
		int val = result.get();
		if (val != 0) {
			ret = val;
		}
	}
	return ret;
}

#else

extern "C" {

#include "lua.h"
#include "lauxlib.h"

static const char moonplusCodes[] =
#include "Moonscript.h"

static int init_moonplus(lua_State* L) {
	MoonP::MoonConfig config;
	std::string s(moonplusCodes, sizeof(moonplusCodes) / sizeof(moonplusCodes[0]) - 1);
	std::string codes, err;
	MoonP::GlobalVars globals;
	std::tie(codes, err, globals) = MoonP::moonCompile(s, config);
	if (codes.empty()) {
		luaL_error(L, "fail to compile moonplus init codes.\n%s", err.c_str());
	}
	int top = lua_gettop(L);
	if (luaL_loadbuffer(L, codes.c_str(), codes.size(), "=(moonplus)") != 0) {
		luaL_error(L, "fail to init moonplus module.");
	} else {
		lua_call(L, 0, 0);
	}
	lua_settop(L, top);
	return 0;
}

static const char stpCodes[] =
#include "StackTracePlus.h"

static int init_stacktraceplus(lua_State* L) {
	if (luaL_loadbuffer(L, stpCodes, sizeof(stpCodes) / sizeof(stpCodes[0]) - 1, "=(stacktraceplus)") != 0) {
		luaL_error(L, "fail to init stacktraceplus module.");
	} else {
		lua_call(L, 0, 1);
	}
	return 1;
}

static int moontolua(lua_State* L) {
	size_t size = 0;
	const char* input = luaL_checklstring(L, 1, &size);
	MoonP::MoonConfig config;
	if (lua_gettop(L) == 2) {
		lua_pushstring(L, "lint_global");
		lua_gettable(L, -2);
		if (!lua_isnil(L, -1)) {
			config.lintGlobalVariable = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushstring(L, "implicit_return_root");
		lua_gettable(L, -2);
		if (!lua_isnil(L, -1)) {
			config.implicitReturnRoot = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
		lua_pushstring(L, "reserve_line_number");
		lua_gettable(L, -2);
		if (!lua_isnil(L, -1)) {
			config.reserveLineNumber = lua_toboolean(L, -1) != 0;
		}
		lua_pop(L, 1);
	}
	std::string s(input, size);
	std::string codes, err;
	MoonP::GlobalVars globals;
	std::tie(codes, err, globals) = MoonP::moonCompile(s, config);
	if (codes.empty()) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, codes.c_str(), codes.size());
	}
	if (err.empty()) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, err.c_str(), err.size());
	}
	if (globals && !globals->empty()) {
		lua_createtable(L, static_cast<int>(globals->size()), 0);
		int i = 1;
		for (const auto& var : *globals) {
			lua_createtable(L, 3, 0);
			lua_pushlstring(L, var.name.c_str(), var.name.size());
			lua_rawseti(L, -2, 1);
			lua_pushinteger(L, var.line);
			lua_rawseti(L, -2, 2);
			lua_pushinteger(L, var.col);
			lua_rawseti(L, -2, 3);
			lua_rawseti(L, -2, i);
			i++;
		}
	} else {
		lua_pushnil(L);
	}
	return 3;
}

int luaopen_moonp(lua_State* L) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_createtable(L, 0, 0);
	lua_pushcfunction(L, moontolua);
	lua_setfield(L, -2, "to_lua");
	lua_pushcfunction(L, init_stacktraceplus);
	lua_setfield(L, -2, "load_stacktraceplus");
	lua_setfield(L, -2, "moonp");
	lua_pop(L, 2);
	init_moonplus(L);
	return 0;
}

} // extern "C"

#endif // LIBMOONP

