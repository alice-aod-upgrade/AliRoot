#ifndef EXPT_OPERATORS_H
#define EXPT_OPERATORS_H

#include <vectorized_type.h>
#include "types.hpp"
#include <iostream>
#include <immintrin.h>
#include <functional>
#include <cmath>

////////////////////////////////////////
// START FUNCTORS
///////////////////////////////////////

struct Sum{
template<typename Lhs, typename Rhs>
  static auto Map(Lhs l, Rhs r){
    return l+r;
  }
};

struct Sub{
  template<typename Lhs, typename Rhs>
  static auto Map(Lhs l, Rhs r){
    return l-r;
  }
};

struct Mul{
template<typename Lhs, typename Rhs>
  static auto Map(Lhs l, Rhs r){
    return l*r;
  }
};

struct Div{
template<typename Lhs, typename Rhs>
  static auto Map(Lhs l, Rhs r){
    return l/r;
  }
};

//(possibly) vectorized mathetical operations, see <vectorized_types.h>
// for details.

struct Sqrt{
template<typename T>
  static auto Map(T val){
    return vec::sqrt(val);
  }
};
struct Log{
template<typename T>
  static auto Map(T val){
    return vec::log(val);
  }
};
struct Sin{
template<typename T>
  static auto Map(T val){
    return vec::sin(val);
  }
};
struct Cos{
template<typename T>
  static auto Map(T val){
    return vec::cos(val);
  }
};
struct Tan{
template<typename T>
  static auto Map(T val){
    return vec::tan(val);
  }
};


template<typename T>
struct Max{
  inline static auto Map(T a, T b) {
    return a > b ? a : b;
  }
};

template<typename T>
struct Min{
  inline static auto Map(T a, T b) {
    return a < b ? a : b;
  }
};

struct Clamp{
  template<typename TVal>
  inline static TVal Map(TVal val, decltype(val) min, decltype(val) max) {
    assert(min <= max);
    return val < min? min : (val > max? max : val);
  }
};

// template operation, takes an expression and an operation
// and returns another expression describing the application of
// OP to Args
template<typename OP, typename... Args>
inline auto
ApplyFunctor(const Expression<Args>&... args) {
  return ExpressionMap<OP, Args...>(GetSubtype(args)...);
}

//wrappers for operators
/*two expressions*/
template<typename TLhs, typename TRhs>
inline auto
operator*(const Expression<TLhs>& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Mul>(lhs, rhs);
}
template<typename TLhs, typename TRhs>
inline auto
operator/(const Expression<TLhs>& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Div>(lhs, rhs);
}
template<typename TLhs, typename TRhs>
inline auto
operator+(const Expression<TLhs>& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Sum>(lhs, rhs);
}
template<typename TLhs, typename TRhs>
inline auto
operator-(const Expression<TLhs>& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Sub>(lhs, rhs);
}

/*expression on right, non on left*/
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TRhs>::value>::type* = nullptr>
inline auto
operator*(const Expression<TLhs>& lhs, const TRhs& rhs) {
  return ApplyFunctor<Mul>(lhs, MakeExpression(rhs, GetSubtype(lhs).size()));
}
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TRhs>::value>::type* = nullptr>
inline auto
operator/(const Expression<TLhs>& lhs, const TRhs& rhs) {
  return ApplyFunctor<Div>(lhs, MakeExpression(rhs, GetSubtype(lhs).size()));
}

template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TRhs>::value>::type* = nullptr>
inline auto
operator+(const Expression<TLhs>& lhs, const TRhs& rhs) {
  return ApplyFunctor<Sum>(lhs, MakeExpression(rhs, lhs.size()));
}
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TRhs>::value>::type* = nullptr>
inline auto
operator-(const Expression<TLhs>& lhs, const TRhs& rhs) {
  return ApplyFunctor<Sub>(lhs, MakeExpression(rhs, lhs.size()));
}

/*expression on the left, non on right*/
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TLhs>::value>::type* = nullptr>
inline auto
operator*(const TLhs& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Mul>(MakeExpression(lhs,GetSubtype(rhs).size()), rhs);
}
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TLhs>::value>::type* = nullptr>
inline auto
operator/(const TLhs& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Div>(MakeExpression(lhs,GetSubtype(rhs).size()), rhs);
}
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TLhs>::value>::type* = nullptr>
inline auto
operator+(const TLhs& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Sum>(MakeExpression(lhs,rhs.size()), rhs);
}
template<typename TLhs, typename TRhs, typename std::enable_if<!std::is_base_of<TraitExpression, TLhs>::value>::type* = nullptr>
inline auto
operator-(const TLhs& lhs, const Expression<TRhs>& rhs) {
  return ApplyFunctor<Sub>(MakeExpression(lhs,rhs.size()), rhs);
}

template<typename TVal, typename TMin, typename TMax>
inline auto clamp(const Expression<TVal>& val, const TMin& min, const TMax& max) {
  return ApplyFunctor<Clamp>(val, MakeExpression(min), MakeExpression(max));
}

//wrapper for mathematical functions
template<typename TVal>
inline auto sqrt(const Expression<TVal>& expr) {
  return ApplyFunctor<Sqrt>(expr);
}

template<typename TVal>
inline auto log(const Expression<TVal>& expr) {
  return ApplyFunctor<Log>(expr);
}
template<typename TVal>
inline auto tan(const Expression<TVal>& expr) {
  return ApplyFunctor<Tan>(expr);
}
template<typename TVal>
inline auto sin(const Expression<TVal>& expr) {
  return ApplyFunctor<Sin>(expr);
}
template<typename TVal>
inline auto cos(const Expression<TVal>& expr) {
  return ApplyFunctor<Cos>(expr);
}


template<typename TVal>
inline auto sum(const Expression<TVal>& expr){
  const TVal& subexpr = GetSubtype(expr);
  typename TVal::Type sum = 0;
  #pragma omp parallel for reduction(+:sum) schedule(static)
  for(int i = 0; i < subexpr.size(); i++){
    sum += subexpr[i];
  }
  return sum;
}

template<typename TExpr>
inline auto vec_sum(const Expression<TExpr>& expr){
  using namespace vec;
  const TExpr& subexpr = GetSubtype(expr);
  vectorized_type<typename TExpr::Type> v_sum(0);
  #pragma omp declare reduction(+:vectorized_type<typename TExpr::Type>: omp_out = omp_out+omp_in) initializer(omp_priv = 0)
  #pragma omp parallel for reduction(+:v_sum) schedule(static)
  for(int i = 0; i < subexpr.size()/decltype(v_sum)::Width; i++){
    v_sum += subexpr.get_vec(i);
  }
  return v_sum.sum();
}

template<typename TExpr>
inline void map_expr(const Expression<TExpr>& expr, std::function<void (typename TExpr::Type)> func){
  const TExpr& subexpr = GetSubtype(expr);
  constexpr int Width = vec::vectorized_type<typename TExpr::Type>::Width;
  for(int i = 0; i <  subexpr.size()/Width; i++){
    auto vec_element = subexpr.get_vec(i);
    for(int j = 0; j < decltype(vec_element)::Width; j++){
      func(vec_element[j]);
    }
  }
  for(int i = (subexpr.size()/Width)*Width;i < subexpr.size(); i++){
    func(subexpr[i]);
  }
}

#endif
