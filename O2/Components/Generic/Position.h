#ifndef ALI_O2_COMPONENTS_GENERIC_POSITION_H
#define ALI_O2_COMPONENTS_GENERIC_POSITION_H
#include "../Macros/Definition.h"
#include "../../ecs/Dependency.h"
namespace ecs {
namespace generic {
DEFINE_COMPONENT(X, component_f);
DEFINE_COMPONENT(Y, component_f);
DEFINE_COMPONENT(Z, component_f);
DEFINE_COMPONENT(T, component_f);
class SpatialPosition : public Dependency<X, Y, Z> {};
}
}
#endif
