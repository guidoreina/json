#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include "json/document.hpp"
#include "json/object.hpp"
#include "json/array.hpp"
#include "json/string.hpp"
#include "json/number.hpp"
#include "json/boolean.hpp"
#include "json/null.hpp"
#include "json/ctype.hpp"

json::value* json::document::parse(const void* buf,
                                   size_t len,
                                   char* errmsg,
                                   size_t errmsglen)
{
  // Make `root` point to the root node.
  value* root = nullptr;

  // Make `v` point to the current value.
  value* v = nullptr;

  // Member name.
  string tmpname;

  // Make `b` point to the beginning of the buffer.
  const uint8_t* const b = static_cast<const uint8_t*>(buf);

  enum class state {
    parsing_element,
    parsing_object,
    after_member_name,
    after_object_member,
    after_array_element,
    no_more_elements
  };

  state s = state::parsing_element;

  size_t lineno = 1;
  size_t column = 0;

  size_t i = 0;
  while (i < len) {
    // Set `c` to the current character.
    const uint8_t c = b[i];

    // If the current character is not a line feed...
    if (c != '\n') {
      column++;
    } else {
      lineno++;
      column = 0;

      i++;

      continue;
    }

    switch (s) {
      case state::parsing_element:
        if (c == '{') {
          // If not the first value...
          if (root) {
            // Create a new object.
            value* const val = new (std::nothrow) object{};

            // If the object could be created...
            if (val) {
              // Set object's parent.
              val->_M_parent = v;

              // If the current value is an object...
              if (v->_M_type == value::type::object) {
                // Create a JSON string for storing the member name.
                string* const name = new (std::nothrow) string{};

                // If the JSON string could be created...
                if (name) {
                  name->swap(tmpname);

                  // Add member to the object.
                  if (!static_cast<object*>(v)->add(name, val)) {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete name;
                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  if (errmsg) {
                    // Compose error message.
                    snprintf(errmsg, errmsglen, "error allocating memory");
                  }

                  delete val;
                  delete root;

                  return nullptr;
                }
              } else {
                // Append object to the array.
                if (!static_cast<array*>(v)->append(val)) {
                  if (errmsg) {
                    // Compose error message.
                    snprintf(errmsg, errmsglen, "error allocating memory");
                  }

                  delete val;
                  delete root;

                  return nullptr;
                }
              }

              // Set current value.
              v = val;
            } else {
              if (errmsg) {
                // Compose error message.
                snprintf(errmsg, errmsglen, "error allocating memory");
              }

              delete root;
              return nullptr;
            }
          } else {
            // Create a new object.
            v = new (std::nothrow) object{};

            // If the object could be created...
            if (v) {
              // `v` is the root node and has no parent.
              v->_M_parent = nullptr;

              // Set root node.
              root = v;
            } else {
              if (errmsg) {
                // Compose error message.
                snprintf(errmsg, errmsglen, "error allocating memory");
              }

              return nullptr;
            }
          }

          s = state::parsing_object;
        } else if (c == '[') {
          // If not the first value...
          if (root) {
            // Create a new array.
            value* const val = new (std::nothrow) array{};

            // If the array could be created...
            if (val) {
              // Set array's parent.
              val->_M_parent = v;

              // If the current value is an object...
              if (v->_M_type == value::type::object) {
                // Create a JSON string for storing the member name.
                string* const name = new (std::nothrow) string{};

                // If the JSON string could be created...
                if (name) {
                  name->swap(tmpname);

                  // Add member to the object.
                  if (!static_cast<object*>(v)->add(name, val)) {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete name;
                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  if (errmsg) {
                    // Compose error message.
                    snprintf(errmsg, errmsglen, "error allocating memory");
                  }

                  delete val;
                  delete root;

                  return nullptr;
                }
              } else {
                // Append array to the array.
                if (!static_cast<array*>(v)->append(val)) {
                  if (errmsg) {
                    // Compose error message.
                    snprintf(errmsg, errmsglen, "error allocating memory");
                  }

                  delete val;
                  delete root;

                  return nullptr;
                }
              }

              // Set current value.
              v = val;
            } else {
              if (errmsg) {
                // Compose error message.
                snprintf(errmsg, errmsglen, "error allocating memory");
              }

              delete root;
              return nullptr;
            }
          } else {
            // Create a new array.
            v = new (std::nothrow) array{};

            // If the array could be created...
            if (v) {
              // `v` is the root node and has no parent.
              v->_M_parent = nullptr;

              // Set root node.
              root = v;
            } else {
              if (errmsg) {
                // Compose error message.
                snprintf(errmsg, errmsglen, "error allocating memory");
              }

              return nullptr;
            }
          }
        } else if (c == '"') {
          // Parse JSON string.
          string jstr;
          size_t l;
          if (jstr.parse(b + i, len - i, l)) {
            // If not the first value...
            if (root) {
              // Create a new string.
              value* const val = new (std::nothrow) string{};

              // If the string could be created...
              if (val) {
                static_cast<string*>(val)->swap(jstr);

                // Set string's parent.
                val->_M_parent = v;

                // If the current value is an object...
                if (v->_M_type == value::type::object) {
                  // Create a JSON string for storing the member name.
                  string* const name = new (std::nothrow) string{};

                  // If the JSON string could be created...
                  if (name) {
                    name->swap(tmpname);

                    // Add member to the object.
                    if (static_cast<object*>(v)->add(name, val)) {
                      s = state::after_object_member;
                    } else {
                      if (errmsg) {
                        // Compose error message.
                        snprintf(errmsg, errmsglen, "error allocating memory");
                      }

                      delete name;
                      delete val;
                      delete root;

                      return nullptr;
                    }
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  // Append string to the array.
                  if (static_cast<array*>(v)->append(val)) {
                    s = state::after_array_element;
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                }
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                delete root;
                return nullptr;
              }
            } else {
              // Create a new string.
              v = new (std::nothrow) string{};

              // If the string could be created...
              if (v) {
                static_cast<string*>(v)->swap(jstr);

                // `v` is the root node and has no parent.
                v->_M_parent = nullptr;

                // Set root node.
                root = v;

                s = state::no_more_elements;
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                return nullptr;
              }
            }

            // Skip string.
            i += l;

            column += (l - 1);

            // Go to the beginning of the while loop.
            continue;
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "error parsing string starting at L%zu:%zu",
                       lineno,
                       column);
            }

            if (root) {
              delete root;
            }

            return nullptr;
          }
        } else if ((isdigit(c)) || (c == '-')) {
          // Parse number.
          number n;
          size_t l;
          if (n.parse(b + i, len - i, l)) {
            // If not the first value...
            if (v) {
              // Create a new number.
              value* const val = new (std::nothrow) number{};

              // If the number could be created...
              if (val) {
                static_cast<number*>(val)->init(n);

                // Set number's parent.
                val->_M_parent = v;

                // If the current value is an object...
                if (v->_M_type == value::type::object) {
                  // Create a JSON string for storing the member name.
                  string* const name = new (std::nothrow) string{};

                  // If the JSON string could be created...
                  if (name) {
                    name->swap(tmpname);

                    // Add member to the object.
                    if (static_cast<object*>(v)->add(name, val)) {
                      s = state::after_object_member;
                    } else {
                      if (errmsg) {
                        // Compose error message.
                        snprintf(errmsg, errmsglen, "error allocating memory");
                      }

                      delete name;
                      delete val;
                      delete root;

                      return nullptr;
                    }
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  // Append number to the array.
                  if (static_cast<array*>(v)->append(val)) {
                    s = state::after_array_element;
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                }
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                delete root;
                return nullptr;
              }
            } else {
              // Create a new number.
              v = new (std::nothrow) number{};

              // If the number could be created...
              if (v) {
                static_cast<number*>(v)->init(n);

                // `v` is the root node and has no parent.
                v->_M_parent = nullptr;

                // Set root node.
                root = v;

                s = state::no_more_elements;
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                return nullptr;
              }
            }

            // Skip number.
            i += l;

            column += (l - 1);

            // Go to the beginning of the while loop.
            continue;
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "error parsing number starting at L%zu:%zu",
                       lineno,
                       column);
            }

            if (root) {
              delete root;
            }

            return nullptr;
          }
        } else if ((c == 't') || (c == 'f')) {
          boolean boolean;
          size_t l;
          if (boolean.parse(b + i, len - i, l)) {
            // If not the first value...
            if (v) {
              // Create a new boolean.
              value* const val = new (std::nothrow) json::boolean{};

              // If the boolean could be created...
              if (val) {
                static_cast<json::boolean*>(val)->init(boolean);

                // Set boolean's parent.
                val->_M_parent = v;

                // If the current value is an object...
                if (v->_M_type == value::type::object) {
                  // Create a JSON string for storing the member name.
                  string* const name = new (std::nothrow) string{};

                  // If the JSON string could be created...
                  if (name) {
                    name->swap(tmpname);

                    // Add member to the object.
                    if (static_cast<object*>(v)->add(name, val)) {
                      s = state::after_object_member;
                    } else {
                      if (errmsg) {
                        // Compose error message.
                        snprintf(errmsg, errmsglen, "error allocating memory");
                      }

                      delete name;
                      delete val;
                      delete root;

                      return nullptr;
                    }
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  // Append boolean to the array.
                  if (static_cast<array*>(v)->append(val)) {
                    s = state::after_array_element;
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                }
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                delete root;
                return nullptr;
              }
            } else {
              // Create a new boolean.
              v = new (std::nothrow) json::boolean{};

              // If the boolean could be created...
              if (v) {
                static_cast<json::boolean*>(v)->init(boolean);

                // `v` is the root node and has no parent.
                v->_M_parent = nullptr;

                // Set root node.
                root = v;

                s = state::no_more_elements;
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                return nullptr;
              }
            }

            // Skip boolean.
            i += l;

            column += (l - 1);

            // Go to the beginning of the while loop.
            continue;
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "error parsing boolean starting at L%zu:%zu",
                       lineno,
                       column);
            }

            if (root) {
              delete root;
            }

            return nullptr;
          }
        } else if (c == 'n') {
          null null;
          size_t l;
          if (null.parse(b + i, len - i, l)) {
            // If not the first value...
            if (v) {
              // Create a new null.
              value* const val = new (std::nothrow) json::null{};

              // If the null could be created...
              if (val) {
                // Set null's parent.
                val->_M_parent = v;

                // If the current value is an object...
                if (v->_M_type == value::type::object) {
                  // Create a JSON string for storing the member name.
                  string* const name = new (std::nothrow) string{};

                  // If the JSON string could be created...
                  if (name) {
                    name->swap(tmpname);

                    // Add member to the object.
                    if (static_cast<object*>(v)->add(name, val)) {
                      s = state::after_object_member;
                    } else {
                      if (errmsg) {
                        // Compose error message.
                        snprintf(errmsg, errmsglen, "error allocating memory");
                      }

                      delete name;
                      delete val;
                      delete root;

                      return nullptr;
                    }
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                } else {
                  // Append null to the array.
                  if (static_cast<array*>(v)->append(val)) {
                    s = state::after_array_element;
                  } else {
                    if (errmsg) {
                      // Compose error message.
                      snprintf(errmsg, errmsglen, "error allocating memory");
                    }

                    delete val;
                    delete root;

                    return nullptr;
                  }
                }
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                delete root;
                return nullptr;
              }
            } else {
              // Create a new null.
              v = new (std::nothrow) json::null{};

              // If the null could be created...
              if (v) {
                // `v` is the root node and has no parent.
                v->_M_parent = nullptr;

                // Set root node.
                root = v;

                s = state::no_more_elements;
              } else {
                if (errmsg) {
                  // Compose error message.
                  snprintf(errmsg, errmsglen, "error allocating memory");
                }

                return nullptr;
              }
            }

            // Skip null.
            i += l;

            column += (l - 1);

            // Go to the beginning of the while loop.
            continue;
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "error parsing null starting at L%zu:%zu",
                       lineno,
                       column);
            }

            if (root) {
              delete root;
            }

            return nullptr;
          }
        } else if (c == ']') {
          // Empty array?
          if ((v) &&
              (v->_M_type == value::type::array) &&
              (static_cast<const array*>(v)->empty())) {
            v = v->_M_parent;

            // If not the last value...
            if (v) {
              s = (v->_M_type == value::type::object) ?
                    state::after_object_member :
                    state::after_array_element;
            } else {
              s = state::no_more_elements;
            }
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "invalid character ']' found at L%zu:%zu while "
                       "parsing element",
                       lineno,
                       column);
            }

            if (root) {
              delete root;
            }

            return nullptr;
          }
        } else if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu while "
                     "parsing element",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          if (root) {
            delete root;
          }

          return nullptr;
        }

        break;
      case state::parsing_object:
        if (c == '"') {
          // Parse JSON string.
          size_t l;
          if (tmpname.parse(b + i, len - i, l)) {
            s = state::after_member_name;

            // Skip string.
            i += l;

            column += (l - 1);

            continue;
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "error parsing string starting at L%zu:%zu",
                       lineno,
                       column);
            }

            delete root;
            return nullptr;
          }
        } else if (c == '}') {
          // Empty object?
          if (static_cast<const object*>(v)->empty()) {
            v = v->_M_parent;

            // If not the last value...
            if (v) {
              s = (v->_M_type == value::type::object) ?
                    state::after_object_member :
                    state::after_array_element;
            } else {
              s = state::no_more_elements;
            }
          } else {
            if (errmsg) {
              // Compose error message.
              snprintf(errmsg,
                       errmsglen,
                       "invalid character '}' found at L%zu:%zu while "
                       "parsing object, opening quotation mark expected",
                       lineno,
                       column);
            }

            delete root;
            return nullptr;
          }
        } else if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu while "
                     "parsing object, opening quotation mark expected",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          delete root;
          return nullptr;
        }

        break;
      case state::after_member_name:
        if (c == ':') {
          s = state::parsing_element;
        } else if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu while "
                     "parsing object, ':' expected",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          delete root;
          return nullptr;
        }

        break;
      case state::after_object_member:
        if (c == ',') {
          s = state::parsing_object;
        } else if (c == '}') {
          v = v->_M_parent;

          // If not the last value...
          if (v) {
            // If the current value is an array...
            if (v->_M_type == value::type::array) {
              s = state::after_array_element;
            }
          } else {
            s = state::no_more_elements;
          }
        } else if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu while "
                     "parsing object, ',' or '}' expected",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          delete root;
          return nullptr;
        }

        break;
      case state::after_array_element:
        if (c == ',') {
          s = state::parsing_element;
        } else if (c == ']') {
          v = v->_M_parent;

          // If not the last value...
          if (v) {
            // If the current value is an object...
            if (v->_M_type == value::type::object) {
              s = state::after_object_member;
            }
          } else {
            s = state::no_more_elements;
          }
        } else if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu while "
                     "parsing array, ',' or ']' expected",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          delete root;
          return nullptr;
        }

        break;
      case state::no_more_elements:
        if (!isws(c)) {
          if (errmsg) {
            // Compose error message.
            snprintf(errmsg,
                     errmsglen,
                     "invalid character '%c' (0x%02x) found at L%zu:%zu, no "
                     "more characters expected",
                     static_cast<char>(c),
                     c,
                     lineno,
                     column);
          }

          delete root;
          return nullptr;
        }

        break;
    }

    i++;
  }

  if (s == state::no_more_elements) {
    return root;
  } else {
    if (errmsg) {
      // Compose error message.
      snprintf(errmsg, errmsglen, "unexpected end of document");
    }

    if (root) {
      delete root;
    }

    return nullptr;
  }
}
