#ifndef ALI_O2_ENITIES_TRACK_H
#define ALI_O2_ENITIES_TRACK_H
#include "../Components/Track.h"
#include "../ecs/Entity.h"
#include "../ecs/EntityCollection.h"
#include <cmath>

namespace ecs {
template<typename Getter>
struct TrackFunctions : public Getter{
  using Getter::Getter;

  template <typename U = Getter, typename std::enable_if<U::template Contains<
                track::X, track::Y, track::Z>()>::type * = nullptr>
  auto length_xyz() const {
    auto x = this->template get<track::X>();
    auto y = this->template get<track::Y>();
    auto z = this->template get<track::Z>();
    return sqrt(x * x + y * y + z * z);
  }
  template <typename U = Getter, typename std::enable_if<U::template Contains<track::Px, track::Py>()>::type * = nullptr>
  auto pt() const {
    auto px = this->template get<track::Px>();
    auto py = this->template get<track::Py>();
    return sqrt(px * px + py * py);
  }
  template <typename U = Getter, typename std::enable_if<U::template Contains<
                track::mc::MonteCarloIndex>()>::type * = nullptr>
  auto mcLabel() const {
    return this->template get<track::mc::MonteCarloIndex>();
  }
  template <typename U = Getter, typename std::enable_if<U::template Contains<
                track::mc::AmbiguousClustersFlag>()>::type * = nullptr>
  auto containsAmbigousClusters() const {
    return this->template get<track::mc::AmbiguousClustersFlag>();
  }
};

template <typename... Components> class Track : public TrackFunctions<Entity<Components...>> {
public:
  static const char *Id() { return "Track"; }
  using TrackFunctions<Entity<Components...>>::TrackFunctions;
};

template <typename... Components>
class EntityCollection< Track<Components...> > : public TrackFunctions< EntityCollectionBase<Track<Components...>>>{
public:
  using TrackFunctions<EntityCollectionBase<Track<Components...>>>::TrackFunctions;
};


}

#endif
