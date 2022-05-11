#pragma once

#include "json/print-format.hpp"
#include "string/buffer.hpp"

namespace json {

// Forward declaration.
class document;

// JSON value.
class value {
  friend class document;

  public:
    // Value type.
    enum class type {
      object,
      array,
      string,
      number,
      boolean,
      null
    };

    // Constructor.
    value(enum type t);

    // Destructor.
    virtual ~value();

    // Get type.
    enum type type() const;

    // Serialize.
    virtual bool serialize(print::format format,
                           ::string::buffer& buf,
                           size_t depth = 0) const = 0;

  private:
    // Value type.
    enum type _M_type;

    // Parent value.
    value* _M_parent;

    // Disable copy constructor and assignment operator.
    value(const value&) = delete;
    value& operator=(const value&) = delete;
};

} // namespace json
