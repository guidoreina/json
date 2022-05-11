#include "json/value.hpp"

json::value::value(enum type t)
  : _M_type{t}
{
}

json::value::~value()
{
}

enum json::value::type json::value::type() const
{
  return _M_type;
}
