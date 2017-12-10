#ifndef ALI_O2_ENITIES_VERTEX_H
#define ALI_O2_ENITIES_VERTEX_H
#include "functions/math.h"
#include "../Components/Vertex.h"
#include "../ecs/Entity.h"
namespace o2{
namespace ecs {
template <typename... Components> class Vertex : public Entity<Components...> {
public:
  static const char *Id() { return "Vertex"; }
  using ecs::Entity<Components...>::Entity;
  template <typename U = Vertex<Components...>,
            typename std::enable_if<U::template Contains<
                vertex::X, vertex::Y, vertex::Z>()>::type * = nullptr>
  auto length_xyz() const {
    auto x = this->template get<vertex::X>();
    auto y = this->template get<vertex::Y>();
    auto z = this->template get<vertex::Z>();
    return o2::sqrt(x * x + y * y + z * z);
  }
};
}
}
#endif
