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

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 5);
    CHECK(jv[2] == "2");
  }

  {
    json::value jv_source = json::value_type::object;
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

    CHECK(jv.type() == json::value_type::array);
    CHECK(jv.size() == 5);
    CHECK(jv[2] == "2");
  }
}

TEST(JsonValueParseFromFile)
{
  std::string filepath  = "";
  std::string dest_file = "";

  std::ifstream is;
  is.open(filepath, std::ios::in | std::ios::binary);

  if (is.is_open())
  {
    formats::json::parse_flag parse_flag = formats::json::parse_flag::strict;
    formats::json::error      json_error;
    formats::json::value      json_value = formats::json::parse(is, json_error, parse_flag);

    if (json_error.code() != formats::json::error_code::none)
    {
      printf("Faield. \n File: %s\n error:%s\n", filepath.c_str(), json_error.what());

      return;
    }

    auto json_string = formats::json::stringify(json_value, formats::json::stringify_style::pretty);

    // write
    std::ofstream out;
    out.open(dest_file, std::ios::out | std::ios::binary);
    if (out.is_open())
    {
      out << json_string;
      out.close();
    }
  }
}

TEST(JsonValueParseFromJson5File)
{
  std::string filepath  = "";
  std::string dest_file = "";

  std::ifstream is;
  is.open(filepath, std::ios::in | std::ios::binary);

  if (is.is_open())
  {
    formats::json::parse_flag parse_flag = formats::json::parse_flag::JSON5;
    formats::json::error      json_error;
    formats::json::value      json_value = formats::json::parse(is, json_error, parse_flag);

    if (json_error.code() != formats::json::error_code::none)
    {
      printf("Faield. \n File: %s\n error:%s\n", filepath.c_str(), json_error.what());

      return;
    }

    auto json_string = formats::json::stringify(json_value, formats::json::stringify_style::pretty);

    // write
    std::ofstream out;
    out.open(dest_file, std::ios::out | std::ios::binary);
    if (out.is_open())
    {
      out << json_string;
      out.close();
    }
  }
}
