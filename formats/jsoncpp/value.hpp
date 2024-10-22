#pragma once

#include <string>

#include <formats/jsoncpp/fwd.hpp>

#include <formats/common/value/array.hpp>
#include <formats/common/value/ordered_map.hpp>
#include <formats/common/type_traits.hpp>
#include <formats/common/exception.hpp>
#include <formats/common/marco.hpp>
#include <formats/common/reference.hpp>
#include <formats/common/iterator/iterator.hpp>
#include <formats/common/cctypes.hpp>

#include <formats/jsoncpp/detail/detector.hpp>
#include <formats/jsoncpp/error.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

/* json value type */
enum class kind : unsigned char
{
  error = 0,     // error. only has effect when parse failed.
  null,          // null value. corresponding nullptr
  boolean,       // true or false
  number_int,    // the signed integer, long long.
  number_uint,   // the signed integer, unsigned long long
  number_float,  // double
  string,        // string value
  array,         // array value
  object,        // ordered map
};

class value
{
public:
  using boolean_t      = bool;
  using number_float_t = double;
  using number_int_t   = signed long long;
  using number_uint_t  = unsigned long long;
  using string_t       = std::string;
  using array_t        = formats::array<value>;
  using object_t       = formats::ordered_map<string_t, value>;
  using null_t         = std::nullptr_t;

  using initializer_list_t = std::initializer_list<refex_wrapper<value>>;

  using size_type       = typename string_t::size_type;
  using reference       = value&;
  using const_reference = const value&;

  using pointer       = value*;
  using const_pointer = const value*;

  using iterator       = formats::iterator<value>;
  using const_iterator = formats::iterator<const value>;

public:
  value() noexcept;
  value(const value& value) noexcept;
  value(value&& value) noexcept;
  value(json::kind) noexcept;

  template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
  inline value(T val)
      : kind_(json::kind::boolean)
      , data_(val)
  {}

  value(std::nullptr_t) noexcept;
  value(float val) noexcept;
  value(double val) noexcept;
  value(signed char val) noexcept;
  value(signed short val) noexcept;
  value(signed int val) noexcept;
  value(signed long val) noexcept;
  value(signed long long val) noexcept;
  value(unsigned char val) noexcept;
  value(unsigned short val) noexcept;
  value(unsigned int val) noexcept;
  value(unsigned long val) noexcept;
  value(unsigned long long val) noexcept;

  value(const string_t& val) noexcept;
  value(string_t&& val) noexcept;
  // value(std::string_view sv) noexcept;
  value(const char* val) noexcept;

  value(const array_t& val) noexcept;
  value(array_t&& val) noexcept;
  value(const object_t& val) noexcept;
  value(object_t&& val) noexcept;
  value(initializer_list_t ilist) noexcept;

  /*
   * @brief: construct a json object from pair. first_type is string and json value could be
   * constructed from second_type.
   * @param: pair to construct this
   */
  template <
      typename Pair,
      typename std::enable_if<formats::detail::is_pair_v<Pair> &&
                                  std::is_same<string_t, typename Pair::first_type>::value &&
                                  std::is_constructible<value, typename Pair::second_type>::value,
                              int>::type = 0>
  value(const Pair& pair) noexcept
  {
    kind_ = json::kind::object;
    new (&data_.v_object_) object_t();

    data_.v_object_.emplace(pair.first, value(pair.second));
  }

  /*
   * @brief: construct a json array from unmapped container. such as std::array, vector, list,
   * deque, set .etc.
   * @param: unmapped container to construct this.
   */
  template <
      typename Container,
      typename std::enable_if<
          formats::detail::is_unmapped_traversable_container<Container>::value &&
          std::is_constructible<value, typename Container::value_type>::value>::type* = nullptr>
  value(const Container& container) noexcept
  {
    kind_ = json::kind::array;
    new (&data_.v_array_) array_t();

    for (const typename Container::value_type& element : container)
    {
      data_.v_array_.emplace_back(element);
    }
  }

