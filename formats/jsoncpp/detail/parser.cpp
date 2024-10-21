#include <formats/jsoncpp/detail/parser.hpp>

#include <cmath>
#include <cstring>

#include <formats/common/cctypes.hpp>
#include <formats/common/number.hpp>
#include <formats/common/unicode/unicode.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

namespace detail
{
// clang-format off
#define json_parse_flag(__flag) (((unsigned long long)flag & (unsigned long long)__flag) == (unsigned long long)__flag)


#define parse_array_trailing_comma   json_parse_flag(parse_flag::array_trailing_comma)
#define parse_object_trailing_comma  json_parse_flag(parse_flag::object_trailing_comma)
#define parse_single_quotes          json_parse_flag(parse_flag::string_single_quotes)
#define parse_unquoted_string        json_parse_flag(parse_flag::string_unquoted)
#define parse_multi_line_string      json_parse_flag(parse_flag::string_multi_lines)
#define parse_leading_dp_num         json_parse_flag(parse_flag::num_leading_point)
#define parse_ending_dp_num          json_parse_flag(parse_flag::num_ending_point)
#define parse_hex_num                json_parse_flag(parse_flag::num_hexadecimal)
#define parse_infinity_num           json_parse_flag(parse_flag::num_infinity)
#define parse_nan_num                json_parse_flag(parse_flag::num_nan)
#define parse_escaped_hex            json_parse_flag(parse_flag::escaped_hex)
#define parse_escaped_null           json_parse_flag(parse_flag::escaped_null)
#define parse_comments               json_parse_flag(parse_flag::comments)
#define parse_add_white_space        json_parse_flag(parse_flag::white_space)
#define parse_lenient_root           json_parse_flag(parse_flag::root_lenient)
#define parse_illegal_utf8           json_parse_flag(parse_flag::illegal_utf8)


#define reach_key_end(c)  (c == c_name_separator)
#define reach_val_end(c)  (c == c_value_separator || c == c_end_array || c == c_end_object)
#define is_white_space(c) (c == c_space || c == c_horizontal_tab || c == c_carriage_return || c == c_line_feed)

#define none_error (error_.code_ == error_code::none)
// clang-format on

unsigned long hex_code_point(const char* ptr)
{
  unsigned long code = 0;

  for (int i = 0; i < 4; ++i)
  {
    code = (code << 4) + char_to_num(*(ptr + i));
  }

  return code;
}

class DepthAutoCounter
{
public:
  DepthAutoCounter(int& depth)
      : depth_(depth)
  {
    ++depth_;
  }
  ~DepthAutoCounter() { --depth_; }

private:
  int& depth_;
};

value parser::parse(const char* begin, const char* end, error& error, parse_flag flag)
{
  if (!begin || !end || begin >= end) return value();

  this->begin_ = begin;
  this->ptr_   = begin;
  this->end_   = end;
  this->flag   = flag;

  value val;
  if (!parse(val))
  {
    val   = kind::error;
    error = error_;
  }

  return val;
}

value parser::parse(const char* begin, std::size_t len, error& error, parse_flag flag)
{
  return (begin && len) ? parse(begin, begin + len, error, flag) : value();
}

value parser::parse(std::istream& is, error& error, parse_flag flag)
{
  if (is.bad() || is.eof()) return value();

  {
    is_ = &is;

    is_buf_ = std::unique_ptr<char[]>(new char[is_buf_size]);
    memset(is_buf_.get(), 0, is_buf_size);
    is_->read(is_buf_.get(), is_buf_size - 1);

    this->begin_ = is_buf_.get();
    this->ptr_   = is_buf_.get();
    this->end_   = is_buf_.get() + std::char_traits<char>::length(is_buf_.get());
    this->flag   = flag;
  }

  value val;
  if (!parse(val))
  {
    val   = kind::error;
    error = error_;
  }

  return val;
}

bool parser::parse(value& v)
{
  if ((end_ - ptr_ >= 3) && unicode::start_with_u8bom(ptr_, end_)) skip_multi_bytes(3);

  if (skip_space_and_comments())
  {
    if (*ptr_ == c_begin_object) return parse_object(v);
    if (*ptr_ == c_begin_array) return parse_array(v);

    if (parse_lenient_root)  // JSON5 top level can be a value
    {
      if (*ptr_ == c_double_quotes)
        parse_value_string(v);
      else if (*ptr_ == c_single_quotes)
        parse_value_string(v);
      else if (leading_unquoted(*ptr_))
        parse_value_unquoted(v);
      else if (leading_number(*ptr_))
        parse_value_number(v);
      else if (*ptr_ == c_letter_n)
        parse_value_null(v);
      else if (*ptr_ == c_letter_t)
        parse_value_true(v);
      else if (*ptr_ == c_letter_f)
        parse_value_false(v);
      else if (*ptr_ == c_letter_N)
        parse_value_nan(v);
      else if (*ptr_ == c_letter_I)
        parse_value_infinity(v);
      else
      {
        throw_error(error_code::missing_begin_object_array);
        return false;
      }

      skip_space_and_comments();
      return none_error;
    }
  }

  throw_error(error_code::missing_begin_object_array);
  return false;
}

bool parser::parse_object_element(value::object_t& object)
{
  std::string key;
  if (!parse_key(key) || !skip_space_and_comments()) return false;

  if (*ptr_ != c_name_separator)
  {
    throw_error(error_code::missing_name_separator);
    return false;
  }

  ++ptr_;

  if (skip_space_and_comments())
  {
    auto insert_ret = object.emplace(std::move(key), nullptr);
    return parse_value(insert_ret.first->second);
  }

  throw_error(error_code::illeagl_character, "expecte object value");
  return false;
}

bool parser::parse_object(value& v)
{
  DepthAutoCounter depth(depth_);
  if (depth_ > JSON_MAX_DEPTH)
  {
    throw_error(error_code::too_deep);
    return false;
  }

  skip_multi_bytes(1);

  value::object_t object;

  bool has_trailing_comma = false;

  unsigned char c = -1;
  while (skip_space_and_comments())
  {
    c = *ptr_;

    if (c == c_end_object) break;

    if (likely(c == c_double_quotes))
      parse_object_element(object);
    else if (c == c_single_quotes && parse_single_quotes)
      parse_object_element(object);
    else if (leading_unquoted(c) && parse_unquoted_string)
      parse_object_element(object);
    else
      throw_error(error_code::missing_quotation_mark);

    if (!none_error || !skip_space_and_comments()) { return false; }

    has_trailing_comma = false;
    if (*ptr_ != c_value_separator) break;

    has_trailing_comma = true;
    skip_multi_bytes(1);
  }

  if (ptr_ < end_ && *ptr_ == c_end_object)
  {
    if (has_trailing_comma && !parse_object_trailing_comma)
    {
      throw_error(error_code::surplus_trailing_comma);
      return false;
    }

    v.kind_ = kind::object;
    new (&v.data_.v_object_) value::object_t(std::move(object));

    skip_multi_bytes(1);
    return true;
  }

  throw_error(error_code::missing_end_object);
  return false;
}

bool parser::parse_array(value& v)
{
  DepthAutoCounter line(depth_);
  if (depth_ > JSON_MAX_DEPTH)
  {
    throw_error(error_code::too_deep);
    return false;
  }

  ++ptr_;

  value::array_t array;

  bool has_trailing_comma = false;

  while (skip_space_and_comments())
  {
    if (*ptr_ == c_end_array) break;

    has_trailing_comma = false;
    if (!parse_value(array.emplace_back(nullptr)) || !skip_space_and_comments()) { return false; }

    if (*ptr_ != c_value_separator) break;

    has_trailing_comma = true;
    skip_multi_bytes(1);
  }

  if (ptr_ < end_ && *ptr_ == c_end_array)
  {
    if (has_trailing_comma && !parse_array_trailing_comma)
    {
      throw_error(error_code::surplus_trailing_comma);
      return false;
    }

    skip_multi_bytes(1);

    v.kind_ = kind::array;
    new (&v.data_.v_array_) value::array_t(std::move(array));

    return true;
  }

  throw_error(error_code::missing_end_array);
  return false;
}

bool parser::parse_value(value& v)
{
  unsigned char c = *ptr_;

  if (likely(c == c_begin_object)) return parse_object(v);
  if (likely(c == c_begin_array)) return parse_array(v);
  if (likely(c == c_double_quotes)) return parse_value_string(v);
  if (c == c_single_quotes && parse_single_quotes) return parse_value_string(v);
  if (leading_unquoted(c) && parse_unquoted_string) return parse_value_unquoted(v);
  if (leading_number(c)) return parse_value_number(v);
  if (c == c_letter_n) return parse_value_null(v);
  if (c == c_letter_t) return parse_value_true(v);
  if (c == c_letter_f) return parse_value_false(v);
  if (c == c_letter_N && parse_nan_num) return parse_value_nan(v);
  if (c == c_letter_I && parse_infinity_num) return parse_value_infinity(v);

  throw_error(error_code::illeagl_character, "illeagl value");
  return false;
}

bool parser::parse_value_string(value& v)
{
  std::string buffer;
  if (parse_string_quoted(buffer))
  {
    v.kind_ = kind::string;
    new (&v.data_.v_string_) value::string_t(std::move(buffer));

    return true;
  }

  return false;
}

bool parser::parse_value_unquoted(value& v)
{
  std::string buffer;

  if (!parse_string_unquoted(buffer, parse_action::parse_val)) { return false; }

  {
    number num;
    if (num.parse(buffer.c_str(), buffer.c_str() + buffer.size()))
    {
      if (unlikely(num.is_fraction))
      {
        v.kind_ = kind::number_float;
        new (&v.data_.v_double_) value::number_float_t(num.negative ? -num.fraction : num.fraction);
      }
      else if (num.negative)
      {
        v.kind_ = kind::number_int;
        new (&v.data_.v_int_) value::number_int_t((long long)(~num.integer + 1));
      }
      else
      {
        v.kind_ = kind::number_uint;
        new (&v.data_.v_uint_) value::number_uint_t(num.integer);
      }

      return true;
    }
  }

  if (equal(buffer, "true", 4))
  {
    v.kind_ = kind::boolean;
    new (&v.data_.v_bool_) value::boolean_t(true);

    return true;
  }

  if (equal(buffer, "false", 5))
  {
    v.kind_ = kind::boolean;
    new (&v.data_.v_bool_) value::boolean_t(false);

    return true;
  }

  if (equal(buffer, "null", 4)) { return true; }

  if (parse_nan_num && equal(buffer, "NaN", 3))
  {
    v.kind_ = kind::number_float;
    new (&v.data_.v_double_) value::number_float_t(std::numeric_limits<double>::quiet_NaN());

    return true;
  }

  if (parse_infinity_num)
  {
    if (equal(buffer, "Infinity", 8) || equal(buffer, "+Infinity", 9))
    {
      v.kind_ = kind::number_float;
      new (&v.data_.v_double_) value::number_float_t(std::numeric_limits<double>::infinity());

      return true;
    }

    if (equal(buffer, "-Infinity", 9))
    {
      v.kind_ = kind::number_float;
      new (&v.data_.v_double_) value::number_float_t(-std::numeric_limits<double>::infinity());

      return true;
    }
  }

  v.kind_ = kind::string;
  new (&v.data_.v_string_) value::string_t(std::move(buffer));

  return true;
}

bool parser::parse_value_number(value& v)
{
  sync_begin_pos();

  unsigned char c = *ptr_;

  if (c == c_minus_sign || c == c_plus_sign) ++ptr_;

  if (ensure_multi_bytes(1))
  {
    c = *ptr_;

    if (likely(isdigit(c))) return parse_value_digit(v);
    if (c == c_letter_I) return parse_value_infinity(v);

    throw_error(error_code::illeagl_number);
    return false;
  }

  throw_error(error_code::no_end);
  return false;
}

bool parser::parse_value_null(value& v)
{
  if (skip_if_starts_with("null", 4))
  {
    v.kind_ = kind::null;
    return true;
  }

  throw_error(error_code::expect_null);
  return false;
}

bool parser::parse_value_true(value& v)
{
  if (skip_if_starts_with("true", 4))
  {
    v.kind_ = kind::boolean;
    new (&v.data_.v_bool_) value::boolean_t(true);

    return true;
  }

  throw_error(error_code::expect_true);
  return false;
}

bool parser::parse_value_false(value& v)
{
  if (skip_if_starts_with("false", 5))
  {
    v.kind_ = kind::boolean;
    new (&v.data_.v_bool_) value::boolean_t(false);

    return true;
  }

  throw_error(error_code::expect_false);
  return false;
}

bool parser::parse_value_digit(value& v)
{
  const char* leading_zero_ptr = nullptr;

  if (*ptr_ == c_number_zero && ensure_multi_bytes(2))
  {
    ++ptr_;
    if ((toupper(*ptr_) == 'X') && parse_hex_num) return parse_value_hexadecimal(v);
    leading_zero_ptr = ptr_ - 1;
  }

  bool negative    = (*begin_ == c_minus_sign);
  bool is_fraction = false;

  value::number_uint_t  integer  = 0;
  value::number_float_t fraction = 0.0;

  auto cutoff = __flowover<sizeof(integer)>::cutoff_tab[8];
  auto cutlim = __flowover<sizeof(integer)>::cutlim_tab[8];

  unsigned char c = *ptr_;

  while (!eof() && isdigit(*ptr_))
  {
    c = char_to_num(*ptr_);

    if (!is_fraction && (integer > cutoff || (integer == cutoff && c > cutlim)))
    {
      is_fraction = true;
      fraction    = static_cast<value::number_float_t>(integer);
    }

    if (!is_fraction)
    {
      integer = integer * 10 + c;

      if (negative && integer > (decltype(integer))(std::numeric_limits<long long>::max)() + 1)
      {
        is_fraction = true;
        fraction    = static_cast<value::number_float_t>(integer);
      }
    }
    else { fraction = fraction * 10 + c; }

    ++ptr_;
  }

  if (ptr_ < end_ && *ptr_ == c_decimal_point)
  {
    ++ptr_;
    fraction    = is_fraction ? fraction : static_cast<double>(integer);
    is_fraction = true;

    long long digit_count   = 0;
    long long fraction_part = 0;

    while (!eof() && isdigit(*ptr_))
    {
      ++digit_count;
      fraction_part = fraction_part * 10 + char_to_num(*ptr_);

      ++ptr_;
    }

    fraction += (static_cast<double>(fraction_part) * std::pow(10, -digit_count));
  }

  if (ptr_ < end_ && (*ptr_ == 'e' || *ptr_ == 'E'))
  {
    ++ptr_;

    fraction    = is_fraction ? fraction : static_cast<double>(integer);
    is_fraction = true;

    bool      exp_neg  = false;
    long long exponent = 0;

    if (ensure_multi_bytes(1))
    {
      if (*ptr_ == c_minus_sign)
      {
        exp_neg = true;
        ++ptr_;
      }
      else if (*ptr_ == c_plus_sign)
        ++ptr_;
    }

    while (!eof() && isdigit(*ptr_))
    {
      exponent = exponent * 10 + char_to_num(*ptr_);
      ++ptr_;
    }

    if (exp_neg) exponent = -exponent;
    fraction *= std::pow(10, exponent);
  }

  if (eof())
  {
    throw_error(error_code::no_end);
    return false;
  }

  if (leading_zero_ptr && (ptr_ - leading_zero_ptr > 1) &&
      *(leading_zero_ptr + 1) != c_decimal_point)
  {
    throw_error(error_code::leading_number_zero);
    return false;
  }

  if (unlikely(is_fraction))
  {
    v.kind_ = kind::number_float;
    new (&v.data_.v_double_) value::number_float_t(negative ? -fraction : fraction);
  }
  else if (negative)
  {
    v.kind_ = kind::number_int;
    new (&v.data_.v_int_) value::number_int_t((long long)(~integer + 1));
  }
  else
  {
    v.kind_ = kind::number_uint;
    new (&v.data_.v_uint_) value::number_uint_t(integer);
  }

  sync_begin_pos();
  return true;
}

bool parser::parse_value_hexadecimal(value& v)
{
  ++ptr_;

  auto cutoff = __flowover<sizeof(unsigned long long)>::cutoff_tab[16 - 2];
  auto cutlim = __flowover<sizeof(unsigned long long)>::cutlim_tab[16 - 2];

  value::number_uint_t number = 0;

  char c = 0;
  while (!eof())
  {
    c = char_to_num(*ptr_);

    if (unlikely(c < 0)) break;

    if (number > cutoff || (number == cutoff && (unsigned char)c > cutlim))
    {
      throw_error(error_code::illeagl_number, "hex num out of range");
      break;
    }

    number = ((number << 4) | c);
    ++ptr_;
  }

  if (*begin_ == c_minus_sign)
  {
    v.kind_ = kind::number_int;
    new (&v.data_.v_int_) value::number_int_t((int64_t)(~number + 1));
  }
  else
  {
    v.kind_ = kind::number_uint;
    new (&v.data_.v_int_) value::number_uint_t(number);
  }

  return none_error;
}

bool parser::parse_value_nan(value& v)
{
  if (skip_if_starts_with("NaN", 3))
  {
    v.kind_ = kind::number_float;
    new (&v.data_.v_double_) value::number_float_t(std::numeric_limits<double>::quiet_NaN());

    return true;
  }

  throw_error(error_code::illeagl_number, "expected NaN");
  return false;
}

bool parser::parse_value_infinity(value& v)
{
  if (skip_if_starts_with("Infinity", 8))
  {
    auto inf_val = (*begin_ != c_minus_sign) ? std::numeric_limits<double>::infinity()
                                             : -std::numeric_limits<double>::infinity();

    v.kind_ = kind::number_float;
    new (&v.data_.v_double_) value::number_float_t(inf_val);

    return true;
  }

  throw_error(error_code::illeagl_number, "expected Infinity");
  return false;
}

bool parser::parse_key(std::string& key)
{
  return (*ptr_ == c_double_quotes || *ptr_ == c_single_quotes)
             ? parse_string_quoted(key)
             : parse_string_unquoted(key, parse_action::parse_key);
}

bool parser::is_comment_start()
{
  if (*ptr_ == c_solidus && parse_comments)
  {
    return (ensure_multi_bytes(2) && (*(ptr_ + 1) == c_solidus || *(ptr_ + 1) == c_asterisk));
  }

  return false;
}

bool parser::skip_comment()
{
  switch (*(ptr_ + 1))
  {
    case c_solidus: return skip_single_line_comment();
    case c_asterisk: return skip_multi_line_comment();

    default: throw_error(error_code::illeagl_comments); return false;
  }
}

bool parser::skip_single_line_comment()
{
  skip_multi_bytes(2);

  while (!eof())
  {
    if (*ptr_ == c_line_feed || *ptr_ == c_carriage_return)
    {
      skip_multi_bytes(1);
      break;
    }

    ++ptr_;
  }

  return true;
}

bool parser::skip_multi_line_comment()
{
  skip_multi_bytes(2);

  while (!eof())
  {
    if (*ptr_ == c_asterisk && ensure_multi_bytes(2) && *(ptr_ + 1) == c_solidus)
    {
      skip_multi_bytes(2);
      return true;
    }

    ++ptr_;
  }

  throw_error(error_code::illeagl_comments, "unclosed multi comments");
  return false;
}

bool parser::skip_space_and_comments()
{
  while (!eof())
  {
    unsigned char c = *ptr_;

    if (likely(c == c_space || c == c_horizontal_tab || c == c_form_feed))
    {
      ++ptr_;
      continue;
    }

    if (likely(c == c_carriage_return))  // terminated with `\r`
    {
      ++line_;
      ++ptr_;

      // terminated with `\r\n`
      if (ensure_multi_bytes(1) && *ptr_ == c_line_feed) ++ptr_;

      continue;
    }

    if (likely(c == c_line_feed))  // terminated with `\n`.
    {
      ++ptr_;
      ++line_;
      continue;
    }

    if (parse_comments && c == c_solidus && skip_comment()) { continue; }

    if (parse_add_white_space)
    {
      if (c == c_non_breaking_space)
      {
        ++ptr_;
        continue;
      }

      if (unlikely(c == c_reverse_solidus &&
                   ((::strncmp(ptr_, "\u0028", 6) == 0) || (::strncmp(ptr_, "\u0029", 6) == 0) ||
                    (::strncmp(ptr_, "\uFEFF", 6) == 0))))
      {
        ptr_ += 6;
        continue;
      }

      if (c == 0XFE && ensure_multi_bytes(1) && (unsigned char)*ptr_ == 0XFF)
      {
        ptr_ += 2;
        continue;
      }
    }

    break;
  }

  sync_begin_pos();
  return !eof();
}

void parser::skip_multi_bytes(size_t length)
{
  ptr_ += length;
  begin_ = ptr_;
}

bool parser::parse_string_quoted(std::string& buffer)
{
  unsigned char quoted_ch = *ptr_;
  skip_multi_bytes(1);

  unsigned char c = -1;

  while (!eof())
  {
    c = *ptr_;

    if (unlikely(c == quoted_ch))
    {
      buffer.append(begin_, ptr_ - begin_);
      skip_multi_bytes(1);
      return true;
    }

    if (unlikely(c == c_reverse_solidus))
    {
      if (parse_escaped_sequence(buffer)) continue;

      return false;
    }

    if (unlikely(c == c_horizontal_tab))
    {
      throw_error(error_code::tab_in_string);
      return false;
    }

    if (unlikely(c == c_line_feed))
    {
      if (parse_multi_line_string)
      {
        ++ptr_;
        continue;
      }

      throw_error(error_code::linefeed_in_string);
      return false;
    }

    if (likely(!parse_illegal_utf8 && !parse_utf8_sequence()))
    {
      throw_error(error_code::illegal_unicode);
      return false;
    }
  }

  throw_error(error_code::missing_quotation_mark);
  return false;
}

bool parser::parse_string_unquoted(std::string& buffer, parse_action action)
{
  unsigned char c = -1;

  while (!eof())
  {
    c = *ptr_;

    if (unlikely(c == c_reverse_solidus))
    {
      if (parse_escaped_sequence(buffer)) continue;

      return false;
    }

    if (unlikely((reach_key_end(c) && action == parse_action::parse_key) ||
                 (reach_val_end(c) && action == parse_action::parse_val) ||
                 (is_comment_start() && action == parse_action::parse_val) || (c == c_line_feed)))
    {
      auto tmp = ptr_ - 1;
      while (is_white_space(*tmp))
        --tmp;

      buffer.append(begin_, tmp - begin_ + 1);

      return true;
    }

    if (unlikely(c == c_horizontal_tab))
    {
      throw_error(error_code::tab_in_string);
      return false;
    }

    if (likely(!parse_illegal_utf8 && !parse_utf8_sequence()))
    {
      throw_error(error_code::illegal_unicode);
      return false;
    }
  }

  if (ptr_ - begin_ > 0) buffer.append(begin_, ptr_ - begin_);

  return true;
}

bool parser::parse_escaped_sequence(std::string& buffer)
{
  buffer.append(begin_, ptr_ - begin_);
  sync_begin_pos();

  ++ptr_;
  if (!ensure_multi_bytes(1))
  {
    throw_error(error_code::no_end);
    return false;
  }

  switch (*ptr_)
  {
    case c_double_quotes:
    case c_single_quotes:
    case c_reverse_solidus:
    case c_solidus:
    case c_letter_b:
    case c_letter_f:
    case c_letter_n:
    case c_letter_r:
    case c_letter_t:
    case c_letter_v: ++ptr_; break;

    case c_line_feed:
    case c_carriage_return:
    case c_horizontal_tab: break;

    case c_letter_u: parse_utf16_sequence(buffer); break;
    case c_letter_x: parse_escaped_hexnum(buffer); break;
    case c_number_zero: parse_escaped_zero(buffer); break;

    default: throw_error(error_code::illeagl_escaped); break;
  }

  return none_error;
}

bool parser::parse_escaped_zero(std::string& buffer)
{
  if (parse_escaped_null)
  {
    buffer.append("null");
    return true;
  }

  throw_error(error_code::illeagl_escaped, "escaped zero. please set parse flag");
  return false;
}

bool parser::parse_escaped_hexnum(std::string& buffer)
{
  if (!parse_escaped_hex)
  {
    throw_error(error_code::illeagl_escaped, "escaped hex num. please set parse flag");
    return false;
  }

  if (!ensure_multi_bytes(3))
  {
    throw_error(error_code::illeagl_escaped, "escaped hex num need two characters");
    return false;
  }

  char c1 = char_to_num(*(ptr_ + 1));
  char c2 = char_to_num(*(ptr_ + 2));

  if (c1 == -1 || c2 == -1)
  {
    throw_error(error_code::illeagl_escaped, "escaped hex num only in range[0-F]");
    return false;
  }

  skip_multi_bytes(3);
  buffer.append(std::to_string((int)(c1 << 4) + c2));
  return true;
}

bool parser::parse_utf8_sequence()
{
  auto length = unicode::u8sequence_length(*ptr_);

  if (likely(length == 1))
  {
    ptr_++;
    return true;
  }

  if (ensure_multi_bytes(length) && unicode::is_legal_u8sequence(ptr_, ptr_ + length))
  {
    ptr_ += length;
    return true;
  }

  return false;
}

bool parser::parse_utf16_sequence(std::string& buffer)
{
  ++ptr_;  // current character is u, move 1 bytes

  if (ensure_multi_bytes(4))
  {
    auto code = hex_code_point(ptr_);

    if (unlikely(code >= 0xD800 && code <= 0xDBFF))
    {
      ptr_ += 4;
      if (ensure_multi_bytes(6) && *ptr_ == c_reverse_solidus && *(ptr_ + 1) == c_letter_u)
      {
        ptr_ += 2;
        code = hex_code_point(ptr_);

        if (code >= 0xDC00 && code <= 0xDFFF)
        {
          ptr_ += 4;
          return true;
        }
      }
    }
    else if (likely(code > 0X7F))
    {
      ptr_ += 4;
      return true;
    }
    else
    {
      buffer.append(1, (unsigned char)code);  // ascii: trans to utf8
      skip_multi_bytes(4);

      return true;
    }
  }

  throw_error(error_code::illegal_unicode);
  return false;
}

bool parser::skip_if_starts_with(const char* s, int length)
{
  if (ensure_multi_bytes(length) && (::strncmp(ptr_, s, length) == 0))
  {
    skip_multi_bytes(length);
    return true;
  }

  return false;
}

bool parser::ensure_multi_bytes(int length)
{
  if (end_ - ptr_ < length) feed();

  return (end_ - ptr_ >= length);
}

void parser::sync_begin_pos()
{
  begin_ = ptr_;
}

bool parser::eof()
{
  if (ptr_ < end_) return false;

  if (!is_->eof()) { feed(); }

  return (ptr_ >= end_);
}

void parser::feed()
{
  feed_count++;

  auto data_size = end_ - begin_;

  if (begin_ == is_buf_.get())
  {
    is_buf_size <<= 1;

    std::unique_ptr<char[]> new_buff(new char[is_buf_size]);
    memset(new_buff.get(), 0, is_buf_size);
    memcpy(new_buff.get(), is_buf_.get(), data_size);

    is_buf_ = std::move(new_buff);
    is_->read(is_buf_.get() + data_size, (is_buf_size - data_size - 1));

    begin_ = is_buf_.get();
    ptr_   = is_buf_.get() + data_size;
    end_   = is_buf_.get() + std::char_traits<char>::length(is_buf_.get());

    return void();
  }

  memmove(is_buf_.get(), begin_, data_size);

  ptr_   = is_buf_.get() + (ptr_ - begin_);
  begin_ = is_buf_.get();

  memset((char*)begin_ + data_size, 0, is_buf_size - data_size);
  is_->read((char*)begin_ + data_size, is_buf_size - data_size - 1);

  end_ = begin_ + std::char_traits<char>::length(begin_);
}

bool parser::has_error()
{
  return error_.code_ != error_code::none;
}

void parser::throw_error(error_code code, const char* errmsg)
{
  char __msg[256] = {0};

  if (errmsg)
    sprintf(__msg, "Line[%d] Parse Character[%c], Error: %s", line_, *ptr_, errmsg);
  else
    sprintf(__msg, "Line[%d] Parse Character[%c], Error: %s", line_, *ptr_, error_desc(code));

  error_.code_ = code;
  error_.message_.assign(__msg);
}

}  // namespace detail

FORMATS_JSON_NAMESPACE_END
