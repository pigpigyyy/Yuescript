/* Copyright (c) 2022 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "yuescript/yue_compiler.h"
#include "yuescript/yue_parser.h"

#include <cstdlib>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string_view>
#include <tuple>
using namespace std::string_view_literals;
using namespace std::string_literals;
#include "ghc/fs_std.hpp"
#include "linenoise.hpp"

#if not(defined YUE_NO_MACRO && defined YUE_COMPILER_ONLY)
#define _DEFER(code, line) std::shared_ptr<void> _defer_##line(nullptr, [&](auto) { code; })
#define DEFER(code) _DEFER(code, __LINE__)
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
int luaopen_yue(lua_State* L);
} // extern "C"

static void openlibs(void* state) {
	lua_State* L = static_cast<lua_State*>(state);
	luaL_openlibs(L);
#if LUA_VERSION_NUM > 501
	luaL_requiref(L, "yue", luaopen_yue, 1);
#else
	lua_pushcfunction(L, luaopen_yue);
	lua_call(L, 0, 0);
#endif
	lua_pop(L, 1);
}

void pushYue(lua_State* L, std::string_view name) {
	lua_getglobal(L, "package"); // package
	lua_getfield(L, -1, "loaded"); // package loaded
	lua_getfield(L, -1, "yue"); // package loaded yue
	lua_pushlstring(L, &name.front(), name.size()); // package loaded yue name
	lua_gettable(L, -2); // loaded[name], package loaded yue item
	lua_insert(L, -4); // item package loaded yue
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
#endif // not (defined YUE_NO_MACRO && defined YUE_COMPILER_ONLY)

#ifndef YUE_NO_MACRO
#define YUE_ARGS nullptr, openlibs
#else
#define YUE_ARGS
#endif // YUE_NO_MACRO

#ifndef YUE_COMPILER_ONLY
static const char luaminifyCodes[] =
#include "LuaMinify.h"

	static void pushLuaminify(lua_State * L) {
	if (luaL_loadbuffer(L, luaminifyCodes, sizeof(luaminifyCodes) / sizeof(luaminifyCodes[0]) - 1, "=(luaminify)") != 0) {
		std::string err = "failed to load luaminify module.\n"s + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	} else if (lua_pcall(L, 0, 1, 0) != 0) {
		std::string err = "failed to init luaminify module.\n"s + lua_tostring(L, -1);
		luaL_error(L, err.c_str());
	}
}
#endif // YUE_COMPILER_ONLY

int main(int narg, const char** args) {
	const char* help =
		"Usage: yue [options|files|directories] ...\n\n"
		"   -h       Print this message\n"
#ifndef YUE_COMPILER_ONLY
		"   -e str   Execute a file or raw codes\n"
		"   -m       Generate minified codes\n"
#endif // YUE_COMPILER_ONLY
		"   -t path  Specify where to place compiled files\n"
		"   -o file  Write output to file\n"
		"   -s       Use spaces in generated codes instead of tabs\n"
		"   -p       Write output to standard out\n"
		"   -b       Dump compile time (doesn't write output)\n"
		"   -g       Dump global variables used in NAME LINE COLUMN\n"
		"   -l       Write line numbers from source codes\n"
		"   -v       Print version\n"
#ifndef YUE_COMPILER_ONLY
		"   --       Read from standard in, print to standard out\n"
		"            (Must be first and only argument)\n\n"
		"   --target=version  Specify the Lua version that codes will be generated to\n"
		"                     (version can only be 5.1, 5.2, 5.3 or 5.4)\n"
		"   --path=path_str   Append an extra Lua search path string to package.path\n\n"
		"   Execute without options to enter REPL, type symbol '$'\n"
		"   in a single line to start/stop multi-line mode\n"
#endif // YUE_COMPILER_ONLY
		;
#ifndef YUE_COMPILER_ONLY
	if (narg == 1) {
		lua_State* L = luaL_newstate();
		openlibs(L);
		DEFER(lua_close(L));
		pushYue(L, "insert_loader"sv);
		if (lua_pcall(L, 0, 0, 0) != 0) {
			std::cout << lua_tostring(L, -1) << '\n';
			return 1;
		}
		int count = 0;
		linenoise::SetMultiLine(false);
		linenoise::SetCompletionCallback([](const char* editBuffer, std::vector<std::string>& completions) {
			std::string buf = editBuffer;
			std::string tmp = buf;
			yue::Utils::trim(tmp);
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
					completions.push_back(pre + "macro ");
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
		std::cout << "Yuescript "sv << yue::version << '\n';
		while (true) {
			count++;
			std::string codes;
			bool quit = linenoise::Readline("> ", codes);
			if (quit) return 0;
			linenoise::AddHistory(codes.c_str());
			yue::Utils::trim(codes);
			if (codes == "$"sv) {
				codes.clear();
				for (std::string line; !(quit = linenoise::Readline("", line));) {
					auto temp = line;
					yue::Utils::trim(temp);
					if (temp == "$"sv) {
						break;
					}
					codes += '\n';
					codes += line;
					linenoise::AddHistory(line.c_str());
					yue::Utils::trim(codes);
				}
				if (quit) return 0;
			}
			codes.insert(0, "global *\n"sv);
			int top = lua_gettop(L);
			DEFER(lua_settop(L, top));
			pushYue(L, "loadstring"sv);
			lua_pushlstring(L, codes.c_str(), codes.size());
			lua_pushstring(L, ("=(repl "s + std::to_string(count) + ')').c_str());
			pushOptions(L, -1);
			const std::string_view Err = "\033[35m"sv, Val = "\033[33m"sv, Stop = "\033[0m\n"sv;
			if (lua_pcall(L, 3, 2, 0) != 0) {
				std::cout << Err << lua_tostring(L, -1) << Stop;
				continue;
			}
			if (lua_isnil(L, -2) != 0) {
				std::string err = lua_tostring(L, -1);
				auto modName = "(repl "s + std::to_string(count) + "):"s;
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
			pushYue(L, "pcall"sv);
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
#if LUA_VERSION_NUM > 501
						std::cout << Val << luaL_tolstring(L, -retCount + i, nullptr) << Stop;
#else // LUA_VERSION_NUM
						lua_getglobal(L, "tostring");
						lua_pushvalue(L, -retCount + i - 1);
						lua_call(L, 1, 1);
						std::cout << Val << lua_tostring(L, -1) << Stop;
#endif // LUA_VERSION_NUM
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
#endif // YUE_COMPILER_ONLY
	yue::YueConfig config;
	config.implicitReturnRoot = true;
	config.lintGlobalVariable = false;
	config.reserveLineNumber = false;
	config.useSpaceOverTab = false;
	bool writeToFile = true;
	bool dumpCompileTime = false;
	bool lintGlobal = false;
	std::string targetPath;
	std::string resultFile;
	std::string workPath;
	std::list<std::pair<std::string, std::string>> files;
	for (int i = 1; i < narg; ++i) {
		std::string arg = args[i];
		if (arg == "--"sv) {
			if (i != 1) {
				std::cout << help;
				return 1;
			}
			char ch;
			std::string codes;
			while ((ch = std::cin.get()) && !std::cin.eof()) {
				codes += ch;
			}
			yue::YueConfig conf;
			conf.implicitReturnRoot = true;
			conf.lintGlobalVariable = false;
			conf.reserveLineNumber = false;
			conf.useSpaceOverTab = true;
			auto result = yue::YueCompiler{YUE_ARGS}.compile(codes, conf);
			if (result.error.empty()) {
				std::cout << result.codes;
				return 0;
			} else {
				std::ostringstream buf;
				std::cout << result.error << '\n';
				return 1;
			}
#ifndef YUE_COMPILER_ONLY
		} else if (arg == "-e"sv) {
			++i;
			if (i < narg) {
				lua_State* L = luaL_newstate();
				openlibs(L);
				DEFER(lua_close(L));
				pushYue(L, "insert_loader"sv);
				if (lua_pcall(L, 0, 0, 0) != 0) {
					std::cout << lua_tostring(L, -1) << '\n';
					return 1;
				}
				std::string evalStr = args[i];
				lua_newtable(L);
				i++;
				for (int j = i, index = 1; j < narg; j++) {
					lua_pushstring(L, args[j]);
					lua_rawseti(L, -2, index);
					index++;
				}
				lua_setglobal(L, "arg");
				std::ifstream input(evalStr, std::ios::in);
				if (input) {
					auto ext = fs::path(evalStr).extension().string();
					for (auto& ch : ext) ch = std::tolower(ch);
					if (ext == ".lua") {
						lua_getglobal(L, "load");
					} else {
						pushYue(L, "loadstring"sv);
					}
					std::string s(
						(std::istreambuf_iterator<char>(input)),
						std::istreambuf_iterator<char>());
					lua_pushlstring(L, s.c_str(), s.size());
					lua_pushlstring(L, evalStr.c_str(), evalStr.size());
				} else {
					pushYue(L, "loadstring"sv);
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
				pushYue(L, "pcall"sv);
				lua_insert(L, -2);
				int argCount = 0;
				while (i < narg) {
					argCount++;
					lua_pushstring(L, args[i]);
					i++;
				}
				if (lua_pcall(L, 1 + argCount, 2, 0) != 0) {
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
#endif // YUE_COMPILER_ONLY
		} else if (arg == "-s"sv) {
			config.useSpaceOverTab = true;
		} else if (arg == "-l"sv) {
			config.reserveLineNumber = true;
		} else if (arg == "-p"sv) {
			writeToFile = false;
		} else if (arg == "-g"sv) {
			writeToFile = false;
			lintGlobal = true;
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
			std::cout << "Yuescript version: "sv << yue::version << '\n';
			return 0;
		} else if (arg == "-o"sv) {
			++i;
			if (i < narg) {
				resultFile = args[i];
			} else {
				std::cout << help;
				return 1;
			}
		} else if (arg.size() > 2 && arg.substr(0, 2) == "--"sv && arg.substr(2, 1) != "-"sv) {
			auto argStr = arg.substr(2);
			yue::Utils::trim(argStr);
			size_t idx = argStr.find('=');
			if (idx != std::string::npos) {
				auto key = argStr.substr(0, idx);
				auto value = argStr.substr(idx + 1);
				yue::Utils::trim(key);
				yue::Utils::trim(value);
				config.options[key] = value;
			} else {
				config.options[argStr] = std::string();
			}
		} else {
			if (fs::is_directory(arg)) {
				workPath = arg;
				for (auto item : fs::recursive_directory_iterator(arg)) {
					if (!item.is_directory()) {
						auto ext = item.path().extension().string();
						for (char& ch : ext) ch = std::tolower(ch);
						if (!ext.empty() && ext.substr(1) == yue::extension) {
							files.emplace_back(item.path().string(), item.path().lexically_relative(arg).string());
						}
					}
				}
			} else {
				workPath = fs::path(arg).parent_path().string();
				files.emplace_back(arg, arg);
			}
		}
	}
	if (files.empty()) {
		std::cout << help;
		return 0;
	}
	if (!resultFile.empty() && files.size() > 1) {
		std::cout << "Error: -o can not be used with multiple input files\n"sv;
		std::cout << help;
	}
	std::list<std::future<std::tuple<int, std::string, std::string>>> results;
	for (const auto& file : files) {
		auto task = std::async(std::launch::async, [=]() {
			std::ifstream input(file.first, std::ios::in);
			if (input) {
				std::string s(
					(std::istreambuf_iterator<char>(input)),
					std::istreambuf_iterator<char>());
				auto conf = config;
				conf.module = file.first;
				if (!workPath.empty()) {
					auto it = conf.options.find("path");
					if (it != conf.options.end()) {
						it->second += ';';
						it->second += (fs::path(workPath) / "?.lua"sv).string();
					} else {
						conf.options["path"] = (fs::path(workPath) / "?.lua"sv).string();
					}
				}
				if (dumpCompileTime) {
					conf.profiling = true;
					auto result = yue::YueCompiler{YUE_ARGS}.compile(s, conf);
					if (!result.codes.empty()) {
						std::ostringstream buf;
						buf << file.first << " \n"sv;
						buf << "Parse time:     "sv << std::setprecision(5) << result.parseTime * 1000 << " ms\n";
						buf << "Compile time:   "sv << std::setprecision(5) << result.compileTime * 1000 << " ms\n\n";
						return std::tuple{0, file.first, buf.str()};
					} else {
						std::ostringstream buf;
						buf << "Failed to compile: "sv << file.first << '\n';
						buf << result.error << '\n';
						return std::tuple{1, file.first, buf.str()};
					}
				}
				conf.lintGlobalVariable = lintGlobal;
				auto result = yue::YueCompiler{YUE_ARGS}.compile(s, conf);
				if (result.error.empty()) {
					if (!writeToFile) {
						if (lintGlobal) {
							std::ostringstream buf;
							for (const auto& global : *result.globals) {
								buf << global.name << ' ' << global.line << ' ' << global.col << '\n';
							}
							return std::tuple{0, file.first, buf.str() + '\n'};
						} else {
							return std::tuple{0, file.first, result.codes + '\n'};
						}
					} else {
						std::string targetExtension("lua"sv);
						if (result.options) {
							auto it = result.options->find("target_extension"s);
							if (it != result.options->end()) {
								targetExtension = it->second;
							}
						}
						fs::path targetFile;
						if (!resultFile.empty()) {
							targetFile = resultFile;
						} else {
							if (!targetPath.empty()) {
								targetFile = fs::path(targetPath) / file.second;
							} else {
								targetFile = file.first;
							}
							targetFile.replace_extension('.' + targetExtension);
						}
						if (!targetPath.empty()) {
							fs::create_directories(targetFile.parent_path());
						}
						if (result.codes.empty()) {
							return std::tuple{0, targetFile.string(), "Built "s + file.first + '\n'};
						}
						std::ofstream output(targetFile, std::ios::trunc | std::ios::out);
						if (output) {
							const auto& codes = result.codes;
							if (config.reserveLineNumber) {
								auto head = "-- [yue]: "s + file.first + '\n';
								output.write(head.c_str(), head.size());
							}
							output.write(codes.c_str(), codes.size());
							return std::tuple{0, targetFile.string(), "Built "s + file.first + '\n'};
						} else {
							return std::tuple{1, std::string(), "Failed to write file: "s + targetFile.string() + '\n'};
						}
					}
				} else {
					std::ostringstream buf;
					buf << "Failed to compile: "sv << file.first << '\n';
					buf << result.error << '\n';
					return std::tuple{1, std::string(), buf.str()};
				}
			} else {
				return std::tuple{1, std::string(), "Failed to read file: "s + file.first + '\n'};
			}
		});
		results.push_back(std::move(task));
	}
	int ret = 0;
#ifndef YUE_COMPILER_ONLY
	lua_State* L = nullptr;
	DEFER({
		if (L) lua_close(L);
	});
	if (minify) {
		L = luaL_newstate();
		luaL_openlibs(L);
		pushLuaminify(L);
	}
#endif // YUE_COMPILER_ONLY
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
#ifndef YUE_COMPILER_ONLY
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
						errs.push_back("Failed to minify: "s + file + '\n' + err + '\n');
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
					errs.push_back("Failed to minify: "s + file + '\n');
				}
			} else {
				std::cout << msg;
			}
#else
			std::cout << msg;
#endif // YUE_COMPILER_ONLY
		}
	}
	for (const auto& err : errs) {
		std::cout << err;
	}
	return ret;
}
