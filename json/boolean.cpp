#include <stdint.h>
#include "json/boolean.hpp"

json::boolean::boolean()
  : value{type::boolean},
    _M_value{false}
{
}

json::boolean::boolean(bool val)
  : value{type::boolean},
    _M_value{val}
{
}

void json::boolean::init(bool val)
{
  _M_value = val;
}

void json::boolean::init(const boolean& other)
{
  _M_value = other._M_value;
}

json::boolean::operator bool() const
{
  return _M_value;
}

bool json::boolean::serialize(print::format format,
                              ::string::buffer& buf,
                              size_t depth) const
{
  return _M_value ? buf.append("true", 4) : buf.append("false", 5);
}

bool json::boolean::parse(const void* buf, size_t bufsize, size_t& len)
{
  if (bufsize >= 4) {
    // Make `b` point to the beginning of the buffer.
    const uint8_t* const b = static_cast<const uint8_t*>(buf);

    if ((b[0] == 't') &&
        (b[1] == 'r') &&
        (b[2] == 'u') &&
        (b[3] == 'e')) {
      _M_value = true;

      len = 4;

      return true;
    } else if ((bufsize >= 5) &&
               (b[0] == 'f') &&
               (b[1] == 'a') &&
               (b[2] == 'l') &&
               (b[3] == 's') &&
               (b[4] == 'e')) {
      _M_value = false;

      len = 5;

      return true;
    }
  }

  return false;
}
