/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "MoonP/moon_compiler.h"
#include "MoonP/moon_parser.h"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>
#include <tuple>
#include <string_view>
#include <memory>
using namespace std::string_view_literals;
#include "ghc/fs_std.hpp"
#include "linenoise.hpp"

#ifndef MOONP_NO_MACRO
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

void pushOptions(lua_State* L, int lineOffset) {
	lua_newtable(L);
	lua_pushliteral(L, "lint_global");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);
	lua_pushliteral(L, "implicit_return_root");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "reserve_line_number");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "space_over_tab");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);
	lua_pushliteral(L, "same_module");
	lua_pushboolean(L, 1);
	lua_rawset(L, -3);
	lua_pushliteral(L, "line_offset");
	lua_pushinteger(L, lineOffset);
	lua_rawset(L, -3);
}

#define MOONP_ARGS nullptr,openlibs
#else
#define MOONP_ARGS
#endif // MOONP_NO_MACRO

#ifndef MOONP_COMPILER_ONLY
static const char luaminifyCodes[] =
#include "LuaMinify.h"

static void pushLuaminify(lua_State* L) {
	if (luaL_loadbuffer(L, luaminifyCodes, sizeof(luaminifyCodes) / sizeof(luaminifyCodes[0]) - 1, "=(luaminify)") != 0) {
		std::string err = std::string("fail to load luaminify module.\n") + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	} else if (lua_pcall(L, 0, 1, 0) != 0) {
		std::string err = std::string("fail to init luaminify module.\n") + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	}
}
#endif // MOONP_COMPILER_ONLY

