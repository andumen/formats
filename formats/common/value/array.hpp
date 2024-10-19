#pragma once

#include <vector>

#include <formats/common/marco.hpp>
#include <formats/jsoncpp/fwd.hpp>

FORMATS_NAMESPACE_BEGIN

template <typename T>
class array
{
public:
  using array_type = std::vector<T>;

  using value_type      = typename array_type::value_type;
  using size_type       = typename array_type::size_type;
  using reference       = typename array_type::reference;
  using const_reference = typename array_type::const_reference;

  using iterator               = typename array_type::iterator;
  using const_iterator         = typename array_type::const_iterator;
  using reverse_iterator       = typename array_type::reverse_iterator;
  using const_reverse_iterator = typename array_type::const_reverse_iterator;

public:
  inline iterator       begin() noexcept { return values_.begin(); }
  inline const_iterator begin() const noexcept { return values_.begin(); }
  inline const_iterator cbegin() const noexcept { return values_.cbegin(); }

  inline reverse_iterator       rbegin() noexcept { return values_.rbegin(); }
  inline const_reverse_iterator rbegin() const noexcept { return values_.rbegin(); }
  inline const_reverse_iterator crbegin() const noexcept { return values_.crbegin(); }

  inline iterator       end() noexcept { return values_.end(); }
  inline const_iterator end() const noexcept { return values_.end(); }
  inline const_iterator cend() const noexcept { return values_.cend(); }

  inline reverse_iterator       rend() noexcept { return values_.rend(); }
  inline const_reverse_iterator rend() const noexcept { return values_.rend(); }
  inline const_reverse_iterator crend() const noexcept { return values_.crend(); }

public:
  array() {}

  array(const array& other)
      : values_(other.values_)
  {}

  array(array&& other)
      : values_(std::move(other.values_))
  {}

  ~array() { values_.clear(); }

public:
  void assign(size_type size, const T& value) { values_.assign(size, value); }

public:
  inline bool      empty() const noexcept { return values_.empty(); }
  inline size_type size() const noexcept { return values_.size(); }

public:  // Element access
  inline reference       operator[](size_type pos) { return values_[pos]; }
  inline const_reference operator[](size_type pos) const { return values_[pos]; }

public:
  inline reference       front() { return values_.front(); }
  inline const_reference front() const { return values_.front(); }

  inline reference       back() { return values_.back(); }
  inline const_reference back() const { return values_.back(); }

public:
  inline void clear() noexcept { values_.clear(); }

  inline void swap(array& other) noexcept { values_.swap(other.values_); }

  inline iterator erase(iterator pos) noexcept { return values_.erase(pos); }
  inline iterator erase(const_iterator pos) noexcept { return values_.erase(pos); }
  inline iterator erase(iterator first, iterator last) noexcept
  {
    return values_.erase(first, last);
  }
  inline iterator erase(const_iterator first, const_iterator last) noexcept
  {
    return values_.erase(first, last);
  }

  inline iterator insert(const_iterator pos, const value_type& val) noexcept
  {
    return values_.insert(pos, val);
  }

  inline iterator insert(const_iterator pos, value_type&& val) noexcept
  {
    return values_.insert(pos, std::move(val));
  }

  iterator insert(const_iterator pos, size_type cnt, const value_type& val) noexcept
  {
    return values_.insert(pos, cnt, val);
  }

  iterator insert(const_iterator pos, const_iterator first, const_iterator last) noexcept
  {
    return values_.insert(pos, first, last);
  }

  inline void push_back(const value_type& v) noexcept { values_.push_back(v); }
  inline void push_back(value_type&& v) noexcept { values_.push_back(std::move(v)); }

  template <typename... Args>
  inline reference emplace_back(Args&&... args) noexcept
  {
#if __cplusplus__ >= 201703L
    return values_.emplace_back(std::forward<Args&&>(args)...);
#else
    values_.emplace_back(std::forward<Args&&>(args)...);
    return values_.back();
#endif
  }

private:
  array_type values_;

public:
  friend bool operator==(const array& left, const array& right)
  {
    if (left.size() != right.size()) return false;

    for (size_t i = 0; i < right.size(); ++i)
    {
      if (left[i] != right[i]) return false;
    }

    return true;
  }

  friend bool operator!=(const array& left, const array& right) { return !(left == right); }
};

FORMATS_NAMESPACE_END