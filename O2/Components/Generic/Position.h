#ifndef ALI_O2_COMPONENTS_GENERIC_POSITION_H
#define ALI_O2_COMPONENTS_GENERIC_POSITION_H
#include "../Macros/Definition.h"
#include "../../ecs/Dependency.h"
namespace o2{
namespace ecs {
namespace generic {
DEFINE_COMPONENT(X, float);
DEFINE_COMPONENT(Y, float);
DEFINE_COMPONENT(Z, float);
DEFINE_COMPONENT(T, float);
class SpatialPosition : public o2::ecs::Dependency<X, Y, Z> {};
}
}
}
#endif
