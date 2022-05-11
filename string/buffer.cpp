#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string/buffer.hpp"

string::buffer::~buffer()
{
  if (_M_data) {
    free(_M_data);
  }
}

void string::buffer::swap(buffer& other)
{
  char* const data = _M_data;
  _M_data = other._M_data;
  other._M_data = data;

  const size_t size = _M_size;
  _M_size = other._M_size;
  other._M_size = size;

  const size_t used = _M_used;
  _M_used = other._M_used;
  other._M_used = used;
}

void string::buffer::clear()
{
  _M_used = 0;
}

bool string::buffer::empty() const
{
  return (_M_used == 0);
}

const char* string::buffer::data() const
{
  return _M_data;
}

size_t string::buffer::length() const
{
  return _M_used;
}

bool string::buffer::append(const char* buf, size_t len)
{
  // If the string is not empty...
  if (len > 0) {
    // Reserve memory.
    if (reserve(len)) {
      // Append string.
      memcpy(_M_data + _M_used, buf, len);

      // Increment length of the string.
      _M_used += len;

      return true;
    }

    return false;
  }

  return true;
}

bool string::buffer::append(const char* buf)
{
  return append(buf, strlen(buf));
}

bool string::buffer::append(size_t count, char ch)
{
  // If there are characters to be appended...
  if (count > 0) {
    // Reserve memory.
    if (reserve(count)) {
      // Append characters.
      memset(_M_data + _M_used, ch, count);

      // Increment length of the string.
      _M_used += count;

      return true;
    }

    return false;
  }

  return true;
}

bool string::buffer::push_back(char c)
{
  // Reserve memory.
  if (reserve(1)) {
    // Append character and increment length of the string.
    _M_data[_M_used++] = c;

    return true;
  }

  return false;
}

bool string::buffer::format(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  const bool ret = vformat(format, ap);

  va_end(ap);

  return ret;
}

bool string::buffer::vformat(const char* format, va_list ap)
{
  // Reserve memory.
  if (reserve(initial_size)) {
    // Compute how much space is left in the buffer.
    size_t size = _M_size - _M_used;

    do {
      va_list aq;
      va_copy(aq, ap);

      const int n = vsnprintf(_M_data + _M_used, size, format, aq);

      va_end(aq);

      if (n > -1) {
        // If the buffer is big enough...
        if (static_cast<size_t>(n) < size) {
          // Increment length of the string.
          _M_used += n;

          return true;
        } else {
          size = n + 1;
        }
      } else {
        size *= 2;
      }
    } while (reserve(size));
  }

  return false;
}

bool string::buffer::reserve(size_t n)
{
  // Compute length of the string.
  const size_t len = _M_used + n;

  // If `len` doesn't overflow...
  if (len >= n) {
    // If the string won't become too long...
    if (len <= max_length) {
      // If we don't have to reallocate memory...
      if (len <= _M_size) {
        return true;
      }

      // If not the first allocation...
      size_t size;
      if (_M_size > 0) {
        // Double the size.
        size = _M_size * 2;

        // If `size` overflows...
        if (size < _M_size) {
          return false;
        }
      } else {
        size = initial_size;
      }

      while (size < len) {
        // Double the size.
        const size_t tmp = size * 2;

        // If `tmp` doesn't overflow...
        if (tmp > size) {
          size = tmp;
        } else {
          return false;
        }
      }

      char* const data = static_cast<char*>(realloc(_M_data, size));
      if (data) {
        _M_data = data;
        _M_size = size;

        return true;
      }
    }
  }

  return false;
}
