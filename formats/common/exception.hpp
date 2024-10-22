#pragma once

#include <exception>
#include <stdexcept>
#include <cinttypes>

#ifdef  __GNUC__
#pragma GCC diagnostic ignored "-Wformat"
#endif //  __GNUC__


FORMATS_NAMESPACE_BEGIN

enum except_type
{
  parse_error,
  type_error,
  range_error,
  overflow_error,
  underflow_error,
};

class exception : public std::exception
{
public:
  const char* what() const noexcept { return error_.what(); }

protected:
  exception(const std::string& what_arg)
      : error_(what_arg)
  {}

private:
  std::runtime_error error_;
};

class parse_except : public exception
{
public:
  static parse_except create(const char* what_arg)
  {
    std::string error_msg(std::string("parse error: ").append(what_arg));
    return parse_except(error_msg);
  }

private:
  parse_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

class type_except : public exception
{
public:
  static type_except create(const std::string what_arg)
  {
    std::string error_msg(std::string("type error: ").append(what_arg));
    return type_except(error_msg);
  }

  static type_except create(const char* desc, const std::string& type_name)
  {
    std::string error_msg(std::string("type error: ").append(desc).append(":").append(type_name));
    return type_except(error_msg);
  }

private:
  type_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

class range_except : public exception
{
public:
  static range_except create(const char* what_arg, long long index)
  {
    char buffer[32] = {0};
    sprintf(buffer, " index: %" PRId64 "", index);
    std::string error_msg(std::string("range error: ").append(what_arg).append(buffer));
    return range_except(error_msg);
  }

  static range_except create(std::string&& what_arg)
  {
    std::string error_msg(std::string("range error: ").append(std::move(what_arg)));
    return range_except(error_msg);
  }

private:
  range_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

class iterator_except : public exception
{
public:
  static iterator_except create(const char* what_arg)
  {
    std::string error_msg(std::string("range error: ").append(what_arg));
    return iterator_except(error_msg);
  }

private:
  iterator_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

class overflow_except : public exception
{
public:
  static overflow_except create(const char* what_arg)
  {
    std::string error_msg(std::string("overflow: ").append(what_arg));
    return overflow_except(error_msg);
  }

private:
  overflow_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

class underflow_except : public exception
{
public:
  static underflow_except create(const char* what_arg)
  {
    std::string error_msg(std::string("underflow: ").append(what_arg));
    return underflow_except(error_msg);
  }

private:
  underflow_except(const std::string what_arg)
      : exception(what_arg)
  {}
};

FORMATS_NAMESPACE_END
