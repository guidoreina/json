#include <stdint.h>
#include "json/null.hpp"

json::null::null()
  : value{type::null}
{
}

bool json::null::serialize(print::format format,
                           ::string::buffer& buf,
                           size_t depth) const
{
  return buf.append("null", 4);
}

bool json::null::parse(const void* buf, size_t bufsize, size_t& len)
{
  if (bufsize >= 4) {
    // Make `b` point to the beginning of the buffer.
    const uint8_t* const b = static_cast<const uint8_t*>(buf);

    if ((b[0] == 'n') &&
        (b[1] == 'u') &&
        (b[2] == 'l') &&
        (b[3] == 'l')) {
      len = 4;

      return true;
    }
  }

  return false;
}
