#include "json_test.h"

#include <formats/jsoncpp/json.h>

using namespace formats;

TEST(JsonValueAccessObjectAt)
{
  auto legend_obj = legend(0);
  {
    json::value jv(*legend_obj);

    Legend legend_copy;

    // access use at()
    legend_copy.serialno   = (uint32_t)jv.at("No").to_uint64();
    legend_copy.name       = jv.at(std::string("Name")).to_string();
    legend_copy.class_type = (ClassType)jv.at(std::string("ClassType")).to_uint64();
    legend_copy.region     = (Region)jv.at(std::string("Region")).to_uint64();
    legend_copy.position   = (Position)jv.at(std::string("Position")).to_uint64();

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);
  }

  {
    json::value const jv(*legend_obj);

    Legend legend_copy;

    // access use at()
    legend_copy.serialno   = (uint32_t)jv.at("No").to_uint64();
    legend_copy.name       = jv.at(std::string("Name")).to_string();
    legend_copy.class_type = (ClassType)jv.at(std::string("ClassType")).to_uint64();
    legend_copy.region     = (Region)jv.at(std::string("Region")).to_uint64();
    legend_copy.position   = (Position)jv.at(std::string("Position")).to_uint64();

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);
  }
}

TEST(JsonValueAccessObjectoperator)
{
  auto legend_obj = legend(0);

  {
    json::value jv(*legend_obj);

    Legend legend_copy;

    // access use operator[]
    legend_copy.serialno   = (uint32_t)jv["No"].to_uint64();
    legend_copy.name       = jv["Name"].to_string();
    legend_copy.class_type = (ClassType)jv["ClassType"].to_uint64();
    legend_copy.region     = (Region)jv[std::string("Region")].to_uint64();
    legend_copy.position   = (Position)jv[std::string("Position")].to_uint64();

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);

    auto new_value = jv["NotExisted"];
    CHECK(jv.contains("NotExisted"));
    CHECK(jv["NotExisted"].is_null());
  }

  {
    json::value const jv(*legend_obj);

    Legend legend_copy;

    // access use operator[]
    legend_copy.serialno   = (uint32_t)jv["No"].to_uint64();
    legend_copy.name       = jv["Name"].to_string();
    legend_copy.class_type = (ClassType)jv["ClassType"].to_uint64();
    legend_copy.region     = (Region)jv[std::string("Region")].to_uint64();
    legend_copy.position   = (Position)jv[std::string("Position")].to_uint64();

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);
  }
}

TEST(JsonValueAccessObjectGet)
{
  auto legend_obj = legend(0);

  {
    json::value jv(*legend_obj);

    Legend legend_copy;

    legend_copy.serialno   = jv.get("No", legend_copy.serialno);
    legend_copy.name       = jv.get("Name", legend_copy.name);
    legend_copy.class_type = (ClassType)jv.get("ClassType", legend_copy.class_type);
    legend_copy.region     = (Region)jv.get("Region", legend_copy.region);
    legend_copy.position   = jv.get("Position", legend_copy.position);

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);

    // access get by path
    legend_copy.proptrty.HP  = jv.get("Proptrty.HP", '.', 0);
    legend_copy.proptrty.MP  = jv.get("Proptrty.MP", '.', 0);
    legend_copy.proptrty.ATK = jv.get("Proptrty.ATK", '.', 0);

    CHECK_EQ(legend_copy.proptrty.HP, legend_obj->proptrty.HP);
    CHECK_EQ(legend_copy.proptrty.MP, legend_obj->proptrty.MP);
    CHECK_EQ(legend_copy.proptrty.ATK, legend_obj->proptrty.ATK);
    CHECK_EQ(jv.get("Proptrty.None", '.', "none").c_str(), "none");
  }

  {
    json::value const jv(*legend_obj);

    Legend legend_copy;

    legend_copy.serialno   = jv.get("No", legend_copy.serialno);
    legend_copy.name       = jv.get("Name", legend_copy.name);
    legend_copy.class_type = (ClassType)jv.get("ClassType", legend_copy.class_type);
    legend_copy.region     = (Region)jv.get("Region", legend_copy.region);
    legend_copy.position   = jv.get("Position", legend_copy.position);

    CHECK_EQ(jv.size(), jv.key_set<std::vector<std::string>>().size());
    CHECK_EQ(legend_copy.serialno, legend_obj->serialno);
    CHECK_EQ(legend_copy.name.c_str(), legend_obj->name.c_str());
    CHECK_EQ(legend_copy.class_type, legend_obj->class_type);
    CHECK_EQ(legend_copy.region, legend_obj->region);
    CHECK_EQ(legend_copy.position, legend_obj->position);

    // access get by path
    legend_copy.proptrty.HP = jv.get("Proptrty.HP", '.', 0);
    legend_copy.proptrty.MP = jv.get("Proptrty.MP", '.', 0);

    CHECK_EQ(legend_copy.proptrty.HP, legend_obj->proptrty.HP);
    CHECK_EQ(legend_copy.proptrty.MP, legend_obj->proptrty.MP);
  }
}

TEST(JsonValueAccessObjectIfContains)
{
  auto legend_obj = legend(0);

  {
    json::value jv(*legend_obj);

    auto name = jv.if_contains("Name");
    auto null = jv.if_contains("NotExists");

    CHECK(name);
    CHECK(!null);
  }

  {
    json::value const jv(*legend_obj);

    auto name = jv.if_contains("Name");
    auto null = jv.if_contains("NotExists");

    CHECK(name);
    CHECK(!null);
  }
}

TEST(JsonValueAccessArrayElement)
{
  auto legend_obj1 = legend(0);
  auto legend_obj2 = legend(1);

  {
    json::value js_array = {*legend_obj1, *legend_obj2};

    CHECK(!js_array.empty());
    CHECK_EQ(js_array.size(), 2);
    CHECK_EQ(js_array[0]["Name"].to_string().c_str(), legend_obj1->name.c_str());
    CHECK_EQ(js_array.at(1).get("Position", 0), legend_obj2->position);
  }

  {
    json::value const c_js_array = {*legend_obj1, *legend_obj2};

    CHECK(!c_js_array.empty());
    CHECK_EQ(c_js_array.size(), 2);
    CHECK_EQ(c_js_array[0]["Name"].to_string().c_str(), legend_obj1->name.c_str());
    CHECK_EQ(c_js_array.at(1).get("Position", 0), legend_obj2->position);
  }
}
