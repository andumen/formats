#pragma once

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <inttypes.h>
#include <string>

#include <formats/formats.h>
#include "custom_type.hpp"

#ifndef TEST
#define TEST(Name)                                                           \
  static void Test##Name();                                                  \
  FormatsTest register_test_##Name(Test##Name, __FILE__, #Name, NULL, true); \
  static void Test##Name()
#endif

#ifndef DEPENDENT_TEST
#define DEPENDENT_TEST(Name, Dep)                                            \
  static void Test##Name();                                                  \
  FormatsTest register_test_##Name(Test##Name, __FILE__, #Name, #Dep, true); \
  static void Test##Name()
#endif

#ifndef DISABLED_TEST
#define DISABLED_TEST(Name)                                                   \
  static void Test##Name();                                                   \
  FormatsTest register_test_##Name(Test##Name, __FILE__, #Name, NULL, false); \
  static void Test##Name()
#endif

#define CHECK(condition) CheckHelper(__FILE__, __LINE__, #condition, condition)
#define CHECK_GE(a, b) CHECK((a) >= (b))

static inline void CheckHelper(const char* file, int line, const char* source, bool condition)
{
  if (!condition)
  {
    printf("%s:%d:\n CHECK(%s) failed\n", file, line, source);
    std::abort();
  }
}

#define CHECK_EQ(a, b) CheckEqualsHelper(__FILE__, __LINE__, #a, a, #b, b)

template <typename T>
inline void PrintfValue(T x);
template <>
inline void PrintfValue(std::nullptr_t)
{
  printf("%s", "null");
}
template <>
inline void PrintfValue(int x)
{
  printf("%d", x);
}
template <>
inline void PrintfValue(unsigned int x)
{
  printf("%u", x);
}
template <>
inline void PrintfValue(short x)
{
  printf("%hd", x);
}
template <>
inline void PrintfValue(unsigned short x)
{
  printf("%hu", x);
}
template <>
inline void PrintfValue(long x)
{
    printf("%" PRId64, (long long)x);
}
template <>
inline void PrintfValue(unsigned long x)
{
    printf("%" PRId64, (unsigned long long)x);
}
template <>
inline void PrintfValue(long long x)
{
  printf("%" PRId64, x);
}
template <>
inline void PrintfValue(unsigned long long x)
{
  printf("%" PRIu64, x);
}
template <>
inline void PrintfValue(float x)
{
  printf("%.30e", static_cast<double>(x));
}
template <>
inline void PrintfValue(double x)
{
  printf("%.30e", x);
}
template <>
inline void PrintfValue(bool x)
{
  printf("%s", x ? "true" : "false");
}

template <>
inline void PrintfValue(const std::string& s)
{
  printf("%s", s.c_str());
}

template <>
inline void PrintfValue(formats::json::value_type x)
{
  printf("%d", (int)x);
}

template <>
inline void PrintfValue(formats::json::value x)
{
  printf("%s", x.dump().c_str());
}

template <>
inline void PrintfValue(Region x)
{
  printf("%d", (int)x);
}

template <>
inline void PrintfValue(Position x)
{
  printf("%d", (int)x);
}

template <>
inline void PrintfValue(ClassType x)
{
  printf("%d", (int)x);
}

template <typename T1, typename T2>
inline void CheckEqualsHelper(const char* file,
                              int         line,
                              const char* expected_source,
                              T1          expected,
                              const char* value_source,
                              T2          value)
{
  // If expected and value are NaNs then expected != value.
  if (expected != value && (expected == expected || value == value))
  {
    printf("%s:%d:\n CHECK_EQ(%s, %s) failed\n", file, line, expected_source, value_source);
    printf("#  Expected: ");
    PrintfValue(expected);
    printf("\n");
    printf("#  Found:    ");
    PrintfValue(value);
    printf("\n");
    std::abort();
  }
}

template <>
inline void CheckEqualsHelper(const char* file,
                              int         line,
                              const char* expected_source,
                              const char* expected,
                              const char* value_source,
                              const char* value)
{
  if ((expected == NULL && value != NULL) || (expected != NULL && value == NULL)) { std::abort(); }

  if ((expected != NULL && value != NULL && strcmp(expected, value) != 0))
  {
    printf("%s:%d:\n CHECK_EQ(%s, %s) failed\n"
           "#  Expected: %s\n"
           "#  Found:    %s\n",
           file, line, expected_source, value_source, expected, value);
    std::abort();
  }
}

template <>
inline void CheckEqualsHelper(const char* file,
                              int         line,
                              const char* expected_source,
                              const char* expected,
                              const char* value_source,
                              char*       value)
{
  CheckEqualsHelper(file, line, expected_source, expected, value_source,
                    static_cast<const char*>(value));
}

class FormatsTest
{
public:
  typedef void(TestFunction)();
  FormatsTest(TestFunction* callback,
              const char*   file,
              const char*   name,
              const char*   dependency,
              bool          enabled);
  void                Run() { callback_(); }
  static int          test_count();
  static FormatsTest* last() { return last_; }
  FormatsTest*        prev() { return prev_; }
  const char*         file() const { return file_; }
  const char*         name() const { return name_; }
  const char*         dependency() const { return dependency_; }
  bool                enabled() const { return enabled_; }

private:
  TestFunction*       callback_;
  const char*         file_;
  const char*         name_;
  const char*         dependency_;
  bool                enabled_;
  static FormatsTest* last_;
  FormatsTest*        prev_;
};