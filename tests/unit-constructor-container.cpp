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

TEST(ConstructFromSequenceContainer)
{
  auto legend_obj1 = legend(0);
  auto legend_obj2 = legend(1);

  std::array<std::nullptr_t, 1> container_array = {nullptr};
  std::vector<bool>             container_vec   = {false, true, true};
  std::list<int>                container_list  = {1, -2, -3, 4};

  std::deque<double> container_deque = {1.1, 2.0, 3.14, 4.18};

  std::set<std::string> container_set = {"hello", "world"};

  std::forward_list<Legend> container_forward_list = {*legend_obj1, *legend_obj2};

  json::value js_array(container_array);
  CHECK(js_array.is_array());
  CHECK(js_array[0] == nullptr);

  js_array = container_vec;
  CHECK(js_array.is_array());
  CHECK(js_array[0] == false);

  js_array = container_list;
  CHECK(js_array.is_array());
  CHECK(js_array[0] == 1);

  js_array = container_deque;
  CHECK(js_array.is_array());
  CHECK(js_array[1].type() == json::value_type::number_float);

  js_array = container_forward_list;
  CHECK(js_array.is_array());
  CHECK(js_array[1].type() == json::value_type::object);
}

TEST(ConstructFromAssociationContainer)
{
  std::set<int>              set      = {1, 2, 3, 4};
  std::multiset<std::string> multiset = {"A", "B", "C"};

  std::unordered_set<int>              unordered_set      = {1, 2, 3, 4};
  std::unordered_multiset<std::string> unordered_multiset = {"hello", "world"};

  json::value js(set);
  CHECK(js.is_array());

  js = multiset;
  CHECK(js[0] == "A");

  js = unordered_set;
  CHECK(js.is_array());
  CHECK(js.size() == 4);

  js = unordered_multiset;
  CHECK(js.is_array());
  CHECK(js.size() == 2);

  std::pair<std::string, int>      pair     = {"AAA", 1};
  std::map<std::string, int>       map      = {{"Aatrox", 0}, {"Fizz", 1}};
  std::multimap<const char*, bool> multimap = {{"Aatrox", true}, {"Fizz", false}};
  std::unordered_map<std::string, double> unordered_map = {{"AAA", 3.5}, {"BBB", -4.8}};
  std::unordered_multimap<std::string, int> unordered_multimap = {{"Gnar", 2}, {"Lillia", -3}};

  js = pair;
  CHECK(js.is_object());

  js = map;
  CHECK(js.is_object());
  CHECK(js["Aatrox"]== 0);

  js = multimap;
  CHECK(js.is_object());
  CHECK(js["Fizz"] == false);

  js = unordered_map;
  CHECK(js.is_object());
  CHECK(js["BBB"] == -4.8);

  js = unordered_multimap;
  CHECK(js.is_object());
  CHECK(js["Lillia"] == -3);
}
