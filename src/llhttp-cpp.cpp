//
// Copyright (c) Wade Christie and contributors. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "llhttp-cpp.hpp"

namespace {

using namespace llhttp_cpp;

static int return_result(event_func_result const& result)
{
    int i = -1;
    switch (result)
    {
    case event_func_result::OK:
        i = 0;
        break;
    case event_func_result::PAUSE:
        i = HPE_PAUSED;
        break;
    case event_func_result::ERROR:
        break;
    }
    return i;
}

static int return_result(headers_complete_result const& result)
{
    int i = -1;
    switch (result)
    {
    case headers_complete_result::PROCEED:
        i = 0;
        break;
    case headers_complete_result::NO_BODY:
        i = 1;
        break;
    case headers_complete_result::PAUSE_FOR_UPGRADE:
        i = 2;
        break;
    case headers_complete_result::PAUSE:
        i = HPE_PAUSED;
        break;
    case headers_complete_result::ERROR:
        break;
    }
    return i;
}

static int return_result(data_func_result const& result)
{
    int i = -1;
    switch (result)
    {
    case data_func_result::OK:
        i = 0;
        break;
    case data_func_result::USER:
        i = HPE_USER;
        break;
    case data_func_result::ERROR:
        break;
    }
    return i;
}

void init_settings(::llhttp_settings_t& settings, parser::callbacks& callbacks)
{
    ::llhttp_settings_init(&settings);
    if (callbacks.on_message_begin)
        settings.on_message_begin = parser::on_message_begin;
    if (callbacks.on_message_complete)
        settings.on_message_complete = parser::on_message_complete;
    if (callbacks.on_url_complete)
        settings.on_url_complete = parser::on_url_complete;
    if (callbacks.on_method_complete)
        settings.on_method_complete = parser::on_method_complete;
    if (callbacks.on_status_complete)
        settings.on_status_complete = parser::on_status_complete;
    if (callbacks.on_version_complete)
        settings.on_version_complete = parser::on_version_complete;
    if (callbacks.on_header_field_complete)
        settings.on_header_field_complete = parser::on_header_field_complete;
    if (callbacks.on_header_value_complete)
        settings.on_header_value_complete = parser::on_header_value_complete;
    if (callbacks.on_headers_complete)
        settings.on_headers_complete = parser::on_headers_complete;
    if (callbacks.on_chunk_header)
        settings.on_chunk_header = parser::on_chunk_header;
    if (callbacks.on_chunk_extension_name_complete)
        settings.on_chunk_extension_name_complete = parser::on_chunk_extension_name_complete;
    if (callbacks.on_chunk_extension_value_complete)
        settings.on_chunk_extension_value_complete = parser::on_chunk_extension_value_complete;
    if (callbacks.on_chunk_complete)
        settings.on_chunk_complete = parser::on_chunk_complete;
    if (callbacks.on_reset)
        settings.on_reset = parser::on_reset;

    if (callbacks.on_url)
        settings.on_url = parser::on_url;
    if (callbacks.on_method)
        settings.on_method = parser::on_method;
    if (callbacks.on_status)
        settings.on_status = parser::on_status;
    if (callbacks.on_version)
        settings.on_version = parser::on_version;
    if (callbacks.on_header_field)
        settings.on_header_field = parser::on_header_field;
    if (callbacks.on_header_value)
        settings.on_header_value = parser::on_header_value;
    if (callbacks.on_body)
        settings.on_body = parser::on_body;
    if (callbacks.on_chunk_extension_name)
        settings.on_chunk_extension_name = parser::on_chunk_extension_name;
    if (callbacks.on_chunk_extension_value)
        settings.on_chunk_extension_value = parser::on_chunk_extension_value;
}

void init_parser(::llhttp_t& parser, ::llhttp_settings_t& settings, type type, void* data)
{
    ::llhttp_init(&parser, type, &settings);
    parser.data = data;
}

} // namespace

