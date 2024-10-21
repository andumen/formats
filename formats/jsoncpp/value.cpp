#include <array>
#include <algorithm>

#include <formats/jsoncpp/value.hpp>
#include <formats/jsoncpp/detail/stringifier.hpp>
#include <formats/common/cctypes.hpp>
#include <formats/common/marco.hpp>
#include <formats/common/exception.hpp>

FORMATS_JSON_NAMESPACE_BEGIN

value::value() noexcept
    : kind_(kind::null)
{}

value::value(const value& other) noexcept
    : kind_(other.kind_)
{
  switch (kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(other.data_.v_bool_); break;
    case kind::string: new (&data_.v_string_) string_t(other.data_.v_string_); break;
    case kind::number_int: new (&data_.v_int_) number_uint_t(other.data_.v_int_); break;
    case kind::number_uint: new (&data_.v_uint_) number_uint_t(other.data_.v_uint_); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(other.data_.v_double_); break;
    case kind::array: new (&data_.v_array_) array_t(other.data_.v_array_); break;
    case kind::object: new (&data_.v_object_) object_t(other.data_.v_object_); break;
    case kind::error:
      // new (&data_.v_error_) error(other.data_.v_error_);
      break;

    default: break;
  }
}

value::value(value&& other) noexcept
    : kind_(other.kind_)
{
  switch (kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(other.data_.v_bool_); break;
    case kind::string: new (&data_.v_string_) string_t(std::move(other.data_.v_string_)); break;
    case kind::number_int: new (&data_.v_int_) number_int_t(other.data_.v_int_); break;
    case kind::number_uint: new (&data_.v_uint_) number_uint_t(other.data_.v_uint_); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(other.data_.v_double_); break;
    case kind::array: new (&data_.v_array_) array_t(std::move(other.data_.v_array_)); break;
    case kind::object: new (&data_.v_object_) object_t(std::move(other.data_.v_object_)); break;
    case kind::error:  // new (&data_.v_error_) error(std::move(other.data_.v_error_)); break;

    default: break;
  }

  other.destory();
}

value::value(json::kind kind) noexcept
    : kind_(kind)
{
  switch (kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(false); break;
    case kind::string: new (&data_.v_string_) string_t{}; break;
    case kind::number_int: new (&data_.v_int_) number_uint_t(0); break;
    case kind::number_uint: new (&data_.v_uint_) number_int_t(0); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(0.0); break;
    case kind::array: new (&data_.v_array_) array_t{}; break;
    case kind::object: new (&data_.v_object_) object_t{}; break;
    case kind::error:
      // new (&data_.v_error_) error(other.data_.v_error_);
      break;

    default: break;
  }
}

value::value(std::nullptr_t) noexcept
    : kind_(kind::null)
{}

value::value(float v) noexcept
    : kind_(kind::number_float)
{
  new (&data_.v_double_) number_float_t(v);
}

value::value(double v) noexcept
    : kind_(kind::number_float)
{
  new (&data_.v_double_) number_float_t(v);
}

value::value(signed char v) noexcept
    : kind_(kind::number_int)
{
  new (&data_.v_int_) int64_t(v);
}

value::value(signed short v) noexcept
    : kind_(kind::number_int)
{
  new (&data_.v_int_) int64_t(v);
}

value::value(signed int v) noexcept
    : kind_(kind::number_int)
{
  new (&data_.v_int_) int64_t(v);
}

value::value(signed long v) noexcept
    : kind_(kind::number_int)
{
  new (&data_.v_int_) int64_t(v);
}

value::value(signed long long v) noexcept
    : kind_(kind::number_int)
{
  new (&data_.v_int_) int64_t(v);
}

value::value(unsigned char v) noexcept
    : kind_(kind::number_uint)
{
  new (&data_.v_uint_) uint64_t(v);
}

value::value(unsigned short v) noexcept
    : kind_(kind::number_uint)
{
  new (&data_.v_uint_) uint64_t(v);
}

value::value(unsigned int v) noexcept
    : kind_(kind::number_uint)
{
  new (&data_.v_uint_) uint64_t(v);
}

value::value(unsigned long v) noexcept
    : kind_(kind::number_uint)
{
  new (&data_.v_uint_) uint64_t(v);
}

value::value(unsigned long long v) noexcept
    : kind_(kind::number_uint)
{
  new (&data_.v_uint_) uint64_t(v);
}

