#include <stdlib.h>
#include "json/array.hpp"

json::array::array()
  : value{type::array}
{
}

json::array::~array()
{
  if (_M_values) {
    // For each value...
    for (size_t i = 0; i < _M_used; i++) {
      // Delete value.
      delete _M_values[i];
    }

    free(_M_values);
  }
}

void json::array::swap(array& other)
{
  value** const values = _M_values;
  _M_values = other._M_values;
  other._M_values = values;

  const size_t size = _M_size;
  _M_size = other._M_size;
  other._M_size = size;

  const size_t used = _M_used;
  _M_used = other._M_used;
  other._M_used = used;
}

bool json::array::append(value* val)
{
  // Allocate space for new values.
  if (allocate()) {
    // Append value and increment number of values.
    _M_values[_M_used++] = val;

    return true;
  }

  return false;
}

bool json::array::empty() const
{
  return (_M_used == 0);
}

size_t json::array::count() const
{
  return _M_used;
}

const json::value* json::array::get(size_t idx) const
{
  return (idx < _M_used) ? _M_values[idx] : nullptr;
}

bool json::array::serialize(print::format format,
                            ::string::buffer& buf,
                            size_t depth) const
{
  // If the array is not empty...
  if (_M_used > 0) {
    // Pretty print?
    if (format == print::format::pretty) {
      if (buf.append("[\n", 2)) {
        depth++;

        // For each value...
        for (size_t i = 0; i < _M_used; i++) {
          // If not the first value...
          if (i > 0) {
            if (!buf.append(",\n", 2)) {
              return false;
            }
          }

          if ((!buf.append(depth * print::indent, ' ')) ||
              (!_M_values[i]->serialize(format, buf, depth))) {
            return false;
          }
        }

        return ((buf.push_back('\n')) &&
                (buf.append(--depth * print::indent, ' ')) &&
                (buf.push_back(']')));
      }
    } else {
      if (buf.push_back('[')) {
        // For each value...
        for (size_t i = 0; i < _M_used; i++) {
          // If not the first value...
          if (i > 0) {
            if (!buf.push_back(',')) {
              return false;
            }
          }

          if (!_M_values[i]->serialize(format, buf)) {
            return false;
          }
        }

        return buf.push_back(']');
      }
    }

    return false;
  } else {
    return buf.append("[]", 2);
  }
}

bool json::array::allocate()
{
  if (_M_used < _M_size) {
    return true;
  } else {
    const size_t size = (_M_size > 0) ? _M_size * 2 : allocation;

    value** const values = static_cast<value**>(
                             realloc(_M_values, size * sizeof(value*))
                           );

    if (values) {
      _M_values = values;
      _M_size = size;

      return true;
    } else {
      return false;
    }
  }
}
