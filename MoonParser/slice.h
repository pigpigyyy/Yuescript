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

#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <list>

namespace silly {

namespace slice {
// A Slice object wraps a "const char *" or a "const std::string&" but
// without copying their contents.
class Slice {
 private:
  struct TrustedInitTag {};
  constexpr Slice(const char* s, size_t n, TrustedInitTag) : str_(s), len_(n) {}

 public:
  // implicit conversion from std::string to Slice
  Slice(const std::string &s) : Slice(s.data(), s.size()) {}

  Slice(const char *s) : Slice(s, s ? strlen(s) : 0) {}

  Slice(const char *s, size_t n) : str_(s), len_(n) {}

  Slice(std::pair<const char*,size_t> sp) : str_(sp.first), len_(sp.second) {}

  constexpr Slice(std::nullptr_t p = nullptr) : str_(nullptr), len_(0) {}

  operator std::string() const {
  	return std::string(str_, len_);
  }

  const char &operator[](size_t n) const {
    return str_[n];
  }

  size_t size() const {
    return len_;
  }

  const char *rawData() const {
    return str_;
  }

  std::string toString() const {
    return std::string(str_, len_);
  }

  bool empty() const {
    return len_ == 0;
  }

  // similar with std::string::compare
  // http://en.cppreference.com/w/cpp/string/basic_string/compare
  int compare(const Slice &rhs) const;

  void skip(size_t n) {
    assert(n <= len_);
    str_ += n;
    len_ -= n;
  }

  void copyTo(char *dest, bool appendEndingNull = true) const {
    memcpy(dest, str_, len_);
    if (appendEndingNull) {
      dest[len_] = '\0';
    }
  }

  Slice &trimSpace() {
    assert(len_ > 0);
    size_t start = 0, end = len_ - 1;
    while (start < end && isspace(str_[start])) {
      start++;
    }
    while (start < end && isspace(str_[end])) {
      end--;
    }
    str_ += start;
    len_ = end - start + 1;
    return *this;
  }

  typedef const char *const_iterator;

  const_iterator begin() const {
    return str_;
  }

  const_iterator end() const {
    return str_ + len_;
  }

  std::string getFilePath() const;
  std::string getFileName() const;
  std::string getFileExtension() const;
  std::string toLower() const;
  std::string toUpper() const;
  std::list<Slice> split(const Slice& delimer) const;

  static const std::string Empty;
  static float stof(const Slice& str);
  static int stoi(const Slice& str);

  constexpr friend Slice operator"" _slice(const char* s, size_t n);
 private:
  const char *str_;
  size_t len_;
};

inline Slice trimSpace(const Slice &s) {
  Slice tmp = s;
  return tmp.trimSpace();
}

inline bool operator==(const Slice &lhs, const Slice &rhs) {
  return lhs.compare(rhs) == 0;
}

inline bool operator!=(const Slice &lhs, const Slice &rhs) {
  return !(lhs == rhs);
}

inline std::string operator+(const std::string& lhs, const Slice &rhs) {
  return lhs + rhs.toString();
}

constexpr Slice operator"" _slice(const char* s, size_t n) {
	return Slice(s, n, Slice::TrustedInitTag{});
}

} // namespace slice

}  // namespace silly
