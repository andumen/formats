#pragma once

#include <formats/common/type_traits.hpp>
#include <ostream>

FORMATS_JSON_NAMESPACE_BEGIN

namespace detail
{
class output_adapter
{
public:
  using ptr = std::shared_ptr<output_adapter>;

public:
  static ptr create_string(std::string& output);
  static ptr create_stream(std::ostream& output);

public:
  virtual output_adapter& write(char c)                          = 0;
  virtual output_adapter& write(int count, char c)               = 0;
  virtual output_adapter& write(const char* data, size_t length) = 0;
};

class string_output : public output_adapter
{
public:
  using super_type = output_adapter;

public:
  string_output(std::string& str)
      : string_(str)
  {}

  virtual ~string_output() {}

public:
  inline super_type& write(char c)
  {
    string_.append(1, c);
    return *this;
  }

  inline super_type& write(int count, char c)
  {
    string_.append(count, c);
    return *this;
  }

  inline super_type& write(const char* data, size_t length)
  {
    string_.append(data, length);
    return *this;
  }

public:
  std::string& string_;
};

class stream_output : public output_adapter
{
public:
  using super_type = output_adapter;

public:
  stream_output(std::ostream& ostream)
      : stream_(ostream)
  {}

  virtual ~stream_output() {}

public:
  inline super_type& write(char c)
  {
    stream_.put(c);
    return *this;
  }

  inline super_type& write(int count, char c)
  {
    while (count-- > 0)
      stream_.put(c);

    return *this;
  }

  inline super_type& write(const char* data, size_t length)
  {
    stream_.write(data, length);
    return *this;
  }

public:
  std::ostream& stream_;
};

inline output_adapter::ptr output_adapter::create_string(std::string& output)
{
  return std::make_shared<string_output>(output);
}

inline output_adapter::ptr output_adapter::create_stream(std::ostream& output)
{
  return std::make_shared<stream_output>(output);
}

}  // namespace detail

FORMATS_JSON_NAMESPACE_END