#pragma once

#include <formats/formats.h>

using namespace formats;

struct Property {
  uint32_t HP;    // hit points
  uint32_t MP;    // magic points
  double   ATK;   // attack
  double   MATK;  // Magic Attack
  double   DEF;   // defense
  double   MDEF;  // Magic Defense
  double   SPD;   // speed
  double   CRT;   // Critical Rate
  double   CRD;   // Critical Damage
  double   ATSP;  // Attack Speed
  double   CD;    // Cool Down
  // double   CDRP;  // Cool Down Reduction Percent

  json::value to_json() const
  {
    return {{"HP", HP},   {"MP", MP},   {"ATK", ATK}, {"MATK", MATK}, {"DEF", DEF}, {"MDEF", MDEF},
            {"SPD", SPD}, {"CRT", CRT}, {"CRD", CRD}, {"ATSP", ATSP}, {"CD", CD}};
  }

  inline void from_json(const json::value& js)
  {
    HP   = js.get("HP", 0);
    MP   = js.get("MP", 0);
    ATK  = js.get("ATK", 0.0);
    MATK = js.get("MATK", 0.0);
    DEF  = js.get("DEF", 0.0);
    MDEF = js.get("MDEF", 0.0);
    SPD  = js.get("SPD", 0.0);
    CRT  = js.get("CRT", 0.0);
    CRD  = js.get("CRD", 0.0);
    ATSP = js.get("ATSP", 0.0);
    CD   = js.get("CD", 0.0);
  }
};

enum Region
{
  Demacia = 0,
  Noxus,
  Piltover,
  Freljord,
  Ionia,
  ShadowIsles,
  Shurima,
  Bilgewater,
  Zaun,
  Targon,
};

enum Position
{
  TopLaner = 1,
  Jungler,
  MidLaner,
  AttackDamage,
  Support
};

enum ClassType
{
  Mage = 0,
  Assassin,
  Tank,
  Fighter,
  Marksman,
};

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

class LegendMgr
{};

void    build_custom_data_once();
Legend* legend(int serialno);