value::value(const string_t& v) noexcept
    : kind_(kind::string)
{
  new (&data_.v_string_) string_t(v);
}

value::value(string_t&& v) noexcept
    : kind_(kind::string)
{
  new (&data_.v_string_) string_t(std::move(v));
}

value::value(const char* v) noexcept
    : kind_(kind::string)
{
  new (&data_.v_string_) string_t(v);
}

value::value(const array_t& v) noexcept
    : kind_(kind::array)
{
  new (&data_.v_array_) array_t(v);
}

value::value(array_t&& v) noexcept
    : kind_(kind::array)
{
  new (&data_.v_array_) array_t(std::move(v));
}

value::value(const object_t& v) noexcept
    : kind_(kind::object)
{
  new (&data_.v_object_) object_t(v);
}

value::value(object_t&& v) noexcept
    : kind_(kind::object)
{
  new (&data_.v_object_) object_t(std::move(v));
}

value::value(initializer_list_t init_list) noexcept
{
  if (init_list.size() == 1)
  {
    auto& other = init_list.begin()->operator*();

    if (other.is_array() && other.data_.v_array_.size() == 2 && other[0].is_string())
    {
      value v = other[static_cast<size_type>(1)];

      kind_ = kind::object;
      new (&data_.v_object_) object_t();
      data_.v_object_.emplace(other[static_cast<size_type>(0)].as_string(), v);

      return;
    }

    *this = other;
    return;
  }

  bool is_object = std::all_of(init_list.begin(), init_list.end(), [](const auto& element_ref) {
    return element_ref->is_array() && element_ref->size() == 2 && (*element_ref)[0].is_string();
  });

  if (is_object)
  {
    kind_ = kind::object;
    new (&data_.v_object_) object_t();

    for (auto& value_ref : init_list)
    {
      value v = (*value_ref)[static_cast<size_type>(1)];

      data_.v_object_.emplace((*value_ref)[size_type(0)].as_string(), v);
    }
  }
  else
  {
    kind_ = kind::array;
    new (&data_.v_array_) array_t();

    for (auto& value_ref : init_list)
    {
      data_.v_array_.push_back(*value_ref);
    }
  }
}

value::~value() noexcept
{
  this->destory();
}

value& value::operator=(const value& other) noexcept
{
  this->destory();

  switch (other.kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(other.data_.v_bool_); break;
    case kind::string: new (&data_.v_string_) string_t(other.data_.v_string_); break;
    case kind::number_int: new (&data_.v_int_) int64_t(other.data_.v_int_); break;
    case kind::number_uint: new (&data_.v_uint_) uint64_t(other.data_.v_uint_); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(other.data_.v_double_); break;
    case kind::array: new (&data_.v_array_) array_t(other.data_.v_array_); break;
    case kind::object: new (&data_.v_object_) object_t(other.data_.v_object_); break;
    case kind::error:
      // new (&data_.v_error_) error(std::move(other.data_.v_error_));
      break;

    default: break;
  }

  this->kind_ = other.kind_;

  return *this;
}

value& value::operator=(value&& other) noexcept
{
  this->destory();

  switch (other.kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(other.data_.v_bool_); break;
    case kind::string: new (&data_.v_string_) string_t(std::move(other.data_.v_string_)); break;
    case kind::number_int: new (&data_.v_int_) int64_t(other.data_.v_int_); break;
    case kind::number_uint: new (&data_.v_uint_) uint64_t(other.data_.v_uint_); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(other.data_.v_double_); break;
    case kind::array: new (&data_.v_array_) array_t(std::move(other.data_.v_array_)); break;
    case kind::object: new (&data_.v_object_) object_t(std::move(other.data_.v_object_)); break;
    case kind::error:
      // new (&data_.v_error_) error(std::move(other.data_.v_error_));
      break;

    default: break;
  }

  this->kind_ = other.kind_;

  other.destory();
  return *this;
}

value& value::operator=(json::kind kind) noexcept
{
  value(kind).swap(*this);
  return *this;
}

value& value::operator=(std::nullptr_t) noexcept
{
  value(nullptr).swap(*this);
  return *this;
}

