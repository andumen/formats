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
std::string stringify(const value&    json_value,
                      stringify_style style = stringify_style::pretty,
                      stringify_flag  flag  = stringify_flag::strict) noexcept;

FORMATS_JSON_NAMESPACE_END