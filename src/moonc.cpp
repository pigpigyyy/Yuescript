/* Copyright (c) 2020 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include "MoonP/moon_compiler.h"
#include "MoonP/parser.hpp"
#include "MoonP/moon_ast.h"

int main(int narg, const char** args) {
	const char* help =
"Usage: moonc [options|files] ...\n\n"
"    -h          Print this message\n"
"    -t path     Specify where to place compiled files\n"
"    -o file     Write output to file\n"
"    -p          Write output to standard out\n"
"    -b          Dump compile time (doesn't write output)\n"
"    -l          Write line numbers from source codes\n"
"    -a          Allow expression list not in the end of body block\n"
"    -s          Use space over tab\n"
"    -v          Print version\n";
	if (narg == 0) {
		std::cout << help;
		return 0;
	}
	MoonP::MoonConfig config;
	bool writeToFile = true;
	bool dumpCompileTime = false;
	std::string targetPath;
	std::string resultFile;
	std::list<std::string> files;
	for (int i = 1; i < narg; ++i) {
		switch (hash(args[i])) {
			case "-a"_id:
				config.allowExprNotInTheEndOfBody = true;
				break;
			case "-s"_id:
				config.spaceOverTab = true;
				break;
			case "-l"_id:
				config.reserveLineNumber = true;
				break;
			case "-r"_id:
				config.reuseVariable = true;
				break;
			case "-p"_id:
				writeToFile = false;
				break;
			case "-t"_id:
				++i;
				if (i < narg) {
					targetPath = args[i];
				} else {
					std::cout << help;
					return 1;
				}
				break;
			case "-b"_id:
				dumpCompileTime = true;
				break;
			case "-h"_id:
				std::cout << help;
				return 0;
			case "-v"_id:
				std::cout << "Moonscript version: " << MoonP::moonScriptVersion() << '\n';
				break;
			case "-o"_id:
				++i;
				if (i < narg) {
					resultFile = args[i];
				} else {
					std::cout << help;
					return 1;
				}
				break;
			default:
				files.push_back(args[i]);
				break;
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
	for (const auto& file : files) {
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
					error_list el;
					MoonP::State st;
					start = std::chrono::high_resolution_clock::now();
					auto input = Converter{}.from_bytes(s);
					parserlib::parse<MoonP::File_t>(input, MoonP::File, el, &st);
					end = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> parseDiff = end - start;
					std::cout << file << " \n";
					std::cout << "Parse time:     " << std::setprecision(5) << parseDiff.count() * 1000 << " ms\n";
					std::cout << "Compile time:   " << std::setprecision(5) << (diff.count() - parseDiff.count()) * 1000 << " ms\n\n";
				} else {
					std::cout << "Fail to compile: " << file << ".\n";
					std::cout << std::get<1>(result) << '\n';
					return 1;
				}
				continue;
			}
			auto result = MoonP::moonCompile(s, config);
			if (!std::get<0>(result).empty()) {
				if (!writeToFile) {
					std::cout << std::get<0>(result) << '\n';
				} else {
					std::string targetFile;
					if (resultFile.empty()) {
						std::string ext;
						targetFile = file;
						size_t pos = file.rfind('.');
						if (pos != std::string::npos) {
							ext = file.substr(pos + 1);
							for (size_t i = 0; i < ext.length(); i++) {
								ext[i] = static_cast<char>(tolower(ext[i]));
							}
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
							if (targetPath.back() != '/' && targetPath.back() != '\\') {
								targetPath.append("/");
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
	}
	return 0;
}
