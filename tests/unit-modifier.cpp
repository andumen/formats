#include <cmath>

#include "json_test.h"
#include "custom_type.hpp"

using namespace formats;

TEST(JsonValueModifierClear)
{
  {
    json::value jv;
    jv.clear();

    CHECK(jv.type() == json::value_type::null);
    CHECK(jv == nullptr);
  }

  {
    json::value jv = true;
    jv.clear();

    CHECK(jv.type() == json::value_type::boolean);
    CHECK(jv == false);
  }

  {
    json::value jv = std::numeric_limits<uint64_t>::max();
    jv.clear();

    CHECK(jv.type() == json::value_type::number_uint);
    CHECK(jv == 0);
  }

  {
    json::value jv = std::numeric_limits<int64_t>::min();
    jv.clear();

    CHECK(jv.type() == json::value_type::number_int);
    CHECK(jv == 0);
  }

  {
    json::value jv = std::numeric_limits<float>::quiet_NaN();
    jv.clear();

    CHECK(jv.type() == json::value_type::number_float);
    CHECK(jv == 0);
  }

  {
    json::value jv = "i love coffee";
    // json::value jv("i love coffee");
    jv.clear();

    CHECK(jv.type() == json::value_type::string);
    CHECK(jv == std::string());
  }

  {
    json::value js_empty_array = json::value_type::array;
    js_empty_array.clear();

    CHECK(js_empty_array.type() == json::value_type::array);
    CHECK(js_empty_array == json::value_type::array);
    CHECK(js_empty_array.size() == 0);
    CHECK(js_empty_array.empty());

    json::value js_array = {"111", 2, 3};
    js_array.clear();

    CHECK(js_array.type() == json::value_type::array);
    CHECK(js_array == json::value_type::array);
    CHECK(js_array.size() == 0);
    CHECK(js_array.empty());
  }

  {
    json::value js_empty_object = json::value_type::object;
    js_empty_object.clear();

    CHECK(js_empty_object.type() == json::value_type::object);
    CHECK(js_empty_object == json::value_type::object);
    CHECK(js_empty_object.size() == 0);
    CHECK(js_empty_object.empty());

    json::value js_object = {{"111", 111}, {"2", 2}, {"3", 3}};
    js_object.clear();

    CHECK(js_object.type() == json::value_type::object);
    CHECK(js_object == json::value_type::object);
    CHECK(js_object.size() == 0);
    CHECK(js_object.empty());
  }
}

TEST(JsonValueModifierErase)
{
  // erase array
  {
    json::value jv = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    json::value::iterator iter;

    jv.erase(0);
    CHECK(jv.size() == 9);
    CHECK(jv[0] == 1);

    iter = jv.erase(jv.begin());
    CHECK(jv.size() == 8);
    CHECK(jv[0] == 2);
    CHECK(*iter == 2);

    iter = jv.erase(jv.begin(), jv.begin() + 2);
    CHECK(jv.size() == 6);
    CHECK(jv[0] == 4);
    CHECK(*iter == 4);

    iter = jv.erase(jv.cbegin(), jv.cend());
    CHECK(jv.size() == 0);
    CHECK(iter == jv.end());
  }

  // erase object
  {
    json::value jv = {{"Name", "Alice"}, {"Hegigt", 168}, {"Weight", 52}, {"Score", 97}};

    jv.erase("Name");
    CHECK(!jv.contains("Name"));

    jv.erase("Weight");
    CHECK(!jv.contains("Weight"));
  }
}

TEST(JsonValueModifierArrayPushBack)
{
  auto legend_obj = legend(0);

  {  // empty json
    json::value jv;
    jv.push_back(111);
    jv.push_back("2");
    jv.push_back(*legend_obj);

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 3);
    CHECK(jv == json::value({111, "2", *legend_obj}));
  }

  {  // array
    json::value jv = {4, 5, 6};
    jv.push_back(111);
    jv.push_back("2");
    jv.push_back(*legend_obj);

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 6);
  }

  {  // push_back initialize list
    json::value jv;
    jv.push_back({"111", "222"});
    jv.push_back({333, 444});

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv[0] == json::value({"111", "222"}));
    CHECK(jv[1] == json::value({333, 444}));
    CHECK(jv == json::value({{"111", "222"}, {333, 444}}));
  }
}

TEST(JsonValueModifierArrayEmplaceBack)
{
  auto legend_obj = legend(0);

  {  // empty json
    json::value jv;
    jv.emplace_back(111);
    jv.emplace_back("2");
    jv.emplace_back(*legend_obj);

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 3);
    CHECK(jv == json::value({111, "2", *legend_obj}));
  }

  {  // array
    json::value jv = {4, 5, 6};
    jv.emplace_back(111);
    jv.emplace_back("2");
    jv.emplace_back(*legend_obj);

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 6);
  }
}

