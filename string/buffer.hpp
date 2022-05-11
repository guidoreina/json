#pragma once

#include <sys/types.h>
#include <stdarg.h>

namespace string {

// String buffer.
class buffer {
  public:
    // Constructor.
    buffer() = default;

    // Destructor.
    ~buffer();

    // Swap strings.
    void swap(buffer& other);

    // Clear buffer.
    void clear();

    // Empty?
    bool empty() const;

    // Get data.
    const char* data() const;

    // Get length.
    size_t length() const;

    // Append.
    bool append(const char* buf, size_t len);
    bool append(const char* buf);
    bool append(size_t count, char ch);

    // Append a single character.
    bool push_back(char c);

    // Format.
    bool format(const char* format, ...);
    bool vformat(const char* format, va_list ap);

  private:
    // Maximum length of the string.
    static constexpr const size_t max_length = 1024ul * 1024ul * 1024ul;

    // Initial size of the string.
    static constexpr const size_t initial_size = 32;

    // String.
    char* _M_data = nullptr;
    size_t _M_size = 0;
    size_t _M_used = 0;

    // Reserve memory.
    bool reserve(size_t n);

    // Disable copy constructor and assignment operator.
    buffer(const buffer&) = delete;
    buffer& operator=(const buffer&) = delete;
};

} // namespace string
