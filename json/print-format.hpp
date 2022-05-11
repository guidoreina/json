#pragma once

#include <sys/types.h>

namespace json {
namespace print {

// Print format.
enum class format {
  pretty,
  compact
};

// Indent size.
static constexpr const size_t indent = 2;

} // namespace print
} // namespace json
