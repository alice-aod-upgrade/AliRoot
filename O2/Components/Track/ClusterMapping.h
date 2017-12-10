#ifndef ALI_O2_COMPONENTS_TRACK_CLUSTER_MAPPING_H
#define ALI_O2_COMPONENTS_TRACK_CLUSTER_MAPPING_H

#include "../Macros/Definition.h"
namespace o2{
namespace ecs {
namespace track {
DEFINE_ONE_TO_MANY_COMPONENT(ClusterMapping, int32_t);
}
}
}
#endif
