#ifndef ECS_ENTITY_COLLECTION_H
#define ECS_ENTITY_COLLECTION_H
#include "Handler.h"
#include <compute_arrays.hpp>
#include "VariableComponent.h"
#include "extended_type_traits.h"
#include <exception>
#include <iostream>
#include <stddef.h>
#include <tuple>
namespace ecs {
/// EntityCollectionInner is an EntityCollection which deals only in pure
/// Entity<...> objects. Because these will be used as basetypes for the user
/// defined entities this helps deal type conflicts.
/// To be more precise: the Entity class has a reference to it's container. It
/// is unaware of the whatever type the user wrapped the Entity in  (e.g. Track
/// : Entity<...>). The collection is aware. While it could be made unaware by
/// having handler return an oblivious Collection This would make a
/// EntityCollectin<Vertex<X>>  the same as a EntityCollection<Track<X>>, this
/// workaround maintains a clear distinction of types and lets the user directly
/// use typedefs for their entites in declaring variables.
///
template <typename Entity> class EntityCollectionInner {
protected:
  uint64_t mSize = 0;
  uint64_t mOffset = 0;
  /// how many entities there are
  uint64_t mCount = 0;
  /// data pointer for each component of the entity. Direct Component* for basic
  /// components, special structure for N->M mapped components (array of index,
  /// size pairs for each entity followed by the array of component values)
  void *mComponentCollections[Entity::Size];

public:

  using ComponentsTuple = typename  Entity::ComponentsTuple;
  constexpr static size_t Size =  Entity::Size;

  // IndexOf returns the index of a component in the component list
  template <typename G, typename std::enable_if<tuple_contains<
                            G, ComponentsTuple>::value>::type * = nullptr>
  constexpr static int IndexOf() {
    return Entity::template IndexOf<G>();
  }

  // define type<N> as the type of the N'th component
  template <unsigned N,
            typename std::enable_if<N<Size>::type * = nullptr> struct TypeOf {
    using type = typename Entity::template TypeOf<N>::type;
  };

  // Check if this entity contains all off the components <head, Rest...>
  template <typename Head, typename... Rest,
            typename std::enable_if<0 != sizeof...(Rest)>::type * = nullptr>
  constexpr static bool Contains() {
    return Entity::template Contains<Head, Rest...>();
  }
  // Specialization to stop recursion
  template <typename Head> constexpr static bool Contains() {
    return Entity::template Contains<Head>();
  }
  // Checks if none of the types in <Head, Rest...> are included.
  template <typename Head, typename... Rest,
            typename std::enable_if<0 != sizeof...(Rest)>::type * = nullptr>
  constexpr static bool DoesntContain() {
    return Entity::template DoesntContain<Head,Rest...>();
  }
  // Specialization to stop recursion
  template <typename Head> constexpr static bool DoesntContain() {
    return Entity::template DoesntContain<Head>();
  }

  // Get a pointer to the component array based on type.
  template <
      typename C, int index = Entity::template IndexOf<C>(),
      typename std::enable_if<!std::is_base_of<IVariableComponent, C>::value &&
                              (index >= 0)>::type * = nullptr>
  constexpr compute_arrays::Slice<typename C::Type> get() const {
    auto dataptr = (typename C::Type*)  mComponentCollections[index];
    return compute_arrays::Slice<typename C::Type>(dataptr+mOffset, mSize);
  }
  // Same but for N->M, returns a pair of pointers. The first one points to an
  // array of <index, size> and the second points to the component data. The
  // index is into the second array.
  template <
      typename C, int index = Entity::template IndexOf<C>(),
      typename std::enable_if<std::is_base_of<IVariableComponent, C>::value &&
                              (index >= 0)>::type * = nullptr>
  std::pair<uint64_t *, typename C::Type *> get() const {
    return std::make_pair(
        (uint64_t *)mComponentCollections[index],
        (typename C::Type *)((uint64_t *)mComponentCollections[index] + 2 * mCount));
  }
  size_t size() const { return mSize; }
};

/// The 'real' EntityCollection.
template <typename Entity>
class EntityCollectionBase : public EntityCollectionInner<typename Entity::Self> {
private:
  /// fetches the data pointers from a given handler. probably better to from
  /// handler instead.
  template <unsigned N = 0,
            typename std::enable_if<N<Entity::Size>::type * = nullptr> void
                internal_tryGetHandler(const Handler &handler) {
    this->mComponentCollections[N] =
        (void *)handler
            .template getUntypedComponentData<
                Entity, typename Entity::template TypeOf<N>::type>()
            .data();
    internal_tryGetHandler<N + 1>(handler);
  }
  /// termination specialization
  template <unsigned N,
            typename std::enable_if<N == Entity::Size>::type * = nullptr>
  void internal_tryGetHandler(const Handler &handler) {}

public:
  EntityCollectionBase<Entity>() {}
  /// Construct a collection from a handler reference, grabbing the data
  /// pointers from there.
  void fill(const Handler &handler, size_t offset = 0, int64_t size = -1) {
    this->mOffset = offset;
    this->mCount = handler.getCount<Entity>();
    this->mSize = size >= 0 ? size : this->mCount - offset;
    internal_tryGetHandler<0>(handler);
  }
  EntityCollectionBase<Entity>(const Handler &handler, size_t offset = 0,
                           int64_t size = -1) {
    fill(handler, offset, size);
  }
  // returns an Entity i.e. (index, reference) pair
  const Entity operator[](size_t index) const {
    return Entity(this, index);
  }
};
template<typename Entity>
 class EntityCollection : public EntityCollectionBase<Entity>{
 public:
   using EntityCollectionBase<Entity>::EntityCollectionBase;
 };
}

#endif
