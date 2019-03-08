/*-
 * Copyright 2012-2018 Matthew Endsley
 * Modified by Confetti and DeanoC
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#ifndef TINYSTL_STRING_H
#define TINYSTL_STRING_H

#include "allocator.h"
#include "stddef.h"
#include "hash.h"
#include <tinystl/string_view.h>
#include <cctype>
#include <cstring>
#include <cstdarg>
#include <cstdio>

namespace tinystl {

template<typename allocator>
class basic_string {
 public:
  typedef char value_type;
  typedef char *pointer;
  typedef const char *const_pointer;
  typedef char& reference;
  typedef const char& const_reference;
  typedef const_pointer iterator;
  typedef const_pointer const_iterator;
  typedef size_t size_type;

  basic_string();
  basic_string(const basic_string& other);
  basic_string(basic_string&& other);
  basic_string(const char *sz);
  basic_string(const char *sz, size_t len);
  basic_string(const string_view& other);
  ~basic_string();

  static const size_type npos;

  basic_string& operator=(const basic_string& other);
  basic_string& operator=(basic_string&& other);

  const char *c_str() const;
  char *c_str();
  const char& at(size_t index) const { return m_first[index]; }
  char& at(size_t index) { return m_first[index]; }

  size_type size() const;

  bool empty() const { return size() == 0; }

  void reserve(size_type size);
  void resize(size_type size);

  void clear();
  void append(const char *first, const char *last);
  void append(const char c) { append(&c, (&c) + 1); };
  void assign(const char *s, size_t n);
  void push_back(char c) { append(c); }

  char const *data() const { return c_str(); }
  char *data() { return c_str(); }
  char const *begin() const { return c_str(); }
  char *begin() { return c_str(); }
  char const *end() const { return c_str() + size(); }
  char *end() { return c_str() + size(); }

  char front() const { return *c_str(); }
  char back() const;

  void shrink_to_fit();
  void swap(basic_string& other);

  void replace(char replaceThis, char replaceWith, bool caseSensitive = true);

  size_type find(char c, size_type startPos, bool caseSensitive = true) const;
  size_type find(const basic_string& str, size_type startPos, bool caseSensitive = true) const;
  bool rfind(const char ch, size_t pos, unsigned int *index) const;
  size_type find_last(char c, size_type startPos = npos, bool caseSensitive = true) const;
  size_type find_last(basic_string const& str, size_type startPos = npos, bool caseSensitive = true) const;
  static basic_string format(const char *fmt, ...);

 private:
  pointer m_first;
  pointer m_last;
  pointer m_capacity;

  static const size_t c_nbuffer = 12;
  char m_buffer[12];
};

template<typename allocator>
const typename basic_string<allocator>::size_type
    basic_string<allocator>::npos = basic_string<allocator>::size_type(-1);

template<typename allocator>
inline basic_string<allocator>::basic_string()
    : m_first(m_buffer), m_last(m_buffer), m_capacity(m_buffer + c_nbuffer) {
  resize(0);
}

template<typename allocator>
inline basic_string<allocator>::basic_string(const basic_string& other)
    : m_first(m_buffer), m_last(m_buffer), m_capacity(m_buffer + c_nbuffer) {
  reserve(other.size());
  append(other.m_first, other.m_last);
}

template<typename allocator>
inline basic_string<allocator>::basic_string(basic_string&& other) {
  if (other.m_first == other.m_buffer) {
    m_first = m_buffer;
    m_last = m_buffer;
    m_capacity = m_buffer + c_nbuffer;
    reserve(other.size());
    append(other.m_first, other.m_last);
  } else {
    m_first = other.m_first;
    m_last = other.m_last;
    m_capacity = other.m_capacity;
  }
  other.m_first = other.m_last = other.m_buffer;
  other.m_capacity = other.m_buffer + c_nbuffer;
  other.resize(0);
}

template<typename allocator>
inline basic_string<allocator>::basic_string(const char *sz)
    : m_first(m_buffer), m_last(m_buffer), m_capacity(m_buffer + c_nbuffer) {
  size_t len = 0;
  for (const char *it = sz; *it; ++it) {
    ++len;
  }

  reserve(len);
  append(sz, sz + len);
}

template<typename allocator>
inline basic_string<allocator>::basic_string(const char *sz, size_t len)
    : m_first(m_buffer), m_last(m_buffer), m_capacity(m_buffer + c_nbuffer) {
  reserve(len);
  append(sz, sz + len);
}

template<typename allocator>
inline basic_string<allocator>::basic_string(const tinystl::string_view& other) {
  basic_string(other.data(), other.size());
}

template<typename allocator>
inline basic_string<allocator>::~basic_string() {
  if (m_first != m_buffer) {
    allocator::static_deallocate(m_first, m_capacity - m_first);
  }
}

template<typename allocator>
inline basic_string<allocator>& basic_string<allocator>::operator=(const basic_string& other) {
  basic_string(other).swap(*this);
  return *this;
}

template<typename allocator>
inline basic_string<allocator>& basic_string<allocator>::operator=(basic_string&& other) {
  basic_string(static_cast<basic_string&&>(other)).swap(*this);
  return *this;
}

template<typename allocator>
inline const char *basic_string<allocator>::c_str() const {
  return m_first;
}

template<typename allocator>
inline char *basic_string<allocator>::c_str() {
  return m_first;
}

template<typename allocator>
inline typename basic_string<allocator>::size_type basic_string<allocator>::size() const {
  return (size_t) (m_last - m_first);
}

template<typename allocator>
inline void basic_string<allocator>::reserve(size_type capacity) {
  if (m_first + capacity + 1 <= m_capacity) {
    return;
  }

  const size_t size = (size_t) (m_last - m_first);

  pointer newfirst = (pointer) allocator::static_allocate(capacity + 1);
  for (pointer it = m_first, newit = newfirst, end = m_last; it != end; ++it, ++newit) {
    *newit = *it;
  }
  if (m_first != m_buffer) {
    allocator::static_deallocate(m_first, m_capacity - m_first);
  }

  m_first = newfirst;
  m_last = newfirst + size;
  m_capacity = m_first + capacity;
}

template<typename allocator>
inline void basic_string<allocator>::resize(size_type size) {
  const size_t prevSize = m_last - m_first;
  reserve(size);
  if (size > prevSize) {
    for (pointer it = m_last, end = m_first + size + 1; it < end; ++it) {
      *it = 0;
    }
  } else if (m_last != m_first) {
    m_first[size] = 0;
  }

  m_last = m_first + size;
}

template<typename allocator>
inline void basic_string<allocator>::clear() {
  resize(0);
}

template<typename allocator>
inline void basic_string<allocator>::append(const char *first, const char *last) {
  const size_t newsize = (size_t) ((m_last - m_first) + (last - first) + 1);
  if (m_first + newsize > m_capacity) {
    reserve((newsize * 3) / 2);
  }

  for (; first != last; ++m_last, ++first) {
    *m_last = *first;
  }
  *m_last = 0;
}

template<typename allocator>
inline void basic_string<allocator>::assign(const char *sz, size_t n) {
  clear();
  append(sz, sz + n);
}

template<typename allocator>
inline void basic_string<allocator>::shrink_to_fit() {
  if (m_first == m_buffer) {
  } else if (m_last == m_first) {
    const size_t capacity = (size_t) (m_capacity - m_first);
    if (capacity) {
      allocator::static_deallocate(m_first, capacity + 1);
    }
    m_capacity = m_first;
  } else if (m_capacity != m_last) {
    const size_t size = (size_t) (m_last - m_first);
    char *newfirst = (pointer) allocator::static_allocate(size + 1);
    for (pointer in = m_first, out = newfirst;
    in != m_last + 1;
    ++in, ++out)
    *out = *in;
    if (m_first != m_capacity) {
      allocator::static_deallocate(m_first, m_capacity + 1 - m_first);
    }
    m_first = newfirst;
    m_last = newfirst + size;
    m_capacity = m_last;
  }
}

template<typename allocator>
inline void basic_string<allocator>::swap(basic_string& other) {
  const pointer tfirst = m_first, tlast = m_last, tcapacity = m_capacity;
  m_first = other.m_first, m_last = other.m_last, m_capacity = other.m_capacity;
  other.m_first = tfirst, other.m_last = tlast, other.m_capacity = tcapacity;

  char tbuffer[c_nbuffer];

  if (m_first == other.m_buffer) {
    for (pointer it = other.m_buffer, end = m_last, out = tbuffer; it != end; ++it, ++out) {
      *out = *it;
    }
  }

  if (other.m_first == m_buffer) {
    other.m_last = other.m_last - other.m_first + other.m_buffer;
    other.m_first = other.m_buffer;
    other.m_capacity = other.m_buffer + c_nbuffer;

    for (pointer it = other.m_first, end = other.m_last, in = m_buffer;
    it != end;
    ++it, ++in)
    *it = *in;
    *other.m_last = 0;
  }

  if (m_first == other.m_buffer) {
    m_last = m_last - m_first + m_buffer;
    m_first = m_buffer;
    m_capacity = m_buffer + c_nbuffer;

    for (pointer it = m_first, end = m_last, in = tbuffer;
    it != end;
    ++it, ++in)
    *it = *in;
    *m_last = 0;
  }
}

template<typename allocator>
inline void basic_string<allocator>::replace(char replaceThis, char replaceWith, bool caseSensitive /* = true*/) {
  if (caseSensitive) {
    for (unsigned i = 0; i < (unsigned) size(); ++i) {
      if (m_first[i] == replaceThis) {
        m_first[i] = replaceWith;
      }
    }
  } else {
    replaceThis = (char) ::tolower(replaceThis);
    for (unsigned i = 0; i < (unsigned) size(); ++i) {
      if (tolower(m_first[i]) == replaceThis) {
        m_first[i] = replaceWith;
      }
    }
  }
}

