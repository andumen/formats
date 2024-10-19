#include <formats/jsoncpp/stringify.hpp>
#include <formats/jsoncpp/detail/stringifier.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

std::string stringify(const value& json_value, stringify_style style, stringify_flag flag) noexcept
{
  std::string result;
  formats::json::detail::stringifier(result, json_value, style).dump();

  return result;
}

std::string fast_stringify(const value& v)
{
  std::string result;
  formats::json::detail::stringifier(result, v, stringify_style::compact).dump();

  return result;
}

std::string style_stringify(const value& v)
{
  std::string result;
  formats::json::detail::stringifier(result, v, stringify_style::pretty).dump();

  return result;
}

FORMATS_JSON_NAMESPACE_END