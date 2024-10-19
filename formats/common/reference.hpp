#pragma once

#include <initializer_list>
#include <utility>

FORMATS_NAMESPACE_BEGIN

template <typename T>
class refex_wrapper
{
public:
  using value_type = T;

public:
  refex_wrapper(value_type&& value)
      : value_held(std::move(value))
  {}

  refex_wrapper(const value_type& value)
      : value_ref(&value)
  {}

  refex_wrapper(std::initializer_list<refex_wrapper> init_list)
      : value_held(init_list)
  {}

  template <typename... Args,
            std::enable_if_t<std::is_constructible<value_type, Args...>::value, int> = 0>
  refex_wrapper(Args&&... args)
      : value_held(std::forward<Args>(args)...)
  {}

  refex_wrapper(refex_wrapper&&) noexcept = default;
  refex_wrapper(const refex_wrapper&)     = delete;

  refex_wrapper& operator=(const refex_wrapper&) = delete;
  refex_wrapper& operator=(refex_wrapper&&)      = delete;

  ~refex_wrapper() = default;

public:
  value_type get() const { return (value_ref == nullptr) ? std::move(value_held) : *value_ref; }

  value_type const& operator*() const { return value_ref ? *value_ref : value_held; }

  value_type const* operator->() const { return &**this; }

private:
  mutable value_type value_held = nullptr;
  value_type const*  value_ref  = nullptr;
};

FORMATS_NAMESPACE_END