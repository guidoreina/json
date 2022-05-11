#include <stdlib.h>
#include <string.h>
#include "json/string.hpp"
#include "json/ctype.hpp"

json::string::string()
  : value{type::string}
{
}

json::string::~string()
{
  if (_M_data) {
    free(_M_data);
  }
}

void json::string::swap(string& other)
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

bool json::string::init(const void* buf, size_t len)
{
  // If the string is not empty...
  if (len > 0) {
    // Reserve memory.
    if (reserve(len)) {
      // Copy string.
      memcpy(_M_data, buf, len);

      // Append terminating '\0' character.
      _M_data[len] = 0;

      // Save length of the string.
      _M_used = len;

      return true;
    }

    return false;
  }

  return true;
}

bool json::string::init(const char* buf)
{
  return init(buf, strlen(buf));
}

const char* json::string::data() const
{
  return (_M_used > 0) ? _M_data : "";
}

size_t json::string::length() const
{
  return _M_used;
}

bool json::string::equals(const void* buf, size_t len) const
{
  return ((len == _M_used) && (memcmp(buf, _M_data, len) == 0));
}

bool json::string::equals(const char* buf) const
{
  return (strcmp(buf, data()) == 0);
}

bool json::string::serialize(print::format format,
                             ::string::buffer& buf,
                             size_t depth) const
{
  // Push opening quotation mark.
  if (buf.push_back('"')) {
    static constexpr const char hex[] = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    // Make `end` point to the end of the buffer `_M_data`.
    const char* const end = _M_data + _M_used;

    // Make `begin` point to the beginning of the buffer `_M_data`.
    const char* begin = _M_data;

    const char* str = nullptr;

    const char* ptr = begin;
    while (ptr < end) {
      const uint8_t c = static_cast<uint8_t>(*ptr);

      if (c <= 0x7f) {
        char escaped[6];
        size_t len;

        switch (c) {
          default:
            if (c >= ' ') {
              ptr++;
              continue;
            } else {
              escaped[0] = '\\';
              escaped[1] = 'u';
              escaped[2] = '0';
              escaped[3] = '0';
              escaped[4] = hex[(c >> 4) & 0x0f];
              escaped[5] = hex[c & 0x0f];

              str = escaped;
              len = 6;
            }

            break;
          case '"':
            str = "\\\"";
            len = 2;

            break;
          case '\\':
            str = "\\\\";
            len = 2;

            break;
          case '/':
            str = "\\/";
            len = 2;

            break;
          case '\b':
            str = "\\b";
            len = 2;

            break;
          case '\f':
            str = "\\f";
            len = 2;

            break;
          case '\n':
            str = "\\n";
            len = 2;

            break;
          case '\r':
            str = "\\r";
            len = 2;

            break;
          case '\t':
            str = "\\t";
            len = 2;

            break;
        }

        // If there are some characters to be appended...
        if (begin < ptr) {
          // Append characters.
          if (!buf.append(begin, ptr - begin)) {
            return false;
          }
        }

        // Append UTF-8 character.
        if (buf.append(str, len)) {
          // Skip UTF-8 character.
          ptr++;

          // Make `begin` point after the UTF-8 character.
          begin = ptr;
        } else {
          return false;
        }
      } else {
        // Convert from UTF-8.
        uint32_t codepoint;
        size_t len;
        if ((from_utf8(ptr, end - ptr, codepoint, len)) &&
            (codepoint <= 0x10ffffu)) {
          // If there are some characters to be appended...
          if (begin < ptr) {
            // Append characters.
            if (!buf.append(begin, ptr - begin)) {
              return false;
            }
          }

          char escaped[12];
          size_t escapedlen;

          // If not a UTF-16 surrogate pair...
          if ((codepoint < 0xd800u) ||
              ((codepoint > 0xdfffu) && (codepoint <= 0xffffu))) {
            escaped[0] = '\\';
            escaped[1] = 'u';
            escaped[2] = hex[(codepoint >> 12) & 0x0f];
            escaped[3] = hex[(codepoint >> 8) & 0x0f];
            escaped[4] = hex[(codepoint >> 4) & 0x0f];
            escaped[5] = hex[codepoint & 0x0f];

            escapedlen = 6;
          } else if (codepoint >= 0x10000u) {
            codepoint -= 0x10000u;
            const uint32_t hi = 0xd800u + (codepoint >> 10);
            const uint32_t lo = 0xdc00u + (codepoint & 0x3ffu);

            escaped[0] = '\\';
            escaped[1] = 'u';
            escaped[2] = hex[(hi >> 12) & 0x0f];
            escaped[3] = hex[(hi >> 8) & 0x0f];
            escaped[4] = hex[(hi >> 4) & 0x0f];
            escaped[5] = hex[hi & 0x0f];

            escaped[6] = '\\';
            escaped[7] = 'u';
            escaped[8] = hex[(lo >> 12) & 0x0f];
            escaped[9] = hex[(lo >> 8) & 0x0f];
            escaped[10] = hex[(lo >> 4) & 0x0f];
            escaped[11] = hex[lo & 0x0f];

            escapedlen = 12;
          } else {
            return false;
          }

          // Append UTF-8 character.
          if (buf.append(escaped, escapedlen)) {
            // Skip UTF-8 character.
            ptr += len;

            // Make `begin` point after the UTF-8 character.
            begin = ptr;
          } else {
            return false;
          }
        } else {
          return false;
        }
      }
    }

    // If there are some characters to be appended...
    if (begin < ptr) {
      // Append characters.
      if (!buf.append(begin, ptr - begin)) {
        return false;
      }
    }

    // Push closing quotation mark.
    return buf.push_back('"');
  }

  return false;
}

