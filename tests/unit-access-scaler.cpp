#include <cmath>

#include "json_test.h"
#include "custom_type.hpp"

using namespace formats;

TEST(JsonValueAccessScaler)
{
  json::value jv;

  {  // nullptr
    jv = nullptr;
    CHECK_EQ(jv.to_string("").c_str(), "null");
  }

  {  // bool value
    bool bool_value = true;

    jv = bool_value;

    CHECK(jv.is_bool() && (jv.as_bool() == bool_value));

    CHECK_EQ(jv.to_bool(0), bool_value);
    CHECK_EQ(jv.to_int64(0), 1);
    CHECK_EQ(jv.to_uint64(0), 1);
    CHECK_EQ(jv.to_double(0), 1.0);
    CHECK_EQ(jv.to_string("").c_str(), "true");
  }

  {  // int value
    int32_t int_value = (std::numeric_limits<int32_t>::max)();

    jv = int_value;

    CHECK(jv.is_int64() && (jv.as_int64() == int_value));

    CHECK_EQ(jv.to_bool(0), int_value != 0);
    CHECK_EQ(jv.to_int64(0), int_value);
    CHECK_EQ(jv.to_uint64(0), int_value);
    CHECK_EQ(jv.to_double(0), int_value);
    CHECK_EQ(jv.to_string("").c_str(), std::to_string(int_value).c_str());
  }

  {  // uint value
    uint64_t uint_value = (std::numeric_limits<int32_t>::max)();

    jv = uint_value;

    CHECK(jv.is_uint64() && (jv.as_uint64() == uint_value));

    CHECK_EQ(jv.to_bool(0), uint_value != 0);
    CHECK_EQ(jv.to_int64(0), uint_value);
    CHECK_EQ(jv.to_uint64(0), uint_value);
    CHECK_EQ(jv.to_double(0), uint_value);
    CHECK_EQ(jv.to_string("").c_str(), std::to_string(uint_value).c_str());
  }

  {  // float value
    double double_value = -123456789.01234;

    jv = double_value;

    CHECK(jv.is_double() && (jv.as_double() == double_value));

    CHECK_EQ(jv.to_bool(0), double_value != 0);
    CHECK_EQ(jv.to_int64(0), -123456789);
    CHECK_EQ(jv.to_uint64(0), 0);
    CHECK_EQ(jv.to_double(0), -123456789.01234);
    CHECK_EQ(strtod(jv.to_string("").c_str(), nullptr), -123456789.01234);
  }

  {  // unsigned integer string value

    json::value jv;
    {
      std::string string_value = "12345678901234";

      jv = string_value;

      CHECK(jv.is_string() && (jv.as_string() == string_value));

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), 12345678901234);
      CHECK_EQ(jv.to_uint64(0), 12345678901234);
      CHECK_EQ(jv.to_double(0), 12345678901234.0);
      CHECK_EQ(jv.to_string("").c_str(), "12345678901234");
    }

    // unsigned integer string value overflow
    {
      std::string string_value = "1234567890123421213212141212112121";

      jv = string_value;

      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
    }

    {  // signed integer  string value
      std::string string_value = "-12345678901234";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), -12345678901234);
      CHECK_EQ(jv.to_uint64(0), -12345678901234);
      CHECK_EQ(jv.to_double(0), -12345678901234.0);
      CHECK_EQ(jv.to_string("").c_str(), "-12345678901234");
    }

    // signed integer  string value overflow
    {
      std::string string_value = "-123456789012341113131313131";

      jv = string_value;
      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
    }

    {  // signed integer  string value trailing junk
      std::string string_value = "-12345678901234abcd";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), -12345678901234);
      CHECK_EQ(jv.to_uint64(0), -12345678901234);
      CHECK_EQ(jv.to_double(0), -12345678901234.0);
      CHECK_EQ(jv.to_string("").c_str(), "-12345678901234abcd");
    }

    {  // float number string value
      std::string string_value = "-1234567890.1234";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), -1234567890);
      CHECK_EQ(jv.to_uint64(0), -1234567890);
      CHECK_EQ(jv.to_double(0), -1234567890.12340);
      CHECK_EQ(jv.to_string("").c_str(), "-1234567890.1234");
    }

    {  // float number string value trailing junk
      std::string string_value = "-1234567890.1234abcd";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), -1234567890);
      CHECK_EQ(jv.to_uint64(0), -1234567890);
      CHECK_EQ(jv.to_double(0), -1234567890.12340);
      CHECK_EQ(jv.to_string("").c_str(), "-1234567890.1234abcd");
    }

    {  // NaN
      std::string string_value = "NaN";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
      CHECK_EQ(jv.to_double(0), std::numeric_limits<double>::quiet_NaN());
      CHECK_EQ(jv.to_string("").c_str(), "NaN");
    }

    {  // infinity
      std::string string_value = "-infinity";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
      CHECK_EQ(jv.to_double(0), -std::numeric_limits<double>::infinity());
      CHECK_EQ(jv.to_string("").c_str(), "-infinity");
    }

    {  // not number string
      std::string string_value = "i love coffee";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), true);
      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
      CHECK_EQ(jv.to_double(0), 0.0);
      CHECK_EQ(jv.to_string("").c_str(), "i love coffee");
    }

    {  // bool string
      std::string string_value = "False";

      jv = string_value;

      CHECK_EQ(jv.to_bool(0), false);
      CHECK_EQ(jv.to_int64(0), 0);
      CHECK_EQ(jv.to_uint64(0), 0);
      CHECK_EQ(jv.to_double(0), 0.0);
      CHECK_EQ(jv.to_string("").c_str(), "False");
    }
  }
}