template<typename allocator>
inline typename basic_string<allocator>::size_type basic_string<allocator>::find_last(
    char c,
    size_t startPos /* = npos*/,
    bool caseSensitive /* = true*/) const {

  if (startPos >= size()) {
    startPos = size() - 1;
  }

  if (caseSensitive) {
    for (size_type i = startPos; i < size(); --i) {
      if (m_first[i] == c) {
        return i;
      }
    }
  } else {
    c = (char) tolower(c);
    for (size_type i = startPos; i < size(); --i) {
      if (tolower(m_first[i]) == c) {
        return i;
      }
    }
  }

  return npos;
}

template<typename allocator>
inline typename basic_string<allocator>::size_type basic_string<allocator>::find_last(
    basic_string<allocator> const& str, size_type startPos /* = npos*/, bool caseSensitive /* = true*/) const {
  if (!str.size() || str.size() > size()) {
    return npos;
  }
  if (startPos > size() - str.size()) {
    startPos = size() - str.size();
  }

  char first = *str.m_first;
  if (!caseSensitive) {
    first = (char) tolower(first);
  }

  for (size_type i = startPos; i < size(); --i) {
    char c = m_first[i];
    if (!caseSensitive) {
      c = (char) tolower(c);
    }

    if (c == first) {
      bool found = true;
      for (size_type j = 1; j < str.size(); ++j) {
        c = m_first[i + j];
        char d = str.m_first[j];
        if (!caseSensitive) {
          c = (char) tolower(c);
          d = (char) tolower(d);
        }

        if (c != d) {
          found = false;
          break;
        }
      }
      if (found) {
        return i;
      }
    }
  }

  return npos;
}

