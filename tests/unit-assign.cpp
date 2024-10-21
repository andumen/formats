#include <formats/formats.h>
#include <array>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <forward_list>
// #include <string_view>
#include <map>
#include <iostream>

#include "json_test.h"
#include "custom_type.hpp"

using namespace formats;

TEST(JsonValueAssign)
{
  auto legend_obj1 = legend(0);
  auto legend_obj2 = legend(1);

  json::value js;

  js = nullptr;
  CHECK(js == nullptr);

  std::array<std::nullptr_t, 1> container_array = {nullptr};
  js                                            = container_array;
  CHECK(js.is_array());
  CHECK(js == container_array);

  js = true;
  CHECK(js.is_bool());
  CHECK(js == true);

  std::vector<bool> container_vec = {false, true, true};
  js                              = container_vec;
  CHECK(js.is_array());
  CHECK(js == container_vec);

  js = -3;
  CHECK(js.is_int64());
  CHECK(js == -3);

  std::list<int> container_list = {1, -2, -3, 4};
  js                            = container_list;
  CHECK(js.is_array());
  CHECK(js == container_list);

  js = (uint64_t)-3;
  CHECK(js.is_uint64());
  CHECK(js == uint64_t(-3));

  std::deque<float> container_deque = {1, 2, 3, 4};
  js                                = container_deque;
  CHECK(js.is_array());
  CHECK(js == container_deque);

  js = -3.14;
  CHECK(js.is_double());
  CHECK(js == -3.14);

  std::set<std::string> container_set = {"hello", "world"};
  js                                  = container_set;
  CHECK(js.is_array());
  CHECK(js == container_set);

  js = "i love coffee";
  CHECK(js.is_string());
  CHECK(js == "i love coffee");

  std::forward_list<Legend> container_forward_list = {*legend_obj1, *legend_obj2};

  js = container_forward_list;
  CHECK(js.is_array());
  CHECK(js[1].kind() == json::kind::object);

  json::value js_array(container_forward_list);
  CHECK(js == js_array);
}

TEST(JsonValueEmplaceType)
{
  json::value jv;

  jv = nullptr;
  CHECK(jv == nullptr);

  jv.emplace_null();
  CHECK(jv == nullptr);

  auto& b = jv.emplace_bool();
  b       = true;
  CHECK(jv == true);

  auto& i = jv.emplace_int64();
  i       = -10;
  CHECK(jv == -10);

  auto& ui = jv.emplace_uint64();
  ui       = 10000;
  CHECK(jv == 10000);

  auto& d = jv.emplace_double();
  d       = 0.158;
  CHECK(jv == 0.158);

  auto& s = jv.emplace_string();
  s       = "i love c++";
  CHECK(jv == "i love c++");

  auto& arr = jv.emplace_array();
  arr.emplace_back(1);
  arr.emplace_back("two");
  CHECK(jv == json::value({1, "two"}));

  auto& obj = jv.emplace_object();
  obj.emplace("one", 1);
  obj.emplace("two", 2);
  CHECK(jv == json::value({{"one", 1}, {"two", 2}}));
}