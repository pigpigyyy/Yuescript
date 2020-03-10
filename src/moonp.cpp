/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>
#include <tuple>
#include "MoonP/moon_compiler.h"
#include "MoonP/moon_parser.h"

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
		if (arg == "-l") {
			config.reserveLineNumber = true;
		} else if (arg == "-p") {
			writeToFile = false;
		} else if (arg == "-t") {
			++i;
			if (i < narg) {
				targetPath = args[i];
			} else {
				std::cout << help;
				return 1;
			}
		} else if (arg == "-b") {
			dumpCompileTime = true;
		} else if (arg == "-h") {
			std::cout << help;
			return 0;
		} else if (arg == "-v") {
			std::cout << "Moonscript version: " << MoonP::moonScriptVersion() << '\n';
			return 0;
		} else if (arg == "-o") {
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
	std::list<std::future<std::pair<int,std::string>>> results;
	for (const auto& file : files) {
		auto task = std::async(std::launch::async, [=]() {
			std::ifstream input(file, std::ios::in);
			if (input) {
				std::string s(
					(std::istreambuf_iterator<char>(input)),
					std::istreambuf_iterator<char>());
				if (dumpCompileTime) {
					auto start = std::chrono::high_resolution_clock::now();
					auto result = MoonP::MoonCompiler{nullptr,openlibs}.compile(s, config);
					auto end = std::chrono::high_resolution_clock::now();
					if (!std::get<0>(result).empty()) {
						std::chrono::duration<double> diff = end - start;
						start = std::chrono::high_resolution_clock::now();
						MoonP::MoonParser{}.parse<MoonP::File_t>(s);
						end = std::chrono::high_resolution_clock::now();
						std::chrono::duration<double> parseDiff = end - start;
						std::ostringstream buf;
						buf << file << " \n";
						buf << "Parse time:     " << std::setprecision(5) << parseDiff.count() * 1000 << " ms\n";
						buf << "Compile time:   " << std::setprecision(5) << (diff.count() - parseDiff.count()) * 1000 << " ms\n\n";
						return std::pair{0, buf.str()};
					} else {
						std::ostringstream buf;
						buf << "Fail to compile: " << file << ".\n";
						buf << std::get<1>(result) << '\n';
						return std::pair{1, buf.str()};
					}
				}
				auto result = MoonP::MoonCompiler{nullptr,openlibs}.compile(s, config);
				if (!std::get<0>(result).empty()) {
					if (!writeToFile) {
						return std::pair{1, std::get<0>(result) + '\n'};
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
						std::ofstream output(targetFile, std::ios::trunc | std::ios::out);
						if (output) {
							const auto& codes = std::get<0>(result);
							output.write(codes.c_str(), codes.size());
							return std::pair{0, std::string("Built ") + file + '\n'};
						} else {
							return std::pair{1, std::string("Fail to write file: ") + targetFile + '\n'};
						}
					}
				} else {
					std::ostringstream buf;
					buf << "Fail to compile: " << file << ".\n";
					buf << std::get<1>(result) << '\n';
					return std::pair{1, buf.str()};
				}
			} else {
				return std::pair{1, std::string("Fail to read file: ") + file + ".\n"};
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