template<typename allocator>
inline typename basic_string<allocator>::size_type basic_string<allocator>::find(char c,
                                                                                 size_type startPos,
                                                                                 bool caseSensitive /* = true*/) const {
  if (caseSensitive) {
    for (size_type i = startPos; i < size(); ++i) {
      if (m_first[i] == c) {
        return i;
      }
    }
  } else {
    c = (char) tolower(c);
    for (size_type i = startPos; i < size(); ++i) {
      if (tolower(m_first[i]) == c) {
        return i;
      }
    }
  }

  return npos;
}

template<typename allocator>
inline typename basic_string<allocator>::size_type basic_string<allocator>::find(const basic_string& str,
                                                                                 size_t startPos,
                                                                                 bool caseSensitive /* = true*/) const {
  if (!str.size() || str.size() > size()) {
    return npos;
  }

  char first = str.m_first[0];
  if (!caseSensitive) {
    first = (char) tolower(first);
  }

  for (size_type i = startPos; i <= size() - str.size(); ++i) {
    char c = m_first[i];
    if (!caseSensitive) {
      c = (char) tolower(c);
    }

    if (c == first) {
      size_type skip = npos;
      bool found = true;
      for (unsigned j = 1; j < (unsigned) str.size(); ++j) {
        c = m_first[i + j];
        char d = str.m_first[j];
        if (!caseSensitive) {
          c = (char) tolower(c);
          d = (char) tolower(d);
        }

        if (skip == npos && c == first) {
          skip = i + j - 1;
        }

        if (c != d) {
          found = false;
          if (skip != npos) {
            i = skip;
          }
          break;
        }
      }
      if (found) {
        return i;
      }
    }
  }

  return npos;
}

