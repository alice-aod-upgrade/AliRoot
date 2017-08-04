#ifndef ECS_SLICE_H
#define ECS_SLICE_H
#include <sstream>
#include <stddef.h>
// namespace ecs {
// 
//
// /// A helper class that wraps a sized array. Does not own the data it points
// /// too. That is, it does not allocate or free memory NOTE: I believe a better
// /// implementation already exists in the O2
// /// repository and that should replace this one.
// template <typename T> class Slice {
//   size_t mSize;
//   // It might make sense to make this a shared_ptr later. If one does, make sure
//   // to set the deleter argument:
//   // https://stackoverflow.com/questions/13061979/shared-ptr-to-an-array-should-it-be-used
//   T *mData;
//
// public:
//   /// Default constructor, creates a zero-sized nullptr. Arguments are ptr/size
//   /// pair.
//   Slice<T>(T *data = nullptr, size_t size = 0) : mSize(size), mData(data) {}
//   // Slice<T>(Slice<T> &other) : mData(other.mData), mSize(other.mSize){};
//   /// Return a raw pointer to the array. Same as std::vector<T>.
//   T *data() { return mData; }
//   /// Return a raw const pointer to the array. Same as std::vector<T>.
//   const T *data() const { return mData; }
//   /// Returns the size of the slice.
//   size_t size() const { return mSize; }
//   /// Indexing operator, returns mutable reference.
//   T &operator[](size_t index) { return mData[index]; }
//   /// bounds checked indexing, returns mutable reference.
//   T &at(size_t index) {
//     if (index > size()) {
//       std::stringstream msg;
//       msg << "requested index " << index << " out of range of size " << size();
//       throw std::out_of_range(msg.str());
//     }
//     this[index];
//   }
//   /// Start of the data ptr, useful for sorting algorithms in std.
//   T *begin() { return data(); }
//   /// Start of the data ptr, useful for sorting algorithms in std.
//   T *end() { return data() + mSize; }
// };
// }
#endif
