#ifndef ALI_O2_ENITIES_PARTICLE_H
#define ALI_O2_ENITIES_PARTICLE_H
#include "functions/math.h"
#include "../Components/Particle.h"
#include "../ecs/EntityCollection.h"
#include "../ecs/Entity.h"
#include <cmath>

namespace ecs {
  template<typename Getter>
  struct ParticleFunctions : public Getter{
    using Getter::Getter;

    template <typename U = Getter, typename std::enable_if<U::template Contains<
                  particle::X, particle::Y, particle::Z>()>::type * = nullptr>
    auto length_xyz() const {
      auto x = this->template get<particle::X>();
      auto y = this->template get<particle::Y>();
      auto z = this->template get<particle::Z>();
      return o2::sqrt(x * x + y * y + z * z);
    }
    template <typename U = Getter, typename std::enable_if<U::template Contains<particle::Px, particle::Py>()>::type * = nullptr>
    auto pt() const {
      auto px = this->template get<particle::Px>();
      auto py = this->template get<particle::Py>();
      return o2::sqrt(px * px + py * py);
    }
  };

  template <typename... Components> class Particle : public ParticleFunctions<Entity<Components...>> {
  public:
    static const char *Id() { return "Particle"; }
    using ParticleFunctions<Entity<Components...>>::ParticleFunctions;
  };

  template <typename... Components>
  class EntityCollection< Particle<Components...> > : public ParticleFunctions< EntityCollectionBase<Particle<Components...>>>{
  public:
    using ParticleFunctions<EntityCollectionBase<Particle<Components...>>>::ParticleFunctions;
  };
}

#endif
