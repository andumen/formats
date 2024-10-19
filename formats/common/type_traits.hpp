#pragma once

#include <type_traits>

#include <formats/jsoncpp/fwd.hpp>

FORMATS_NAMESPACE_BEGIN

namespace detail
{

template <typename... Ts>
struct make_void {
  typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

struct nonesuch {
  nonesuch()                      = delete;
  ~nonesuch()                     = delete;
  nonesuch(nonesuch const&)       = delete;
  void operator=(nonesuch const&) = delete;
};

template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type    = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type    = Op<Args...>;
};

template <template <class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template <typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// simulate std::conjunction, effectively performing a logical AND on the sequence of traits
template <class...>
struct conjunction : std::true_type {};

template <class B1>
struct conjunction<B1> : B1 {};

template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

template <class...>
struct disjunction : std::false_type {};

template <class B1>
struct disjunction<B1> : B1 {};

template <class B1, class... Bn>
struct disjunction<B1, Bn...> : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type {};

template <class... B>
constexpr bool conjunction_v = conjunction<B...>::value;

/*********************************scaler type*********************************/
// signed integer
template <typename T>
struct is_signed_integer {
  static constexpr bool value =
      conjunction<std::is_integral<T>, std::is_signed<T>>::value || std::is_enum<T>::value;
};

// unsigned integer
template <typename T>
struct is_unsigned_integer {
  static constexpr bool value =
      conjunction<std::is_integral<T>, std::is_unsigned<T>>::value && !std::is_same<T, bool>::value;
};

// float
template <typename T>
struct is_double
    : disjunction<typename std::is_same<T, float>::type, typename std::is_same<T, double>::type> {};

template <typename T>
struct is_number {
  static constexpr bool value =
      is_signed_integer<T>::value || is_unsigned_integer<T>::value || is_double<T>::value;
};

template <typename T>
constexpr bool is_signed_integer_v = is_signed_integer<T>::value;

template <typename T>
constexpr bool is_unsigned_integer_v = is_unsigned_integer<T>::value;

template <typename T>
constexpr bool is_float_v = is_double<T>::value;

template <typename T>
struct is_char_pointer : std::false_type {};

template <>
struct is_char_pointer<char*> : std::true_type {};

template <>
struct is_char_pointer<const char*> : std::true_type {};

template <typename T>
struct is_char_array : std::false_type {};

template <size_t N>
struct is_char_array<char[N]> : std::true_type {};

template <typename T>
struct is_char_array_or_pointer
    : std::integral_constant<bool, is_char_pointer<T>::value || is_char_array<T>::value> {};

template <typename T>
struct is_scaler {
  static constexpr bool value =
      std::is_same<std::nullptr_t, T>::value || is_char_array_or_pointer<T>::value ||
      std::is_same<std::string, T>::value || std::is_same<bool, T>::value || is_number<T>::value;
};

/*********************************struct type*********************************/

// traversable container
template <typename T, typename = void>
struct is_traversable_container : std::false_type {};

template <typename Container>
struct is_traversable_container<
    Container,
    void_t<typename Container::value_type,
           typename Container::const_iterator,
           decltype(std::declval<Container>().begin()),
           decltype(std::declval<Container>().end()),
           decltype(std::declval<typename Container::const_iterator&>()++),
           decltype(std::declval<typename Container::const_iterator&>() !=
                    (std::declval<typename Container::const_iterator&>()))>> : std::true_type {};

template <typename Container>
constexpr bool is_traversable_container_v = is_traversable_container<Container>::value;

// is_mapped
template <typename Container, typename = void>
struct is_mapped_container : std::false_type {};

template <typename Container>
struct is_mapped_container<Container,
                           void_t<typename Container::key_type, typename Container::mapped_type>>
    : is_traversable_container<Container> {};

template <typename Container>
struct is_unmapped_traversable_container {
  constexpr static bool value = is_traversable_container<Container>::value &&
                                !is_mapped_container<Container>::value &&
                                !std::is_same<Container, std::string>::value;
};

template <typename Container, typename = void>
struct is_unmapped_emplace_container : std::false_type {};

template <typename Container>
struct is_unmapped_emplace_container<Container, void_t<typename Container::value_type>> {
  template <typename T, typename ArgT>
  using emplace_t = decltype(std::declval<T>().emplace(std::declval<ArgT>()));

  using arg_type = typename std::add_rvalue_reference<typename Container::value_type>::type;

  static constexpr bool value =
      is_detected<emplace_t, Container, arg_type>::value && !is_mapped_container<Container>::value;
};

template <typename Container, typename = void>
struct is_unmapped_emplace_back_container : std::false_type {};

template <typename Container>
struct is_unmapped_emplace_back_container<Container, void_t<typename Container::value_type>> {
  template <typename T, typename ArgT>
  using emplace_back_t = decltype(std::declval<T>().emplace_back(std::declval<ArgT>()));

  using arg_type = typename std::add_rvalue_reference<typename Container::value_type>::type;

  static constexpr bool value = is_detected<emplace_back_t, Container, arg_type>::value &&
                                !is_mapped_container<Container>::value;
};

template <typename T>
constexpr bool is_mapped_container_v = is_mapped_container<T>::value;

template <typename T>
constexpr bool is_unmapped_traversable_container_v = is_unmapped_traversable_container<T>::value;

template <typename T>
constexpr bool is_unmapped_emplace_container_v = is_unmapped_emplace_container<T>::value;

template <typename T>
constexpr bool is_unmapped_emplace_back_container_v = is_unmapped_emplace_back_container<T>::value;

template <typename T, typename = void>
struct is_pair : std::false_type {};

template <typename T>
struct is_pair<T, void_t<typename T::first_type, typename T::second_type>> : std::true_type {};

template <typename T>
constexpr bool is_pair_v = is_pair<T>::value;

template <typename Container, typename = void>
struct is_value_type_pair_container : std::false_type {};

template <typename Container>
struct is_value_type_pair_container<Container, void_t<typename Container::value_type>>
    : is_pair<typename Container::value_type> {};

template <typename Container>
constexpr bool is_value_type_non_pair_container_v = !is_value_type_pair_container<Container>::value;
}  // namespace detail

FORMATS_NAMESPACE_END