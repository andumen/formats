/*===--- unicode.hpp - Universal Character Names conversions ---------------===
 *
 * reference resources llvm convertUTF.h/cpp
 *
 *==------------------------------------------------------------------------==*/

#pragma once

#include <iterator>
#include <string>
#include <formats/common/marco.hpp>

FORMATS_NAMESPACE_BEGIN

namespace unicode
{

/*
 * @brief: get length of utf8 character
 * @param[c]: utf8 character
 * @ret: the length, 0 if not utf8
 */
inline int u8sequence_length(unsigned char uc)
{
  static const char TRAILING_BYTES_FOR_UTF8[256] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

  return TRAILING_BYTES_FOR_UTF8[uc] + 1;
}

/*
 * @brief: verify utf8 sequence is start with uft8 bom header
 * @param[begin]: utf8 sequence begin
 * @param[end]: utf8 sequence end
 */
template <typename u8iter>
bool start_with_u8bom(u8iter begin, u8iter end)
{
  static const unsigned char bom_header[] = {0xEF, 0xBB, 0xBF};

  auto iter = begin;
  return ((iter != end) && ((unsigned char)*iter++ == bom_header[0])) &&
         ((iter != end) && ((unsigned char)*iter++ == bom_header[1])) &&
         ((iter != end) && ((unsigned char)*iter++ == bom_header[2]));
}

// inline bool start_with_u16bebom(const char* begin, const char* end)
//{
//   static const unsigned char bom_header[] = {0xFE, 0xFF};
//
//   auto iter = begin;
//   return ((iter != end) && ((unsigned char)*iter++ == bom_header[0])) &&
//          ((iter != end) && ((unsigned char)*iter++ == bom_header[1]));
// }
//
// inline bool start_with_u16lebom(const char* begin, const char* end)
//{
//   static const unsigned char bom_header[] = {0xFF, 0xFE};
//
//   auto iter = begin;
//   return ((iter != end) && ((unsigned char)*iter++ == bom_header[0])) &&
//          ((iter != end) && ((unsigned char)*iter++ == bom_header[1]));
// }

/*
 * verify an utf8 character is legal
 * @param[begin]: utf8 sequence begin
 * @param[length]:  the character length
 */
template <typename u8iter>
bool is_legal_utf8(u8iter begin, size_t length);

/*
 * verify an utf8 string is legal
 * @param[begin]: the string begin
 * @param[end]:  the string end
 */
template <typename u8iter>
bool is_legal_u8sequence(u8iter begin, u8iter end)
{
  auto length = u8sequence_length(*begin);
  return (length <= (end - begin)) && is_legal_utf8(begin, length);
}

template <typename u8iter>
bool is_legal_utf8(u8iter source, size_t length)
{
  unsigned char c;
  u8iter        srcptr = (u8iter)(source + length);

  switch (length)
  {
    default: return false;
    case 4:
      if ((c = (*--srcptr)) < 0x80 || c > 0xBF) return false;
    case 3:
      if ((c = (*--srcptr)) < 0x80 || c > 0xBF) return false;
    case 2:
      if ((c = (*--srcptr)) < 0x80 || c > 0xBF) return false;

      switch (*((unsigned char*)source))
      {
        case 0xE0:
          if (c < 0xA0) return false;
          break;
        case 0xED:
          if (c > 0x9F) return false;
          break;
        case 0xF0:
          if (c < 0x90) return false;
          break;
        case 0xF4:
          if (c > 0x8F) return false;
          break;
        default:
          if (c < 0x80) return false;
      }

    case 1:
      if (*source >= 0x80 && *source < 0xC2) return false;
  }

  return (*source <= 0xF4) ? true : false;
}

}  // namespace unicode

FORMATS_NAMESPACE_END
