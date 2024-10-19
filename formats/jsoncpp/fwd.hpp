#pragma once

#include <string>

#include <formats/common/marco.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

class error;
class value;

namespace detail
{
class parser;
}

template <typename T>
class stringifier;

// typedef bool           bool_t;
// typedef double         double_t;
// typedef std::nullptr_t null_t;
// typedef std::string    string_t;

FORMATS_JSON_NAMESPACE_END