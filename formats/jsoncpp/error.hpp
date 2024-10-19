#pragma once

#include <string>
#include <array>
#include <formats/common/marco.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

enum error_code
{
  none = 0,
  illegal_unicode,
  missing_begin_object_array,
  missing_end_object,
  missing_end_array,
  missing_quotation_mark,
  missing_name_separator,
  missing_value_separator,
  surplus_trailing_comma,
  tab_in_string,
  linefeed_in_string,
  expect_null,
  expect_true,
  expect_false,
  leading_number_zero,
  illeagl_number,
  illeagl_escaped,
  illeagl_character,
  illeagl_comments,

  too_deep,
  no_end,
};

inline const char* error_desc(error_code ec)
{
  const char* messages[] = {
      "none",                    /*none*/
      "illegal unicode",         /*illegal_unicode*/
      "missing begin",           /*missing_begin_object_array*/
      "missing end object",      /*missing_end_object*/
      "missing end array",       /*missing_end_array*/
      "missing quotation",       /*missing_quotation_mark*/
      "missing name separator",  /*missing_name_separator*/
      "missing value separator", /*missing_value_separator*/
      "surplus trailing comma",  /*surplus_trailing_comma*/
      "tab in string",           /*tab_in_string*/
      "linefeed in string",      /*linefeed_in_string*/
      "expect null",             /*expect_null*/
      "expect true",             /*expect_true*/
      "expect false",            /*expect_false*/
      "leading number zero",     /*leading_number_zero*/
      "illeagl number",          /*illeagl_number*/
      "illeagl escaped",         /*illeagl_escaped*/
      "illeagl character",       /*illeagl_character*/
      "illeagl comments",        /*illeagl_comments*/
      "too deep",                /*too_deep*/
      "no end",                  /*no_end*/
  };

  return messages[ec];
}

class error
{
public:
  error()
      : code_(none)
  {}

  error(error_code ec)
      : code_(ec)
  {}

  error(error_code ec, const char* msg)
      : code_(ec)
      , message_(msg)
  {}

public:
  inline operator bool(void) { return code_ != error_code::none; }

public:
  void set(error_code ec) noexcept { code_ = ec; }
  void set(error_code ec, int line, const char* c) noexcept
  {
    char __msg[256] = {0};
    sprintf(__msg, "Line[%d] Parse Character[%c], Error: %s", line, *c, error_desc(ec));

    code_ = ec;
    message_.assign(__msg);
  }

  void set(error_code ec, const char* msg, int line, const char* c) noexcept
  {
    char __msg[256] = {0};
    sprintf(__msg, "Line[%d] Parse Character[%c], Error: %s", line, *c, msg);

    code_ = ec;
    message_.assign(__msg);
  }

public:
  const char* what() const noexcept { return message_.c_str(); }

  int code() { return code_; }

private:
  error_code  code_ = error_code::none;
  std::string message_;

  friend class detail::parser;
};

FORMATS_JSON_NAMESPACE_END