namespace llhttp_cpp {

parser::parser(type type, parser::callbacks callbacks, void* data)
    : _callbacks(std::move(callbacks))
    , _data(data)
{
    init_settings(_settings, _callbacks);
    init_parser(_parser, _settings, type, this);
}

void* parser::get_data() const
{
    return _data;
}

err parser::execute(char const* data, size_t length)
{
    return ::llhttp_execute(&_parser, data, length);
}

err parser::execute(char const* data, size_t length, size_t* remaining)
{
    err result = ::llhttp_execute(&_parser, data, length);
    if (result == HPE_PAUSED)
    {
        *remaining = length - std::distance(reinterpret_cast<char const*>(data), ::llhttp_get_error_pos(&_parser));
    }
    return result;
}

err parser::finish()
{
    return ::llhttp_finish(&_parser);
}

bool parser::message_needs_eof() const
{
    return ::llhttp_message_needs_eof(&_parser) == 1;
}

bool parser::should_keep_alive() const
{
    return ::llhttp_should_keep_alive(&_parser) == 1;
}

void parser::resume()
{
    ::llhttp_resume(&_parser);
}

void parser::resume_after_upgrade()
{
    ::llhttp_resume_after_upgrade(&_parser);
}

err parser::get_errno() const
{
    return llhttp_get_errno(&_parser);
}

std::string_view parser::get_error_reason() const
{
    return llhttp_get_error_reason(&_parser);
}

char const* parser::get_error_pos() const
{
    return llhttp_get_error_pos(&_parser);
}

type parser::get_type() const
{
    return static_cast<type>(::llhttp_get_type(&_parser));
}

uint8_t parser::get_http_major() const
{
    return ::llhttp_get_http_major(&_parser);
}

uint8_t parser::get_http_minor() const
{
    return ::llhttp_get_http_minor(&_parser);
}

method parser::get_method() const
{
    return static_cast<method>(::llhttp_get_method(&_parser));
}

status parser::get_status_code() const
{
    return static_cast<status>(::llhttp_get_status_code(&_parser));
}

void parser::reset()
{
    ::llhttp_reset(&_parser);
}

int parser::on_message_begin(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_message_begin(context);
    return return_result(result);
}

int parser::on_message_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_message_complete(context);
    return return_result(result);
}

int parser::on_url(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_url(context, { value, length });
    return return_result(result);
}

int parser::on_url_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_url_complete(context);
    return return_result(result);
}

int parser::on_method(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_method(context, { value, length });
    return return_result(result);
}

int parser::on_method_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_method_complete(context);
    return return_result(result);
}

int parser::on_status(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_status(context, { value, length });
    return return_result(result);
}

int parser::on_status_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_status_complete(context);
    return return_result(result);
}

int parser::on_version(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_version(context, { value, length });
    return return_result(result);
}

int parser::on_version_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_version_complete(context);
    return return_result(result);
}

int parser::on_header_field(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_header_field(context, { value, length });
    return return_result(result);
}

int parser::on_header_field_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_header_field_complete(context);
    return return_result(result);
}

int parser::on_header_value(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_header_value(context, { value, length });
    return return_result(result);
}

int parser::on_header_value_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_header_value_complete(context);
    return return_result(result);
}

int parser::on_headers_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_headers_complete(context);
    return return_result(result);
}

int parser::on_body(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_body(context, { value, length });
    return return_result(result);
}

int parser::on_chunk_header(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_header(context);
    return return_result(result);
}

int parser::on_chunk_extension_name(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_extension_name(context, { value, length });
    return return_result(result);
}

int parser::on_chunk_extension_name_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_extension_name_complete(context);
    return return_result(result);
}

int parser::on_chunk_extension_value(::llhttp_t* parser, char const* value, size_t length)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_extension_value(context, { value, length });
    return return_result(result);
}

int parser::on_chunk_extension_value_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_extension_value_complete(context);
    return return_result(result);
}

int parser::on_chunk_complete(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_chunk_complete(context);
    return return_result(result);
}

int parser::on_reset(::llhttp_t* parser)
{
    auto& context = *reinterpret_cast<llhttp_cpp::parser*>(parser->data);
    auto result = context._callbacks.on_reset(context);
    return return_result(result);
}

} // namespace llhttp_cpp

std::string_view std::to_string(::llhttp_errno_t err)
{
    return ::llhttp_errno_name(err);
}

std::string_view std::to_string(::llhttp_method_t method)
{
    return ::llhttp_method_name(method);
}

std::string_view std::to_string(::llhttp_status_t status)
{
    return ::llhttp_status_name(status);
}
