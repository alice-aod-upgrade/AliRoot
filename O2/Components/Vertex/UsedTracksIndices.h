#ifndef ALI_O2_COMPONENENTS_VERTEX_USED_TRACKS_INDICES_H
#define ALI_O2_COMPONENENTS_VERTEX_USED_TRACKS_INDICES_H
#include "../Macros/Definition.h"
namespace o2{
namespace ecs {
namespace vertex {
  DEFINE_ONE_TO_MANY_COMPONENT(UsedTracksIndices, int32_t);
}
}
}
#endif