  /*
   * @brief: construct a json object from mapped container. such as std::map, unordered_map .etc.
   * @param: unmapped container to construct this.
   */
  template <
      typename Container,
      typename std::enable_if<
          formats::detail::is_mapped_container<Container>::value &&
          std::is_constructible<string_t, typename Container::key_type>::value &&
          std::is_constructible<value, typename Container::mapped_type>::value>::type* = nullptr>
  value(const Container& container) noexcept
  {
    kind_ = json::kind::object;
    new (&data_.v_object_) object_t();

    for (const typename Container::value_type& entry : container)
      data_.v_object_.emplace(entry.first, entry.second);
  }

  /*
   * @brief: construct json object from custom type which has member function to_json.
   * @param: custom type to construct this.
   */
  template <typename T, typename std::enable_if<has_member_to_json_v<T>, int>::type = 0>
  value(const T& t) noexcept
  {
    *this = t.to_json();
  }

  /*
   * @brief: construct json object from custom type if specialization adl_serializer<T>.
   * @param: custom type to construct this.
   */
  template <typename T, typename std::enable_if<has_global_to_json_v<T>, int>::type = 0>
  value(const T& t) noexcept
  {
    *this = adl_serializer<T>::to_json(t);
  }

  ~value() noexcept;

  value& operator=(const value& val) noexcept;
  value& operator=(value&& val) noexcept;
  value& operator=(json::kind) noexcept;

  template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
  inline value& operator=(T val) noexcept
  {
    value(val).swap(*this);
    return *this;
  }

  value& operator=(std::nullptr_t) noexcept;
  value& operator=(float val) noexcept;
  value& operator=(double val) noexcept;
  value& operator=(signed char val) noexcept;
  value& operator=(signed short val) noexcept;
  value& operator=(signed int val) noexcept;
  value& operator=(signed long val) noexcept;
  value& operator=(signed long long val) noexcept;
  value& operator=(unsigned char val) noexcept;
  value& operator=(unsigned short val) noexcept;
  value& operator=(unsigned int val) noexcept;
  value& operator=(unsigned long val) noexcept;
  value& operator=(unsigned long long val) noexcept;
  value& operator=(const string_t&) noexcept;
  value& operator=(string_t&&) noexcept;
  value& operator=(const char*) noexcept;
  value& operator=(const array_t& val) noexcept;
  value& operator=(array_t&& val) noexcept;
  value& operator=(const object_t& val) noexcept;
  value& operator=(object_t&& val) noexcept;
  value& operator=(initializer_list_t ilist) noexcept;

public:
  /*
   * @brief: return a pointer if this is certain type, else return nullptr
   */
  boolean_t*      if_bool() noexcept;
  number_int_t*   if_int64() noexcept;
  number_uint_t*  if_uint64() noexcept;
  number_float_t* if_double() noexcept;
  string_t*       if_string() noexcept;
  array_t*        if_array() noexcept;
  object_t*       if_object() noexcept;

  const boolean_t*      if_bool() const noexcept;
  const number_int_t*   if_int64() const noexcept;
  const number_uint_t*  if_uint64() const noexcept;
  const number_float_t* if_double() const noexcept;
  const string_t*       if_string() const noexcept;
  const array_t*        if_array() const noexcept;
  const object_t*       if_object() const noexcept;

  /*
   * @brief: check this is certain type
   */
  bool is_bool() const noexcept;
  bool is_null() const noexcept;
  bool is_int64() const noexcept;
  bool is_uint64() const noexcept;
  bool is_double() const noexcept;
  bool is_string() const noexcept;
  bool is_array() const noexcept;
  bool is_object() const noexcept;
  bool is_number() const noexcept;
  bool is_error() const noexcept;

  /*
   * @brief: return a reference to the underlying value if this is certain type, else throw an
   * exception.
   */
  bool&           as_bool() noexcept(false);
  number_int_t&   as_int64() noexcept(false);
  number_uint_t&  as_uint64() noexcept(false);
  number_float_t& as_double() noexcept(false);
  string_t&       as_string() noexcept(false);
  array_t&        as_array() noexcept(false);
  object_t&       as_object() noexcept(false);

