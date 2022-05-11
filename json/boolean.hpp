#pragma once

#include "json/value.hpp"

namespace json {

// Forward declaration.
class document;

// JSON boolean.
class boolean final : public value {
  friend class document;

  public:
    // Constructor.
    boolean();
    boolean(bool val);

    // Destructor.
    ~boolean() = default;

    // Initialize.
    void init(bool val);
    void init(const boolean& other);

    // Boolean operator.
    operator bool() const;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Value.
    bool _M_value;

    // Parse.
    bool parse(const void* buf, size_t bufsize, size_t& len);

    // Disable copy constructor and assignment operator.
    boolean(const boolean&) = delete;
    boolean& operator=(const boolean&) = delete;
};

} // namespace json
