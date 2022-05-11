#pragma once

#include "json/value.hpp"

namespace json {

// JSON array.
class array final : public value {
  public:
    // Constructor.
    array();

    // Destructor.
    ~array();

    // Swap arrays.
    void swap(array& other);

    // Append.
    bool append(value* val);

    // Empty?
    bool empty() const;

    // Get number of values.
    size_t count() const;

    // Get value at position.
    const value* get(size_t idx) const;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Allocation.
    static constexpr const size_t allocation = 8;

    // Values.
    value** _M_values = nullptr;
    size_t _M_size = 0;
    size_t _M_used = 0;

    // Allocate slots for values.
    bool allocate();

    // Disable copy constructor and assignment operator.
    array(const array&) = delete;
    array& operator=(const array&) = delete;
};

} // namespace json
