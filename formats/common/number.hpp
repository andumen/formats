#pragma once

#include <string>
#include <limits>
#include <formats/common/marco.hpp>

FORMATS_NAMESPACE_BEGIN

#define DEF(type, name)                                                                   \
  constexpr type name[] = {F(2),  F(3),  F(4),  F(5),  F(6),  F(7),  F(8),  F(9),  F(10), \
                           F(11), F(12), F(13), F(14), F(15), F(16), F(17), F(18), F(19), \
                           F(20), F(21), F(22), F(23), F(24), F(25), F(26), F(27), F(28), \
                           F(29), F(30), F(31), F(32), F(33), F(34), F(35), F(36)}

#define F(X) (std::numeric_limits<unsigned long>::max)() / X
DEF(unsigned long, __ul_max_tab);
#undef F

#define F(X) (std::numeric_limits<unsigned long>::max)() % X
DEF(unsigned char, __ul_rem_tab);
#undef F

#define F(X) (std::numeric_limits<unsigned long long>::max)() / X
DEF(unsigned long long, __ull_max_tab);
#undef F

#define F(X) (std::numeric_limits<unsigned long long>::max)() % X
DEF(unsigned char, __ull_rem_tab);
#undef F

#undef DEF

#define cutoff_tab_ul __ul_max_tab
#define cutlim_tab_ul __ul_rem_tab

#define cutoff_tab_ull __ull_max_tab
#define cutlim_tab_ull __ull_rem_tab

template <size_t>
struct __flowover {};

template <>
struct __flowover<4> {
  static constexpr auto& cutoff_tab = cutoff_tab_ul;
  static constexpr auto& cutlim_tab = cutlim_tab_ul;
};

template <>
struct __flowover<8> {
  static constexpr auto& cutoff_tab = cutoff_tab_ull;
  static constexpr auto& cutlim_tab = cutlim_tab_ull;
};

// class value;
class number
{
public:
  number()  = default;
  ~number() = default;

public:
  bool parse(const char* ptr, const char* end);

public:
  bool negative    = false;
  bool is_fraction = false;

  unsigned long long integer = 0;

  double fraction;

private:
  bool from_hex_string();
  bool from_dec_string();

private:
  const char* ptr_;
  const char* end_;
};

std::pair<long long, bool>          strtoll(const std::string& str);
std::pair<unsigned long long, bool> strtoull(const std::string& str);
std::pair<double, bool>             strtod(const std::string& str);
bool                                dtoa(double v, char* buffer, int& size);
std::string                         dtoa(double v);

FORMATS_NAMESPACE_END