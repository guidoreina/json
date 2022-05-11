#pragma once

#include "json/value.hpp"

namespace json {

// JSON document.
class document {
  public:
    // Constructor.
    document() = default;

    // Destructor.
    ~document() = default;

    // Parse.
    static value* parse(const void* buf,
                        size_t len,
                        char* errmsg = nullptr,
                        size_t ermsglen = 0);
};

} // namespace json