  bool            as_bool() const noexcept(false);
  number_int_t    as_int64() const noexcept(false);
  number_uint_t   as_uint64() const noexcept(false);
  number_float_t  as_double() const noexcept(false);
  const string_t& as_string() const noexcept(false);
  const array_t&  as_array() const noexcept(false);
  const object_t& as_object() const noexcept(false);

  /*
   * @brief: return value if convert this value to certain type success, else dflt
   */
  bool           to_bool(bool dflt = false) const noexcept;
  number_int_t   to_int64(number_int_t dflt = 0) const noexcept;
  number_uint_t  to_uint64(number_uint_t dflt = 0) const noexcept;
  number_float_t to_double(double dflt = 0.0) const noexcept;
  string_t       to_string(string_t&& dflt = {}) const noexcept;

public:
  json::kind kind() const noexcept;

public:
  template <
      typename Container,
      typename std::enable_if<formats::detail::is_unmapped_emplace_back_container_v<Container>,
                              int>::type = 0>
  Container key_set() const noexcept
  {
    Container res;

    if (is_object())
      for (const auto& entry : data_.v_object_)
        res.emplace_back(entry.first);

    return res;
  }

  template <typename Container,
            typename std::enable_if<formats::detail::is_unmapped_emplace_container_v<Container>,
                                    int>::type = 0>
  Container key_set() const noexcept
  {
    Container res;

    if (is_object())
      for (const auto& entry : data_.v_object_)
        res.emplace(entry.first);

    return res;
  }

  /*
   * @brief: return the element value with specified key.
   *
   * @param:
   *  key: Object key.
   *
   *  key_path: the path of Object key.
   *  separator: the separator of the key path.
   *  dflt: default value
   *
   * @return: return value if the element with specified key exists and could be convert to T.
   * otherwise dflt.
   */
  string_t get(const char* key, string_t dflt) const noexcept;
  string_t get(const char* key_path, char separator, string_t dflt) const noexcept;

  template <typename T, typename std::enable_if<std::is_same<T, bool>::value, int>::type = 0>
  T get(const char* key, T dflt) const noexcept
  {
    auto result = if_contains(key);
    return result ? result->to_bool(std::move(dflt)) : dflt;
  }

