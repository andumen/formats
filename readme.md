# Formats

## Introduction

The goal of this project is to build a simpler, faster and more user-friendly  parser and serializer for formatted languages.  such as json, yaml .etc.

* [Build](#Build)
  * [CMake File](#CMake-File)
  * [CMake Option](#CMake-Option)
* [Usage](#Usage)
  * [parse](#parse)
    * [Read from string or file](#Read-from-string-or-file)
    * [Parse support](#Parse-support)
  * [stringify](#stringify)
    * [stringify to string](#stringify-to-string)
    * [stringify to stream](#stringify-to-string)
    * [stringify support](#stringify-support)
  * [serializer](#serialize/unserialize)
    * [STL](#STL-Container)
    * [Custom Type](#Custom-Type)
  * [json value](#value)
    * [Types](#DataType-of-Json-Value)
    * [Construct](#Construct)
    * [Access](#Access)
    * [Capacity](#Capacity)
    * [Modify](#Modify)
* [Compiler Support](#Compiler-Support)
* [Reference](#Reference)

## Build

### CMake File

You can use this library from a cmake project, with ``find_package`` to load it. The CMakeLists.txt file content may be:

```cmake
project(myapp LANGUAGES CXX)

find_package(formats REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

if(FORMATS_FOUND)
	include_directories(${FORMATS_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${FORMATS_LIBRARY})
endif()
```



### CMake Option 

Here are some CMake options that might be helpful to you: 

| Formats                       | Description                                    | Default |
| ----------------------------- | ---------------------------------------------- | ------- |
| LINK_STATIC_RUNTIME_LIBRARIES | msvc with static runtime library for compilers | OFF     |
| WITH_TESTS                    | build with formats tests project               | OFF     |

* -DLINK_STATIC_RUNTIME_LIBRARIES=ON

  Flags for c++ compiler, compile with static runtime library for compilers

* -DWITH_TESTS=ON

  build option, build with formats tests project



Finally, your cmake command may be like this:

```
mkdir build
cd build
cmake .. [-G generator] [-DWITH_TESTS=ON|OFF] [-DLINK_STATIC_RUNTIME_LIBRARIES=ON|OFF]
```



## Usage

In your self project, only header files ``formasts.h`` are required.  like:

```c++
#include <formats/formasts.h>
using namespace formats;
```



### parse

#### Read from string or file

There are three APIs available for use; you can choose according to your needs. 



**The firstly, parse string and return parse result directly**

```c++
#include <formats/formats.h>

#include <fstream>

using namespace formats;
int main() {
  {  // from string
    std::string s("");

    json::value json_value;
    if (json::parse(json_value, s.c_str(), s.size())) {
      // do something
    }
  }

  {  // from file
    std::string file_path("/data/test.json");
    std::ifstream is;
    is.open(file_path, std::ios::in | std::ios::binary);

    if (is.is_open()) {
      json::value json_value;
      if (json::parse(json_value, is)) {
        // do something
      }
    }
  }

  return 0;
}
```



**The Second, retunr json::value directly with parse error info.**

```c++
#include <formats/formats.h>

#include <fstream>

using namespace formats;
int main() {
  {  // from string
    std::string s("");

    json::error json_error;
    json::value json_value = json::parse(s.c_str(), s.size(), json_error);
    if (!json_error) {
      // do someting
    } else {
      printf("parse failed. error: %d:%s", json_error.code(), json_error.what());
    }
  }

  {  // from file
    std::string file_path("/data/test.json");
    std::ifstream is;
    is.open(file_path, std::ios::in | std::ios::binary);

    if (is.is_open()) {
      json::error json_error;
      json::value json_value = formats::json::parse(is);

      if (!json_error) {
        // do someting
      }
    }
  }

  return 0;
}
```





**The Third, return json::value directly** 

If you're not concerned with the type of error, use this one. The default behavior don't throw error when

parse failed. **If you want throws error, define preprocessor THROW_PARSE_ERROR**

```c++
#include <formats/formats.h>
#include <fstream>

using namespace formats;
int main() {
  {  // from string
    std::string s("");

    json::value json_value = json::parse(s.c_str(), s.size());
    if (json_value.type() != json::value_type::error) {
      // do someting
    }
  }

  {  // from file
    std::string file_path("/data/test.json");
    std::ifstream is;
    is.open(file_path, std::ios::in | std::ios::binary);

    if (is.is_open()) {
      json::value json_value = formats::json::parse(is);

      if (json_value.type() != json::value_type::error) {
        // do someting
      }
    }
  }

  return 0;
}
```



#### Parse support

The default standard which parser use is **ECMA404**, if you want use some extension in your json,  Specify  the flags in parse function. Parse flag is bit-or combination of the possible flags-enum.

The parser supports the following options: 

```c++
enum class parse_flag : unsigned long long
{
  ECMA404 = 0X01,
  strict  = ECMA404,

  object_trailing_comma = strict << 1,  // support trailing comma in object
  array_trailing_comma  = strict << 2,  // support trailing comma in array
  trailing_comma        = object_trailing_comma | array_trailing_comma,

  string_single_quotes = strict << 3,  // support single quoted string
  string_unquoted      = strict << 4,  // support unquoted string
  string_multi_lines   = strict << 5,  // support line break in string

  num_leading_point = strict << 6,   // support .123 parse to 0.123
  num_ending_point  = strict << 7,   // support 123. parse to 123.00
  num_hexadecimal   = strict << 8,   // support hexadecimal number
  num_infinity      = strict << 9,   // support plus or minus Infinity
  num_nan           = strict << 10,  // support NaN

  escaped_hex  = strict << 11,  // \x00-\xFF. \x0c parse to 12
  escaped_null = strict << 12,  // \0 parse to null

  comments     = strict << 13,  // single or multi comments are permitted
  white_space  = strict << 14,  // white space are permitted.U+00A0 U+2028 U+2029 U+FEFF
  root_lenient = strict << 15,  // single value as a json is permitted

  JSON5 = ((strict << 16) - 1),  // ECMAScript 5.1

  illegal_utf8 = JSON5 + 1,
};
```



use like this:

```c++
#include <formats/formats.h>

#include <fstream>

using namespace formats;
int main() {
  {  // from string
    std::string s("");

    json::parse_flag flag = json::parse_flag::JSON5;	// Specify json5

    json::error json_error;
    json::value json_value = json::parse(s.c_str(), s.size(), json_error, flag);
    if (!json_error) {
      // do someting
    } else {
      printf("parse failed. error: %d:%s", json_error.code(), json_error.what());
    }
  }

  return 0;
}
```

### stringify

#### stringify to string

There are three stringify style to select.

**compact** : Minimize output without any additional spaces or line breaks

**pretty** : add indentation and line breaks to make JSON data easier to read

**standard** : like pretty, but single line array

```c++
json::value json_value;

auto s1 = json::stringify(json_value, json::stringify_style::pretty);
auto s2 = json::stringify(json_value, json::stringify_style::standard);
auto s3 = json::stringify(json_value, json::stringify_style::compact);

// member fn
json_value.dump();  // pretty style
```

#### stringify to stream

Wher output stream, the **standard style** is used.

```c++
json::value json_value;
std::cout << "json: " << json_value << std::endl;	// standard style
```



#### stringify support

Like parse flags, This project also provides stringify flags to select. Stringify flag is bit-or combination of the possible flags-enum. As the third parameter of stringfiy function. The default flag value is strict, corresponding json standards ECMA404.

```c++
enum class stringify_flag : unsigned char
{
  strict = 0X01,

  write_single_quotes_string = strict << 1,
  write_unquoted_string      = strict << 2,
  write_nan_number           = strict << 3,
  write_infinity_num         = strict << 4,
};
```



### serialize/unserialize

***

* `serialize`: serialize specified type value to json value
* `unserialize`: unserialize a json value to specified type

***

#### STL Container

example:

```c++
json::value jv1 = json::serialize(std::vector<bool>({ false, true, true }));
json::value jv2 = json::serialize(std::list<int>({ 1, -2, -3, 4 }));
json::value jv3 = json::serialize(std::set<int>({ 1, -2, -3, 4 }));
json::value jv4 = json::serialize(std::map<std::string, int>({ {"Aatrox", 0}, {"Fizz", 1} }));

std::vector<bool> vector;
json::unserialize(jv1, vector);

std::deque<int> deque;
json::unserialize(jv2, deque);

std::unordered_set<int> unordered_set;
json::unserialize(jv3, unordered_set);

std::unordered_map<std::string, int> unordered_map;
json::unserialize(jv4, unordered_map);

// multiset multimap... 
```



#### Custom Type

To make serializer work with one of your types, there three ways you can select:

**1. Provide member functions of your type**

Only you need do is to provide member functions(`to_json` and `from_json`)

 example:

```c++
struct Legend {
  uint32_t    serialno;
  std::string name;

  ClassType class_type;
  Region    region;
  Position  position;
  Property  proptrty;

  json::value to_json() const
  {
    return {
        {"No", serialno},   {"Name", name},         {"ClassType", class_type},
        {"Region", region}, {"Position", position}, {"Proptrty", json::serialize(proptrty)},
    };
  }

  inline void from_json(const json::value& jv)
  {
    this->serialno   = jv.get("No", 0);
    this->name       = jv.get("Name", "undefined");
    this->class_type = jv.get("ClassType", ClassType::Assassin);
    this->region     = jv.get("Region", Region::Bilgewater);
    this->position   = jv.get("Position", Position::TopLaner);

    json::unserialize(jv["Proptrty"], this->proptrty);
  }
};

Legend legend;
json::value jv = json::serialize(legend);
json::unserialize(jv, legend);
```

**2. Specialization adl_serializer<T>**

Only you need do is to specialization adl_serializer (`to_json` and `from_json`)

example:

```c++
struct Equipment {
  std::string name;
  std::string desc;

  ClassType class_type;
  Property  property;
};

template <>
struct formats::adl_serializer<Equipment> {
  inline static json::value to_json(const Equipment& equipment)
  {
    return {
        {"Name", equipment.name},
        {"Desc", equipment.desc},
        {"ClassType", equipment.class_type},
        {"Property", json::serialize(equipment.property)},
    };
  };

  inline void from_json(const json::value& js, Equipment& equipment)
  {
    equipment.name       = js.get("Name", equipment.name);
    equipment.desc       = js.get("Desc", equipment.desc);
    equipment.class_type = js.get("ClassType", equipment.class_type);
    json::unserialize(js["Property"], equipment.property);
  }
};

Equipment equipment;
json::value jv = json::serialize(equipment);
json::unserialize(jv, equipment);
```

**3. Reflection with marco**

There are two marcos to make it work.

* `FORMATS_JSON_SERIALIZE(Struct, member1, member2, ...)` 

  to specialization adl_serializer, in the return json object value, **member key is equal to member name**

  

* `FORMATS_JSON_SERIALIZE_EX(Struct, member1, member1_key_name,  member2, member2_key_name, ...)` 

  to specialization adl_serializer, in the return json object value, **member key is defined by marco parameter**



example:

```c++
struct RoundData {
  uint32_t    uid              = 0;
  uint32_t    roundid          = 0;
  uint32_t    round_begin_time = 0;
  uint32_t    round_end_time   = 0;
  int32_t     score            = 0;
  bool        abstention       = false;
  std::string match_name;
};

// with specified key
FORMATS_JSON_SERIALIZE_EX(RoundData, uid, "UID", roundid, "roundid", score, "score", match_name, "match_name");

RoundData round_data;
round_data.uid = 1000;
json::value jv = json::serialize(round_data);
CHECK(jv["UID"] == 1000);

struct Student {
  int    age    = 12;
  double height = 163.5;
  double weight = 63.5;
};

 // with key same as member data name
FORMATS_JSON_SERIALIZE(Student, age, height, weight);
Student student;
student.age = 13;
json::value jv = json::serialize(student);
CHECK(jv["age"] == 13);
```





### value

The value is the core class of the project. This library provides simple access and modification APIs for this class. Next, we will provide a detailed introduction to this category

#### DataType of Json Value

The following are json data types:

```c++
enum class value_type : char
{
  error = 0,     // error. only has effect when parse failed.
  null,          // null value. corresponding nullptr
  boolean,       // true or false
  number_int,    // the signed integer, long long.
  number_uint,   // the signed integer, unsigned long long
  number_float,  // double
  string,        // string value
  array,         // array value
  object,        // ordered map
};
```



#### Construct

##### From Json Value Type

```c++
json::value json_value = json::value_type::array;
```

##### From Scaler value

```c++
json::value json_value_null = nullptr;
json::value json_value_int = 1;
json::value json_value_uint = (unsigned)1;
json::value json_value_d = 1.0;
json::value json_value_str = "";
```



##### From Initializelist

```c++
json::value jv_array = {false, 1, 2.16, "", nullptr};
json::value jv_object = {{"one", 1}, {"two", 2}};
```



##### From Struct Data(STL)

```c++
// to json array value
std::array<std::nullptr_t, 1> arr = {nullptr};
json::value jv_array1(arr);

std::vector<bool> vector = {false, true, true};
json::value jv_array2(vector);

std::list<int> list = {1, -2, -3, 4};
json::value jv_array3(list);

std::deque<float> deque = {1, 2, 3, 4};
json::value jv_array4(deque);

std::forward_list<uint32_t> forward_list = {1, 1000};
json::value jv_array5(forward_list);

std::set<int> set = {1, 2, 3, 4};
json::value jv_array6(set);

std::multiset<std::string> multiset = {"A", "B", "C"};
json::value jv_array7(multiset);

std::unordered_set<int> unordered_set = {1, 2, 3, 4};
json::value jv_array8(unordered_set);
std::unordered_multiset<std::string> unordered_multiset = {"hello", "world"};
json::value jv_array9(unordered_multiset);

// to json object value
std::pair<std::string, int> pair = {"AAA", 1};
json::value jv_object1(pair);

std::map<std::string, int> map = {{"Aatrox", 0}, {"Fizz", 1}};
json::value jv_object2(map);
std::multimap<const char*, bool> multimap = {{"Aatrox", true}, {"Fizz", false}};
json::value jv_object3(multimap);
std::unordered_multimap<std::string, int> unordered_multimap = {{"Gnar", 2},
                                                                {"Lillia", -3}};
json::value jv_object4(unordered_multimap);
```



#### Access

##### value accessors

| Type         | type checker | pointer access | checked access | convert access |
| ------------ | ------------ | -------------- | -------------- | -------------- |
| null         | is_null      | **None**       | **None**       | **None**       |
| boolean      | is_bool      | if_bool        | as_bool        | to_bool        |
| number_int   | is_int64     | if_int64       | as_int64       | to_int64       |
| number_uint  | is_uint64    | if_uint64      | as_uint64      | to_uint64      |
| number_float | is_double    | if_double      | as_double      | to_double      |
| string       | is_string    | if_string      | as_string      | to_string      |
| array        | is_array     | if_array       | as_array       | **None**       |
| object       | is_object    | if_object      | as_object      | **None**       |



**Description**

* `if_type`:  check this value is certain type
* `if_type`:  return a pointer if this is certain type, else return nullptr
* `as_type`:  return a reference to the underlying value if this is certain type, else throw an exception
* `to_type`:  return value if convert this value to certain type success, else dflt



example:

```c++
json::value json_value = 1;

auto b = json_value.is_int64();   // true
auto& i = json_value.as_int64();  // i is reference to underlying value
auto s = json_value.to_string();  // s is "1"

if (auto p = json_value.if_string()) {
  // p is nullptr, unreachable
}
```



##### array element value

**Random Access By Position**

***

* `operator[]`: return reference to the value of the position, throw an exception if position out of range
* `at`: return reference to the value of the position, throw an exception if position out of range

***

example:

```c++
json::value jv_array = {false, 1, 2.16, "", nullptr};

auto jv1 = jv_array[0];     // jv1 is reference to jv_array first element
auto jv2 = jv_array.at(1);  // jv2 is reference to jv_array third element
```



**Traverse**

example:

```c++
json::value jv_array = {false, 1, 2.16, "", nullptr};

// 1 range-based for loop
for (auto& ele : jv_array) {}

// 2 iterator
auto iter = jv_array.begin();
while (iter != jv_array.end()) ++iter;
```



##### object element value

**Access By Key**

***

* `operator[]`: return reference to the value of specified key, if not exists, insert at end
* `at`: return reference to the value of specified key, if not exists, throw an exception
* `get`: if  the value of specified key exists, convert to specified and return, otherwise, dflt

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

auto& jv1 = jv["one"];     // jv1 is reference to element with key one
auto& jv2 = jv.at("two");  // jv2 is reference to element with key two

auto i = jv.get("one", (int)0);   // get by key
auto s = jv.get("AAA|one", '|', std::string("")); // get by key path, separate by |
```



**Access Pointer With Check** 

***

* `contains`: check the element with specified key exists
* `if_contains`: return the the pointer if element with specified key exists, otherwise, nullptr

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

bool b = jv.contains("None");  // false
if (auto p = jv.if_contains("one")) {
  auto i = p->to_int64();
}
```



**Traverse**

example:

```c++
json::value jv_object = {{"one", 1}, {"two", 2}};

// 1 range-based for loop
for (auto& [key, value] : jv_object.as_object()) {}
```



**Access Keys**

***

* `key_set`: return sub keys of object value

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

auto keys1 = jv.key_set<std::vector<std::string>>();
auto keys2 = jv.key_set<std::set<std::string>>();
```



#### Capacity

***

* empty: checks if the value has no elements. if scaler, return true
* size: returns the number of elements in the value. if scaler, return 0

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

bool empty = jv.empty();  // empty = false
auto size = jv.size();    // size = 3
```



#### Modify

##### Change Value

***

* `emplace_type`: repalce the value whit specified type initialized to default value, return the reference of the underlying content
* `swap`: exchanges the contents of this value with those of other
* `clear`: reset to default value. don't change type

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

jv.emplace_null();                // jv is null type
auto& b = jv.emplace_bool();      // jv is bool
auto& i = jv.emplace_int64();     // jv is int64_t
auto& u = jv.emplace_uint64();    // jv is uint64_t
auto& d = jv.emplace_double();    // jv is double
auto& s = jv.emplace_string();    // jv is string
auto& arr = jv.emplace_array();   // jv is array
auto& obj = jv.emplace_object();  // jv is object

jv.swap(json::value({1, 2, 3}));  // jv is array
jv.clear();                       // jv is array
```



##### Object Modifier

***

* `emplace`: inserts a new element into this constructed in-place with the given args, if there is no element with the key in the container.
* `erase`: removes specified elements from the object value.
* `merge`: extract ("splice") each element in other and insert it into this. If this has no key of an element in other, move it from other to end of this.

***

example:

```c++
json::value jv = {{"one", 1}, {"two", 2}, {"AAA", {{"one", 1}, {"two", 2}}}};

jv.emplace("three", 3);
jv.erase("three");
jv.merge(json::value({{"three", 3}}));
```



##### Array Modifier

***

* `emplace_back`: appends a new element to the end of this. The element is constructed through args
* `push_back`: Appends the given element value to the end of the this
* `insert`: Inserts elements at the specified location in this.
* `erase`: Erases the specified position or element from the this.

***

example:

```c++
json::value jv;

jv.emplace_back(1);            // [1]
jv.push_back(json::value(2));  // [1, 2]

jv.insert(jv.begin(), 0);   // [0, 1, 2]
jv.insert(jv.end(), 1, 3);  // [0, 1, 2, 3]

json::value jv_other = {4, 5};
jv.insert(jv.end(), jv_other.begin(), jv_other.end());  // [0, 1, 2, 3, 4 ,5]
jv.insert(jv.end(), {6, 7, 8});  // [0, 1, 2, 3, 4 ,5, 6, 7, 8]

jv.erase(0);
jv.erase(jv.begin()); // [2, 3, 4 ,5, 6, 7, 8]
```



## Compiler Support

The test project has been compiled and run successfully in the following environments: 

| Compiler           | Operating System     |
| ------------------ | -------------------- |
| GCC 13.2.0         | Ubuntu 24.04.01      |
| GCC 12.3.0         | Ubuntu 24.04.01      |
| GCC 11.4.0         | Ubuntu 24.04.01      |
| GCC 10.5.0         | Ubuntu 24.04.01      |
| GCC 9.5.0          | Ubuntu 24.04.01      |
| GCC 9.4.0          | Ubuntu 20.04.06      |
| GCC 8.4.0          | Ubuntu 20.04.06      |
| GCC 7.5.0          | Ubuntu 20.04.06      |
| GCC 6.4.0          | Ubuntu 20.04.06      |
| GCC 5.5.0          | Ubuntu 20.04.06      |
| Visual Studio 2015 | Windows 10.0.17763.0 |
| Visual Studio 2017 | Windows 10.0.17763.0 |
| Visual Studio 2019 | Windows 10.0.17763.0 |
| Visual Studio 2022 | Windows 10.0.17763.0 |
| Xcode 13.4.1       | macOS 12.4 M2        |
|                    |                      |
|                    |                      |



## Reference

[GitHub - nlohmann/json: JSON for Modern C++](https://github.com/nlohmann/json?tab=readme-ov-file#supported-compilers) 

[value - 1.75.0 (boost.org)](https://www.boost.org/doc/libs/1_75_0/libs/json/doc/html/json/dom/value.html) 

[GitHub - google/double-conversion: Efficient binary-decimal and decimal-binary conversion routines for IEEE doubles.](https://github.com/google/double-conversion/tree/master) 