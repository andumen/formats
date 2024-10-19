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

//TEST(JsonValueParseFromFiles)
//{
//  int64_t total_time = 0;
//
//  auto h = [&total_time](std::filesystem::path& dest_file, const std::string& my_json_path) {
//    std::ifstream is;
//    is.open(my_json_path, std::ios::in | std::ios::binary);
//
//    if (is.is_open())
//    {
//      auto now_begin = std::chrono::steady_clock::now().time_since_epoch();
//      auto ts_begin  = std::chrono::duration_cast<std::chrono::milliseconds>(now_begin).count();
//
//      formats::json::parse_flag parse_flag = formats::json::parse_flag::ECMA404;
//      formats::json::error      json_error;
//      formats::json::value      json_value = formats::json::parse(is, json_error, parse_flag);
//
//      auto now_end = std::chrono::steady_clock::now().time_since_epoch();
//      auto ts_end  = std::chrono::duration_cast<std::chrono::milliseconds>(now_end).count();
//
//      total_time += ts_end - ts_begin;
//      printf("use: %lld, file: %s\n\n", ts_end - ts_begin, my_json_path.c_str());
//
//      if (json_error.code() != formats::json::error_code::none)
//      {
//        printf("Faield. \n File: %s\n error:%s\n", my_json_path.c_str(), json_error.what());
//        return;
//      }
//
//      auto json_string =
//          formats::json::stringify(json_value, formats::json::stringify_style::pretty);
//
//      // write
//      std::ofstream out;
//      out.open(dest_file.string(), std::ios::out | std::ios::binary);
//      if (out.is_open())
//      {
//        out << json_string;
//        out.close();
//      }
//
//      // LOGX(Info, "\n\n%s: \n%s", my_json_path.c_str(), json_string.c_str());
//    }
//
//    is.close();
//  };
//
//  /*auto my_json_dir        = app_path().parent_path().append("json_dataset_compact");
//  auto my_json_result_dir = app_path().parent_path().append("json_dataset_result");*/
//
//  /*std::filesystem::path
//  input_dir("/Users/l/WorkSpace/project/formats-test/formats-test-data/json"); std::filesystem::path
//  output_dir(
//      "/Users/l/WorkSpace/project/formats-test/formats-test-data-result/json");*/
//
//  std::filesystem::path input_dir(
//      "D:\\Self\\gitee\\LS\\bin\\All-x64-MSVC-2019\\Debug\\runtime\\json_dataset");
//  std::filesystem::path output_dir(
//      "D:\\Self\\gitee\\LS\\bin\\All-x64-MSVC-2019\\Debug\\runtime\\json_dataset_result");
//
//  if (!std::filesystem::exists(output_dir)) std::filesystem::create_directory(output_dir);
//
//  for (auto& entry : std::filesystem::directory_iterator(input_dir))
//  {
//    if (entry.is_directory())
//    {
//      auto dest_file_dir = output_dir;
//      dest_file_dir.append(entry.path().filename().string());
//
//      if (!std::filesystem::exists(dest_file_dir)) std::filesystem::create_directory(dest_file_dir);
//
//      for (auto& entry2 : std::filesystem::directory_iterator(entry.path()))
//      {
//        if (entry2.is_regular_file())
//        {
//          auto dest_file = dest_file_dir;
//          dest_file.append(entry2.path().filename().string());
//
//          h(dest_file, entry2.path().string());
//        }
//      }
//    }
//  }
//
//  printf("use total time: %lld. \n\n", total_time);
//}
