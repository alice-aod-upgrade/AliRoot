#ifndef ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#define ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#include "../Macros/Definition.h"
namespace ecs {
namespace vertex {
  class ESDEventMapping_t{
    uint32_t mTrackStart;
    uint32_t mTrackCount;
  public:
    uint32_t trackStart() const { return mTrackStart; }
    uint32_t trackCount() const { return mTrackCount; }
    ESDEventMapping_t(uint32_t TrackStart = 0, uint32_t TrackCount = 0)
        : mTrackStart(TrackStart), mTrackCount(TrackCount) {}
  };
  DEFINE_COMPONENT(ESDEventMapping, ESDEventMapping_t);
}
}
#endif
