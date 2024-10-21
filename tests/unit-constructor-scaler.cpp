
#include "json_test.h"

#include <formats/formats.h>

using namespace formats;

TEST(ConstructFromType)
{
  {
    // none json
    {
      CHECK_EQ(json::value().kind(), json::kind::null);
      CHECK_EQ(json::value(json::kind::null).kind(), json::kind::null);

      CHECK(json::value(json::kind::null) == nullptr);
    }

    // bool json
    {
      CHECK_EQ(json::value(json::kind::boolean).kind(), json::kind::boolean);
      CHECK(json::value(json::kind::boolean) == false);
    }

    // number
    {
      CHECK_EQ(json::value(json::kind::number_int).kind(), json::kind::number_int);
      CHECK_EQ(json::value(json::kind::number_uint).kind(), json::kind::number_uint);
      CHECK_EQ(json::value(json::kind::number_float).kind(), json::kind::number_float);

      CHECK(json::value(json::kind::number_int) == 0);
      CHECK(json::value(json::kind::number_uint) == 0);
      CHECK(json::value(json::kind::number_float) == 0.0);
    }

    // string
    {
      CHECK_EQ(json::value(json::kind::string).kind(), json::kind::string);
      CHECK(json::value(json::kind::string) == "");
    }

    // array
    {
      CHECK_EQ(json::value(json::kind::array).kind(), json::kind::array);
    }
    // object
    {
      CHECK_EQ(json::value(json::kind::object).kind(), json::kind::object);
    }
  }
}

TEST(ConstructFromScaler)
{
  // none json
  {
    CHECK(json::value(nullptr).is_null());
    CHECK(json::value(nullptr) == nullptr);
  }

  // bool json
  {
    CHECK(json::value(true).is_bool());
    CHECK(json::value(true) == true);
    CHECK(json::value(false).is_bool());
    CHECK(json::value(false) == false);
  }

  // number
  {
    CHECK(json::value(3).is_int64());
    CHECK(json::value(3) == 3);

    CHECK(json::value((unsigned)3).is_uint64());
    CHECK(json::value((unsigned)3) == 3);

    CHECK(json::value(3.0).is_double());
    CHECK(json::value(3.0) == 3);
  }

  // string
  {
    CHECK(json::value("i love coffee").is_string());
    CHECK(json::value("i love coffee") == "i love coffee");

    std::string s = "i love tea too";
    CHECK(json::value(s).is_string());
    CHECK(json::value(s) == s);

    CHECK(json::value(std::move(s)) == std::string("i love tea too"));
  }
}

TEST(ConstructAssignScaler)
{
  json::value jv;

  jv = nullptr;
  CHECK(jv.is_null() && jv == nullptr);

  jv = false;
  CHECK(jv.is_bool() && jv == false);

  jv = -3;
  CHECK(jv.is_int64() && jv == -3);

  jv = (unsigned long long)10;
  CHECK(jv.is_uint64() && jv == 10);

  jv = 5.1;
  CHECK(jv.is_double() && jv == 5.1);

  jv = "i love tea too";
  CHECK(jv.is_string() && jv == "i love tea too");

  std::string s1 = "i love tea too";
  jv             = s1;
  CHECK(jv.is_string() && jv == s1);

  std::string s2 = "i love tea too";
  jv             = std::move(s2);
  CHECK(jv.is_string() && jv == std::string("i love tea too"));
}