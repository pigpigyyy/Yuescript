/**
 * Copyright (C) 2017, IppClub. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Slice.h"
#include <cstdlib>

namespace silly {

namespace slice {

const std::string Slice::Empty;

int Slice::compare(const Slice &rhs) const {
  int ret = 0;

  // It's illegal to pass nullptr to memcmp.
  if (str_ && rhs.str_)
    ret = memcmp(str_, rhs.str_, len_);

  if (ret == 0) {
    // Use len_ - rhs.len_ as returned value may cause overflow
    // of size_t type length. Therefore +1, -1 are returned here.
    if (len_ < rhs.len_)
      return -1;
    else if (len_ > rhs.len_)
      return 1;
  }
  return ret;
}

std::string Slice::getFilePath() const {
  std::string filename = toString();
  size_t pos = filename.find_last_of("/\\");
  if (pos == std::string::npos)
  {
    return Slice::Empty;
  }
  return filename.substr(0, pos) + "/";
}

std::string Slice::getFileName() const {
  std::string filename = toString();
  size_t pos = filename.find_last_of("/\\");
  if (pos == std::string::npos) {
    return Slice::Empty;
  }
  return filename.substr(pos+1);
}

std::string Slice::toLower() const {
  std::string tmp = toString();
  for (size_t i = 0; i < tmp.length(); i++) {
    tmp[i] = (char)tolower(tmp[i]);
  }
  return tmp;
}

std::string Slice::toUpper() const {
  std::string tmp = toString();
  for (size_t i = 0; i < tmp.length(); i++) {
    tmp[i] = (char)toupper(tmp[i]);
  }
  return tmp;
}

std::string Slice::getFileExtension() const {
  std::string filename = toString();
  size_t pos = filename.rfind('.');
  if (pos == std::string::npos) {
    return Slice::Empty;
  }
  return Slice(filename.substr(pos + 1)).toLower();
}

std::list<Slice> Slice::split(const Slice& delims) const {
  std::string text = toString();
  std::string delimers = delims.toString();
  std::list<Slice> tokens;
  std::size_t start = text.find_first_not_of(delimers), end = 0;
  while ((end = text.find_first_of(delimers, start)) != std::string::npos) {
    tokens.push_back(Slice(str_ + start, end - start));
    start = text.find_first_not_of(delimers, end);
  }
  if (start != std::string::npos) {
    tokens.push_back(Slice(str_ + start));
  }
  return tokens;
}

float Slice::stof(const Slice& str) {
  return static_cast<float>(std::atof(str.toString().c_str()));
}

int Slice::stoi(const Slice& str) {
  return std::atoi(str.toString().c_str());
}

} // namespace slice

} // namespace silly
