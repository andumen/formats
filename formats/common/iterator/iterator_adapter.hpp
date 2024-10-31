#pragma once

#include <memory>
#include <algorithm>

#include <formats/common/value/array.hpp>
#include <formats/common/value/ordered_map.hpp>
#include <formats/common/cctypes.hpp>
#include <formats/common/marco.hpp>
// #include <formats/jsoncpp/value.hpp>

FORMATS_NAMESPACE_BEGIN

template <typename T>
class iterator_adapter
{
public:
  using ptr = std::shared_ptr<iterator_adapter<T>>;

  using value_type = T;
  using pointer    = value_type*;
  using reference  = value_type&;

  using diff_t = std::ptrdiff_t;

public:
  static ptr create(const iterator_adapter<const T>&);
  static ptr create(const iterator_adapter<typename std::remove_const<T>::type>&);

  static ptr create_array(pointer v, diff_t pos);
  static ptr create_scaler(pointer v, diff_t pos);

  template <typename Iter>
  static ptr create_object(pointer v, Iter pos);

public:
  virtual pointer   operator->() = 0;
  virtual reference operator*()  = 0;

public:
  virtual pointer value_ref() const = 0;

public:
  virtual bool at_begin() = 0;
  virtual bool at_end()   = 0;

  virtual void forward(int distance = -1)  = 0;
  virtual void backward(int distance = -1) = 0;

  virtual diff_t distance_from_begin() const = 0;

  virtual bool equal(const iterator_adapter<const T>&) const                             = 0;
  virtual bool equal(const iterator_adapter<typename std::remove_const<T>::type>&) const = 0;

public:
  virtual bool is_array() const { return false; }
  virtual bool is_object() const { return false; }
  virtual bool is_scaler() const { return false; }
};

template <typename T>
class scaler_iterator : public iterator_adapter<T>
{
public:
  using super_type = iterator_adapter<T>;
  using pointer    = typename super_type::pointer;
  using reference  = typename super_type::reference;
  using diff_t     = typename super_type::diff_t;

public:
  scaler_iterator(pointer v, diff_t pos)
      : value_(*v)
      , pos_(std::min<size_t>(std::max<size_t>(pos, 0), 1))
  {}

  virtual ~scaler_iterator() {}

public:
  pointer value_ref() const { return &value_; }

public:
  pointer operator->() { return &value_; }

  reference operator*() { return value_; }

  bool at_begin() { return pos_ == 0; }
  bool at_end() { return pos_ != 0; }

public:
  void forward(int n) { pos_ += n; }

  void backward(int n) { pos_ -= n; }

  diff_t distance_from_begin() const { return pos_; }

  bool equal(const iterator_adapter<const T>& other) const
  {
    using derived_type = const scaler_iterator<const T>;

    if (other.is_scaler())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() && valid() == derived.valid());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

  bool equal(const iterator_adapter<typename std::remove_const<T>::type>& other) const
  {
    using derived_type = const scaler_iterator<typename std::remove_const<T>::type>;

    if (other.is_scaler())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() && valid() == derived.valid());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

public:
  bool is_scaler() const { return true; }
  bool valid() const { return pos_ == 0; }

private:
  reference value_;
  diff_t    pos_ = 0;
};

template <typename T>
class array_iterator : public iterator_adapter<T>
{
public:
  using super_type = iterator_adapter<T>;
  using pointer    = typename super_type::pointer;
  using reference  = typename super_type::reference;
  using diff_t     = typename super_type::diff_t;

  using iterator = typename std::conditional<
      std::is_const<T>::value,
      typename array<typename std::remove_const<T>::type>::const_iterator,
      typename array<typename std::remove_const<T>::type>::iterator>::type;

public:
  array_iterator(pointer v, diff_t pos)
      : value_(*v)
      , pos_(std::min<size_t>(std::max<size_t>(pos, 0), value_.as_array().size()))
  {}

  virtual ~array_iterator() {}

public:
  pointer value_ref() const { return &value_; }

public:
  pointer operator->()
  {
    if (!at_end()) { return &value_.as_array()[pos_]; }
    FORMATS_THROW(iterator_except::create("can't dereference out of range array iterator"));
  }

  reference operator*()
  {
    if (!at_end()) return value_.as_array()[pos_];
    FORMATS_THROW(iterator_except::create("can't dereference out of range array iterator"));
  }

  bool at_begin() { return pos_ <= 0; }
  bool at_end() { return pos_ >= (diff_t)value_.as_array().size(); }

public:
  void forward(int distance)
  {
    if (pos_ < (diff_t)value_.as_array().size())
    {
      pos_ = std::min<diff_t>(pos_ + distance, (diff_t)value_.as_array().size());
      return void();
    }

    FORMATS_THROW(iterator_except::create("can't seek array iterator after end"));
  }

  void backward(int distance)
  {
    if (pos_ > 0)
    {
      pos_ = std::max<diff_t>(pos_ - distance, -1);
      return void();
    }

    FORMATS_THROW(iterator_except::create("can't seek array iterator before begin"));
  }

  diff_t distance_from_begin() const { return pos_; }

  bool equal(const iterator_adapter<const T>& other) const
  {
    using derived_type = const array_iterator<const T>;

    if (other.is_array())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() && pos_ == derived.distance_from_begin());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

