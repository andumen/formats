#pragma once

#include <cmath>

#include <formats/common/tokens.h>
#include <formats/common/number.hpp>
#include <formats/jsoncpp/value.hpp>
#include <formats/jsoncpp/flags.h>

#include <formats/jsoncpp/detail/stringifier_adapter.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

namespace detail
{
// clang-format off

#define json_stringify_flag(__flag) (((unsigned long long)flag & (unsigned long long)__flag) == (unsigned long long)__flag)

#define write_single_quotes_string json_stringify_flag(stringify_flag::write_single_quotes_string)
#define write_unquoted_string      json_stringify_flag(stringify_flag::write_unquoted_string)
#define write_nan_number           json_stringify_flag(stringify_flag::write_nan_number)
#define write_infinity_num         json_stringify_flag(stringify_flag::write_infinity_num)

// clang-format on

class stringifier
{
public:
  template <typename T,
            typename std::enable_if<std::is_same<T, std::string>::value, bool>::type = true>
  stringifier(T&                 output,
              const json::value& value,
              stringify_style    style = stringify_style::standard,
              stringify_flag     flag  = stringify_flag::strict)
      : stringifier(value, style, flag)
  {
    o = output_adapter::create_string(output);
  }

  template <typename T,
            typename std::enable_if<std::is_same<T, std::ostream>::value, bool>::type = true>
  stringifier(T&                 output,
              const json::value& value,
              stringify_style    style = stringify_style::standard,
              stringify_flag     flag  = stringify_flag::strict)
      : stringifier(value, style, flag)
  {
    o = output_adapter::create_stream(output);
  }

private:
  stringifier(const json::value& value, stringify_style style, stringify_flag flag)
      : value(value)
      , style(style)
      , flag(flag)
      , quote_mark(1, c_double_quotes)
  {
    if (write_unquoted_string) quote_mark = "";
    if (write_single_quotes_string) quote_mark = "'";
  }

public:
  void dump()
  {
    if (value.is_object()) return write_object(value);

    if (value.is_array()) return write_array(value);

    write_scaler(value);
  }

private:
  void write_scaler(const value& v)
  {
    std::string value_str;

    switch (v.kind())
    {
      case kind::string:
        value_str.append(quote_mark).append(v.as_string()).append(quote_mark);
        break;
      case kind::number_int: value_str.assign(std::to_string(v.as_int64())); break;
      case kind::number_uint: value_str.assign(std::to_string(v.as_uint64())); break;
      case kind::number_float: value_str = double_to_string(v); break;
      case kind::boolean: value_str.assign(v.as_bool() ? "true" : "false"); break;
      case kind::null: value_str.assign("null");
      default: break;
    }

    o->write(value_str.c_str(), value_str.size());
  }

  void write_array(const value& v)
  {
    o->write(c_array_begin);

    if (style == stringify_style::pretty) ++depth;

    const auto size = v.size();
    for (std::size_t i = 0; i < size; ++i)
    {
      if (style == stringify_style::pretty)
      {
        o->write(c_line_feed);
        o->write(depth << 2, c_space);
      }

      const auto& element = v[i];

      if (likely(element.is_object()))
        write_object(element);
      else if (likely(element.is_array()))
        write_array(element);
      else
        write_scaler(element);

      if (i != size - 1) o->write(c_value_separator);
    }

    if (style == stringify_style::pretty) --depth;

    if (!v.empty())
    {
      if (style == stringify_style::pretty) o->write(c_line_feed);
      if (style != stringify_style::compact) o->write(depth << 2, c_space);
    }

    o->write(c_array_end);
  }

  void write_object(const value& v)
  {
    o->write(c_object_begin);

    if (style != stringify_style::compact) ++depth;

    auto& object = v.as_object();

    for (auto iter = object.begin(); iter != object.end();)
    {
      if (style != stringify_style::compact)
      {
        o->write(c_line_feed);
        o->write(depth << 2, c_space);
      }

      write_key(iter->first);

      if (likely(iter->second.is_object()))
        write_object(iter->second);
      else if (likely(iter->second.is_array()))
        write_array(iter->second);
      else
        write_scaler(iter->second);

      if (++iter != object.end()) o->write(c_value_separator);
    }

    if (style != stringify_style::compact)
    {
      --depth;

      if (!v.empty())
      {
        o->write(c_line_feed);
        o->write(depth << 2, c_space);
      }
    }

    o->write(c_object_end);
  }

private:
  std::string double_to_string(const value& v)
  {
    auto double_value = v.as_double();

    if (unlikely(std::isnan(double_value)))
    {
      if (write_nan_number)
        return std::string("NaN");
      else
        return std::string("\"NaN\"");
    }

    if (unlikely(std::isinf(double_value)))
    {
      if (write_infinity_num)
        return double_value < 0 ? std::string("-Infinity") : std::string("Infinity");
      else
        return double_value < 0 ? std::string("\"-Infinity\"") : std::string("\"Infinity\"");
    }

    int length = double_conversion_buffer_size;

    char buffer[double_conversion_buffer_size] = {0};
    dtoa(double_value, buffer, length);

    return std::string(buffer, length);
  }

  void write_key(const std::string& key)
  {
    o->write(quote_mark.c_str(), quote_mark.size())
        .write(key.c_str(), key.size())
        .write(quote_mark.c_str(), quote_mark.size())
        .write(c_name_separator)
        .write(c_space);
  }

private:
  int depth = 0;

  const json::value& value;

  stringify_style style;
  stringify_flag  flag;

  std::string quote_mark;

  typename output_adapter::ptr o;

private:
  const static int double_conversion_buffer_size = 128;
};

}  // namespace detail
FORMATS_JSON_NAMESPACE_END