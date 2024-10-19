#pragma once

#include <formats/common/marco.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

#ifndef JSON_MAX_DEPTH
#define JSON_MAX_DEPTH 600
#endif  // !JSON_MAX_DEPTH

/**
 * parse_flag: bit-or combination of the possible flags-enum.
 * ECMA404[strict]: the strict specification for JSON. This parser defaults to using the ECMA404
 * other is the extension based of ECMA404 for string, num .etc
 * JSON5 flag provides support for all extensions except illegal_utf8.
 *
 * pass parse_flag when parse json doc. use | to select multy flags. like num_infinity | num_nan
 */

enum class parse_flag : unsigned long long
{
  ECMA404 = 0X01,
  strict  = ECMA404,

  object_trailing_comma = strict << 1,  // support trailing comma in object
  array_trailing_comma  = strict << 2,  // support trailing comma in array
  trailing_comma        = object_trailing_comma | array_trailing_comma,

  string_single_quotes = strict << 3,  // support single quoted string
  string_unquoted      = strict << 4,  // support unquoted string
  string_multi_lines   = strict << 5,  // support line break in string

  num_leading_point = strict << 6,   // support .123 parse to 0.123
  num_ending_point  = strict << 7,   // support 123. parse to 123.00
  num_hexadecimal   = strict << 8,   // support hexadecimal number
  num_infinity      = strict << 9,   // support plus or minus Infinity
  num_nan           = strict << 10,  // support NaN

  escaped_hex  = strict << 11,  // \x00-\xFF. \x0c parse to 12
  escaped_null = strict << 12,  // \0 parse to null

  comments     = strict << 13,  // single or multi comments are permitted
  white_space  = strict << 14,  // white space are permitted.U+00A0 U+2028 U+2029 U+FEFF
  root_lenient = strict << 15,  // single value as a json is permitted

  JSON5 = ((strict << 16) - 1),  // ECMAScript 5.1

  illegal_utf8 = JSON5 + 1,
};

enum stringify_style
{
  standard = 0,
  compact  = 1,
  pretty   = 2,
};

/**
 * stringify_flag: bit-or combination of the possible flags-enum.
 *
 * strict: stringify according to ECMA404 standard.
 * other: the extension to select.
 *
 * write_single_quotes_string is mutually exclusive with write_unquoted_string, if simultaneously
 * select, write_single_quotes_string having higher priority.
 *
 * pass stringify_flag when stringify json doc. use | to select multy flags. like write_nan_number |
 * write_infinity_num
 */
enum class stringify_flag : unsigned char
{
  strict = 0X01,

  write_single_quotes_string = strict << 1,
  write_unquoted_string      = strict << 2,
  write_nan_number           = strict << 3,
  write_infinity_num         = strict << 4,
};

FORMATS_JSON_NAMESPACE_END