#pragma once

#include <string>

#include <formats/common/iterator/iterator_adapter.hpp>
#include <formats/common/exception.hpp>
#include <formats/common/cctypes.hpp>

FORMATS_NAMESPACE_BEGIN

template <typename T>
class iterator
{
public:
  using value_type = typename iterator_adapter<T>::value_type;
  using pointer    = typename iterator_adapter<T>::pointer;
  using reference  = typename iterator_adapter<T>::reference;
  using diff_t     = typename iterator_adapter<T>::diff_t;

  using iterator_category = std::input_iterator_tag;

public:
  iterator() { iterator_adapter_.reset(); }

  iterator(pointer v)
  {
    if (v->is_object())
      iterator_adapter_ = iterator_adapter<T>::create_object(v, v->as_object().begin());
    else if (v->is_array())
      iterator_adapter_ = iterator_adapter<T>::create_array(v, 0);
    else
      iterator_adapter_ = iterator_adapter<T>::create_scaler(v, 0);
  }

  iterator(pointer v, diff_t pos)
  {
    if (v->is_array())
      iterator_adapter_ = iterator_adapter<T>::create_array(v, pos);
    else
      iterator_adapter_ = iterator_adapter<T>::create_scaler(v, pos);
  }

  template <typename Iter, typename std::enable_if<!std::is_integral<Iter>::value, int>::type = 0>
  iterator(pointer v, Iter pos)
  {
    iterator_adapter_ = iterator_adapter<T>::create_object(v, pos);
  }

  iterator(const iterator<const T>& other)
  {
    iterator_adapter_ = iterator_adapter<T>::create(*other.iter_adapter().get());
  }

  iterator(const iterator<typename std::remove_const<T>::type>& other) noexcept(false)
  {
    iterator_adapter_ = iterator_adapter<T>::create(*other.iter_adapter().get());
  }

  iterator& operator=(const iterator<const T>& other)
  {
    iterator_adapter_ = iterator_adapter<T>::create(*other.iter_adapter().get());
    return *this;
  }

  iterator& operator=(const iterator<typename std::remove_const<T>::type>& other)
  {
    iterator_adapter_ = iterator_adapter<T>::create(*other.iter_adapter().get());
    return *this;
  }

public:
  pointer operator->() { return iterator_adapter_.get()->operator->(); }

  reference operator*() { return iterator_adapter_.get()->operator*(); }

public:
  iterator operator++(int) &
  {
    auto result = *this;
    iterator_adapter_->forward(1);
    return result;
  }

  iterator operator--(int) &
  {
    auto result = *this;
    iterator_adapter_->backward(1);
    return result;
  }

  iterator& operator++()
  {
    iterator_adapter_->forward(1);
    return *this;
  }

  iterator& operator--()
  {
    iterator_adapter_->backward(1);
    return *this;
  }

  iterator operator+=(int n)
  {
    iterator_adapter_->forward(n);
    return *this;
  }

  iterator operator-=(int n)
  {
    iterator_adapter_->backward(n);
    return *this;
  }

  iterator operator+(int n)
  {
    auto result = *this;
    result += n;
    return result;
  }

  iterator operator-(int n)
  {
    auto result = *this;
    result -= n;
    return result;
  }

  bool operator==(const iterator<const T>& other) const
  {
    return (iter_adapter() && other.iter_adapter())
               ? (iterator_adapter_->equal(*other.iter_adapter().get()))
               : (!iter_adapter() && !other.iter_adapter());
  }

  bool operator==(const iterator<typename std::remove_const<T>::type>& other) const
  {
    return (iter_adapter() && other.iter_adapter())
               ? (iterator_adapter_->equal(*other.iter_adapter().get()))
               : (!iter_adapter() && !other.iter_adapter());
  }

  bool operator!=(const iterator<T>& other) const { return !(*this == other); }

public:
  typename iterator_adapter<T>::ptr iter_adapter() noexcept { return iterator_adapter_; }
  typename iterator_adapter<T>::ptr iter_adapter() const noexcept { return iterator_adapter_; }

public:
  const std::string& key() const noexcept(false)
  {
    if (iterator_adapter_->is_object())
    {
      return (dynamic_cast<object_iterator<T>*>(iterator_adapter_.get()))->iter()->first;
    }

    FORMATS_THROW(type_except::create("can't use key() with iterator",
                                      typeid(*iterator_adapter_.get()).name()));
  }

public:
  diff_t distance_from_begin() const noexcept { return iterator_adapter_->distance_from_begin(); }

  typename array_iterator<T>::iterator array_iter() const noexcept(false)
  {
    if (iterator_adapter_->is_array())
    {
      return (dynamic_cast<array_iterator<T>*>(iterator_adapter_.get()))->iter();
    }

    FORMATS_THROW(type_except::create("iterator incompatible with iterator",
                                      typeid(*iterator_adapter_.get()).name()));
  }

  T* owner() const noexcept { return iterator_adapter_->value_ref(); }

private:
  typename iterator_adapter<T>::ptr iterator_adapter_;
};

FORMATS_NAMESPACE_END