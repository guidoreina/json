#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json/document.hpp"
#include "json/object.hpp"
#include "json/string.hpp"

int main()
{
  static constexpr const char* const
    jstr = R"({
                "key0": {},
                "key1": "value1\n",
                "unicode": "\ud834\udd1e",
                "key2": [
                  "value2",
                  "value3",
                  {
                    "key3": "\"value4\""
                  },
                  false,
                  null,
                  true,
                  false,
                  [
                    [],
                    [],
                    {
                      "key4": ""
                    }
                  ],
                  {},
                  {},
                  {},
                  [[[[]]]],
                  [[[[]]]],
                  -123,
                  -0.1e8,
                  -0,
                  0
                ]
              })";

  char errmsg[1024];
  json::value* const value = json::document::parse(jstr,
                                                   strlen(jstr),
                                                   errmsg,
                                                   sizeof(errmsg));
  if (value) {
    // JSON object?
    if (value->type() == json::value::type::object) {
      const json::value* const
        unicode = static_cast<const json::object*>(value)->value("unicode");

      if ((unicode) && (unicode->type() == json::value::type::string)) {
        printf("Unicode: '%s'\n\n",
               static_cast<const json::string*>(unicode)->data());
      }
    }

    string::buffer buf;
    if (value->serialize(json::print::format::pretty, buf)) {
      delete value;

      printf("%.*s\n", static_cast<int>(buf.length()), buf.data());
      return EXIT_SUCCESS;
    } else {
      delete value;
      fprintf(stderr, "Error serializing JSON value.\n");
    }
  } else {
    fprintf(stderr, "Error parsing JSON value (%s).\n", errmsg);
  }

  return EXIT_FAILURE;
}
