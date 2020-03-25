/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "MoonP/moon_compiler.h"
#include "MoonP/moon_parser.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>
#include <tuple>
#include <string_view>
#include <memory>
using namespace std::string_view_literals;
#include "ghc/fs_std.hpp"

#define _DEFER(code,line) std::shared_ptr<void> _defer_##line(nullptr, [&](auto){code;})
#define DEFER(code) _DEFER(code,__LINE__)

extern "C" {

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
int luaopen_moonp(lua_State* L);

} // extern "C"

static void openlibs(void* state) {
	lua_State* L = static_cast<lua_State*>(state);
	luaL_openlibs(L);
	luaopen_moonp(L);
}

void pushMoonp(lua_State* L, std::string_view name) {
	lua_getglobal(L, "package"); // package
	lua_getfield(L, -1, "loaded"); // package loaded
	lua_getfield(L, -1, "moonp"); // package loaded moonp
	lua_pushlstring(L, &name.front(), name.size()); // package loaded moonp name
	lua_gettable(L, -2); // loaded[name], package loaded moonp item
	lua_insert(L, -4); // item package loaded moonp
	lua_pop(L, 3); // item
}

int main(int narg, const char** args) {
	const char* help =
"Usage: moonp [options|files|directories] ...\n\n"
"   -h       Print this message\n"
"   -e str   Execute a file or raw codes\n"
"   -t path  Specify where to place compiled files\n"
"   -o file  Write output to file\n"
"   -p       Write output to standard out\n"
"   -b       Dump compile time (doesn't write output)\n"
"   -l       Write line numbers from source codes\n"
"   -v       Print version\n"
"   --       Read from standard in, print to standard out\n"
"            (Must be first and only argument)\n";
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
	std::list<std::pair<std::string,std::string>> files;
	for (int i = 1; i < narg; ++i) {
		std::string arg = args[i];
		if (arg == "--"sv) {
			if (i != 1) {
				std::cout << help;
				return 1;
			}
			char ch;
			std::string s;
			while ((ch = std::cin.get()) != EOF) {
				s += ch;
			}
			MoonP::MoonConfig conf;
			conf.implicitReturnRoot = true;
			conf.lintGlobalVariable = false;
			conf.reserveLineNumber = false;
			auto result = MoonP::MoonCompiler{nullptr, openlibs}.compile(s, conf);
			if (std::get<1>(result).empty()) {
				std::cout << std::get<0>(result);
				return 0;
			} else {
				std::ostringstream buf;
				std::cout << std::get<1>(result) << '\n';
				return 1;
			}
		} else if (arg == "-e"sv) {
			++i;
			if (i < narg) {
				lua_State* L = luaL_newstate();
				openlibs(L);
				DEFER(lua_close(L));
				pushMoonp(L, "insert_loader"sv);
				if (lua_pcall(L, 0, 0, 0) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				std::string evalStr = args[i];
				std::ifstream input(evalStr, std::ios::in);
				if (input) {
					auto ext = fs::path(evalStr).extension().string();
					for (auto& ch : ext) ch = std::tolower(ch);
					if (ext == ".lua") {
						lua_getglobal(L, "load");
					} else {
						pushMoonp(L, "loadstring"sv);
					}
					std::string s(
						(std::istreambuf_iterator<char>(input)),
						std::istreambuf_iterator<char>());
					lua_pushlstring(L, s.c_str(), s.size());
					lua_pushlstring(L, evalStr.c_str(), evalStr.size());
				} else {
					pushMoonp(L, "loadstring"sv);
					lua_pushlstring(L, evalStr.c_str(), evalStr.size());
					lua_pushliteral(L, "=(eval str)");
				}
				if (lua_pcall(L, 2, 1, 0) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				pushMoonp(L, "pcall"sv);
				lua_insert(L, -2);
				if (lua_pcall(L, 1, 2, 0) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				bool success = lua_toboolean(L, -2) != 0;
				if (!success) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				return 0;
			} else {
				std::cout << help;
				return 1;
			}
		} else if (arg == "-l"sv) {
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
			std::cout << "Moonscript version: "sv << MoonP::moonScriptVersion() << '\n';
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
			if (fs::is_directory(arg)) {
				for (auto item : fs::recursive_directory_iterator(arg)) {
					if (!item.is_directory()) {
						auto ext = item.path().extension().string();
						for (char& ch : ext) ch = std::tolower(ch);
						if (ext == ".moon"sv) {
							files.emplace_back(item.path().string(), item.path().lexically_relative(arg).string());
						}
					}
				}
			} else {
				files.emplace_back(arg, arg);
			}
		}
	}
	if (files.empty()) {
		std::cout << help;
		return 0;
	}
	if (!resultFile.empty() && files.size() > 1) {
		std::cout << "Error: -o can not be used with multiple input files.\n"sv;
		std::cout << help;
	}
	std::list<std::future<std::pair<int,std::string>>> results;
	for (const auto& file : files) {
		auto task = std::async(std::launch::async, [=]() {
			std::ifstream input(file.first, std::ios::in);
			if (input) {
				std::string s(
					(std::istreambuf_iterator<char>(input)),
					std::istreambuf_iterator<char>());
				if (dumpCompileTime) {
					auto start = std::chrono::high_resolution_clock::now();
					auto result = MoonP::MoonCompiler{nullptr, openlibs}.compile(s, config);
					auto end = std::chrono::high_resolution_clock::now();
					if (!std::get<0>(result).empty()) {
						std::chrono::duration<double> diff = end - start;
						start = std::chrono::high_resolution_clock::now();
						MoonP::MoonParser{}.parse<MoonP::File_t>(s);
						end = std::chrono::high_resolution_clock::now();
						std::chrono::duration<double> parseDiff = end - start;
						std::ostringstream buf;
						buf << file.first << " \n"sv;
						buf << "Parse time:     "sv << std::setprecision(5) << parseDiff.count() * 1000 << " ms\n";
						buf << "Compile time:   "sv << std::setprecision(5) << (diff.count() - parseDiff.count()) * 1000 << " ms\n\n";
						return std::pair{0, buf.str()};
					} else {
						std::ostringstream buf;
						buf << "Fail to compile: "sv << file.first << ".\n"sv;
						buf << std::get<1>(result) << '\n';
						return std::pair{1, buf.str()};
					}
				}
				auto result = MoonP::MoonCompiler{nullptr, openlibs}.compile(s, config);
				if (!std::get<0>(result).empty()) {
					if (!writeToFile) {
						return std::pair{1, std::get<0>(result) + '\n'};
					} else {
						fs::path targetFile;
						if (!resultFile.empty()) {
							targetFile = resultFile;
						} else {
							if (!targetPath.empty()) {
								targetFile = fs::path(targetPath) / file.second;
							} else {
								targetFile = file.first;
							}
							targetFile.replace_extension(".lua"sv);
						}
						if (!targetPath.empty()) {
							fs::create_directories(targetFile.parent_path());
						}
						std::ofstream output(targetFile, std::ios::trunc | std::ios::out);
						if (output) {
							const auto& codes = std::get<0>(result);
							if (config.reserveLineNumber) {
								auto head = std::string("-- [moon]: "sv) + file.first + '\n';
								output.write(head.c_str(), head.size());
							}
							output.write(codes.c_str(), codes.size());
							return std::pair{0, std::string("Built "sv) + file.first + '\n'};
						} else {
							return std::pair{1, std::string("Fail to write file: "sv) + targetFile.string() + '\n'};
						}
					}
				} else {
					std::ostringstream buf;
					buf << "Fail to compile: "sv << file.first << ".\n";
					buf << std::get<1>(result) << '\n';
					return std::pair{1, buf.str()};
				}
			} else {
				return std::pair{1, std::string("Fail to read file: "sv) + file.first + ".\n"};
			}
		});
		results.push_back(std::move(task));
	}
	int ret = 0;
	std::string msg;
	for (auto& result : results) {
		int val = 0;
		std::tie(val, msg) = result.get();
		if (val != 0) {
			ret = val;
		}
		std::cout << msg;
	}
	return ret;
}
