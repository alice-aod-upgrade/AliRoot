#ifndef VALEXP_TYPES_H
#define VALEXP_TYPES_H

// #include <typeinfo>
// #include <iostream>
#include <utility>
#include "internal/apply.hpp"
// #include <cxxabi.h>
#include <vectorized_type.h>
#include <cstdint>

// class TraitExpression{};
template<typename T>
class FlatVarr;

/// a Curiously-Recurring-Template-Parameter (CRTP)
/// type. That is, every expression should inherit from this type
/// and pass its own type as the SubType parameter.
///
/// All expressions should implement:
/// * `Type` typedef describing the return type of their operator[]
/// * `size_t size() const` function to get the size of the underlying expression.
/// * `vec::vectorized_type<T> get_vec(index) const` to get a vector element.
/// * `auto gather(const Expression<I>& indices) const` to perform a scattered read.
/// NOTE: currently the gather types do not support gather instructions themselves.

class TraitExpression{};
template<typename SubType>
class Expression : public TraitExpression {};

/// a function for casting a generic expression to it's subtype.
template<typename SubType, typename std::enable_if<std::is_base_of<Expression<SubType>, SubType>::value>::type* = nullptr>
constexpr inline const SubType& GetSubtype(
  const Expression<SubType>& expr){return *static_cast<const SubType*>(&expr);}

//An expression wrapping a constant of type T.
template<typename T>
class Constant : public Expression<Constant<T>>{
  T mData;
  size_t mSize;
public:
  Constant(T t, size_t size) : mData(t), mSize(size){}
  //Returns the same value for any index.
  T operator[](size_t index) const {
    assert(index < mSize);
    return mData;
  }
  vec::vectorized_type<T> get_vec(size_t index) const{
    return vec::vectorized_type<T>(mData);
  }
  template<typename I>
  auto gather(const Expression<I>& indices) const{
    return (*this);
  }
  size_t size() const{return mSize;}
  using Type = T;
};

//An expression which gathers from a data-ptr using an expression type.
template<typename T, typename I>
class GatherExpr : public Expression<GatherExpr<T,I>>{
  const T* mData;
  const I mIndices;
public:
  GatherExpr(const T* t, const Expression<I>& indices) : mData(t), mIndices(GetSubtype(indices)){
      static_assert(std::is_integral<typename I::Type>::value, "Gather(const Expression<T> t, const Expression<I> indices) requires that I produce an integral type");
  }
  T operator[](size_t index) const {
    auto indices = GetSubtype(mIndices);
    assert(index < size());
    return mData[indices[index]];
  }
  vec::vectorized_type<T> get_vec(size_t index) const{
    //how many indices we need.
    constexpr int T_Width = vec::vectorized_type<T>::Width;
    int I_index = index*T_Width;
    auto indices = GetSubtype(mIndices);
    typename I::Type index_buffer[T_Width];
    for(int i = 0; i < T_Width; i++){
      index_buffer[i] = indices[I_index+i];
    }
    return vec::vectorized_type<T>::Gather(mData, index_buffer);
  }
  size_t size() const{return GetSubtype(mIndices).size();}
  using Type = T;
};

//An expression which gathers from a data-ptr using an index ptr.
// and optimization w.r.t. GatherExpr for the case were the indices are stored in memory.
template<typename T, typename I>
class GatherPtr : public Expression<GatherPtr<T,I>>{
  const T* mData;
  const I* mIndices;
  size_t mSize;
public:
  GatherPtr(const T* t, const I* indices, size_t size) : mData(t), mIndices(indices), mSize(size){
      static_assert(std::is_integral<I>::value, "GatherPtr(const T* t, const I* indices, size_t size) requires that I produce an integral type");
  }
  //Returns the same value for any index.
  T operator[](size_t index) const {
    assert(index < size());
    return mData[mIndices[index]];
  }
  vec::vectorized_type<T> get_vec(size_t index) const{
    constexpr int T_Width = vec::vectorized_type<T>::Width;
    return vec::vectorized_type<T>::Gather(mData, mIndices+index*T_Width);
  }
  size_t size() const{return mSize;}
  using Type = T;
};

//
// //if it is not an expression, MakeExpression is the identity function
// template<typename T, typename std::enable_if<!std::is_base_of<Expression<T>, T>::value>::type* = nullptr>
// constexpr inline const Constant<T> MakeExpression(const T& expr, size_t size){return Constant<T>(expr, size);}
//
// //if it's already an expression, just return that.
// template<typename T, typename std::enable_if<std::is_base_of<Expression<T>, T>::value>::type* = nullptr>
// constexpr inline const auto& MakeExpression(const Expression<T>& expr, size_t size){return expr;}

// An expression which performs an operation Op using arguments (Args...).
// If Args[i] is an expression it stores (and uses) a constant reference to this expression.
// If it is not an expression, it gets copied into a "Constant<Args[i]>" type.
template<typename Op, typename... Args>
class ExpressionMap : public Expression<ExpressionMap<Op, Args...>>{
  //A tuple to hold our arguments.
  //If the type is an expression it holds a constant copy (it's copied to allow auto and multi-line chaining)
  // if it is not an expression yet it gets copied into a Constant<Arg> type.
  using tuple_type = std::tuple<typename
    std::conditional<
      std::is_base_of<TraitExpression, Args>::value,
      const Args,
      const Constant<Args>
    >::type...
  >;
  const tuple_type mArgs;

  template<typename I, int... Is>
  auto gather_all(const Expression<I>& indices, expression_templates::internal::seq<Is...>) const{
    // std::cout << "using gather_all..." << GetSubtype(indices).size() << ", " << size() << std::endl;
    return std::make_tuple(
      std::get<Is>(mArgs).gather(indices)...
    );
  }

public:
  ExpressionMap(typename
    std::conditional<
      std::is_base_of<TraitExpression, Args>::value,
      const Args,
      const Constant<Args>
    >::type... args) : mArgs(std::tie(args...)){
      // std::cout << "Constructed " << __PRETTY_FUNCTION__ << " from PACK" << std::endl;
      // std::cout << "Placed Expression at " << this << std::endl;
  };
  ExpressionMap(const tuple_type tuple) : mArgs(tuple){
    // std::cout << "Constructed from tuple: " << size() << std::endl;
    // std::cout << "Constructed " << __PRETTY_FUNCTION__ << " from TUPLE" << std::endl;
      // std::cout << "Placed Expression at " << this << std::endl;
  };

  size_t size() const{ return std::get<0>(mArgs).size();}
  auto operator[](size_t index) const{
    // std::cout << " invoking " << this << std::endl;
    return expression_templates::internal::invoke_at<Op>(index, mArgs);
  }
  auto get_vec(size_t index) const{
    return expression_templates::internal::invoke_at_vec<Op>(index, mArgs);
  }
  template<typename I>
  auto gather(const Expression<I>& indices) const{
    // std::cout << "mapping gather to tree..." << GetSubtype(indices).size() << std::endl;
    return ExpressionMap<Op, const decltype(std::declval<Args>().gather(indices))...>(gather_all(indices, typename expression_templates::internal::gens<sizeof...(Args)>::type()));
  }
  using Type = decltype(expression_templates::internal::invoke_at<Op>(0, mArgs));
};


#endif
