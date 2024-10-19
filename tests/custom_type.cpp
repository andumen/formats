#include "custom_type.hpp"

namespace internal
{

static std::vector<Legend> legends;
};  // namespace internal

void build_custom_data_once()
{
  for (int i = 0; i < 10; ++i)
  {
    Legend legend;

    legend.serialno = 1, legend.name = "Garen";
    legend.proptrty   = {600, 400, 80.0, 10.0, 40.0, 42.0, 335.0, 0.0, 1.5, 0.67};
    legend.class_type = ClassType::Fighter;
    legend.region     = Region::Demacia;
    legend.position   = Position::TopLaner;

    internal::legends.push_back(std::move(legend));
  }
}

Legend* legend(int serialno)
{
  return &internal::legends[serialno % internal::legends.size()];
}