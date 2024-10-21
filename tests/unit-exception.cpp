#include "json_test.h"
#include "custom_type.hpp"

TEST(JsonValueExceptionAsType)
{
  // as bool
  {
    json::value jv = json::kind::object;
    try
    {
      jv.as_bool();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as int
  {
    // type exception
    json::value jv1 = json::kind::object;
    try
    {
      jv1.as_int64();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }

    // out of range
    json::value jv2 = (unsigned long long)-1;
    try
    {
      jv1.as_int64();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as uint
  {
    // type exception
    json::value jv1 = json::kind::object;
    try
    {
      jv1.as_uint64();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }

    // out of range
    json::value jv2 = -1;
    try
    {
      jv1.as_uint64();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as float
  {
    // type exception
    json::value jv = -1;
    try
    {
      jv.as_double();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as string
  {
    // type exception
    json::value jv = -1;
    try
    {
      jv.as_string();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as array
  {
    // type exception
    json::value jv = -1;
    try
    {
      jv.as_array();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }

  // as object
  {
    // type exception
    json::value jv = -1;
    try
    {
      jv.as_object();
    }
    catch (const formats::exception& e)
    {
      printf("%s. \n", e.what());
    }
  }
}

TEST(JsonValueExceptionFind)
{
  json::value jv = nullptr;

  try
  {
    auto iter = jv.find("one");
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());
  }
}

TEST(JsonValueExceptionObjectAt)
{
  try
  {
    json::value jv   = nullptr;
    auto        iter = jv.at("one");
  }
  catch (const formats::exception& e)
  {  // not object
    printf("%s. \n", e.what());
  }

  try
  {
    json::value jv   = {{"one", 1}};
    auto        iter = jv.at("two");
  }
  catch (const formats::exception& e)
  {  // object no such key
    printf("%s. \n", e.what());
  }
}

TEST(JsonValueExceptionObjectOperatorAccess)
{
  try
  {
    json::value jv  = nullptr;
    auto        ref = jv["one"];
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not object
  }

  try
  {
    json::value const jv  = {{"one", 1}};
    auto              ref = jv["two"];
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // const object no such key
  }
}

TEST(JsonValueExceptionobjectEmplace)
{
  try
  {
    json::value jv     = 1;
    auto        result = jv.emplace("one", 2);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not object or null
  }
}

TEST(JsonValueExceptionobjectMerge)
{
  try
  {
    json::value jv = 1;
    jv.merge(json::value({"one", 2}));
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not object or null
  }

  try
  {
    json::value jv = {{"one", 1}};
    jv.merge(json::value({"one", 2}));
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // other is not object
  }
}

TEST(JsonValueExceptionArrayAt)
{
  try
  {
    json::value jv  = nullptr;
    auto        ref = jv.at(10);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not array
  }

  try
  {
    json::value jv  = {1, 2, 3};
    auto        ref = jv.at(10);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // array. out of range
  }
}

TEST(JsonValueExceptionArrayEraseByIndex)
{
  try
  {
    json::value jv = {1, 2, 3};
    jv.erase(20);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // out of range
  }
}

TEST(JsonValueExceptionArrayEraseByIter)
{
  try
  {
    json::value jv1 = 1;
    json::value jv2 = {{"one", 1}, {"two", 2}};
    jv1.erase(jv2.begin());
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not array
  }

  try
  {
    json::value jv = {1, 2, 3};

    auto iter = jv.begin() + 2;
    jv.erase(jv.begin());
    jv.erase(iter);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // iterator incompatible
  }

  try
  {
    json::value jv1 = {1, 2};
    json::value jv2 = {3, 4};
    jv1.erase(jv2.begin());
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // iterator not into this
  }
}

TEST(JsonValueExceptionArrayPushBack)
{
  try
  {
    json::value jv = 1;
    jv.push_back(json::value(2));
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }
}

TEST(JsonValueExceptionArrayInsert)
{
  try
  {
    json::value jv1 = 1;
    json::value jv2 = {1, 2};
    jv1.insert(jv2.begin(), json::value(2));
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }

  try
  {
    json::value jv1 = {3, 4};
    json::value jv2 = { {"one", 1},{"two", 2} };
    jv1.insert(jv2.begin(), json::value(2));
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  //  pos is not iterator to this
  }

  try
  {
    json::value jv = {1, 2, 3, 4};
    jv.insert(jv.begin(), jv.begin(), jv.end());
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  //  first or last are iterators into this
  }
}

TEST(JsonValueExceptionArrayEmplaceBack)
{
  try
  {
    json::value jv = 1;
    jv.emplace_back(4);
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }
}

TEST(JsonValueExceptionIteratorPlus)
{
  try
  {
    json::value jv   = {1, 2};
    auto        iter = jv.end();

    iter += 10;
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }
}

TEST(JsonValueExceptionIteratorMinus)
{
  try
  {
    json::value jv   = {1, 2};
    auto        iter = jv.begin();

    iter--;
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }
}

TEST(JsonValueExceptionParse)
{
  try
  {
    auto val = json::parse("[1, 2, ]");
  }
  catch (const formats::exception& e)
  {
    printf("%s. \n", e.what());  // not null or array
  }
}