template<typename allocator>
inline bool basic_string<allocator>::rfind(const char ch, size_type pos, unsigned int *index) const {
  size_type i = (pos < 0) ? size() : pos;

  while (i) {
    i--;
    if (m_first[i] == ch) {
      if (index != nullptr) {
        *index = i;
        return true;
      }
    }
  }

  return false;
}

template<typename allocator>
inline char basic_string<allocator>::back() const {
  if (empty()) { return 0; }
  else { return *(c_str() + size() - 1); }
}

template<typename allocator>
basic_string<allocator> basic_string<allocator>::format(const char *fmt, ...) {
  int size = int(strlen(fmt) * 2 + 50);
  basic_string str;
  va_list ap;
  while (1) {     // Maximum two passes on a POSIX system...
    str.resize(size);
    va_start(ap, fmt);
    int n = vsnprintf((char *) str.c_str(), size, fmt, ap);
    va_end(ap);
    if (n > -1 && n < size) {  // Everything worked
      str.resize(n);
      return str;
    }
    if (n > -1) {  // Needed size returned
      size = n + 1;   // For null char
    } else {
      size *= 2;
    }      // Guess at a larger size (OS specific)
  }
  return str;
}

template<typename allocatorl, typename allocatorr>
inline bool operator==(const basic_string<allocatorl>& lhs, const basic_string<allocatorr>& rhs) {
  typedef const char *pointer;

  const size_t lsize = lhs.size(), rsize = rhs.size();
  if (lsize != rsize) {
    return false;
  }

  pointer lit = lhs.c_str(), rit = rhs.c_str();
  pointer lend = lit + lsize;
  while (lit != lend) {
    if (*lit++ != *rit++) {
      return false;
    }
  }

  return true;
}

template<typename allocator>
inline bool operator==(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {

  const size_t lsize = lhs.size(), rsize = rhs.size();
  if (lsize != rsize) {
    return false;
  }

  // use memcmp - this is usually an intrinsic on most compilers
  return memcmp(lhs.c_str(), rhs.c_str(), lsize) == 0;
}

template<typename allocator>
inline bool operator==(const basic_string<allocator>& lhs, const char *rhs) {

  const size_t lsize = lhs.size(), rsize = strlen(rhs);
  if (lsize != rsize) {
    return false;
  }

  return memcmp(lhs.c_str(), rhs, lsize) == 0;
}

template<typename allocator>
inline bool operator<(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {

  const size_t lsize = lhs.size(), rsize = rhs.size();
  if (lsize != rsize) {
    return lsize < rsize;
  }
  return memcmp(lhs.c_str(), rhs.c_str(), lsize) < 0;
}

template<typename allocator>
inline bool operator>(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {

  const size_t lsize = lhs.size(), rsize = rhs.size();
  if (lsize != rsize) {
    return lsize > rsize;
  }
  return memcmp(lhs.c_str(), rhs.c_str(), lsize) > 0;
}

template<typename allocator>
inline bool operator<=(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {
  return !(lhs > rhs);
}

template<typename allocator>
inline bool operator>=(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {
  return !(lhs < rhs);
}

template<typename allocator>
inline bool operator!=(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {
  return !(lhs == rhs);
}

template<typename allocator>
inline bool operator!=(const basic_string<allocator>& lhs, const char *rhs) {
  return !(lhs == rhs);
}

template<typename allocator>
inline bool operator!=(const char *lhs, const basic_string<allocator>& rhs) {
  return !(rhs == lhs);
}

template<typename allocator>
inline basic_string<allocator> operator+(const basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {
  basic_string<allocator> ret(lhs);
  ret.append(rhs.begin(), rhs.end());
  return ret;
}

template<typename allocator>
inline basic_string<allocator>& operator+=(basic_string<allocator>& lhs, const basic_string<allocator>& rhs) {
  lhs.append(rhs.begin(), rhs.end());
  return lhs;
}

template<typename allocator>
inline basic_string<allocator> operator+(const basic_string<allocator>& lhs, char const *cs) {
  basic_string<allocator> ret(lhs);
  basic_string<allocator> rhs(cs);

  ret.append(rhs.begin(), rhs.end());
  return ret;
}

template<typename allocator>
inline basic_string<allocator>& operator+=(basic_string<allocator>& lhs, char const *cs) {
  basic_string<allocator> rhs(cs);
  lhs.append(rhs.begin(), rhs.end());
  return lhs;
}

template<typename allocator>
static inline size_t hash(const basic_string<allocator>& value) {
  return hash_string(value.c_str(), value.size());
}

typedef basic_string<TINYSTL_ALLOCATOR> string;
}

#endif
