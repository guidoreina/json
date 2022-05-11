#pragma once

#include <stdint.h>

namespace json {

// Is `c` a digit?
static inline bool isdigit(uint8_t c)
{
  return ((c >= '0') && (c <= '9'));
}

// Is `c` a whitespace?
static inline bool isws(uint8_t c)
{
  return ((c == ' ') || (c == '\n') || (c == '\t') || (c == '\r'));
}

} // namespace json