bool json::string::parse(const void* buf, size_t bufsize, size_t& len)
{
  // Make `b` point to the beginning of the buffer.
  // `b` points to the opening quotation mark.
  const uint8_t* const b = static_cast<const uint8_t*>(buf);

  // Make `end` point to the end of the buffer.
  const uint8_t* const end = b + bufsize;

  // Make `begin` point after the opening quotation mark.
  const uint8_t* begin = b + 1;

  const uint8_t* ptr = begin;
  while (ptr < end) {
    uint8_t c = *ptr;

    switch (c) {
      default:
        // If the character is valid...
        if (c >= ' ') {
          ptr++;
        } else {
          return false;
        }

        break;
      case '"':
        // If there are some characters to be appended...
        if (begin < ptr) {
          // Append characters.
          if (!append(begin, ptr - begin)) {
            return false;
          }
        }

        // If the string is not empty...
        if (_M_used > 0) {
          // Append terminating '\0' character.
          _M_data[_M_used] = 0;
        }

        // Save length of the string with the quotation marks.
        len = ptr - b + 1;

        return true;
      case '\\':
        // If there is space for at least 2 characters...
        if (ptr + 2 < end) {
          switch (ptr[1]) {
            case '"':
            case '\\':
            case '/':
              c = ptr[1];
              break;
            case 'b':
              c = '\b';
              break;
            case 'f':
              c = '\f';
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'u':
              // If there is space for at least 6 characters...
              if (ptr + 6 < end) {
                // If there are 4 hexadecimal characters after the 'u'...
                uint8_t hex[2];
                if ((to_hex(ptr[2], ptr[3], hex[0])) &&
                    (to_hex(ptr[4], ptr[5], hex[1]))) {
                  // Compute code point.
                  uint32_t codepoint = static_cast<uint32_t>(
                                         static_cast<uint32_t>(
                                           static_cast<uint32_t>(hex[0]) << 8
                                         ) |
                                         static_cast<uint32_t>(hex[1])
                                       );

                  // If the code point is valid...
                  if ((codepoint < 0xdc00u) || (codepoint > 0xdfffu)) {
                    // If not a UTF-16 surrogate pair...
                    if ((codepoint < 0xd800u) || (codepoint > 0xdbffu)) {
                      // Convert code point to UTF-8.
                      char utf8[4];
                      size_t l;
                      if (to_utf8(codepoint, utf8, l)) {
                        // If there are some characters to be appended...
                        if (begin < ptr) {
                          // Append characters.
                          if (!append(begin, ptr - begin)) {
                            return false;
                          }
                        }

                        // Append UTF-8 character.
                        if (append(utf8, l)) {
                          // Skip "\uXXXX".
                          ptr += 6;

                          // Make `begin` point after the hexadecimal characters.
                          begin = ptr;

                          // Go to the beginning of the while loop.
                          continue;
                        }
                      }
                    } else {
                      // UTF-16 surrogate pair.

                      // If there is space for at least 12 characters and
                      // "\uXXXX" follows...
                      if ((ptr + 12 < end) &&
                          (ptr[6] == '\\') &&
                          (ptr[7] == 'u') &&
                          (to_hex(ptr[8], ptr[9], hex[0])) &&
                          (to_hex(ptr[10], ptr[11], hex[1]))) {
                        // Compute low surrogate.
                        const uint32_t
                          lo = static_cast<uint32_t>(
                                 static_cast<uint32_t>(
                                   static_cast<uint32_t>(hex[0]) << 8
                                 ) |
                                 static_cast<uint32_t>(hex[1])
                               );

                        // If the low surrogate is valid...
                        if ((lo >= 0xdc00u) && (lo <= 0xdfffu)) {
                          // Compute code point.
                          codepoint = 0x10000u +
                                      ((codepoint - 0xd800u) << 10) +
                                      (lo - 0xdc00u);

                          // Convert code point to UTF-8.
                          char utf8[4];
                          size_t l;
                          if (to_utf8(codepoint, utf8, l)) {
                            // If there are some characters to be appended...
                            if (begin < ptr) {
                              // Append characters.
                              if (!append(begin, ptr - begin)) {
                                return false;
                              }
                            }

                            // Append UTF-8 character.
                            if (append(utf8, l)) {
                              // Skip "\uXXXX\uXXXX".
                              ptr += 12;

                              // Make `begin` point after the hexadecimal
                              // characters.
                              begin = ptr;

                              // Go to the beginning of the while loop.
                              continue;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }

              [[fallthrough]];
            default:
              return false;
          }

          // If there are some characters to be appended...
          if (begin < ptr) {
            // Append characters.
            if (!append(begin, ptr - begin)) {
              return false;
            }
          }

          // Append character.
          if (push_back(c)) {
            // Skip escape character.
            ptr += 2;

            // Make `begin` point to the next character.
            begin = ptr;

            continue;
          }
        }

        return false;
    }
  }

  return false;
}

bool json::string::append(const void* buf, size_t len)
{
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

bool json::string::push_back(uint8_t c)
{
  // Reserve memory.
  if (reserve(1)) {
    // Append character and increment length of the string.
    _M_data[_M_used++] = static_cast<char>(c);

    return true;
  }

  return false;
}

bool json::string::reserve(size_t n)
{
  // Compute length of the string.
  const size_t len = _M_used + n;

  // If `len` doesn't overflow...
  if (len >= n) {
    // If the string won't become too long...
    if (len <= max_length) {
      // If we don't have to reallocate memory...
      if (len < _M_size) {
        return true;
      }

      // Add one for the terminating '\0' character.
      n = len + 1;

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

      while (size < n) {
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

bool json::string::to_hex(uint8_t c, uint8_t& n)
{
  if (isdigit(c)) {
    n = c - '0';
    return true;
  } else if ((c >= 'A') && (c <= 'F')) {
    n = 10 + (c - 'A');
    return true;
  } else if ((c >= 'a') && (c <= 'f')) {
    n = 10 + (c - 'a');
    return true;
  }

  return false;
}

bool json::string::to_hex(uint8_t c1, uint8_t c2, uint8_t& n)
{
  uint8_t n1, n2;
  if ((to_hex(c1, n1)) && (to_hex(c2, n2))) {
    n = static_cast<uint8_t>(static_cast<uint8_t>(n1 << 4) | n2);
    return true;
  }

  return false;
}

bool json::string::from_utf8(const void* buf,
                             size_t len,
                             uint32_t& c,
                             size_t& utf8len)
{
  // Make `b` point to the beginning of the buffer `buf`.
  const uint8_t* const b = static_cast<const uint8_t*>(buf);

  if (b[0] <= 0x7f) {
    c = b[0];
    utf8len = 1;

    return true;
  } else if (b[0] < 0xe0) {
    if ((len >= 2) && ((b[1] & 0xc0) == 0x80)) {
      c = static_cast<uint32_t>(
            static_cast<uint32_t>(static_cast<uint32_t>(b[0] & 0x1fu) << 6) |
            static_cast<uint32_t>(b[1] & 0x3fu)
          );

      utf8len = 2;

      return true;
    }
  } else if (b[0] < 0xf0) {
    if ((len >= 3) && ((b[1] & 0xc0) == 0x80) && ((b[2] & 0xc0) == 0x80)) {
      c = static_cast<uint32_t>(
            static_cast<uint32_t>(static_cast<uint32_t>(b[0] & 0x0fu) << 12) |
            static_cast<uint32_t>(static_cast<uint32_t>(b[1] & 0x3fu) << 6) |
            static_cast<uint32_t>(b[2] & 0x3fu)
          );

      utf8len = 3;

      return true;
    }
  } else if (b[0] < 0xf8) {
    if ((len >= 4) &&
        ((b[1] & 0xc0) == 0x80) &&
        ((b[2] & 0xc0) == 0x80) &&
        ((b[3] & 0xc0) == 0x80)) {
      c = static_cast<uint32_t>(
            static_cast<uint32_t>(static_cast<uint32_t>(b[0] & 0x07u) << 18) |
            static_cast<uint32_t>(static_cast<uint32_t>(b[1] & 0x3fu) << 12) |
            static_cast<uint32_t>(static_cast<uint32_t>(b[2] & 0x3fu) << 6) |
            static_cast<uint32_t>(b[3] & 0x3fu)
          );

      utf8len = 4;

      return true;
    }
  }

  return false;
}

bool json::string::to_utf8(uint32_t c, char* buf, size_t& len)
{
  if (c <= 0x7fu) {
    buf[0] = static_cast<char>(c);

    len = 1;

    return true;
  } else if (c <= 0x07ffu) {
    buf[0] = static_cast<char>(0xc0u | static_cast<uint8_t>((c >> 6) & 0x1fu));
    buf[1] = static_cast<char>(0x80u | static_cast<uint8_t>(c & 0x3fu));

    len = 2;

    return true;
  } else if (c <= 0xffffu) {
    buf[0] = static_cast<char>(0xe0u | static_cast<uint8_t>((c >> 12) & 0x0fu));
    buf[1] = static_cast<char>(0x80u | static_cast<uint8_t>((c >> 6) & 0x3fu));
    buf[2] = static_cast<char>(0x80u | static_cast<uint8_t>(c & 0x3fu));

    len = 3;

    return true;
  } else if (c <= 0x10ffffu) {
    buf[0] = static_cast<char>(0xf0u | static_cast<uint8_t>((c >> 18) & 0x07u));
    buf[1] = static_cast<char>(0x80u | static_cast<uint8_t>((c >> 12) & 0x3fu));
    buf[2] = static_cast<char>(0x80u | static_cast<uint8_t>((c >> 6) & 0x3fu));
    buf[3] = static_cast<char>(0x80u | static_cast<uint8_t>(c & 0x3fu));

    len = 4;

    return true;
  }

  return false;
}