  bool equal(const iterator_adapter<typename std::remove_const<T>::type>& other) const
  {
    using derived_type = const array_iterator<typename std::remove_const<T>::type>;

    if (other.is_array())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() && pos_ == derived.distance_from_begin());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

  bool is_array() const { return true; }

public:
  iterator iter() const
  {
    return pos_ < (diff_t)value_.as_array().size() ? value_.as_array().begin() + pos_
                                                   : value_.as_array().end();
  }

private:
  reference value_;
  diff_t    pos_;
};

template <typename T>
class object_iterator : public iterator_adapter<T>
{
public:
  using super_type = iterator_adapter<T>;
  using pointer    = typename super_type::pointer;
  using reference  = typename super_type::reference;
  using diff_t     = typename super_type::diff_t;

  using iterator = typename std::conditional<
      std::is_const<T>::value,
      typename ordered_map<std::string, typename std::remove_const<T>::type>::const_iterator,
      typename ordered_map<std::string, typename std::remove_const<T>::type>::iterator>::type;

  friend super_type;

public:
  object_iterator(pointer v, iterator iter)
      : value_(*v)
      , iter_(iter)
  {}

  virtual ~object_iterator() {}

public:
  pointer value_ref() const override { return &value_; }

public:
  pointer operator->() override
  {
    if (!at_end()) { return &iter_->second; }
    FORMATS_THROW(iterator_except::create("can't dereference out of range object iterator"));
  }

  reference operator*() override
  {
    if (!at_end()) { return iter_->second; }
    FORMATS_THROW(iterator_except::create("can't dereference out of range object iterator"));
  }

  bool at_begin() override { return iter_ == value_.as_object().begin(); }
  bool at_end() override { return iter_ == value_.as_object().end(); }

public:
  void forward(int distance) override
  {
    if (!at_end())
    {
      iter_ = (distance < std::distance(iter_, value_.as_object().end()))
                  ? std::next(iter_, distance)
                  : value_.as_object().end();

      return void();
    }

    FORMATS_THROW(iterator_except::create("can't seek object iterator after end"));
  }

  void backward(int distance) override
  {
    if (!at_begin())
    {
      iter_ = (distance <= distance_from_begin()) ? std::next(iter_, -distance)
                                                  : value_.as_object().end();

      return void();
    }

    FORMATS_THROW(iterator_except::create("can't seek object iterator before begin"));
  }

  diff_t distance_from_begin() const override
  {
    return std::distance(value_.as_object().begin(), iter_);
  }

  bool equal(const iterator_adapter<const T>& other) const override
  {
    using derived_type = const object_iterator<const T>;

    if (other.is_object())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() &&
              distance_from_begin() == derived.distance_from_begin());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

  bool equal(const iterator_adapter<typename std::remove_const<T>::type>& other) const override
  {
    using derived_type = const object_iterator<typename std::remove_const<T>::type>;

    if (other.is_object())
    {
      auto derived = *dynamic_cast<derived_type*>(&other);
      return (value_ref() == derived.value_ref() &&
              distance_from_begin() == derived.distance_from_begin());
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator", typeid(other).name()));
  }

  bool is_object() const override final { return true; }

public:
  iterator iter() const { return iter_; }

private:
  reference value_;
  iterator  iter_;
};

template <typename T>
inline typename iterator_adapter<T>::ptr iterator_adapter<T>::create(
    const iterator_adapter<const T>& other)
{
  if (other.is_array())
  {
    auto pos = other.distance_from_begin();
    return create_array(other.value_ref(), pos);
  }

  if (other.is_object())
  {
    auto iter = (dynamic_cast<const object_iterator<const T>*>(&other))->iter();
    return create_object(other.value_ref(), iter);
  }

  if (other.is_scaler())
  {
    auto pos = (dynamic_cast<const scaler_iterator<const T>*>(&other))->valid() ? 0 : 1;
    return create_scaler(other.value_ref(), pos);
  }

  return iterator_adapter<T>::ptr();
}

template <typename T>
inline typename iterator_adapter<T>::ptr iterator_adapter<T>::create(
    const iterator_adapter<typename std::remove_const<T>::type>& other)
{
  using non_const_type = typename std::remove_const<T>::type;

  if (other.is_array())
  {
    auto pos = (dynamic_cast<const array_iterator<non_const_type>*>(&other))->distance_from_begin();
    return create_array(other.value_ref(), pos);
  }

  if (other.is_object())
  {
    auto iter = (dynamic_cast<const object_iterator<non_const_type>*>(&other))->iter();
    return create_object(other.value_ref(), iter);
  }

  if (other.is_scaler())
  {
    auto pos = (dynamic_cast<const scaler_iterator<non_const_type>*>(&other))->valid() ? 0 : 1;
    return create_scaler(other.value_ref(), pos);
  }

  return iterator_adapter<T>::ptr();
}

template <typename T>
typename iterator_adapter<T>::ptr iterator_adapter<T>::create_array(pointer v, diff_t pos)
{
  return std::make_shared<array_iterator<T>>(v, pos);
}

template <typename T>
typename iterator_adapter<T>::ptr iterator_adapter<T>::create_scaler(pointer v, diff_t pos)
{
  return std::make_shared<scaler_iterator<T>>(v, pos);
}

template <typename T>
template <typename Iter>
typename iterator_adapter<T>::ptr iterator_adapter<T>::create_object(pointer v, Iter pos)
{
  return std::make_shared<object_iterator<T>>(v, pos);
}

FORMATS_NAMESPACE_END