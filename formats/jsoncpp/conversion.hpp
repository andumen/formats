#pragma once

#include <formats/common/type_traits.hpp>
#include <formats/common/reflection/reflection.hpp>
#include <formats/jsoncpp/detail/detector.hpp>
#include <formats/jsoncpp/value.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

template <typename T>
void unserialize(const value& js, T& t);

template <typename T>
value serialize(const T& t);

namespace impl
{
using namespace formats;

template <typename T, typename Enable = void>
struct unserializer {};

template <typename T>
struct unserializer<T, typename std::enable_if<std::is_same<T, bool>::value>::type> {
  static void from_json(const value& js, T& t) { t = js.to_bool(); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<std::is_same<T, std::string>::value>::type> {
  static void from_json(const value& js, T& t) { t = js.to_string(); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::detail::is_signed_integer_v<T>>::type> {
  static void from_json(const value& js, T& t) { t = static_cast<T>(js.to_int64()); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::detail::is_unsigned_integer_v<T>>::type> {
  static void from_json(const value& js, T& t) { t = static_cast<T>(js.to_uint64()); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::detail::is_float_v<T>>::type> {
  static void from_json(const value& js, T& t) { t = static_cast<T>(js.to_double()); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::json::has_member_from_json_v<T>>::type> {
  static void from_json(const value& js, T& t) { t.from_json(js); }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::json::has_global_from_json_v<T>>::type> {
  static void from_json(const value& js, T& t) { adl_serializer<T>::from_json(js, t); }
};

template <typename T>
struct unserializer<
    T,
    typename std::enable_if<formats::detail::is_unmapped_emplace_container_v<T>>::type> {
  static void from_json(const value& jv, T& t)
  {
    using value_type = typename T::value_type;

    if (!jv.is_array()) return void();

    for (const auto& element : jv)
    {
      value_type v;
      unserializer<value_type>::from_json(element, v);

      t.emplace(std::move(v));
    }
  }
};

template <typename T>
struct unserializer<
    T,
    typename std::enable_if<formats::detail::is_unmapped_emplace_back_container_v<T>>::type> {
  static void from_json(const value& jv, T& t)
  {
    using value_type = typename T::value_type;

    if (!jv.is_array()) return void();

    for (const auto& element : jv)
    {
      value_type v;
      unserializer<value_type>::from_json(element, v);

      t.emplace_back(std::move(v));
    }
  }
};

template <typename T>
struct unserializer<T, typename std::enable_if<formats::detail::is_mapped_container_v<T>>::type> {
  static void from_json(const value& jv, T& t)
  {
    using value_type = typename T::mapped_type;

    if (!jv.is_object()) return void();

    for (const auto& entry : jv.as_object())
    {
      value_type v;
      unserializer<value_type>::from_json(entry.second, v);

      t.emplace(entry.first, v);
    }
  };
};

template <typename T, typename Enable = void>
struct serializer {};

template <typename T>
struct serializer<T, typename std::enable_if<formats::detail::is_scaler<T>::value>::type> {
  static value to_json(const T& t) { return value(t); }
};

template <typename T>
struct serializer<T, typename std::enable_if<has_member_to_json_v<T>>::type> {
  static value to_json(const T& t) { return t.to_json(); }
};

template <typename T>
struct serializer<T, typename std::enable_if<has_global_to_json_v<T>>::type> {
  static value to_json(const T& t) { return adl_serializer<T>::to_json(t); }
};

template <typename T>
struct serializer<
    T,
    typename std::enable_if<formats::detail::is_unmapped_traversable_container_v<T>>::type> {
  static value to_json(const T& t)
  {
    json::value result = json::kind::array;

    for (const auto& element : t)
    {
      result.emplace_back(serializer<typename T::value_type>::to_json(element));
    }

    return result;
  }
};

template <typename T>
struct serializer<T,
                  typename std::enable_if<
                      formats::detail::is_mapped_container_v<T> &&
                      std::is_constructible<std::string, typename T::key_type>::value>::type> {
  static value to_json(const T& t)
  {
    json::value result = json::kind::object;

    for (const auto& entry : t)
    {
      result.emplace(std::string(entry.first),
                     serializer<typename T::mapped_type>::to_json(entry.second));
    }

    return result;
  }
};

}  // namespace impl

template <typename T>
void unserialize(const value& js, T& t)
{
  impl::unserializer<T>::from_json(js, t);
}

template <typename T>
value serialize(const T& t)
{
  return impl::serializer<T>::to_json(t);
}

FORMATS_JSON_NAMESPACE_END

#define FORMATS_JSON_SERIALIZE_EX(Struct, ...)                                                \
  DEFINE_STRUCT_SCHEMA(Struct, PASS_ON(GENERATE_FIELDS_EX)(DEFINE_STRUCT_FIELD, __VA_ARGS__)) \
  template <>                                                                                 \
  struct formats::adl_serializer<Struct> {                                                    \
    inline static json::value to_json(const Struct& t)                                        \
    {                                                                                         \
      json::value v = json::kind::object;                                                     \
      formats::ForEachField(t, [&v](auto&& field, auto&& name) { v[name] = field; });         \
      return v;                                                                               \
    }                                                                                         \
    inline static void from_json(const json::value& v, Struct& t)                             \
    {                                                                                         \
      formats::ForEachField(t, [&v](auto&& field, auto&& name) {                              \
        if (v.contains(name)) formats::json::unserialize(v[name], field);                     \
      });                                                                                     \
    }                                                                                         \
  };

#define FORMATS_JSON_SERIALIZE(Struct, ...)                                           \
  DEFINE_STRUCT_SCHEMA(Struct, PASS_ON(GENERATE_FIELDS)(DEFINE_FIELD, __VA_ARGS__))   \
  template <>                                                                         \
  struct formats::adl_serializer<Struct> {                                            \
    inline static json::value to_json(const Struct& t)                                \
    {                                                                                 \
      json::value v = json::kind::object;                                             \
      formats::ForEachField(t, [&v](auto&& field, auto&& name) { v[name] = field; }); \
      return v;                                                                       \
    }                                                                                 \
    inline static void from_json(const json::value& v, Struct& t)                     \
    {                                                                                 \
      formats::ForEachField(t, [&v](auto&& field, auto&& name) {                      \
        if (v.contains(name)) formats::json::unserialize(v[name], field);             \
      });                                                                             \
    }                                                                                 \
  };
