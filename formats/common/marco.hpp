#pragma once

#define FORMATS_NAMESPACE_BEGIN \
  namespace formats             \
  {

#define FORMATS_JSON_NAMESPACE_BEGIN \
  namespace formats                  \
  {                                  \
  namespace json                     \
  {

#define FORMATS_JSON_DETAIL_NAMESPACE_BEGIN \
  namespace formats                         \
  {                                         \
  namespace json                            \
  {                                         \
  namespace detail                          \
  {

#define FORMATS_NAMESPACE_END }  // namespace formats

#define FORMATS_JSON_NAMESPACE_END \
  }  /* namespace formats */       \
  }  // namespace json

#define FORMATS_JSON_DETAIL_NAMESPACE_END \
  }  /* namespace formats */              \
  }  /* namespace json */                 \
  }  // namespace detail

namespace formats
{
#if (!defined(FORMATS_NO_EXCEPTION))
#define FORMATS_THROW(exception) throw exception
#else
#define FORMATS_THROW(exception) std::abort()

#endif  // FORMATS_NO_EXCEPTION

#define FORMATS_THROW_IF(condition, exception) \
  if (condition) FORMATS_THROW(exception)

#if defined(__GNUC__) || defined(__ICL) || defined(__clang__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#ifndef __cplusplus__
#ifdef _WIN32
#define __cplusplus__ _MSVC_LANG
#else
#define __cplusplus__ __cplusplus
#endif  // _WIN32
#endif  // !__cplusplus__

}  // namespace formats