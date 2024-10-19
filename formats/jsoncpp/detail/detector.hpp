#pragma once

#include <formats/common/type_traits.hpp>
#include <formats/common/adl_serialize_def.hpp>
#include <formats/jsoncpp/fwd.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

// clang-format off

template <typename T, typename... Args>
using member_to_json_t = decltype(std::declval<T>().to_json(std::declval<Args>()...));

template <typename T, typename... Args>
using member_from_json_t = decltype(std::declval<T>().from_json(std::declval<Args>()...));

template <typename T, typename... Args>
using global_to_json_t = decltype(formats::adl_serializer<T>::to_json(std::declval<Args>()...));

template <typename T, typename... Args>
using global_from_json_t = decltype(formats::adl_serializer<T>::from_json(std::declval<Args>()...));

template <typename T>
using has_member_func_to_json = formats::detail::is_detected<member_to_json_t, T>;

template <typename T>
using has_member_func_from_json = formats::detail::is_detected<member_from_json_t, T, const json::value&>;

template <typename T>
using has_global_func_to_json = formats::detail::is_detected<global_to_json_t, T, const T&>;

template <typename T>
using has_global_func_from_json = formats::detail::is_detected<global_from_json_t, T, const json::value&, T&>;

template <typename T>
constexpr bool has_member_to_json_v = has_member_func_to_json<T>::value;
template <typename T>
constexpr bool has_member_from_json_v = has_member_func_from_json<T>::value;

template <typename T>
constexpr bool has_global_to_json_v = has_global_func_to_json<T>::value;
template <typename T>
constexpr bool has_global_from_json_v = has_global_func_from_json<T>::value;

// clang-format on

FORMATS_JSON_NAMESPACE_END