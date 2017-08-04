#ifndef ALI_O2_COMPONENTS_GENERIC_MOMENTUM_H
#define ALI_O2_COMPONENTS_GENERIC_MOMENTUM_H
#include "../Macros/Definition.h"
#include "../../ecs/Dependency.h"
namespace ecs {
namespace generic {
DEFINE_COMPONENT(Px, component_f);
DEFINE_COMPONENT(Py, component_f);
DEFINE_COMPONENT(Pz, component_f);
class Pt : public Dependency<Px, Py> {};
}
}
#endif
