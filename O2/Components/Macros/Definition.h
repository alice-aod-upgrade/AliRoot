#ifndef ALI_O2_COMPONENTS_MACROS_DEFINE_H
#define ALI_O2_COMPONENTS_MACROS_DEFINE_H

#define DEFINE_COMPONENT(NAME, TYPE)                                           \
  class NAME {                                                                 \
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
    void set(TYPE NAME) { m##NAME = NAME; }                                    \
    static const char *Id() { return #NAME; }                                  \
  }

#endif
