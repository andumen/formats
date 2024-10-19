
#include <formats/formats.h>

#include "json_test.h"
#include "custom_type.hpp"

using namespace formats;

TEST(ConstructFromCustom)
{
  Legend legend;
  legend.name       = "Kayle";
  legend.class_type = ClassType::Assassin;

  json::value js_legend(legend);
  CHECK(js_legend["Name"] == "Kayle");

  Equipment equipment;
  equipment.name = "Frostfang";

  json::value js_equipment(equipment);
  CHECK(js_equipment["Name"] == "Frostfang");
}