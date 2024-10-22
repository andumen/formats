#include <formats/jsoncpp/parse.hpp>
#include <formats/jsoncpp/detail/parser.hpp>

#include <formats/common/unicode/unicode.hpp>
#include <formats/common/number.hpp>
#include <formats/common/cctypes.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

bool parse(value& jv, const char* data, parse_flag flag)
{
  return parse(jv, data, std::char_traits<char>::length(data), flag);
}

bool parse(value& jv, const char* begin, const char* end, parse_flag flag)
{
  error err;
  jv = parse(begin, end, err, flag);

  return (jv.kind() != json::kind::error);
}

bool parse(value& jv, const char* begin, std::size_t len, parse_flag flag)
{
  jv = parse(begin, begin + len, flag);

  return (jv.kind() != json::kind::error);
}

bool parse(value& jv, std::istream& is, parse_flag flag)
{
  error err;
  jv = parse(is, err, flag);

  return (jv.kind() != json::kind::error);
}

value parse(const char* data, parse_flag flag)
{
  return parse(data, std::char_traits<char>::length(data), flag);
}

value parse(const char* begin, const char* end, parse_flag flag)
{
  error err;
  auto  res = parse(begin, end, err, flag);

#ifdef THROW_PARSE_ERROR
  if (res.kind() == json::kind::error) { FORMATS_THROW(parse_except::create(err.what())); }
#endif  // THROW_PARSE_ERROR

  return res;
}

value parse(const char* begin, std::size_t len, parse_flag flag)
{
  return parse(begin, begin + len, flag);
}

value parse(std::istream& is, parse_flag flag)
{
  error err;
  auto  res = parse(is, err, flag);

#ifdef THROW_PARSE_ERROR
  if (res.kind() == json::kind::error) { FORMATS_THROW(parse_except::create(err.what())); }
#endif  // THROW_PARSE_ERROR

  return false;
}

value parse(const char* data, error& error, parse_flag flag)
{
  return parse(data, std::char_traits<char>::length(data), error, flag);
}

value parse(const char* begin, const char* end, error& error, parse_flag flag)
{
  value v;

  if (!begin || !end || begin >= end) return v;

  return formats::json::detail::parser().parse(begin, end, error, flag);

  return v;
}

value parse(const char* begin, std::size_t len, error& error, parse_flag flag)
{
  return parse(begin, begin + len, error, flag);
}

value parse(std::istream& is, error& error, parse_flag flag)
{
  value v;

  if (is.bad() || is.eof()) return v;

  return formats::json::detail::parser().parse(is, error, flag);
}

bool load(const std::string& filepath, json::value& value, parse_flag flag)
{
  bool result = false;

  std::ifstream is;
  is.open(filepath, std::ios::in | std::ios::binary);

  if (is.is_open())
  {
    result = formats::json::parse(value, is, flag);
    is.close();
  }

  return result;
}

FORMATS_JSON_NAMESPACE_END