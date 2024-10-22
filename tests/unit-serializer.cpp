#include <array>
#include <queue>

#include <set>
#include <unordered_set>
#include <forward_list>
// #include <string_view>
#include <map>

#include "json_test.h"

#include "custom_type.hpp"

using namespace formats;

template <typename T>
class TraversableContainer
{
public:
  using value_type     = T;
  using container_type = std::vector<T>;
  using iterator       = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

public:
  TraversableContainer(const std::vector<T>& data)
      : data_(data)
  {}

  iterator begin() { return data_.begin(); }
  iterator end() { return data_.end(); }

  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }

private:
  container_type data_;
};

TEST(JsonSerializer)
{
  // serilize from scaler type
  {
    json::value jv;

    jv = json::serialize(nullptr);
    CHECK(jv == nullptr);

    jv = json::serialize(1);
    CHECK(jv == 1);

    jv = json::serialize(3.14);
    CHECK(jv == 3.14);

    jv = json::serialize("i love coffee");
    CHECK(jv == "i love coffee");

    jv = json::serialize(false);
    CHECK(jv == false);
  }

  // serilize from array
  {
    std::array<int, 3> container = {1, 2, 3};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({1, 2, 3});

    CHECK(jv1 == jv2);
  }

  // serilize from vector
  {
    std::vector<uint64_t> container = {1, 2, 3};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({1, 2, 3});

    CHECK(jv1 == jv2);
  }

  // serilize from list
  {
    std::list<std::string> container = {"A", "B", "C"};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({"A", "B", "C"});

    CHECK(jv1 == jv2);
  }

  // serilize from deque
  {
    std::deque<bool> container = {true, true, false};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({true, true, false});

    CHECK(jv1 == jv2);
  }

  // forward list
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    std::forward_list<Legend> container = {*legend_obj1, *legend_obj2};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({*legend_obj1, *legend_obj2});

    CHECK(jv1 == jv2);
  }

  // serilize from set
  {
    std::set<int> container = {1, 2, 3};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({1, 2, 3});

    CHECK(jv1 == jv2);
  }

  // serilize from multiset
  {
    std::multiset<int> container = {1, 2, 3};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({1, 2, 3});

    CHECK(jv1 == jv2);
  }

  // serilize from unordered_set
  {
    std::unordered_set<int> container = {1, 2, 3};

    json::value jv = json::serialize(container);

    CHECK(jv.size() == 3);
  }

  // serilize from unordered_multiset
  {
    std::unordered_multiset<int> container = {1, 2, 3};

    json::value jv = json::serialize(container);

    CHECK(jv.size() == 3);
  }

  // serilize from map
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    std::map<std::string, Legend> container = {{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}});

    CHECK(jv1 == jv2);
  }

  // serilize from multimap
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    std::multimap<std::string, Legend> container = {{"Kayle", *legend_obj1},
                                                    {"Neeko", *legend_obj2}};

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}});

    CHECK(jv1 == jv2);
  }

  // serilize from unordered_map
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    std::unordered_map<std::string, Legend> container = {{"Kayle", *legend_obj1},
                                                         {"Neeko", *legend_obj2}};

    json::value jv = json::serialize(container);
    CHECK(jv.size() == 2);
  }

  // serilize from unordered_multimap
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    std::unordered_multimap<std::string, Legend> container = {{"Kayle", *legend_obj1},
                                                              {"Neeko", *legend_obj2}};

    json::value jv = json::serialize(container);
    CHECK(jv.size() == 2);
  }

  // custom container
  {
    TraversableContainer<uint64_t> container(std::vector<uint64_t>{100, 20, 30});

    json::value jv1 = json::serialize(container);
    json::value jv2 = json::value({100, 20, 30});

    CHECK(jv1 == jv2);
  }

  // custom type
  {
    auto legend_obj1 = legend(0);

    json::value jv1 = json::serialize(*legend_obj1);
    json::value jv2 = legend_obj1->to_json();

    CHECK(jv1 == jv2);
  }
}

TEST(JsonUnserializer)
{
  // serailize to vector
  {
    json::value jv = {1, 2, 3};

    std::vector<uint32_t> result;
    json::unserialize(jv, result);

    CHECK(jv == json::serialize(result));
  }

  // serailize to list
  {
    json::value jv = {-1.0, 2.3, 3.14};

    std::list<double> result;
    json::unserialize(jv, result);

    CHECK(jv == json::serialize(result));
  }

  // serailize to set
  {
    json::value jv = {1, 2, 3};

    std::set<int> result;
    json::unserialize(jv, result);

    CHECK(jv == json::serialize(result));
  }

  // serailize to set
  {
    json::value jv = {1, 2, 3};

    std::multiset<int> result;
    json::unserialize(jv, result);

    CHECK(jv == json::serialize(result));
  }

  // serailize to unordered_set
  {
    json::value jv = {1, 2, 3};

    std::unordered_set<int> result;
    json::unserialize(jv, result);

    CHECK(result.size() == 3);
  }

  // serailize to unordered_multiset
  {
    json::value jv = {1, 2, 3};

    std::unordered_multiset<int> result;
    json::unserialize(jv, result);

    CHECK(result.size() == 3);
  }

  // serailize to map
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    json::value jv = {{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}};

    std::map<std::string, Legend> result;
    json::unserialize(jv, result);

    CHECK(result.size() == 2);
    CHECK(jv == json::serialize(result));
  }

  // serailize to multimap
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    json::value jv = {{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}};

    std::multimap<std::string, Legend> result;
    json::unserialize(jv, result);

    CHECK(jv == json::serialize(result));
  }

  // serailize to unordered_map
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    json::value jv = {{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}};

    std::unordered_map<std::string, Legend> result;
    json::unserialize(jv, result);

    CHECK(result.size() == 2);
  }

  // serailize to unordered_multimap
  {
    auto legend_obj1 = legend(0);
    auto legend_obj2 = legend(1);

    json::value jv = {{"Kayle", *legend_obj1}, {"Neeko", *legend_obj2}};

    std::unordered_multimap<std::string, Legend> result;
    json::unserialize(jv, result);

    CHECK(result.size() == 2);
  }

  // serailize to custom type
  {
    auto legend_obj1 = legend(0);

    json::value jv = *legend_obj1;

    Legend result;
    json::unserialize(jv, result);
    CHECK(jv == json::serialize(result));
  }
}

struct RoundData {
  uint32_t    uid              = 0;
  uint32_t    roundid          = 0;
  uint32_t    round_begin_time = 0;
  uint32_t    round_end_time   = 0;
  int32_t     score            = 0;
  bool        abstention       = false;
  std::string match_name;
};

FORMATS_JSON_SERIALIZE_EX(RoundData, uid, "UID", roundid, "roundid", score, "score", match_name, "match_name")

struct Student {
  int    age    = 12;
  double height = 163.5;
  double weight = 63.5;
};

FORMATS_JSON_SERIALIZE(Student, age, height, weight)

TEST(JsonStaticReflection)
{
  {  // with specified key
    RoundData round_data;
    round_data.uid = 1000;

    json::value jv = json::serialize(round_data);

    CHECK(jv["UID"] == 1000);
  }

  {  // with key same as member data name
    Student student;
    student.age = 13;

    json::value jv = json::serialize(student);

    CHECK(jv["age"] == 13);
  }
}
