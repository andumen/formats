#pragma once

#include <istream>

#include <formats/jsoncpp/value.hpp>
#include <formats/jsoncpp/error.hpp>
#include <formats/jsoncpp/flags.h>

FORMATS_JSON_NAMESPACE_BEGIN

namespace detail
{
enum parse_action : unsigned char
{
  parse_object,
  parse_array,
  parse_key,
  parse_val,
};

class parser
{
public:
  parser()  = default;
  ~parser() = default;

  value parse(const char* begin, const char* end, error& error, parse_flag flag);

  value parse(const char* begin, std::size_t len, error& error, parse_flag flag);

  value parse(std::istream& is, error& error, parse_flag flag);

private:
  bool parse(value& v);

  bool parse_object(value& v);
  bool parse_array(value& v);
  bool parse_value(value& v);
  bool parse_value_string(value& v);
  bool parse_value_unquoted(value& v);
  bool parse_value_number(value& v);
  bool parse_value_null(value& v);
  bool parse_value_true(value& v);
  bool parse_value_false(value& v);

  bool parse_object_element(value::object_t& object);
  bool parse_key(std::string& key);

  bool parse_value_digit(value& v);
  bool parse_value_hexadecimal(value& v);
  bool parse_value_nan(value& v);
  bool parse_value_infinity(value& v);
  bool parse_string_quoted(std::string& buffer);
  bool parse_string_unquoted(std::string& buffer, parse_action action);

private:
  bool skip_space_and_comments();
  void skip_multi_bytes(size_t length);

  bool is_comment_start();

  bool skip_comment();
  bool skip_single_line_comment();
  bool skip_multi_line_comment();

private:
  bool parse_escaped_sequence(std::string& buffer);
  bool parse_escaped_zero(std::string& buffer);
  bool parse_escaped_hexnum(std::string& buffer);
  bool parse_utf8_sequence();
  bool parse_utf16_sequence(std::string& buffer);

private:
  bool has_error();
  bool skip_if_starts_with(const char* s, int length);

private:
  bool ensure_multi_bytes(int length);
  void sync_begin_pos();
  bool eof();
  void feed();

private:
  void throw_error(error_code, const char* errmsg = nullptr);

private:
  const char* begin_ = nullptr;
  const char* ptr_   = nullptr;
  const char* end_   = nullptr;

  error error_;

  size_t                  is_buf_size = 4096;
  std::istream*           is_         = nullptr;
  std::unique_ptr<char[]> is_buf_;

  int line_  = 1;
  int depth_ = 0;

  parse_flag flag = parse_flag::ECMA404;
};

}  // namespace detail

FORMATS_JSON_NAMESPACE_END
