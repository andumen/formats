#pragma once

#include <vector>
#include <string>

#include "tokens.h"

FORMATS_NAMESPACE_BEGIN

inline bool isalpha(unsigned char c)
{
  return ((c >= c_letter_A && c <= c_letter_Z) || (c >= c_letter_a && c <= c_letter_z));
}

inline bool isdigit(unsigned char c)
{
  return (c >= c_number_zero && c <= c_number_nine);
}

inline unsigned char toupper(unsigned char c)
{
  return (c >= c_letter_a && c <= c_letter_z) ? c - c_letter_a + c_letter_A : c;
}

inline bool leading_unquoted(unsigned char c)
{
  return (c > 0X20 && c != '#' && c != ':' && c != ',' && c != ']' && c != '}' && c < 0X7F);
}

inline bool leading_number(unsigned char c)
{
  return (isdigit(c) || c == c_plus_sign || c == c_minus_sign);
}

inline char char_to_num(unsigned char c)
{
  // clang-format off
  const static char char2num[256] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
      -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };

  // clang-format on

  return char2num[c];
}

inline std::string toupper(const std::string& s)
{
  std::string res(s.size(), 0);

  auto source = s.data();
  auto targer = (char*)res.data();

  while (*source != '\0')
  {
    *targer++ = toupper(*source++);
  }

  return res;
}

inline bool equal(const std::string& s1, const char* s2, int length)
{
  if (s1.size() != length) return false;

  while (length-- > 0)
  {
    if (s1[length] != *(s2 + length)) return false;
  }

  return true;
}

inline std::vector<std::string> spilt(const char* s, char separator)
{
  std::vector<std::string> result;

  size_t length = std::char_traits<char>::length(s);
  size_t begin = 0, end = 0;

  while (end < length)
  {
    if (s[end] == separator)
    {
      if (end > begin) { result.emplace_back(s + begin, end - begin); }

      begin = end + 1;
    }

    ++end;
  }

  if (end > begin) { result.emplace_back(s + begin, end - begin); }

  return result;
}

template <typename T>
void concat_to(std::string& s, const T& source)
{
  s.append(source);
}

template <typename T, typename... Args>
void concat_to(std::string& s, const T& source, Args&&... args)
{
  s.append(source);
  concat_to(s, std::forward<Args&&>(args)...);
}

template <typename... Args>
std::string concat(Args&&... args)
{
  std::string result;
  concat_to(result, std::forward<Args&&>(args)...);

  return result;
}

FORMATS_NAMESPACE_END