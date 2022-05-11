#include <stdlib.h>
#include <string.h>
#include "json/object.hpp"

json::object::object()
  : json::value{type::object}
{
}

json::object::~object()
{
  if (_M_members) {
    // For each member...
    for (size_t i = 0; i < _M_used; i++) {
      // Delete member name.
      delete _M_members[i].name;

      // Delete member value.
      delete _M_members[i].val;
    }

    free(_M_members);
  }
}

void json::object::swap(object& other)
{
  member* const members = _M_members;
  _M_members = other._M_members;
  other._M_members = members;

  const size_t size = _M_size;
  _M_size = other._M_size;
  other._M_size = size;

  const size_t used = _M_used;
  _M_used = other._M_used;
  other._M_used = used;
}

bool json::object::add(string* name, class value* val)
{
  // Allocate space for new members.
  if (allocate()) {
    // Save member name.
    _M_members[_M_used].name = name;

    // Save member value and increment number of members.
    _M_members[_M_used++].val = val;

    return true;
  }

  return false;
}

bool json::object::empty() const
{
  return (_M_used == 0);
}

size_t json::object::count() const
{
  return _M_used;
}

const class json::value* json::object::value(const char* name) const
{
  // Compute length of the name.
  const size_t namelen = strlen(name);

  // For each member...
  for (size_t i = 0; i < _M_used; i++) {
    // If it is the member we are searching for...
    if (_M_members[i].name->equals(name, namelen)) {
      return _M_members[i].val;
    }
  }

  return nullptr;
}

bool json::object::get(size_t idx,
                       const string*& name,
                       const class value*& val) const
{
  if (idx < _M_used) {
    name = _M_members[idx].name;
    val = _M_members[idx].val;

    return true;
  }

  return false;
}

bool json::object::serialize(print::format format,
                             ::string::buffer& buf,
                             size_t depth) const
{
  // If the object is not empty...
  if (_M_used > 0) {
    // Pretty print?
    if (format == print::format::pretty) {
      if (buf.append("{\n", 2)) {
        depth++;

        // For each member...
        for (size_t i = 0; i < _M_used; i++) {
          // If not the first member...
          if (i > 0) {
            if (!buf.append(",\n", 2)) {
              return false;
            }
          }

          if ((!buf.append(depth * print::indent, ' ')) ||
              (!_M_members[i].name->serialize(format, buf)) ||
              (!buf.append(": ", 2)) ||
              (!_M_members[i].val->serialize(format, buf, depth))) {
            return false;
          }
        }

        return ((buf.push_back('\n')) &&
                (buf.append(--depth * print::indent, ' ')) &&
                (buf.push_back('}')));
      }
    } else {
      if (buf.push_back('{')) {
        // For each member...
        for (size_t i = 0; i < _M_used; i++) {
          // If not the first member...
          if (i > 0) {
            if (!buf.push_back(',')) {
              return false;
            }
          }

          if ((!_M_members[i].name->serialize(format, buf)) ||
              (!buf.push_back(':')) ||
              (!_M_members[i].val->serialize(format, buf))) {
            return false;
          }
        }

        return buf.push_back('}');
      }
    }

    return false;
  } else {
    return buf.append("{}", 2);
  }
}

bool json::object::allocate()
{
  if (_M_used < _M_size) {
    return true;
  } else {
    const size_t size = (_M_size > 0) ? _M_size * 2 : allocation;

    member* const members = static_cast<member*>(
                              realloc(_M_members, size * sizeof(member))
                            );

    if (members) {
      _M_members = members;
      _M_size = size;

      return true;
    } else {
      return false;
    }
  }
}
