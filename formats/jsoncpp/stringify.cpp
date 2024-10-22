#include <formats/jsoncpp/stringify.hpp>
#include <formats/jsoncpp/detail/stringifier.hpp>

#include <fstream>

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

void dump(const std::string& filepath, const json::value& value, stringify_style style)
{
  auto json_string = json::stringify(value, style);

  std::ofstream out;
  out.open(filepath, std::ios::out | std::ios::binary);
  if (out.is_open())
  {
    out << json_string;
    out.close();
  }
}

FORMATS_JSON_NAMESPACE_END