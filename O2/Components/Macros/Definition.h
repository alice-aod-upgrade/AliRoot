#ifndef ALI_O2_COMPONENTS_MACROS_DEFINE_H
#define ALI_O2_COMPONENTS_MACROS_DEFINE_H
#include "../../ecs/VariableComponent.h"
#include <cstdint>

using component_f = float;
using component_i = int32_t;

#define DEFINE_COMPONENT(NAME, TYPE)                                           \
  class NAME {                                                                 \
    TYPE m##NAME;                                                              \
                                                                               \
  public:                                                                      \
    NAME(TYPE NAME) : m##NAME(NAME) {}                                         \
    NAME() : m##NAME() {}                                                     \
    operator TYPE() const { return m##NAME; }                                  \
    TYPE operator=(TYPE other) {                                               \
      set(other);                                                              \
      return other;                                                            \
    }                                                                          \
    void set(TYPE val) { m##NAME = val; }                                      \
    inline const TYPE& inner(){return m##NAME;}                                \
    static const char *Id() { return #NAME; }                                  \
    using Type = TYPE;\
  }

#define DEFINE_ONE_TO_MANY_COMPONENT(NAME, TYPE)                               \
  class NAME : IVariableComponent {                                            \
    TYPE m##NAME;                                                              \
                                                                               \
  public:                                                                      \
    NAME(TYPE NAME) : m##NAME(NAME) {}                                         \
    NAME() : m##NAME(0) {}                                                     \
    operator TYPE() const { return m##NAME; }                                  \
    TYPE operator=(TYPE other) {                                               \
      set(other);                                                              \
      return other;                                                            \
    }                                                                          \
    void set(TYPE val) { m##NAME = val; }                                      \
    inline const TYPE& inner(){return m##NAME;}                                \
    static const char *Id() { return #NAME; }                                  \
    using Type = TYPE;\
  }

#endif
