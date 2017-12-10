#pragma once
#include <compute_arrays.hpp>
namespace o2{
template<typename T, typename std::enable_if<std::is_base_of<typename compute_arrays::TraitExpression, T>::value>::type* = nullptr>
auto sqrt(const compute_arrays::Expression<T>& val){
  return compute_arrays::sqrt(val);
}

template<typename T, typename std::enable_if<!std::is_base_of<typename compute_arrays::TraitExpression, T>::value>::type* = nullptr>
auto sqrt(T val){
  return std::sqrt(val);
}

template<typename E>
inline auto abs(const compute_arrays::Expression<E> & expr){
  return compute_arrays::abs(expr);
}

template<typename T>
inline auto abs(T val){
  return std::abs(val);
}



template<typename E>
inline auto log(const compute_arrays::Expression<E> & expr){
  return compute_arrays::log(expr);
}

template<typename T>
inline auto log(T val){
  return std::log(val);
}

template<typename E>
inline auto sin(const compute_arrays::Expression<E> & expr){
  return compute_arrays::sin(expr);
}

template<typename T>
inline auto sin(T val){
  return std::sin(val);
}
template<typename E>
inline auto cos(const compute_arrays::Expression<E> & expr){
  return compute_arrays::cos(expr);
}

template<typename T>
inline auto cos(T val){
  return std::cos(val);
}
}
