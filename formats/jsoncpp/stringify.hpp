#pragma once

#include <string>
#include <formats/jsoncpp/value.hpp>
#include <formats/jsoncpp/flags.h>

FORMATS_JSON_NAMESPACE_BEGIN

/*
 * @brief: stringify a json value to string.
 *
 * @param:
 *  value: the reference of the value which to stringify.
 *  stringify_style: stringify style to select.
 *  stringify_flag: bit-or combination of the possible flags-enum.
 *
 * @return: the result of stringfiy.
 */
std::string stringify(const value&    value,
                      stringify_style style = stringify_style::pretty,
                      stringify_flag  flag  = stringify_flag::strict) noexcept;

/*
 * @brief: stringify a json value and save to file.
 *
 * @param:
 *  filepath: the absolute path of the file to load.
 *  value: the reference of the value which to stringify.
 *  stringify_style: stringify style to select.
 *
 * @return: void.
 */
void dump(const std::string& filepath,
          const json::value& value,
          stringify_style    style = stringify_style::compact);

FORMATS_JSON_NAMESPACE_END