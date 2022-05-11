#pragma once

#include "json/value.hpp"
#include "json/string.hpp"

namespace json {

// JSON object.
class object final : public value {
  public:
    // Constructor.
    object();

    // Destructor.
    ~object();

    // Swap objects.
    void swap(object& other);

    // Add member.
    bool add(string* name, class value* val);

    // Empty?
    bool empty() const;

    // Get number of members.
    size_t count() const;

    // Get value.
    const class value* value(const char* name) const;

    // Get member at position.
    bool get(size_t idx, const string*& name, const class value*& val) const;

    // Serialize.
    bool serialize(print::format format,
                   ::string::buffer& buf,
                   size_t depth = 0) const;

  private:
    // Allocation.
    static constexpr const size_t allocation = 4;

    // Member.
    struct member {
      string* name;
      class value* val;
    };

    // Members.
    member* _M_members = nullptr;
    size_t _M_size = 0;
    size_t _M_used = 0;

    // Allocate slots for members.
    bool allocate();

    // Disable copy constructor and assignment operator.
    object(const object&) = delete;
    object& operator=(const object&) = delete;
};

} // namespace json