TEST(JsonValueArrayModifierInsert)
{
  auto        legend_obj = legend(0);
  json::value js_legend  = *legend_obj;

  // json array insert lvalue
  {
    json::value jv = {"one", "ten"};

    json::value::iterator iter;

    // insert at begin
    iter = jv.insert(jv.begin(), js_legend);
    CHECK(jv.size() == 3);
    CHECK(jv[0] == *legend_obj);
    CHECK(*iter == *legend_obj);

    // insert at middle
    std::string middle_str = "i love coffee";

    iter = jv.insert(jv.begin() + 2, middle_str);
    CHECK(jv.size() == 4);
    CHECK(jv[2] == middle_str);
    CHECK(*iter == middle_str);

    // insert at end
    json::value last = "inf";

    iter = jv.insert(jv.end(), last);
    CHECK(jv.size() == 5);
    CHECK(jv[4] == last);
    CHECK(*iter == last);
  }

  //  json array insert rvalue
  {
    json::value jv = {"one", "ten"};

    json::value::iterator iter;

    // insert at begin
    iter = jv.insert(jv.begin(), *legend_obj);
    CHECK(jv.size() == 3);
    CHECK(jv[0] == *legend_obj);
    CHECK(*iter == *legend_obj);

    iter = jv.insert(jv.begin() + 2, "i love tea");
    CHECK(jv.size() == 4);
    CHECK(jv[2] == "i love tea");
    CHECK(*iter == "i love tea");

    iter = jv.insert(jv.end(), 1000);
    CHECK(jv.size() == 5);
    CHECK(jv[4] == 1000);
    CHECK(*iter == 1000);
  }

  // json array insert multy val
  {
    json::value jv = json::value_type::array;

    json::value::iterator iter;

    // insert at begin
    iter = jv.insert(jv.begin(), 3, "i love coffee");
    CHECK(jv.size() == 3);
    CHECK(jv == json::value({"i love coffee", "i love coffee", "i love coffee"}));
    CHECK(*iter == "i love coffee");

    // insert at middle
    iter = jv.insert(jv.begin() + 2, 2, 222);
    CHECK(jv.size() == 5);
    CHECK(jv == json::value({"i love coffee", "i love coffee", 222, 222, "i love coffee"}));
    CHECK(*iter == 222);

    // insert at end
    iter = jv.insert(jv.end(), 1, 333);
    CHECK(jv.size() == 6);
    CHECK(jv == json::value({"i love coffee", "i love coffee", 222, 222, "i love coffee", 333}));
    CHECK(*iter == 333);

    // insert cnt == 0
    iter = jv.insert(jv.end(), 0, 333);
    CHECK(iter == jv.end());

    iter = jv.insert(jv.begin(), 0, 333);
    CHECK(iter == jv.begin());
  }

  // json array insert iterator range
  {
    json::value jv = json::value_type::array;

    json::value js_array = {"one", "two", "three"};

    json::value::iterator iter;

    // insert at begin
    iter = jv.insert(jv.begin(), js_array.begin(), js_array.end());
    CHECK(jv.size() == 3);
    CHECK(jv == js_array);
    CHECK(*iter == "one");

    // insert at middle, insert one
    iter = jv.insert(jv.begin() + 2, js_array.begin(), js_array.begin() + 1);
    CHECK(jv.size() == 4);
    CHECK(jv == json::value({"one", "two", "one", "three"}));
    CHECK(*iter == "one");

    // insert at end, insert empty
    iter = jv.insert(jv.end(), js_array.begin(), js_array.begin());
    CHECK(jv.size() == 4);
    CHECK(jv == json::value({"one", "two", "one", "three"}));
    CHECK(iter == jv.end());
  }

  // json array insert initialize list
  {
    json::value jv = {1, 4, 5};

    json::value::iterator iter;

    // insert at begin
    iter = jv.insert(jv.begin(), {0});
    CHECK(jv.size() == 4);
    CHECK(jv == json::value({0, 1, 4, 5}));
    CHECK(*iter == 0);

    // insert at middle
    iter = jv.insert(jv.begin() + 2, {2, 3});
    CHECK(jv.size() == 6);
    CHECK(jv == json::value({0, 1, 2, 3, 4, 5}));
    CHECK(*iter == 3);

    // insert at end
    iter = jv.insert(jv.end(), {6, 7, 8});
    CHECK(jv.size() == 9);
    CHECK(jv == json::value({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    CHECK(*iter == 8);

    // insert empty list
    iter = jv.insert(jv.end(), {});
    CHECK(iter == jv.end());
  }
}

TEST(JsonValueObjectModifierEmplace)
{
  auto legend_obj = legend(0);

  {  // empty json
    json::value jv;

    auto res1 = jv.emplace("Garen", *legend_obj);
    auto res2 = jv.emplace("Garen", *legend_obj);
    auto res3 = jv.emplace("Name", "Garen");

    CHECK(*res1.first == *legend_obj);
    CHECK(res1.second == true);
    CHECK(res2.second == false);
    CHECK(res3.second == true);

    CHECK(jv.type() == json::value_type::object);
    CHECK(jv == json::value({{"Garen", *legend_obj}, {"Name", "Garen"}}));
  }

  {  // object
    json::value jv = {{"Name", "Garen"}};

    auto res1 = jv.emplace("Garen", *legend_obj);
    auto res2 = jv.emplace("Garen", *legend_obj);

    CHECK(*res1.first == *legend_obj);
    CHECK(res1.second == true);
    CHECK(res2.second == false);

    CHECK(jv.type() == json::value_type::object);
    CHECK(jv == json::value({{"Name", "Garen"}, {"Garen", *legend_obj}}));
  }
}

TEST(JsonValueObjectModifierMerge)
{
  json::value jv1 = {{"one", 1}, {"two", 2}};
  json::value jv2 = {{"three", 3}, {"foue", 4}, {"one", 1}};

  jv1.merge(jv2);

  CHECK(jv1 == json::value({{"one", 1}, {"two", 2}, {"three", 3}, {"foue", 4}}));
  CHECK(jv2 == json::value({{"one", 1}}));
}

TEST(JsonValueSwap)
{
  {
    json::value jv1("i love coffee");
    json::value jv2(3.1415);

    jv1.swap(jv2);

    CHECK(jv1 == 3.1415);
    CHECK(jv2 == "i love coffee");
  }

  {
    json::value jv1("i love coffee");
    json::value jv2(3.1415);

    std::swap(jv1, jv2);

    CHECK(jv1 == 3.1415);
    CHECK(jv2 == "i love coffee");
  }
}