int main(int narg, const char** args) {
	const char* help =
"Usage: moonp [options|files|directories] ...\n\n"
"   -h       Print this message\n"
#ifndef MOONP_COMPILER_ONLY
"   -e str   Execute a file or raw codes\n"
"   -m       Generate minified codes\n"
#endif // MOONP_COMPILER_ONLY
"   -t path  Specify where to place compiled files\n"
"   -o file  Write output to file\n"
"   -s       Use spaces in generated codes instead of tabs\n"
"   -p       Write output to standard out\n"
"   -b       Dump compile time (doesn't write output)\n"
"   -l       Write line numbers from source codes\n"
"   -v       Print version\n"
#ifndef MOONP_COMPILER_ONLY
"   --       Read from standard in, print to standard out\n"
"            (Must be first and only argument)\n\n"
"   Execute without options to enter REPL, type symbol '$'\n"
"   in a single line to start/stop multi-line mode\n"
#endif // MOONP_COMPILER_ONLY
;
#ifndef MOONP_COMPILER_ONLY
	if (narg == 1) {
		lua_State* L = luaL_newstate();
		openlibs(L);
		DEFER(lua_close(L));
		pushMoonp(L, "insert_loader"sv);
		if (lua_pcall(L, 0, 0, 0) != 0) {
			std::cout << lua_tostring(L, -1) << '\n';
			return 1;
		}
		int count = 0;
		linenoise::SetMultiLine(false);
		linenoise::SetCompletionCallback([](const char* editBuffer, std::vector<std::string>& completions) {
			std::string buf = editBuffer;
			std::string tmp = buf;
			MoonP::Utils::trim(tmp);
			if (tmp.empty()) return;
			std::string pre;
			auto pos = buf.find_first_not_of(" \t\n");
			if (pos != std::string::npos) {
				pre = buf.substr(0, pos);
			}
			switch (tmp[0]) {
				case 'b':
					completions.push_back(pre + "break");
					break;
				case 'c':
					completions.push_back(pre + "class ");
					completions.push_back(pre + "continue");
					break;
				case 'e':
					completions.push_back(pre + "else");
					completions.push_back(pre + "export ");
					break;
				case 'i':
					completions.push_back(pre + "import \"");
					break;
				case 'g':
					completions.push_back(pre + "global ");
					break;
				case 'l':
					completions.push_back(pre + "local ");
					break;
				case 'm':
					completions.push_back(pre + "macro expr ");
					completions.push_back(pre + "macro block ");
					completions.push_back(pre + "macro lua ");
					break;
				case 's':
					completions.push_back(pre + "switch ");
					break;
				case 'u':
					completions.push_back(pre + "unless ");
					break;
				case 'w':
					completions.push_back(pre + "with ");
					completions.push_back(pre + "when ");
					break;
			}
		});
		std::cout << "Moonscript+ "sv << MoonP::version() << '\n';
		while (true) {
			count++;
			std::string codes;
			bool quit = linenoise::Readline("> ", codes);
			if (quit) return 0;
			linenoise::AddHistory(codes.c_str());
			MoonP::Utils::trim(codes);
			if (codes == "$"sv) {
				codes.clear();
				for (std::string line; !(quit = linenoise::Readline("", line));) {
					auto temp = line;
					MoonP::Utils::trim(temp);
					if (temp == "$"sv) {
						break;
					}
					codes += '\n';
					codes += line;
					linenoise::AddHistory(line.c_str());
					MoonP::Utils::trim(codes);
				}
				if (quit) return 0;
			}
			codes.insert(0, "global *\n"sv);
			int top = lua_gettop(L);
			DEFER(lua_settop(L, top));
			pushMoonp(L, "loadstring"sv);
			lua_pushlstring(L, codes.c_str(), codes.size());
			lua_pushstring(L, (std::string("=(repl ") + std::to_string(count) + ')').c_str());
			pushOptions(L, -1);
			const std::string_view Err = "\033[35m"sv, Val = "\033[33m"sv, Stop = "\033[0m\n"sv;
			if (lua_pcall(L, 3, 2, 0) != 0) {
				std::cout << Err << lua_tostring(L, -1) << Stop;
				continue;
			}
			if (lua_isnil(L, -2) != 0) {
				std::string err = lua_tostring(L, -1);
				auto modName = std::string("(repl "sv) + std::to_string(count) + "):";
				if (err.substr(0, modName.size()) == modName) {
					err = err.substr(modName.size());
				}
				auto pos = err.find(':');
				if (pos != std::string::npos) {
					int lineNum = std::stoi(err.substr(0, pos));
					err = std::to_string(lineNum - 1) + err.substr(pos);
				}
				std::cout << Err << err << Stop;
				continue;
			}
			lua_pop(L, 1);
			pushMoonp(L, "pcall"sv);
			lua_insert(L, -2);
			int last = lua_gettop(L) - 2;
			if (lua_pcall(L, 1, LUA_MULTRET, 0) != 0) {
				std::cout << Err << lua_tostring(L, -1) << Stop;
				continue;
			}
			int cur = lua_gettop(L);
			int retCount = cur - last;
			bool success = lua_toboolean(L, -retCount) != 0;
			if (success) {
				if (retCount > 1) {
					for (int i = 1; i < retCount; ++i) {
						std::cout << Val << luaL_tolstring(L, -retCount + i, nullptr) << Stop;
						lua_pop(L, 1);
					}
				}
			} else {
				std::cout << Err << lua_tostring(L, -1) << Stop;
			}
		}
		std::cout << '\n';
		return 0;
	}
	bool minify = false;
#endif // MOONP_COMPILER_ONLY
	MoonP::MoonConfig config;
	config.implicitReturnRoot = true;
	config.lintGlobalVariable = false;
	config.reserveLineNumber = false;
	config.useSpaceOverTab = false;
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
			std::string codes;
			while ((ch = std::cin.get()) != EOF) {
				codes += ch;
			}
			MoonP::MoonConfig conf;
			conf.implicitReturnRoot = true;
			conf.lintGlobalVariable = false;
			conf.reserveLineNumber = false;
			conf.useSpaceOverTab = true;
			auto result = MoonP::MoonCompiler{MOONP_ARGS}.compile(codes, conf);
			if (std::get<1>(result).empty()) {
				std::cout << std::get<0>(result);
				return 0;
			} else {
				std::ostringstream buf;
				std::cout << std::get<1>(result) << '\n';
				return 1;
			}
#ifndef MOONP_COMPILER_ONLY
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
				if (lua_pcall(L, 2, 2, 0) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				if (lua_isnil(L, -2) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				lua_pop(L, 1);
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
		} else if (arg == "-m"sv) {
			minify = true;
#endif // MOONP_COMPILER_ONLY
		} else if (arg == "-s"sv) {
			config.useSpaceOverTab = true;
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
			std::cout << "Moonscript+ version: "sv << MoonP::version() << '\n';
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
	std::list<std::future<std::tuple<int,std::string,std::string>>> results;
	for (const auto& file : files) {
		auto task = std::async(std::launch::async, [=]() {
			std::ifstream input(file.first, std::ios::in);
			if (input) {
				std::string s(
					(std::istreambuf_iterator<char>(input)),
					std::istreambuf_iterator<char>());
				if (dumpCompileTime) {
					auto start = std::chrono::high_resolution_clock::now();
					auto result = MoonP::MoonCompiler{MOONP_ARGS}.compile(s, config);
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
						return std::tuple{0, file.first, buf.str()};
					} else {
						std::ostringstream buf;
						buf << "Fail to compile: "sv << file.first << ".\n"sv;
						buf << std::get<1>(result) << '\n';
						return std::tuple{1, file.first, buf.str()};
					}
				}
				auto result = MoonP::MoonCompiler{MOONP_ARGS}.compile(s, config);
				if (std::get<1>(result).empty()) {
					if (!writeToFile) {
						return std::tuple{0, file.first, std::get<0>(result) + '\n'};
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
							return std::tuple{0, targetFile.string(), std::string("Built "sv) + file.first + '\n'};
						} else {
							return std::tuple{1, std::string(), std::string("Fail to write file: "sv) + targetFile.string() + '\n'};
						}
					}
				} else {
					std::ostringstream buf;
					buf << "Fail to compile: "sv << file.first << ".\n";
					buf << std::get<1>(result) << '\n';
					return std::tuple{1, std::string(), buf.str()};
				}
			} else {
				return std::tuple{1, std::string(), std::string("Fail to read file: "sv) + file.first + ".\n"};
			}
		});
		results.push_back(std::move(task));
	}
	int ret = 0;
#ifndef MOONP_COMPILER_ONLY
	lua_State* L = nullptr;
	DEFER({
		if (L) lua_close(L);
	});
	if (minify) {
		L = luaL_newstate();
		luaL_openlibs(L);
		pushLuaminify(L);
	}
#endif // MOONP_COMPILER_ONLY
	std::list<std::string> errs;
	for (auto& result : results) {
		int val = 0;
		std::string file;
		std::string msg;
		std::tie(val, file, msg) = result.get();
		if (val != 0) {
			ret = val;
			errs.push_back(msg);
		} else {
#ifndef MOONP_COMPILER_ONLY
			if (minify) {
				std::ifstream input(file, std::ios::in);
				if (input) {
					std::string s;
					if (writeToFile) {
						s = std::string(
							(std::istreambuf_iterator<char>(input)),
							std::istreambuf_iterator<char>());
					} else {
						s = msg;
					}
					input.close();
					int top = lua_gettop(L);
					DEFER(lua_settop(L, top));
					lua_pushvalue(L, -1);
					lua_pushlstring(L, s.c_str(), s.size());
					if (lua_pcall(L, 1, 1, 0) != 0) {
						ret = 2;
						std::string err = lua_tostring(L, -1);
						errs.push_back(std::string("Fail to minify: "sv) + file + '\n' + err + '\n');
					} else {
						size_t size = 0;
						const char* minifiedCodes = lua_tolstring(L, -1, &size);
						if (writeToFile) {
							std::ofstream output(file, std::ios::trunc | std::ios::out);
							output.write(minifiedCodes, size);
							output.close();
							std::cout << "Minified built "sv << file << '\n';
						} else {
							std::cout << minifiedCodes << '\n';
						}
					}
				} else {
					ret = 2;
					errs.push_back(std::string("Fail to minify: "sv) + file + '\n');
				}
			} else {
				std::cout << msg;
			}
#else
			std::cout << msg;
#endif // MOONP_COMPILER_ONLY
		}
	}
	for (const auto& err : errs) {
		std::cout << err;
	}
	return ret;
}
