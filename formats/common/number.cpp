#include "number.hpp"

#include <cmath>

#include <formats/common/cctypes.hpp>
#include <formats/third_party/double-conversion/double-conversion.h>
#include <formats/third_party/double-conversion/strtod.h>

namespace dcvt = double_conversion;

FORMATS_NAMESPACE_BEGIN

template <typename T, bool fix_overflow>
T stoi(const std::string& str, int base, int& bytes_trans)
{
  if (base < 0 || base == 1 || base > 36) { return 0; }

  std::make_unsigned_t<T> ans = 0;
  const char*             ptr = str.c_str();
  while (*ptr == ' ')
    ptr++;

  bool negative = false;
  bool overflow = false;

  if (*ptr == '-')
  {
    negative = true;
    ++ptr;
  }
  else if (*ptr == '+')
    ++ptr;

  if (*ptr == '0')
  {
    ptr++;

    if ((base == 0 || base == 16) && (*ptr == 'x' || *ptr == 'X'))
    {
      ptr++;
      base = 16;
    }
    else if ((base == 0 || base == 2) && (*ptr == 'b' || *ptr == 'B'))
    {
      ptr++;
      base = 2;
    }
    else if (base == 0) { base = 8; }
  }
  else if (base == 0)
    base = 10;

  const char* begin = ptr;

  auto cutoff = __flowover<sizeof(T)>::cutoff_tab[base - 2];
  auto cutlim = __flowover<sizeof(T)>::cutlim_tab[base - 2];

  unsigned char c = 0;
  while (*ptr != '\0')
  {
    c = char_to_num(*ptr);
    if (c >= base) break;

    if (ans > cutoff || (ans == cutoff && c > cutlim))
      overflow = true;
    else
      ans = (ans * base + c);

    ++ptr;
  }

  bytes_trans = (int)(ptr - begin);

  if (!overflow && std::is_signed<T>::value)
  {
    overflow = (ans > (negative ? (decltype(ans))(std::numeric_limits<T>::max)() + 1
                                : (decltype(ans))(std::numeric_limits<T>::max)()));
  }

  if (overflow)
  {
    if (fix_overflow)
      return (negative ? (std::numeric_limits<T>::min)() : (std::numeric_limits<T>::max)());
    else
      bytes_trans = 0;
  }

  return negative ? (~ans + 1) : ans;
}

bool number::parse(const char* ptr, const char* end)
{
  ptr_ = ptr;
  end_ = end;

  if (*ptr_ == c_minus_sign)
  {
    negative = true;
    ++ptr_;
  }
  else if (*ptr_ == c_plus_sign)
    ++ptr_;

  if (isdigit(*ptr_))
  {
    if (*ptr_ == c_number_zero)
    {
      ++ptr_;

      if ((end_ - ptr_ > 1) && toupper(*ptr_) == 'X')
      {
        ++ptr_;
        return from_hex_string();
      }
    }

    return from_dec_string();
  }

  // json5 number leading decimal point
  if (*ptr_ == c_decimal_point) return from_dec_string();

  return false;
}

bool number::from_hex_string()
{
  char c = -1;

  while (ptr_ < end_)
  {
    c = char_to_num(*ptr_++);

    if (c < 0) { return false; }

    auto cutoff = __flowover<sizeof(unsigned long long)>::cutoff_tab[16 - 2];
    auto cutlim = __flowover<sizeof(unsigned long long)>::cutlim_tab[16 - 2];

    if (integer > cutoff || (integer == cutoff && c > cutlim)) { return false; }

    integer = ((integer << 4) | c);
  }

  // negative number overflow
  if (negative && integer > ((unsigned long long)(std::numeric_limits<long long>::max)() + 1))
  {
    return false;
  }

  return true;
}

bool number::from_dec_string()
{
  char c = -1;

  while (ptr_ < end_ && isdigit(*ptr_))
  {
    c = char_to_num(*ptr_);

    auto cutoff = __flowover<sizeof(unsigned long long)>::cutoff_tab[10 - 2];
    auto cutlim = __flowover<sizeof(unsigned long long)>::cutlim_tab[10 - 2];

    if (!is_fraction && (integer > cutoff || (integer == cutoff && c > cutlim)))
    {
      is_fraction = true;
      fraction    = static_cast<double>(integer);
    }

    if (!is_fraction)
    {
      integer = (integer * 10 + c);

      if (negative && integer > (decltype(integer))(std::numeric_limits<long long>::max)() + 1)
      {
        is_fraction = true;
        fraction    = static_cast<double>(integer);
      }
    }
    else { fraction = (fraction * 10 + c); }

    ++ptr_;
  }

  if (ptr_ < end_ && *ptr_ == c_decimal_point)
  {
    ++ptr_;

    fraction    = is_fraction ? fraction : static_cast<double>(integer);
    is_fraction = true;

    long long digit_count   = 0;
    long long fraction_part = 0;

    while (ptr_ < end_ && isdigit(*ptr_))
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

    bool exp_neg  = false;
    int  exponent = 0;

    if (ptr_ < end_)
    {
      if (*ptr_ == c_minus_sign)
      {
        exp_neg = true;
        ++ptr_;
      }
      else if (*ptr_ == c_plus_sign)
        ++ptr_;
    }

    while (ptr_ < end_ && isdigit(*ptr_))
    {
      exponent = exponent * 10 + (*ptr_ - c_number_zero);
      ++ptr_;
    }

    if (exp_neg) exponent = -exponent;
    fraction *= std::pow(10, exponent);
  }

  return (ptr_ == end_ && (isdigit(*(ptr_ - 1)) || *(ptr_ - 1) == c_decimal_point));
}

bool dtoa(double v, char* buffer, int& size)
{
  int flags = dcvt::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN |
              dcvt::DoubleToStringConverter::UNIQUE_ZERO;

  int decimal_in_shortest_low  = -6;
  int decimal_in_shortest_high = 21;

  dcvt::DoubleToStringConverter converter(flags, nullptr, nullptr, 'e', decimal_in_shortest_low,
                                          decimal_in_shortest_high, 0, 0);

  dcvt::StringBuilder string_builder(buffer, size);

  if (converter.ToShortest(v, &string_builder))
  {
    size = string_builder.position();
    return true;
  }

  return false;
}

std::string dtoa(double v)
{
  int size = 32;

  char buffer[32] = {0};
  dtoa(v, buffer, size);

  return std::string(buffer, size);
}

std::pair<long long, bool> strtoll(const std::string& str)
{
  auto processed = 0;
  auto result    = stoi<long long, false>(str, 0, processed);

  return {result, processed != 0};
}

std::pair<unsigned long long, bool> strtoull(const std::string& str)
{
  auto processed = 0;
  auto result    = stoi<unsigned long long, false>(str, 0, processed);

  return {result, processed != 0};
}

std::pair<double, bool> strtod(const std::string& str)
{
  using namespace double_conversion;

  int flags = StringToDoubleConverter::ALLOW_TRAILING_JUNK |
              StringToDoubleConverter::ALLOW_LEADING_SPACES |
              StringToDoubleConverter::ALLOW_TRAILING_SPACES;

  StringToDoubleConverter converter(flags, 0.0, 1.0, "infinity", "NaN");

  auto processed = 0;
  auto result    = converter.StringToDouble(str.c_str(), (int)str.size(), &processed);

  return {result, processed != 0};
}

FORMATS_NAMESPACE_END
