#pragma once
#include <cstdint>
#include "../../ecs/VariableComponent.h"
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
  class NAME : o2::ecs::IVariableComponent {                                            \
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
