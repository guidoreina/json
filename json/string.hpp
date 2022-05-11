#pragma once

#include <stdint.h>
#include "json/value.hpp"

namespace json {

// Forward declaration.
class document;

// JSON string.
class string final : public value {
  friend class document;

  public:
    // Maximum length of a JSON string.
    static constexpr const size_t max_length = 1024 * 1024;

    // Constructor.
    string();

    // Destructor.
    ~string();

    // Swap strings.
    void swap(string& other);

    // Initialize.
    bool init(const void* buf, size_t len);
    bool init(const char* buf);

    // Get string.
    const char* data() const;

    // Get length.
    size_t length() const;

    // Equals?
    bool equals(const void* buf, size_t len) const;
    bool equals(const char* buf) const;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Initial size of the string.
    static constexpr const size_t initial_size = 16;

    // String.
    char* _M_data = nullptr;
    size_t _M_size = 0;
    size_t _M_used = 0;

    // Parse.
    bool parse(const void* buf, size_t bufsize, size_t& len);

    // Append.
    bool append(const void* buf, size_t len);

    // Push back a character.
    bool push_back(uint8_t c);

    // Reserve memory.
    bool reserve(size_t n);

    // Convert to hexadecimal.
    static bool to_hex(uint8_t c, uint8_t& n);
    static bool to_hex(uint8_t c1, uint8_t c2, uint8_t& n);

    // Convert from UTF-8.
    static bool from_utf8(const void* buf,
                          size_t len,
                          uint32_t& c,
                          size_t& utf8len);

    // Convert to UTF-8.
    static bool to_utf8(uint32_t c, char* buf, size_t& len);

    // Disable copy constructor and assignment operator.
    string(const string&) = delete;
    string& operator=(const string&) = delete;
};

} // namespace json