  template <typename T, typename std::enable_if<std::is_same<T, bool>::value, int>::type = 0>
  T get(const char* key_path, char separator, T dflt) const noexcept
  {
    auto result = if_contains(key_path, separator);
    return result ? result->to_bool(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_unsigned_integer<T>::value, int>::type = 0>
  T get(const K& key, T dflt) const noexcept
  {
    auto result = if_contains(key);
    return result ? (T)result->to_uint64(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_unsigned_integer<T>::value, int>::type = 0>
  T get(const K& key_path, char separator, T dflt) const noexcept
  {
    auto result = if_contains(key_path, separator);
    return result ? (T)result->to_uint64(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_signed_integer<T>::value, int>::type = 0>
  T get(const K& key, T dflt = 0) const noexcept
  {
    auto result = if_contains(key);
    return result ? (T)result->to_int64(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_signed_integer<T>::value, int>::type = 0>
  T get(const K& key_path, char separator, T dflt) const noexcept
  {
    auto result = if_contains(key_path, separator);
    return result ? (T)result->to_int64(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_double<T>::value, int>::type = 0>
  T get(const K& key, T dflt) const noexcept
  {
    auto result = if_contains(key);
    return result ? (T)result->to_double(std::move(dflt)) : dflt;
  }

  template <typename K,
            typename T,
            typename std::enable_if<formats::detail::is_double<T>::value, int>::type = 0>
  T get(const K& key_path, char separator, T dflt) const noexcept
  {
    auto result = if_contains(key_path, separator);
    return result ? (T)result->to_double(std::move(dflt)) : dflt;
  }

public:  // Capacity
  /*
   * @brief: Checks if the value has no elements.
   * @param: none.
   * @return: true if the value is scaler or object or array is empty, false otherwise.
   */
  bool empty() const noexcept;

  /*
   * @brief: Returns the number of elements in the value.
   * @param: none.
   * @return: The number of elements in the object or array value. 0 if scaler
   */
  size_type size() const noexcept;

  // Lookup
public:
  /*
   * @brief: Call when value is object. Finds an element with specified key.
   * If value type is not object, an exception of type std::exception is thrown.
   *
   * @param: Object key.
   *
   * @return: An iterator to the requested element. If no such element is found, past-the-end (see
   * end()) iterator is returned.
   */
  iterator       find(const object_t::key_type& key) noexcept(false);
  const_iterator find(const object_t::key_type& key) const noexcept(false);

  iterator       find(const char* path, char path_separator) noexcept(false);
  const_iterator find(const char* path, char path_separator) const noexcept(false);

  bool          contains(const object_t::key_type& key) const noexcept;
  pointer       if_contains(const object_t::key_type& key) noexcept;
  pointer       if_contains(const string_t& key_path, char separator) noexcept;
  const_pointer if_contains(const object_t::key_type& key) const noexcept;
  const_pointer if_contains(const string_t& key_path, char separator) const noexcept;

  /*
   * @brief: Call when value is object. Returns a reference to the element with specified key.
   * If value type is not object, an exception of type std::exception is thrown.
   * If no such element exists, an exception of type std::exception is thrown.
   *
   * @param: object key.
   *
   * @return: A reference to the object value of the requested element.
   */
  reference       at(const object_t::key_type& key) noexcept(false);
  const_reference at(const object_t::key_type& key) const noexcept(false);

  /*
   * @brief: Call when value is object. Returns a reference to the the element with specified key.
   * If such key does not already exist and this is non-const, performing an insertion to the this
   * If value type is not object, an exception of type std::exception is thrown.
   * If no such element exists and this is const, an exception of type std::exception is thrown.
   *
   * @param: object key.
   *
   * @return: A reference to the object value of the new element if  object value is non const no
   * element with key key existed. Otherwise, a reference to the object value of the existing
   * element whose key is equivalent to key.
   */
  reference       operator[](const object_t::key_type& key) noexcept(false);
  reference       operator[](object_t::key_type&& key) noexcept(false);
  const_reference operator[](const object_t::key_type& key) const noexcept(false);
  const_reference operator[](object_t::key_type&& key) const noexcept(false);

  template <
      typename T,
      typename std::enable_if<std::is_constructible<object_t::key_type, T>::value, int>::type = 0>
  reference operator[](const T& key) noexcept(false)
  {
    return this->operator[](object_t::key_type(key));
  }

  template <
      typename T,
      typename std::enable_if<std::is_constructible<object_t::key_type, T>::value, int>::type = 0>
  const_reference operator[](const T& key) const noexcept(false)
  {
    return this->at(object_t::key_type(key));
  }

  /*
   * @brief: Call when value is array. Returns a reference to the element at specified location pos,
   * with bounds checking.
   * If value type is not array, an exception of type std::exception is thrown.
   * If pos is not within the range of the container, an exception of type std::exception is thrown.
   *
   * @param: position of the element to return.
   *
   * @return: A reference to the requested element.
   */
  reference       at(size_type pos) noexcept(false);
  const_reference at(size_type pos) const noexcept(false);

  /*
   * @brief: Call when value is array. Returns a reference to the element at specified location pos.
   * If value is not array, an exception of type std::exception is thrown.
   * If pos is not within the range of the container, an exception of type std::exception is thrown.
   *
   * @param: position of the element to return.
   *
   * @return: A reference to the requested element.
   */
  template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
  reference operator[](T pos) noexcept(false)
  {
    return at((size_type)pos);
  }

  template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
  const_reference operator[](T pos) const noexcept(false)
  {
    return at((size_type)pos);
  }

  bool operator==(const value& other) const noexcept;
  bool operator!=(const value& other) const noexcept;

public:  // Modifier
  /*
   * @brief: repalce the value whit specified type initialized to default value, destroy the
   * previous contents.
   *
   * @param: none.
   *
   * @return: A reference to the value content.
   */
  void            emplace_null() noexcept;
  boolean_t&      emplace_bool() noexcept;
  number_int_t&   emplace_int64() noexcept;
  number_uint_t&  emplace_uint64() noexcept;
  number_float_t& emplace_double() noexcept;
  string_t&       emplace_string() noexcept;
  array_t&        emplace_array() noexcept;
  object_t&       emplace_object() noexcept;

  /*
   * @brief: reset to default value.
   * If value is array or object, erases all elements from the value.
   * if value is scaler, set to default. boolean->false, number->0, string->empty
   * After this call, size() returns zero.
   *
   * @param: none.
   *
   * @return: none
   */
  void clear() noexcept;

  /*
   * @brief: Exchanges the contents of this value with those of other.
   * @param: none.
   * @return: none.
   */
  void swap(value& other) noexcept;

  // json-object modifier
  /*
   * @brief: Inserts a new element into this constructed in-place with the given args, if there is
   * no element with the key in the container.
   * If value type is null type, after call this, value typee is object.
   * If value type is neither null nor object, an exception of type std::exception is thrown.
   *
   * @param: arguments to forward to the constructor of the element.
   *
   * @return: A pair consisting of an iterator to the inserted element (or to the element that
   * prevented the insertion) and a bool value set to true if and only if the insertion took place.
   */
  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) noexcept(false)
  {
    set_type_if_none(json::kind::object);
    if (is_object())
    {
      auto ret = data_.v_object_.emplace(std::forward<Args&&>(args)...);
      return std::pair<iterator, bool>{iterator(this, ret.first), ret.second};
    }

    FORMATS_THROW(type_except::create("emplace with args... can only be used in object json"));
  }

  /*
   * @brief: Call when value is object. Removes specified elements from the object value. The order
   * of the remaining elements is preserved.
   *
   * @param: key of the elements to remove.
   *
   * @return: none
   */
  void erase(const object_t::key_type& key) noexcept;
  void erase(object_t::key_type&& key) noexcept;

  /*
   * @brief: Attempts to extract ("splice") each element in other and insert it into this.
   * If this has no key of an element in other, move it from other to end of this.
   * If value type is null type, after call this, value type is object.
   * If value type is neither null type nor object, an exception of type std::exception is thrown.
   * If other type is not object, an exception of type std::exception is thrown.
   *
   * @param: value to transfer the nodes from
   *
   * @return: none
   */
  void merge(value& other) noexcept(false);
  void merge(value&& other) noexcept(false);

  // json-array modifier
  /*
   * @brief: Call when value is array. Erases the specified position from the this.
   * If pos is not within the range of this, an exception of type std::exception is thrown.
   * If value is not array, an exception of type std::exception is thrown.
   *
   * @param: position of the element to remove
   *
   * @return: none
   */
  void erase(size_type pos) noexcept(false);

  /*
   * @brief: Call when value is array. Erases the specified element from the this.
   * If pos is not into this, an exception of type std::exception is thrown.
   * If pos is not within the range of this, an exception of type std::exception is thrown.
   * If value is not array, an exception of type std::exception is thrown.
   *
   * @param: iterator to the element to remove
   *
   * @return:
   * 1) Iterator following the last removed element.
   * 2) If last == end() prior to removal, then the updated end() iterator is returned.
   *    If [first, last) is an empty range, then last is returned.
   */
  iterator erase(iterator pos) noexcept(false);
  iterator erase(const_iterator pos) noexcept(false);
  iterator erase(iterator first, iterator last) noexcept(false);
  iterator erase(const_iterator first, const_iterator last) noexcept(false);

  /*
   * @brief: Appends the given element value to the end of the this
   * If value type is null type, after call this, type is array type.
   * If value type is neither null type nor array, an exception of type std::exception is thrown.
   *
   * @param: the value of the element to append.
   *
   * @return: none.
   */
  void push_back(const value& other) noexcept(false);
  void push_back(value&& other) noexcept(false);

  /*
   * @brief:Inserts elements at the specified location in this.
   * If value type is not array, an exception of type std::exception is thrown.
   * If pos is not iterator to this, an exception of type std::exception is thrown.
   * If first or last are iterators into this, an exception of type std::exception is thrown.
   *
   * @param:
   *  pos: iterator before which the content will be inserted (pos may be the end() iterator)
   *  value: element value to insert
   *  count: number of elements to insert
   *  first, last: the range of elements to insert, cannot be iterators into container for which
   *  ilist: std::initializer_list to insert the values from
   *
   * @return:
   * 1,2) Iterator pointing to the inserted value.
   *   3) Iterator pointing to the first element inserted, or pos if count == 0.
   *   4) Iterator pointing to the first element inserted, or pos if first == last.
   *   5) Iterator pointing to the first element inserted, or pos if ilist is empty.
   */
  iterator insert(const_iterator pos, const value& val) noexcept(false);
  iterator insert(const_iterator pos, value&& val) noexcept(false);
  iterator insert(const_iterator pos, size_type cnt, const value& val) noexcept(false);
  iterator insert(const_iterator pos, const_iterator first, const_iterator last) noexcept(false);
  iterator insert(const_iterator pos, initializer_list_t ilist) noexcept(false);

  /*
   * @brief: Appends a new element to the end of this. The element is constructed through args.
   * If value type is null type, after call this, type is array type.
   * If value type is neither null type nor array, an exception of type std::exception is thrown.
   *
   * @param: arguments to forward to the constructor of the element.
   *
   * @return: A reference to the inserted element.
   */
  template <typename... Args>
  reference emplace_back(Args&&... args) noexcept(false)
  {
    set_type_if_none(json::kind::array);

    if (is_array()) { return data_.v_array_.emplace_back(std::forward<Args&&>(args)...); }

    FORMATS_THROW(type_except::create("can't use emplace_back with type", type_name()));
  }

public:
  /*
   * @brief: stringify this into std::string with pretty style.
   * @param: none.
   * @return: stringify result.
   */
  string_t dump() const noexcept;

public:  // Bucket interface
  iterator begin() noexcept;
  iterator end() noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  const_iterator cbegin() noexcept;
  const_iterator cend() noexcept;

public:
  /*
   * @brief: stringify a json value to ostream. the style is standard and stringify_flag is strict.
   *
   * @param:
   *  os: stream the json will be stringify to.
   *  value: the reference of the value which to stringify.
   *
   * @return: stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const value&) noexcept;

private:
  void destory() noexcept;
  void set_type_if_none(json::kind) noexcept;

  template <typename... Args>
  iterator insert_iterator_to_array(const_iterator pos, Args&&... args)
  {
    if (pos.owner() != this)
      FORMATS_THROW(type_except::create("can't insert value by iterators not into *this"));

    if (is_array())
    {
      auto insert_pos = pos.array_iter();
      auto insert_ret = data_.v_array_.insert(insert_pos, std::forward<Args&&>(args)...);

      return insert_ret != data_.v_array_.end()
                 ? iterator(this, std::distance(data_.v_array_.begin(), insert_ret))
                 : end();
    }

    FORMATS_THROW(type_except::create("can not use insert with type", type_name()));
  }

private:
  string_t type_name() const noexcept;

private:
  json::kind kind_ = json::kind::null;

protected:
  union json_data {
    json_data(){};

    json_data(boolean_t val)
        : v_bool_(val)
    {}
    json_data(number_int_t val)
        : v_int_(val)
    {}
    json_data(number_uint_t val)
        : v_uint_(val)
    {}
    json_data(number_float_t val)
        : v_double_(val)
    {}
    json_data(string_t val)
        : v_string_(val)
    {}

    ~json_data(){};

    boolean_t      v_bool_;
    number_int_t   v_int_;
    number_uint_t  v_uint_;
    number_float_t v_double_;
    string_t       v_string_;
    array_t        v_array_;
    object_t       v_object_;
    error          v_error_;
  } data_;

private:
  friend class detail::parser;
};

using array_t = value::array_t;

FORMATS_JSON_NAMESPACE_END
