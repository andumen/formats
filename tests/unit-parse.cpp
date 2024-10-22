#include <fstream>
#include <filesystem>

#include "json_test.h"
#include "custom_type.hpp"

using namespace formats;

TEST(JsonValueParseFromString)
{
  // parse from char pointer
  {
    char buffer[64] = "{\"one\": 1}\0";

    auto ret = json::parse(buffer);
    CHECK(!ret.is_error());
  }

  {
    std::string s = "[0, 1, \"2\", null, false]";

    auto jv = json::parse(s.c_str(), s.size());

    CHECK(jv.kind() == json::kind::array);
    CHECK(jv.size() == 5);
    CHECK(jv[2] == "2");
  }

  {
    json::value jv_source = json::kind::object;
    jv_source["one"]      = 1;
    jv_source["two"]      = 2;
    jv_source["ten"]      = 10;

    auto s          = json::stringify(jv_source);
    s[s.size() - 1] = '!';

    json::error json_error;

    auto jv = json::parse(s.c_str(), s.size(), json_error);

    CHECK(jv.is_error());
    CHECK(json_error.code() != json::error_code::none);
  }

  // parse json5
  {
    std::string s = "[0, 1, \"2\", null, false,]";

    auto jv = json::parse(s.c_str(), s.size(), json::parse_flag::JSON5);

    CHECK(jv.kind() == json::kind::array);
    CHECK(jv.size() == 5);
    CHECK(jv[2] == "2");
  }
}

TEST(JsonValueParseFromFile)
{
  std::string filepath = "";
  std::string destfile = "";

  json::value jv;
  if (!json::load(filepath, jv))
  {
    printf("Faield. \n File: %s\n", filepath.c_str());
    return void();
  }

  json::dump(destfile, jv, json::stringify_style::pretty);
}

TEST(JsonValueParseFromJson5File)
{
  std::string filepath = "";
  std::string destfile = "";

  json::value jv;
  if (!json::load(filepath, jv, json::parse_flag::JSON5))
  {
    printf("Faield. \n File: %s\n", filepath.c_str());
    return void();
  }

  json::dump(destfile, jv, json::stringify_style::pretty);
}