#ifndef ALI_O2_COMPONENTS_GENERIC_MOMENTUM_H
#define ALI_O2_COMPONENTS_GENERIC_MOMENTUM_H
#include "../Macros/Definition.h"
#include "../../ecs/Dependency.h"
namespace o2{
namespace ecs {
namespace generic {
DEFINE_COMPONENT(Px, float);
DEFINE_COMPONENT(Py, float);
DEFINE_COMPONENT(Pz, float);
class Pt : public o2::ecs::Dependency<Px, Py> {};
}
}
}
#endif
