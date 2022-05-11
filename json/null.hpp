#pragma once

#include "json/value.hpp"

namespace json {

// Forward declaration.
class document;

// JSON null.
class null final : public value {
  friend class document;

  public:
    // Constructor.
    null();

    // Destructor.
    ~null() = default;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Parse.
    bool parse(const void* buf, size_t bufsize, size_t& len);

    // Disable copy constructor and assignment operator.
    null(const null&) = delete;
    null& operator=(const null&) = delete;
};

} // namespace json
