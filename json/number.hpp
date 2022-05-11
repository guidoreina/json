#pragma once

#include <stdint.h>
#include "json/value.hpp"

namespace json {

// Forward declaration.
class document;

// JSON number.
class number final : public value {
  friend class document;

  public:
    // Maximum length of a JSON number.
    static constexpr const size_t max_length = 32;

    // Number type.
    enum class type {
      integer,
      real
    };

    // Constructors.
    number();
    number(int64_t n);
    number(double n);

    // Destructor.
    ~number() = default;

    // Initialize.
    void init(int64_t n);
    void init(double n);
    void init(const number& other);

    // Get number type.
    enum type type() const;

    // Get number as an integer number.
    int64_t as_integer() const;

    // Get number as a real number.
    double as_real() const;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Number type.
    enum type _M_type;

    union {
      int64_t _M_integer;
      double _M_real;
    };

    // Parse.
    bool parse(const void* buf, size_t bufsize, size_t& len);

    // Disable copy constructor and assignment operator.
    number(const number&) = delete;
    number& operator=(const number&) = delete;
};

} // namespace json
