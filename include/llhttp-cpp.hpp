//
// Copyright (c) Wade Christie and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//
#ifndef __LLHTTP_CPP_HPP__
#define __LLHTTP_CPP_HPP__

#include <functional>
#include <memory>
#include <string_view>

#include <llhttp.h>

namespace llhttp_cpp {

/** @brief Alias for `llhttp_errno_t`. */
using err = ::llhttp_errno_t;

/** @brief Alias for `llhttp_type_t`. */
using type = ::llhttp_type_t;

/** @brief Alias for `llhttp_method_t`. */
using method = ::llhttp_method_t;

/** @brief Alias for `llhttp_status_t`. */
using status = ::llhttp_status_t;

/** @brief Generic HTTP event callback result. */
enum class event_func_result
{
    OK = 0,
    PAUSE,
    ERROR
};

/** @brief HTTP headers complete event callback result. */
enum class headers_complete_result
{
    PROCEED,
    NO_BODY,
    PAUSE_FOR_UPGRADE,
    PAUSE,
    ERROR,
};

/** @brief Generic HTTP data callback result. */
enum class data_func_result
{
    OK = 0,
    USER,
    ERROR
};

// Forward declarations
class parser;

/** @brief Generic HTTP event callback function. */
using event_func = std::function<event_func_result(parser&)>;

/** @brief HTTP headers complete event callback function. */
using headers_complete_event_func = std::function<headers_complete_result(parser&)>;

/** @brief Generic HTTP data callback function. */
using data_func = std::function<data_func_result(parser&, std::string_view)>;

/** @brief C++ wrapper around `llhttp_t`. */
class parser
{
public:
    /** @brief HTTP parser callback settings. */
    struct callbacks
    {
        // event callbacks
        event_func on_message_begin;
        event_func on_message_complete;
        event_func on_url_complete;
        event_func on_method_complete;
        event_func on_status_complete;
        event_func on_version_complete;
        event_func on_header_field_complete;
        event_func on_header_value_complete;
        headers_complete_event_func on_headers_complete;
        event_func on_chunk_header;
        event_func on_chunk_extension_name_complete;
        event_func on_chunk_extension_value_complete;
        event_func on_chunk_complete;
        event_func on_reset;

        // data callbacks
        data_func on_url;
        data_func on_method;
        data_func on_status;
        data_func on_version;
        data_func on_header_field;
        data_func on_header_value;
        data_func on_body;
        data_func on_chunk_extension_name;
        data_func on_chunk_extension_value;
    };

    parser(type type, callbacks callbacks, void* data = nullptr);

    parser(parser&&) = default;
    parser& operator=(parser&&) = default;

    parser(parser const&) = delete;
    parser& operator=(parser const&) = delete;

    ~parser() = default;

    err execute(char const* data, size_t length);
    err execute(char const* data, size_t length, size_t* remaining);

    err finish();

    bool message_needs_eof() const;
    bool should_keep_alive() const;

    void resume();
    void resume_after_upgrade();

    err get_errno() const;
    std::string_view get_error_reason() const;
    char const* get_error_pos() const;

    type get_type() const;
    uint8_t get_http_major() const;
    uint8_t get_http_minor() const;
    method get_method() const;
    status get_status_code() const;

    void* get_data() const;

    void reset();

    static int on_message_begin(::llhttp_t* parser);
    static int on_message_complete(::llhttp_t* parser);
    static int on_url(::llhttp_t* parser, char const* value, size_t length);
    static int on_url_complete(::llhttp_t* parser);
    static int on_method(::llhttp_t* parser, char const* value, size_t length);
    static int on_method_complete(::llhttp_t* parser);
    static int on_status(::llhttp_t* parser, char const* value, size_t length);
    static int on_status_complete(::llhttp_t* parser);
    static int on_version(::llhttp_t* parser, char const* value, size_t length);
    static int on_version_complete(::llhttp_t* parser);
    static int on_header_field(::llhttp_t* parser, char const* value, size_t length);
    static int on_header_field_complete(::llhttp_t* parser);
    static int on_header_value(::llhttp_t* parser, char const* value, size_t length);
    static int on_header_value_complete(::llhttp_t* parser);
    static int on_headers_complete(::llhttp_t* parser);
    static int on_body(::llhttp_t* parser, char const* value, size_t length);
    static int on_chunk_header(::llhttp_t* parser);
    static int on_chunk_extension_name(::llhttp_t* parser, char const* value, size_t length);
    static int on_chunk_extension_name_complete(::llhttp_t* parser);
    static int on_chunk_extension_value(::llhttp_t* parser, char const* value, size_t length);
    static int on_chunk_extension_value_complete(::llhttp_t* parser);
    static int on_chunk_complete(::llhttp_t* parser);
    static int on_reset(::llhttp_t* parser);

private:
    /** @brief C++ parser callbacks. */
    callbacks _callbacks;
    /** @brief User supplied data context. */
    void* _data;
    /** @brief llhttp parser settings */
    ::llhttp_settings_t _settings { 0 };
    /** @brief llhttp parser. */
    mutable ::llhttp_t _parser { 0 };
};

} // namespace llhttp_cpp

namespace std {

std::string_view to_string(::llhttp_errno_t err);
std::string_view to_string(::llhttp_method_t method);
std::string_view to_string(::llhttp_status_t status);

} // namespace std

#endif // __LLHTTP_CPP_HPP__
