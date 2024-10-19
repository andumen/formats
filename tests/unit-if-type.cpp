#include "json_test.h"
#include "custom_type.hpp"

TEST(JsonValueIfType)
{
  // bool
  {
    json::value jv1 = false;
    json::value jv2 = nullptr;

    CHECK(jv1.if_bool());
    CHECK(*jv1.if_bool() == false);
    CHECK(!jv2.if_bool());
  }

  // int
  {
    json::value jv1 = 1;
    json::value jv2 = nullptr;
    json::value jv3 = (uint64_t)-1;

    CHECK(jv1.if_int64());
    CHECK(*jv1.if_int64() == 1);
    CHECK(!jv2.if_int64());
    CHECK(!jv3.if_int64());
  }

  // uint
  {
    json::value jv1 = 1;
    json::value jv2 = nullptr;
    json::value jv3 = (uint64_t)-1;

    CHECK(jv1.if_int64());
    CHECK(*jv1.if_int64() == 1);
    CHECK(!jv2.if_uint64());
    CHECK(jv3.if_uint64());
  }

  // float
  {
    json::value jv1 = 1.2;
    json::value jv2 = nullptr;

    CHECK(jv1.if_double());
    CHECK(*jv1.if_double() == 1.2);
    CHECK(!jv2.if_double());
  }

  // string
  {
    json::value jv1 = "1.2";
    json::value jv2 = nullptr;

    CHECK(jv1.if_string());
    CHECK_EQ(jv1.if_string()->c_str(), "1.2");
    CHECK(!jv2.if_string());
  }

  // array
  {
    json::value jv1 = {"1.2", 10, false};
    json::value jv2 = nullptr;

    CHECK(jv1.if_array());
    CHECK(jv1.if_array()->size() == 3);
    CHECK(!jv2.if_array());
  }

  // object
  {
    json::value jv1 = {{"one", "1.2"}, {"two", 10}, {"three", false}};
    json::value jv2 = nullptr;

    CHECK(jv1.if_object());
    CHECK(jv1.if_object()->size() == 3);
    CHECK(!jv2.if_object());
  }
}

TEST(JsonValueIfTypeConst)
{
  // bool
  {
    json::value const jv1 = false;
    json::value const jv2 = nullptr;

    CHECK(jv1.if_bool());
    CHECK(*jv1.if_bool() == false);
    CHECK(!jv2.if_bool());
  }

  // int
  {
    json::value const jv1 = 1;
    json::value const jv2 = nullptr;
    json::value const jv3 = (uint64_t)-1;

    CHECK(jv1.if_int64());
    CHECK(*jv1.if_int64() == 1);
    CHECK(!jv2.if_int64());
    CHECK(!jv3.if_int64());
  }

  // uint
  {
    json::value const jv1 = 1;
    json::value const jv2 = nullptr;
    json::value const jv3 = (unsigned)1;

    CHECK(jv1.if_int64());
    CHECK(*jv1.if_int64() == 1);
    CHECK(!jv2.if_uint64());
    CHECK(jv3.if_uint64());
  }

  // float
  {
    json::value const jv1 = 1.2;
    json::value const jv2 = nullptr;

    CHECK(jv1.if_double());
    CHECK(*jv1.if_double() == 1.2);
    CHECK(!jv2.if_double());
  }

  // string
  {
    json::value const jv1 = "1.2";
    json::value const jv2 = nullptr;

    CHECK(jv1.if_string());
    CHECK_EQ(jv1.if_string()->c_str(), "1.2");
    CHECK(!jv2.if_string());
  }

  // array
  {
    json::value const jv1 = {"1.2", 10, false};
    json::value const jv2 = nullptr;

    CHECK(jv1.if_array());
    CHECK(jv1.if_array()->size() == 3);
    CHECK(!jv2.if_array());
  }

  // object
  {
    json::value const jv1 = {{"one", "1.2"}, {"two", 10}, {"three", false}};
    json::value const jv2 = nullptr;

    CHECK(jv1.if_object());
    CHECK(jv1.if_object()->size() == 3);
    CHECK(!jv2.if_object());
  }
}