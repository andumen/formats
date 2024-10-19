#pragma once

#include <istream>

#include <formats/jsoncpp/value.hpp>
#include <formats/jsoncpp/flags.h>

FORMATS_JSON_NAMESPACE_BEGIN

/*
 * @brief: Parse a json value from string or stream. no throw when parse failed.
 *
 * @param:
 *  value: the reference of the value parse to.
 *  data: string to parse
 *  begin: pointer at source string begining
 *  end:   pointer at source string ending
 *  len:   length of source string. bytes.
 *  is:    isstream to parse
 *  parse_flag: bit-or combination of the possible flags-enum.
 *
 * @return: true if parse success, otherwise false.
 */
bool parse(value&, const char* data, parse_flag flag = parse_flag::strict);
bool parse(value&, const char* begin, const char* end, parse_flag flag = parse_flag::strict);
bool parse(value&, const char* begin, std::size_t len, parse_flag flag = parse_flag::strict);
bool parse(value&, std::istream& is, parse_flag flag = parse_flag::strict);

/*
 * @brief: Parse a json value from string or stream. The default behavior don't throw error when
 * parse failed. If you want throws error, define preprocessor(THROW_PARSE_ERROR).
 *
 * @param:
 *  data: string to parse
 *  begin: pointer at source string begining
 *  end:   pointer at source string ending
 *  len:   length of source string. bytes.
 *  is:    isstream to parse
 *  parse_flag: bit-or combination of the possible flags-enum.
 *
 * @return json::value parse to
 */
value parse(const char* data, parse_flag flag = parse_flag::strict);
value parse(const char* begin, const char* end, parse_flag flag = parse_flag::strict);
value parse(const char* begin, std::size_t len, parse_flag flag = parse_flag::strict);
value parse(std::istream& is, parse_flag flag = parse_flag::strict);

/*
 * @brief: Parse a json value from string or stream. Sets a error if parse failed.
 *
 * @param:
 *  error: the reference of parse error
 *  data: string to parse
 *  begin: pointer at source string begining
 *  end:   pointer at source string ending
 *  len:   length of source string. bytes.
 *  is:    isstream to parse
 *  parse_flag: bit-or combination of the possible flags-enum.
 *
 * @return json::value parse to
 */
value parse(const char* data, error& error, parse_flag flag = parse_flag::strict);
value parse(const char* begin, const char* end, error& error, parse_flag flag = parse_flag::strict);
value parse(const char* begin, std::size_t len, error& error, parse_flag flag = parse_flag::strict);
value parse(std::istream& is, error& error, parse_flag flag = parse_flag::strict);

FORMATS_JSON_NAMESPACE_END