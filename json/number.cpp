#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "json/number.hpp"
#include "json/ctype.hpp"

json::number::number()
  : value{value::type::number},
    _M_type{type::integer},
    _M_integer{0}
{
}

json::number::number(int64_t n)
  : value{value::type::number},
    _M_type{type::integer},
    _M_integer{n}
{
}

json::number::number(double n)
  : value{value::type::number},
    _M_type{type::real},
    _M_real{n}
{
}

void json::number::init(int64_t n)
{
  _M_type = type::integer;
  _M_integer = n;
}

void json::number::init(double n)
{
  _M_type = type::real;
  _M_real = n;
}

void json::number::init(const number& other)
{
  _M_type = other._M_type;
  _M_integer = other._M_integer;
}

enum json::number::type json::number::type() const
{
  return _M_type;
}

int64_t json::number::as_integer() const
{
  return _M_integer;
}

double json::number::as_real() const
{
  return _M_real;
}

bool json::number::serialize(print::format format,
                             ::string::buffer& buf,
                             size_t depth) const
{
  return (_M_type == type::integer) ? buf.format("%lld", _M_integer) :
                                      buf.format("%f", _M_real);
}

bool json::number::parse(const void* buf, size_t bufsize, size_t& len)
{
  // Make `b` point to the beginning of the buffer.
  const uint8_t* const b = static_cast<const uint8_t*>(buf);

  int64_t n = 0;
  int64_t sign = 1;

  int state = 0; // Initial state.

  const size_t max = (max_length < bufsize) ? max_length : bufsize;

  size_t i;
  for (i = 0; i < max; i++) {
    const uint8_t c = b[i];

    switch (state) {
      case 0: // Initial state.
        if ((c >= '1') && (c <= '9')) {
          n = c - '0';

          state = 3; // Parsing integer.
        } else if (c == '0') {
          state = 1; // Zero.
        } else if (c == '-') {
          sign = -1;

          state = 2; // After sign.
        } else {
          return false;
        }

        break;
      case 1: // Zero.
        if (c == '.') {
          state = 4; // After '.'.
        } else if ((c == 'E') || (c == 'e')) {
          state = 6; // After 'E'.
        } else if ((isws(c)) || (c == ',') || (c == '}') || (c == ']')) {
          _M_type = type::integer;
          _M_integer = 0;

          len = i;

          return true;
        } else {
          return false;
        }

        break;
      case 2: // After sign.
        if ((c >= '1') && (c <= '9')) {
          n = c - '0';

          state = 3; // Parsing integer.
        } else if (c == '0') {
          state = 1; // Zero.
        } else {
          return false;
        }

        break;
      case 3: // Parsing integer.
        if (isdigit(c)) {
          const int64_t tmp = (n * 10) + (c - '0');

          // If the number doesn't overflow...
          if (tmp > n) {
            n = tmp;
          } else {
            return false;
          }
        } else if (c == '.') {
          state = 4; // After '.'.
        } else if ((c == 'E') || (c == 'e')) {
          state = 6; // After 'E'.
        } else if ((isws(c)) || (c == ',') || (c == '}') || (c == ']')) {
          _M_type = type::integer;
          _M_integer = sign * n;

          len = i;

          return true;
        } else {
          return false;
        }

        break;
      case 4: // After '.'.
        if (isdigit(c)) {
          state = 5; // Parsing fraction.
        } else {
          return false;
        }

        break;
      case 5: // Parsing fraction.
        if ((c == 'E') || (c == 'e')) {
          state = 6; // After 'E'.
        } else if ((isws(c)) || (c == ',') || (c == '}') || (c == ']')) {
          // Copy number.
          char s[max_length + 1];
          memcpy(s, b, i);
          s[i] = 0;

          // Clear errno.
          errno = 0;

          char* endptr;
          const double d = strtod(s, &endptr);

          // Success?
          if ((errno == 0) && (!*endptr)) {
            _M_type = type::real;
            _M_real = d;

            len = i;

            return true;
          }

          return false;
        } else if (!isdigit(c)) {
          return false;
        }

        break;
      case 6: // After 'E'.
        if ((c == '+') || (c == '-')) {
          state = 7; // After sign in exponent.
        } else if (isdigit(c)) {
          state = 8; // Parsing exponent.
        } else {
          return false;
        }

        break;
      case 7: // After sign in exponent.
        if (isdigit(c)) {
          state = 8; // Parsing exponent.
        } else {
          return false;
        }

        break;
      case 8: // Parsing exponent.
        if ((isws(c)) || (c == ',') || (c == '}') || (c == ']')) {
          // Copy number.
          char s[max_length + 1];
          memcpy(s, b, i);
          s[i] = 0;

          // Clear errno.
          errno = 0;

          char* endptr;
          const double d = strtod(s, &endptr);

          // Success?
          if ((errno == 0) && (!*endptr)) {
            _M_type = type::real;
            _M_real = d;

            len = i;

            return true;
          }

          return false;
        } else if (!isdigit(c)) {
          return false;
        }

        break;
    }
  }

  // If we have reached the end of the buffer...
  if (i == bufsize) {
    switch (state) {
      case 1: // Zero.
        _M_type = type::integer;
        _M_integer = 0;

        len = i;

        return true;
      case 3: // Parsing integer.
        _M_type = type::integer;
        _M_integer = sign * n;

        len = i;

        return true;
      case 5: // Parsing fraction.
      case 8: // Parsing exponent.
        {
          // Copy number.
          char s[max_length + 1];
          memcpy(s, b, i);
          s[i] = 0;

          // Clear errno.
          errno = 0;

          char* endptr;
          const double d = strtod(s, &endptr);

          // Success?
          if ((errno == 0) && (!*endptr)) {
            _M_type = type::real;
            _M_real = d;

            len = i;

            return true;
          }
        }

        break;
    }
  }

  return false;
}