value& value::operator=(float v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(double v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(signed char v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(signed short v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(signed int v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(signed long v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(signed long long v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(unsigned char v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(unsigned short v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(unsigned int v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(unsigned long v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(unsigned long long v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(const string_t& v) noexcept
{
  value(v).swap(*this);
  return *this;
}

value& value::operator=(string_t&& v) noexcept
{
  value(std::move(v)).swap(*this);
  return *this;
}

value& value::operator=(const char* v) noexcept
{
  value(v).swap(*this);
  return *this;
}

value& value::operator=(const array_t& v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(array_t&& v) noexcept
{
  value(std::move(v)).swap(*this);
  return *this;
}
value& value::operator=(const object_t& v) noexcept
{
  value(v).swap(*this);
  return *this;
}
value& value::operator=(object_t&& v) noexcept
{
  value(std::move(v)).swap(*this);
  return *this;
}

value& value::operator=(initializer_list_t init_list) noexcept
{
  value v(std::move(init_list));
  swap(v);

  return *this;
}

value::boolean_t* value::if_bool() noexcept
{
  return (is_bool()) ? &data_.v_bool_ : nullptr;
}

value::number_int_t* value::if_int64() noexcept
{
  return (is_int64()) ? &data_.v_int_ : nullptr;
}

value::number_uint_t* value::if_uint64() noexcept
{
  return (is_uint64()) ? &data_.v_uint_ : nullptr;
}

value::number_float_t* value::if_double() noexcept
{
  return (is_double()) ? &data_.v_double_ : nullptr;
}

value::string_t* value::if_string() noexcept
{
  return (is_string()) ? &data_.v_string_ : nullptr;
}

value::array_t* value::if_array() noexcept
{
  return (is_array()) ? &data_.v_array_ : nullptr;
}

value::object_t* value::if_object() noexcept
{
  return (is_object()) ? &data_.v_object_ : nullptr;
}

const value::boolean_t* value::if_bool() const noexcept
{
  return (is_bool()) ? &data_.v_bool_ : nullptr;
}

const value::number_int_t* value::if_int64() const noexcept
{
  return (is_int64()) ? &data_.v_int_ : nullptr;
}

const value::number_uint_t* value::if_uint64() const noexcept
{
  return (is_uint64()) ? &data_.v_uint_ : nullptr;
}

const value::number_float_t* value::if_double() const noexcept
{
  return (is_double()) ? &data_.v_double_ : nullptr;
}

const value::string_t* value::if_string() const noexcept
{
  return (is_string()) ? &data_.v_string_ : nullptr;
}
const value::array_t* value::if_array() const noexcept
{
  return (is_array()) ? &data_.v_array_ : nullptr;
}
const value::object_t* value::if_object() const noexcept
{
  return (is_object()) ? &data_.v_object_ : nullptr;
}

bool value::is_bool() const noexcept
{
  return kind_ == kind::boolean;
}

bool value::is_null() const noexcept
{
  return kind_ == kind::null;
}

bool value::is_int64() const noexcept
{
  return kind_ == kind::number_int;
}

bool value::is_uint64() const noexcept
{
  return kind_ == kind::number_uint;
}

bool value::is_double() const noexcept
{
  return kind_ == kind::number_float;
}

bool value::is_string() const noexcept
{
  return kind_ == kind::string;
}

bool value::is_array() const noexcept
{
  return kind_ == kind::array;
}

bool value::is_object() const noexcept
{
  return kind_ == kind::object;
}

bool value::is_number() const noexcept
{
  return (kind_ == kind::number_int || kind_ == kind::number_uint || kind_ == kind::number_float);
}

bool value::is_error() const noexcept
{
  return kind_ == kind::error;
}

bool& value::as_bool() noexcept(false)
{
  FORMATS_THROW_IF(!is_bool(), type_except::create("can't use as_bool with type", type_name()));
  return data_.v_bool_;
}

value::number_int_t& value::as_int64() noexcept(false)
{
  FORMATS_THROW_IF(!is_int64(), type_except::create("can't use as_int with type", type_name()));
  return data_.v_int_;
}

value::number_uint_t& value::as_uint64() noexcept(false)
{
  FORMATS_THROW_IF(!is_uint64(), type_except::create("can't use as_uint with type", type_name()));
  return data_.v_uint_;
}

value::number_float_t& value::as_double() noexcept(false)
{
  FORMATS_THROW_IF(!is_double(), type_except::create("can't use as_float with type", type_name()));
  return data_.v_double_;
}

value::string_t& value::as_string() noexcept(false)
{
  FORMATS_THROW_IF(!is_string(), type_except::create("can't use as_string with type", type_name()));
  return data_.v_string_;
}
value::array_t& value::as_array() noexcept(false)
{
  FORMATS_THROW_IF(!is_array(), type_except::create("can't use as_array with type", type_name()));
  return data_.v_array_;
}
value::object_t& value::as_object() noexcept(false)
{
  FORMATS_THROW_IF(!is_object(), type_except::create("can't use as_object with type", type_name()));
  return data_.v_object_;
}

bool value::as_bool() const noexcept(false)
{
  FORMATS_THROW_IF(!is_bool(), type_except::create("can't use as_bool with type", type_name()));
  return data_.v_bool_;
}

value::number_int_t value::as_int64() const noexcept(false)
{
  FORMATS_THROW_IF(!is_int64(), type_except::create("can't use as_int with type", type_name()));
  return data_.v_int_;
}

value::number_uint_t value::as_uint64() const noexcept(false)
{
  FORMATS_THROW_IF(!is_uint64(), type_except::create("can't use as_uint with type", type_name()));
  return data_.v_uint_;
}

double value::as_double() const noexcept(false)
{
  FORMATS_THROW_IF(!is_double(), type_except::create("can't use as_float with type", type_name()));
  return data_.v_double_;
}

const value::string_t& value::as_string() const noexcept(false)
{
  FORMATS_THROW_IF(!is_string(), type_except::create("can't use as_string with type", type_name()));
  return data_.v_string_;
}

const value::array_t& value::as_array() const noexcept(false)
{
  FORMATS_THROW_IF(!is_array(), type_except::create("can't use as_array with type", type_name()));
  return data_.v_array_;
}

const value::object_t& value::as_object() const noexcept(false)
{
  FORMATS_THROW_IF(!is_object(), type_except::create("can't use as_object with type", type_name()));
  return data_.v_object_;
}

bool value::to_bool(bool dflt) const noexcept
{
  switch (kind_)
  {
    case kind::boolean: return data_.v_bool_;
    case kind::number_int: return data_.v_int_ != 0;
    case kind::number_uint: return data_.v_uint_ != 0;
    case kind::number_float: return data_.v_double_ != 0.0;
    case kind::string:
      return !data_.v_string_.empty() && !equal(toupper(data_.v_string_), "FALSE", 5);

    default: break;
  }

  return dflt;
}

value::number_int_t value::to_int64(value::number_int_t dflt) const noexcept
{
  switch (kind_)
  {
    case kind::number_int: return data_.v_int_;

    case kind::number_uint:
      return (data_.v_uint_ <= (std::numeric_limits<number_int_t>::max)())
                 ? (number_int_t)data_.v_uint_
                 : dflt;

    case kind::number_float:
      return (data_.v_double_ >= (std::numeric_limits<number_int_t>::min)() &&
              data_.v_double_ <= (std::numeric_limits<number_int_t>::max)())
                 ? (number_int_t)data_.v_double_
                 : dflt;

    case kind::boolean: return data_.v_bool_ ? 1 : 0;
    case kind::string: {
      auto result = strtoll(data_.v_string_);
      if (result.second) return result.first;
    }

    default: break;
  }

  return dflt;
}

value::number_uint_t value::to_uint64(value::number_uint_t dflt) const noexcept
{
  switch (kind_)
  {
    case kind::number_uint: return data_.v_uint_;
    case kind::number_int: return data_.v_int_ >= 0 ? (number_uint_t)data_.v_int_ : dflt;
    case kind::number_float:
      return (data_.v_double_ >= 0.0 &&
              data_.v_double_ <= (std::numeric_limits<number_uint_t>::max)())
                 ? (number_uint_t)data_.v_double_
                 : dflt;

    case kind::boolean: return data_.v_bool_ ? 1 : 0;
    case kind::string: {
      auto result = strtoull(data_.v_string_);
      if (result.second) return result.first;
    }

    default: break;
  }

  return dflt;
}

double value::to_double(double dflt) const noexcept
{
  switch (kind_)
  {
    case kind::number_float: return data_.v_double_;
    case kind::number_uint: return (number_float_t)data_.v_uint_;
    case kind::number_int: return (number_float_t)data_.v_int_;
    case kind::boolean: return (number_float_t)data_.v_bool_;
    case kind::string: {
      auto result = strtod(data_.v_string_);
      if (result.second) return result.first;
    }

    default: break;
  }

  return dflt;
}

value::string_t value::to_string(string_t&& dflt) const noexcept
{
  switch (kind_)
  {
    case kind::string: return data_.v_string_;
    case kind::number_int: return std::to_string(data_.v_int_);
    case kind::number_uint: return std::to_string(data_.v_uint_);
    case kind::number_float: return dtoa(data_.v_double_);
    case kind::boolean: return data_.v_bool_ ? "true" : "false";
    case kind::null: return "null";
    default: break;
  }

  return dflt;
}

json::kind value::kind() const noexcept
{
  return kind_;
}

value::string_t value::type_name() const noexcept
{
  const char* types_name[] = {"error",        "null",   "boolean", "number_int", "number_uint",
                              "number_float", "string", "array",   "object"};

  return types_name[(int)kind_];
}

value::string_t value::get(const char* key, string_t dflt) const noexcept
{
  auto result = if_contains(key);
  return result ? result->to_string(std::move(dflt)) : dflt;
}

value::string_t value::get(const char* key_path, char separator, string_t dflt) const noexcept
{
  auto val = if_contains(key_path, separator);
  return val ? val->to_string(std::move(dflt)) : dflt;
}

bool value::empty() const noexcept
{
  if (is_array()) return data_.v_array_.empty();
  if (is_object()) return data_.v_object_.empty();

  return true;
}

value::size_type value::size() const noexcept
{
  if (is_array()) return data_.v_array_.size();
  if (is_object()) return data_.v_object_.size();

  return 0;
}

value::iterator value::find(const object_t::key_type& key) noexcept(false)
{
  if (is_object()) { return value::iterator(this, data_.v_object_.find(key)); }

  FORMATS_THROW(type_except::create("can't use find() with type", type_name()));
}

value::const_iterator value::find(const object_t::key_type& key) const noexcept(false)
{
  if (is_object()) { return value::const_iterator(this, data_.v_object_.find(key)); }

  FORMATS_THROW(type_except::create("can't use find() with type", type_name()));
}

value::iterator value::find(const char* path, char path_separator) noexcept(false)
{
  if (is_object())
  {
    auto keys = spilt(path, path_separator);

    value::iterator iter;

    for (const auto& key : keys)
    {
      iter = find(key);
      if (iter == end()) break;
    }

    return iter;
  }

  FORMATS_THROW(type_except::create("can't use find() with type", type_name()));
}

value::const_iterator value::find(const char* path, char path_separator) const noexcept(false)
{
  if (is_object())
  {
    auto keys = spilt(path, path_separator);

    value::const_iterator iter;

    for (const auto& key : keys)
    {
      iter = find(key);
      if (iter == end()) break;
    }

    return iter;
  }

  FORMATS_THROW(type_except::create("can't use find() with type", type_name()));
}

bool value::contains(const object_t::key_type& key) const noexcept
{
  return (is_object()) ? data_.v_object_.contains(key) : false;
}

value::pointer value::if_contains(const object_t::key_type& key) noexcept
{
  if (is_object())
  {
    auto iter = data_.v_object_.find(key);
    if (iter != data_.v_object_.end()) return &iter->second;
  }

  return nullptr;
}

value::pointer value::if_contains(const string_t& key_path, char separator) noexcept
{
  auto paths = spilt(key_path.c_str(), separator);

  value::pointer val = this;

  for (const auto& key : paths)
  {
    if (likely(val))
    {
      val = val->if_contains(key);
      continue;
    }

    break;
  }

  return val;
}

value::const_pointer value::if_contains(const object_t::key_type& key) const noexcept
{
  if (is_object())
  {
    auto iter = data_.v_object_.find(key);
    if (iter != data_.v_object_.end()) return &iter->second;
  }

  return nullptr;
}

value::const_pointer value::if_contains(const string_t& key_path, char separator) const noexcept
{
  auto paths = spilt(key_path.c_str(), separator);

  value::const_pointer val = this;

  for (const auto& key : paths)
  {
    if (likely(val))
    {
      val = val->if_contains(key);
      continue;
    }

    break;
  }

  return val;
}

value::reference value::at(const object_t::key_type& key) noexcept(false)
{
  if (is_object())
  {
    if (auto value = if_contains(key)) return *value;

    FORMATS_THROW(range_except::create(concat("at() out of range with key:", key)));
  }

  FORMATS_THROW(type_except::create("can't use at() with type", type_name()));
}

// value::reference value::at(object_t::key_type&& key) noexcept(false)
//{
//  return at(key);
//}

value::const_reference value::at(const object_t::key_type& key) const noexcept(false)
{
  if (is_object())
  {
    if (auto value = if_contains(key)) return *value;

    FORMATS_THROW(range_except::create(concat("at() out of range with key:", key)));
  }

  FORMATS_THROW(type_except::create("can't use at() with type", type_name()));
}

// value::const_reference value::at(object_t::key_type&& key) const noexcept(false)
//{
//  return at(key);
//}

value::reference value::operator[](const object_t::key_type& key) noexcept(false)
{
  if (is_object()) return data_.v_object_[key];
  FORMATS_THROW(type_except::create("can't use operator[] with string whth type", type_name()));
}

value::reference value::operator[](object_t::key_type&& key) noexcept(false)
{
  if (is_object()) return data_.v_object_[std::move(key)];
  FORMATS_THROW(type_except::create("can't use operator[] with string whth type", type_name()));
}

value::const_reference value::operator[](const object_t::key_type& key) const noexcept(false)
{
  return at(key);
}

value::const_reference value::operator[](object_t::key_type&& key) const noexcept(false)
{
  return at(key);
}

value::reference value::at(size_type pos) noexcept(false)
{
  if (is_array())
  {
    if (pos < data_.v_array_.size()) return data_.v_array_[pos];

    FORMATS_THROW(range_except::create("operator[] index out of range", pos));
  }

  FORMATS_THROW(type_except::create("can't use operator[] with type", type_name()));
}

value::const_reference value::at(size_type pos) const noexcept(false)
{
  if (is_array())
  {
    if (pos < data_.v_array_.size()) return data_.v_array_[pos];

    FORMATS_THROW(range_except::create("operator[] index out of range", pos));
  }

  FORMATS_THROW(type_except::create("can't use operator[] with type", type_name()));
}

bool value::operator==(const value& other) const noexcept
{
  if (kind_ == other.kind_)
  {
    switch (kind_)
    {
      case kind::boolean: return data_.v_bool_ == other.data_.v_bool_;
      case kind::number_int: return data_.v_int_ == other.data_.v_int_;
      case kind::number_uint: return data_.v_uint_ == other.data_.v_uint_;
      case kind::number_float: return data_.v_double_ == other.data_.v_double_;
      case kind::string: return data_.v_string_ == other.data_.v_string_;
      case kind::array: return data_.v_array_ == other.data_.v_array_;
      case kind::object: return data_.v_object_ == other.data_.v_object_;
      case kind::null: return true;
      case kind::error: return true; break;

      default: break;
    }
  }

  if (this->is_number() && other.is_number())
  {
    return (is_double() || other.is_double()) ? (this->to_double() == other.to_double())
                                              : (this->to_uint64() == other.to_uint64());
  }

  return false;
}

bool value::operator!=(const value& other) const noexcept
{
  return !(this->operator==(other));
}

void value::emplace_null() noexcept
{
  destory();
}

value::boolean_t& value::emplace_bool() noexcept
{
  destory();
  kind_ = kind::boolean;
  new (&data_.v_bool_) boolean_t(false);

  return data_.v_bool_;
}

value::number_int_t& value::emplace_int64() noexcept
{
  destory();
  kind_ = kind::number_int;
  new (&data_.v_int_) number_int_t(0);

  return data_.v_int_;
}

value::number_uint_t& value::emplace_uint64() noexcept
{
  destory();
  kind_ = kind::number_uint;
  new (&data_.v_uint_) number_uint_t(0);

  return data_.v_uint_;
}

value::number_float_t& value::emplace_double() noexcept
{
  destory();
  kind_ = kind::number_float;
  new (&data_.v_double_) number_float_t(0);

  return data_.v_double_;
}

value::string_t& value::emplace_string() noexcept
{
  destory();
  kind_ = kind::string;
  new (&data_.v_string_) string_t("");

  return data_.v_string_;
}

value::array_t& value::emplace_array() noexcept
{
  destory();
  kind_ = kind::array;
  new (&data_.v_array_) array_t();

  return data_.v_array_;
}

value::object_t& value::emplace_object() noexcept
{
  destory();
  kind_ = kind::object;
  new (&data_.v_object_) object_t();

  return data_.v_object_;
}

void value::clear() noexcept
{
  switch (kind_)
  {
    case kind::array: data_.v_array_.clear(); break;
    case kind::object: data_.v_object_.clear(); break;
    case kind::string: data_.v_string_.clear(); break;
    case kind::boolean: data_.v_bool_ = false; break;
    case kind::number_int: data_.v_int_ = 0; break;
    case kind::number_uint: data_.v_uint_ = 0; break;
    case kind::number_float: data_.v_double_ = 0.0; break;
    default: break;
  }
}

void value::swap(value& other) noexcept
{
  if (this == &other) return void();

  if (kind() == other.kind())
  {
    switch (kind_)
    {
      case kind::array: data_.v_array_.swap(other.data_.v_array_); break;
      case kind::object: data_.v_object_.swap(other.data_.v_object_); break;
      case kind::string: data_.v_string_.swap(other.data_.v_string_); break;
      case kind::boolean: std::swap(data_.v_bool_, other.data_.v_bool_); break;
      case kind::number_int: std::swap(data_.v_int_, other.data_.v_int_); break;
      case kind::number_uint: std::swap(data_.v_uint_, other.data_.v_uint_); break;
      case kind::number_float: std::swap(data_.v_double_, other.data_.v_double_); break;
      default: break;
    }

    return void();
  }

  value v(std::move(*this));
  *this = std::move(other);
  other = std::move(v);
}

void value::merge(value& other) noexcept(false)
{
  set_type_if_none(kind::object);

  if (is_object() && other.is_object())
  {
    data_.v_object_.merge(other.data_.v_object_);
    return void();
  }

  FORMATS_THROW(type_except::create("can't use merge with type", type_name()));
}

void value::merge(value&& other) noexcept(false)
{
  set_type_if_none(kind::object);

  if (is_object() && other.is_object())
  {
    data_.v_object_.merge(std::move(other.data_.v_object_));
    other.destory();

    return void();
  }

  FORMATS_THROW(type_except::create("can't use merge with type", type_name()));
}

void value::erase(const object_t::key_type& key) noexcept
{
  if (is_object()) { data_.v_object_.erase(key); }
}

void value::erase(object_t::key_type&& key) noexcept
{
  if (is_object()) { data_.v_object_.erase(std::move(key)); }
}

void value::erase(size_type pos) noexcept(false)
{
  if (is_array())
  {
    if (pos < data_.v_array_.size())
    {
      data_.v_array_.erase(data_.v_array_.begin() + pos);
      return void();
    }

    FORMATS_THROW(range_except::create("erase() array index is out of range", pos));
  }

  FORMATS_THROW(type_except::create("can't use erase with type", type_name()));
}

value::iterator value::erase(iterator pos) noexcept(false)
{
  return erase(const_iterator(pos));
}

value::iterator value::erase(const_iterator pos) noexcept(false)
{
  if (is_array())
  {
    if (pos.owner() != this)
    {
      FORMATS_THROW(type_except::create("can't erase iterators not into this"));
    }

    if ((size_t)pos.distance_from_begin() >= size())
    {
      FORMATS_THROW(range_except::create("can't erase iterators not within the range of this"));
    }

    auto dis = std::distance(data_.v_array_.begin(), data_.v_array_.erase(pos.array_iter()));
    return iterator(this, dis);

    /*auto pos_next = data_.v_array_.erase(pos.array_iter());
    return (pos_next != data_.v_array_.end())
               ? iterator(this, std::distance(data_.v_array_.begin(), pos_next))
               : end();*/
  }

  return end();
}

value::iterator value::erase(iterator first, iterator last) noexcept(false)
{
  return erase(const_iterator(first), const_iterator(last));
}

value::iterator value::erase(const_iterator first, const_iterator last) noexcept(false)
{
  if (is_array())
  {
    if (first.owner() != this || last.owner() != this)
    {
      FORMATS_THROW(type_except::create("can't erase iterators not into *this"));
    }

    auto pos_next = data_.v_array_.erase(first.array_iter(), last.array_iter());
    return (pos_next != data_.v_array_.end())
               ? iterator(this, std::distance(data_.v_array_.begin(), pos_next))
               : end();
  }

  return end();
}

void value::push_back(const value& other) noexcept(false)
{
  set_type_if_none(kind::array);

  if (is_array())
  {
    data_.v_array_.push_back(other);
    return void();
  }

  FORMATS_THROW(type_except::create("can't use push_back with type", type_name()));
}

void value::push_back(value&& other) noexcept(false)
{
  set_type_if_none(kind::array);

  if (is_array())
  {
    data_.v_array_.push_back(std::move(other));
    return void();
  }

  FORMATS_THROW(type_except::create("can't use push_back with type", type_name()));
}

value::iterator value::insert(const_iterator pos, const value& val) noexcept(false)
{
  return insert_iterator_to_array(pos, val);
}

value::iterator value::insert(const_iterator pos, value&& val) noexcept(false)
{
  return insert_iterator_to_array(pos, std::move(val));
}

value::iterator value::insert(const_iterator pos, size_type cnt, const value& val) noexcept(false)
{
  return (cnt == 0)
             ? value::iterator(this, std::distance(data_.v_array_.cbegin(), pos.array_iter()))
             : insert_iterator_to_array(pos, cnt, val);
}

value::iterator value::insert(const_iterator pos,
                              const_iterator first,
                              const_iterator last) noexcept(false)
{
  if (first.owner() == this || last.owner() == this)
  {
    FORMATS_THROW(type_except::create("can't insert iterators into *this, behavior is undefined"));
  }

  return insert_iterator_to_array(pos, first.array_iter(), last.array_iter());
}

value::iterator value::insert(const_iterator pos, initializer_list_t init_list) noexcept(false)
{
  if (is_array())
  {
    value::iterator iter(this, std::distance(data_.v_array_.cbegin(), pos.array_iter()));

    auto val_ref_iter = init_list.begin();
    while (val_ref_iter != init_list.end())
    {
      iter = insert_iterator_to_array(iter, **val_ref_iter++);

      // ++val_ref_iter;

      if (val_ref_iter == init_list.end() || iter == end()) break;

      ++iter;
    }

    return iter;
  }

  FORMATS_THROW(type_except::create("can't use insert with type", type_name()));
}

value::iterator value::begin() noexcept
{
  return is_object() ? iterator(this, data_.v_object_.begin()) : iterator(this, (size_t)0);
}

value::iterator value::end() noexcept
{
  return is_object() ? iterator(this, data_.v_object_.end()) : iterator(this, (size_t)-1);
}

value::const_iterator value::begin() const noexcept
{
  return is_object() ? const_iterator(this, data_.v_object_.begin())
                     : const_iterator(this, (size_t)0);
}

value::const_iterator value::end() const noexcept
{
  return is_object() ? const_iterator(this, data_.v_object_.end())
                     : const_iterator(this, (size_t)-1);
}

value::const_iterator value::cbegin() noexcept
{
  return begin();
}

value::const_iterator value::cend() noexcept
{
  return end();
}

value::string_t value::dump() const noexcept
{
  string_t result;
  detail::stringifier(result, *this, stringify_style::pretty).dump();

  return result;
}

void value::destory() noexcept
{
  switch (kind_)
  {
    case kind::string: data_.v_string_.~string_t(); break;
    case kind::number_float: data_.v_double_.~number_float_t(); break;
    case kind::number_int: data_.v_int_.~number_int_t(); break;
    case kind::number_uint: data_.v_uint_.~number_uint_t(); break;
    case kind::boolean: data_.v_bool_.~boolean_t(); break;
    case kind::array: data_.v_array_.~array_t(); break;
    case kind::object: data_.v_object_.~object_t(); break;
    default: break;
  }

  kind_ = kind::null;
}

void value::set_type_if_none(json::kind kind) noexcept
{
  if (kind_ != kind::null) return void();

  kind_ = kind;
  switch (kind_)
  {
    case kind::boolean: new (&data_.v_bool_) bool(false); break;
    case kind::string: new (&data_.v_string_) string_t{}; break;
    case kind::number_int: new (&data_.v_int_) int64_t(0); break;
    case kind::number_uint: new (&data_.v_uint_) uint64_t(0); break;
    case kind::number_float: new (&data_.v_double_) number_float_t(0.0); break;
    case kind::array: new (&data_.v_array_) array_t{}; break;
    case kind::object: new (&data_.v_object_) object_t{}; break;
    case kind::error:
      // new (&data_.v_error_) error(other.data_.v_error_);
      break;

    default: break;
  }
}

std::ostream& operator<<(std::ostream& ostream, const value& v) noexcept
{
  detail::stringifier(ostream, v, stringify_style::standard).dump();
  return ostream;
}

FORMATS_JSON_NAMESPACE_END
