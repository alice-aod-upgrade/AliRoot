#ifndef VALEXP_VEC_H
#define VALEXP_VEC_H

#include <type_traits>
#include <vector>
#include <valarray>
#include <cassert>
#include <memory>
#include "operators.hpp"
#include "types.hpp"

template<typename T>
class Slice : public Expression<Slice<T>> {
  using vec_t = vec::vectorized_type<T>;
protected:
  T* mData;
  size_t mSize;
public:
  using Type = T;
  Slice() : mData(nullptr), mSize(0){}
  Slice(T* data, size_t size) : mData(data), mSize(size){}
  size_t size() const{ return mSize; }
  size_t vec_size() const{ return mSize/vec_t::Width; }
  size_t tail_size() const{ return size() - vec_size()*vec_t::Width;}
  size_t tail_start_index() const{ return vec_size()*vec_t::Width;}

  const T* data() const {return mData;}

  T operator[](size_t index) const {return mData[index];}
  vec_t get_vec(size_t index) const{
    return vec_t(mData+vec_t::Width*index);
  }

  template<typename I>
  auto gather (const Expression<I>& indices) const{
    return GatherExpr<T,I>(data(), indices);
  }

  template<typename I>
  auto gather (const Expression<Slice<I>>& indices) const{
    return GatherPtr<T,I>(data(), GetSubtype(indices).data(), GetSubtype(indices).size());
  }
};

template<typename T>
class Varray : public Slice<T>{
protected:
  using vec_t = vec::vectorized_type<T>;
  std::shared_ptr<vec_t> mManagedData;
public:
  using Type = T;
  template<typename E, typename std::enable_if<!std::is_same<typename E::Type, T>::value && std::is_convertible<typename E::Type, T>::value>::type* = nullptr>
  Varray(const Expression<E> &v) {
    const E& subexpr = GetSubtype(v);
    this->mSize = subexpr.size();
    mManagedData = std::shared_ptr<vec_t>(new vec_t[this->vec_size()+1], std::default_delete<vec_t[]>());
    this->mData = (T*)mManagedData.get();
    #pragma omp parallel for
    for(int index = 0;index < this->size(); index++){
      this->data()[index] = subexpr[index];
    }
  }

template<typename E, typename std::enable_if<std::is_same<typename E::Type, T>::value>::type* = nullptr>
  Varray(const Expression<E> &v) {
    const E& subexpr = GetSubtype(v);
    this->mSize = subexpr.size();
    mManagedData = std::shared_ptr<vec_t>(new vec_t[this->vec_size()+1], std::default_delete<vec_t[]>());
    this->mData = (T*)mManagedData.get();
    #pragma omp parallel for
    for(int index=0; index < this->vec_size(); index++){
      mManagedData.get()[index] = subexpr.get_vec(index);
    }
    for(int index = this->tail_start_index();index < this->size(); index++){
      this->mData[index] = subexpr[index];
    }
  }
Varray(size_t size){
  this->mSize = size;
  mManagedData = std::shared_ptr<vec_t>(new vec_t[this->vec_size()+1], std::default_delete<vec_t[]>());
  this->mData = (T*)mManagedData.get();
}
  T& operator[](size_t index){return this->mData[index];}
  vec_t& get_vec(size_t index){
    return mManagedData.get()[index];
  }
  vec_t get_vec(size_t index) const{
    return mManagedData.get()[index];
  }
};

#endif
