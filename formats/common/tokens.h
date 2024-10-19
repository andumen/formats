#pragma once

#include <formats/common/marco.hpp>

FORMATS_NAMESPACE_BEGIN

enum : unsigned char
{
  c_horizontal_tab  = 0X09,  //  '\t'
  c_line_feed       = 0X0A,  //  '\n'
  c_form_feed       = 0X0C,  //  '\n'
  c_carriage_return = 0X0D,  //  '\r'
  c_space           = 0X20,  //  ' '

  c_begin_array = 0X5B,  // '['
  c_end_array   = 0X5D,  // ']'

  c_begin_object = 0X7B,  // '{'
  c_end_object   = 0X7D,  // '}'

  c_name_separator  = 0X3A,  //  ':'
  c_double_quotes   = 0X22,  //  '"'
  c_single_quotes   = 0X27,  //  '''
  c_plus_sign       = 0X2B,  //  '+'
  c_value_separator = 0X2C,  //  ','
  c_minus_sign      = 0X2D,  //  '-'
  c_decimal_point   = 0X2E,  //  '.'

  c_reverse_solidus = 0X5C,  // '\\'
  c_solidus         = 0X2F,  // '/'
  c_asterisk        = 0X2A,  // '*'

  c_underline   = 0X5F,  // _
  c_dollar_sige = 0X24,  // $

  c_letter_A = 0X41,  // A
  c_letter_I = 0X49,  // I
  c_letter_N = 0X4E,  // N
  c_letter_Z = 0X5A,  // N

  c_letter_a = 0X61,  // 'b'
  c_letter_b = 0X62,  // 'b'
  c_letter_f = 0X66,  // 'f'
  c_letter_n = 0X6E,  // 'n'
  c_letter_r = 0X72,  // 'r'
  c_letter_t = 0X74,  // 't'
  c_letter_u = 0X75,  // 'u'
  c_letter_v = 0X76,  // 'v'
  c_letter_x = 0X78,  // 'x'
  c_letter_z = 0X7A,  // 'z'

  c_number_zero = 0X30,  // '0'
  c_number_nine = 0X39,  // '9'

  c_non_breaking_space = 0XA0,  // '0'
};

FORMATS_NAMESPACE_END