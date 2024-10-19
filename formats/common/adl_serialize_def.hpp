#pragma once

#include <formats/common/marco.hpp>

FORMATS_NAMESPACE_BEGIN

template <typename T>
struct adl_serializer {
    //  inline static void from_json(const json::value& jv, T& val);
    //  inline static json::value to_json(const T& val);
};

FORMATS_NAMESPACE_END