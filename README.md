# llhttp-cpp

C++ wrapper library for [https://github.com/nodejs/llhttp](https://github.com/nodejs/llhttp).

## Example
```cpp
#include <llhttp-cpp.hpp>

// User defined parser context to container state
struct context
{
    // event/data callbacks
    llhttp_cpp::event_func_result on_message_begin();
    llhttp_cpp::data_func_result on_url(std::string_view data);
    llhttp_cpp::event_func_result on_url_complete();
    llhttp_cpp::data_func_result on_header_field(std::string_view data);
    llhttp_cpp::event_func_result on_header_field_complete();
    llhttp_cpp::data_func_result on_header_value(std::string_view data);
    llhttp_cpp::event_func_result on_header_value_complete();
    llhttp_cpp::headers_complete_result on_headers_complete();
    llhttp_cpp::data_func_result on_body(std::string_view data);
    llhttp_cpp::event_func_result on_message_complete();

    // Custom parser state
};

// Initialize the parser
context parser_context;
llhttp_cpp::parser parser{
    HTTP_REQUEST,
    llhttp_cpp::parser::callbacks {
        .on_message_begin = std::bind(&parser_context::on_message_begin, context),
        .on_message_complete = std::bind(&parser_context::on_message_complete, context),
        .on_url_complete = std::bind(&parser_context::on_url_complete, context),
        .on_header_field_complete = std::bind(&parser_context::on_header_field_complete, context),
        .on_header_value_complete = std::bind(&parser_context::on_header_value_complete, context),
        .on_headers_complete = std::bind(&parser_context::on_headers_complete, context),
        .on_url = std::bind(&parser_context::on_url, context, std::placeholders::_2),
        .on_header_field = std::bind(&parser_context::on_header_field, context, std::placeholders::_2),
        .on_header_value = std::bind(&parser_context::on_header_value, context, std::placeholders::_2),
        .on_body = std::bind(&parser_context::on_body, context, std::placeholders::_2),
    }};

// Push content through the parser
llhttp_cpp::parser::err result = parser.execute(data, length);
```

## CMake
```
FetchContent_Declare(
    llhttp-cpp
    GIT_REPOSITORY https://github.com/wadechristie/llhttp-cpp.git
    GIT_TAG master
)

FetchContent_MakeAvailable(llhttp-cpp)

# Link with the llhttp-cpp::llhttp-cpp target
target_link_libraries(${EXAMPLE_PROJECT_NAME} ${PROJECT_LIBRARIES} llhttp-cpp::llhttp-cpp)

```
