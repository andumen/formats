#include "json_test.h"
#include "custom_type.hpp"

TEST(JsonValueIteratorConstruct)
{
  {
    json::value jv(10);

    json::value::iterator iter(&jv);
    CHECK(*iter == 10);
  }

  {
    json::value jv({1, 2, 3});

    json::value::iterator iter1(&jv);
    CHECK(iter1 == jv.begin());

    json::value::iterator iter2(&jv, 2);
    CHECK(*iter2 == 3);

    json::value::iterator iter3(&jv, 10);
    CHECK(iter3 == jv.end());
  }

  {
    json::value jv({{"Name", "Alice"}, {"Score", 96}});

    json::value::iterator iter(&jv);
    CHECK(*iter == "Alice");
  }

  // const_iterator copy from itertor
  {
    json::value jv({1, 2, 3});

    json::value::iterator iter(&jv);

    json::value::const_iterator c_iter(iter);
    CHECK(c_iter == jv.begin());
  }
}

TEST(JsonValueArrayIteratorAccess)
{
  json::value jv({1, 2, 3});

  // range based for loop
  {
    int index = 0;
    for (const auto& v : jv)
    {
      CHECK(v == jv[index++]);
    }
  }

  {
    auto iter = jv.begin();

    int index = 0;
    while (iter != jv.end())
    {
      CHECK(*iter++ == jv[index++]);
    }
  }
}

TEST(JsonValueObjecIteratorAccess)
{
  json::value jv({{"Name", "Alice"}, {"Height", 166}, {"Score", 96}});

  // range based for loop
  {
    for (const auto& entry : jv.as_object())
    {
      CHECK(entry.second == jv[entry.first]);
    }
  }

  {
    auto iter = jv.begin();
    while (iter != jv.end())
    {
      CHECK(!iter.key().empty());
      ++iter;
    }
  }
}

TEST(JsonValueIteratorInc)
{
  // scaler json
  {
    json::value jv(10);

    json::value::iterator iter1 = jv.begin();
    iter1++;
    CHECK(iter1 == jv.end());

    json::value::iterator iter2 = jv.begin();
    ++iter2;
    CHECK(iter2 == jv.end());
  }

  // json array
  {
    json::value jv({1, 2, 3});

    json::value::iterator iter1 = jv.begin();
    CHECK(iter1->as_int64() == 1);
    CHECK(*iter1++ == 1);
    CHECK(*iter1++ == 2);
    CHECK(*iter1++ == 3);

    CHECK(iter1 == jv.end());

    iter1 = jv.begin();
    iter1 += 3;
    CHECK(iter1 == jv.end());

    json::value::iterator iter2 = jv.begin();
    CHECK(*++iter2 == 2);
    CHECK(*++iter2 == 3);
    CHECK(++iter2 == jv.end());
  }

  // json object
  {
    json::value jv({{"Name", "Alice"}, {"Score", 96}});

    json::value::iterator iter1 = jv.begin();
    CHECK(iter1->as_string() == "Alice");
    iter1++;
    CHECK(*iter1++ == jv["Score"]);
    CHECK(iter1 == jv.end());

    json::value::iterator iter2 = jv.begin();
    CHECK(*++iter2 == jv["Score"]);
    ++iter2;
    CHECK(iter2 == jv.end());
  }
}

TEST(JsonValueIteratorDec)
{
  // scaler json
  {
    json::value jv(10);

    json::value::iterator iter1 = jv.end();
    iter1--;
    CHECK(iter1 == jv.begin());

    json::value::iterator iter2 = jv.end();
    --iter2;
    CHECK(iter2 == jv.begin());
  }

  // json array
  {
    json::value jv({1, 2, 3});

    json::value::iterator iter1 = jv.end();

    iter1--;
    CHECK(iter1->as_int64() == 3);
    CHECK(*iter1-- == 3);
    CHECK(*iter1-- == 2);
    CHECK(*iter1 == 1);
    CHECK(iter1 == jv.begin());

    iter1 = jv.end();
    iter1 -= 3;
    CHECK(iter1 == jv.begin());

    json::value::iterator iter2 = jv.end();
    CHECK(*--iter2 == 3);
    CHECK(*--iter2 == 2);
    CHECK(*--iter2 == 1);
    CHECK(iter2 == jv.begin());
  }

  // json object
  {
    json::value jv({{"Name", "Alice"}, {"Score", 96}});

    json::value::iterator iter1 = jv.end();
    iter1--;
    CHECK(iter1->as_int64() == 96);
    CHECK(*iter1-- == jv["Score"]);
    CHECK(iter1 == jv.begin());

    json::value::iterator iter2 = jv.end();

    --iter2;
    CHECK(*iter2 == jv["Score"]);

    --iter2;
    CHECK(iter2 == jv.begin());
  }
}
