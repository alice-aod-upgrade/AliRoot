#ifndef O2_HISTOGRAM_H
#define O2_HISTOGRAM_H
#include <vector>
#include <thread>
// #include <mutex>
#include <algorithm>
#include <TH1I.h>
// #include <omp.h>
#include <compute_arrays.hpp>
#include <boost/math/common_factor_ct.hpp>

namespace o2{
class Histogram{
  double mMin;
  double mMax;
  double mDelta;
  std::vector<int> mBins;
public:
  Histogram(int nBins, double min, double max) : mMin(min), mMax(max), mDelta((max-min)/nBins), mBins(nBins+2,0){}
  Histogram() : Histogram(0,0,0){}

  size_t nBins() const{
    return mBins.size()-2;
  }

  int index_of(double value) const{
    int shifted = (value-mMin)/mDelta;
    shifted += 1;
    return shifted < 0? 0 : (shifted >= mBins.size() ? mBins.size()-1 : shifted);
  }

  void Fill(double value){
    mBins[index_of(value)]++;
  }

// #include <iostream>
  ///Fill the histogram using an expression. The expression will be evaluated in multithreaded
  /// and vectorized.
  template<typename ExprT>
  void Fill(const compute_arrays::Expression<ExprT> &base_expression){
    // std::mutex mtx;
    //get the inner expression and the vector width
    const ExprT& expr = compute_arrays::inner(base_expression);
    constexpr int Width = vec::vectorized_type<typename ExprT::Type>::Width;
    // how many vector elements each thread handles.
    //Statically compute the lcm of the vector size and a cacheline.
    // constexpr size_t lcm = boost::math::static_lcm<sizeof(vec::vectorized_type<typename ExprT::Type>),64>();
    //Each thread reads a multiple of cachelines per iteration.
    constexpr int thread_stride = (4096/sizeof(typename ExprT::Type));
    int nThreads = std::max(1ul,std::min((size_t)std::thread::hardware_concurrency()/2, expr.size()/thread_stride));
    if(nThreads <= 1){
      for(int i = 0; i < expr.size()/Width; i++){
        auto vec_element = expr.getVec(i*Width);
        for(int e = 0; e < Width; e++){
          Fill(vec_element[e]);
        }
      }
      for(int i = (expr.size()/Width)*Width; i < expr.size(); i++){
        Fill(expr[i]);
      }
      return;
    }
    // scratch space for each thread.
    std::vector<int> partialSum = std::vector<int>(nThreads*mBins.size(),0);
    std::vector<std::thread> threads(nThreads);
    for(int ithread = 0; ithread < nThreads; ithread++){
      //start a thread
      threads[ithread] = std::thread([&,ithread]{
        //partition the work into chunks of 'thread_stride' elements. Then iterate over each
        // chunk of vector elemetns
        // int count = 0;
        for(int i = ithread; i < expr.size()/thread_stride; i+=nThreads){
          for(int j = 0; j < thread_stride; j += Width){
            //fetch the j'th element of chunk i
            auto vec_element = expr.getVec(i*thread_stride+j);
            //And add each element to the partial sum
            //not the most effective loop, we could do the bin computation in vector form too, but it works(tm)
            for(int e = 0; e < Width; e++){
              partialSum[mBins.size()*ithread + index_of(vec_element[e])]++;
              // count++;
            }
          }
        }
      });
    }
    //add the remaining elements in a scalar fashion.
    for(int i = (expr.size()/thread_stride)*thread_stride;i < expr.size(); i++){
     Fill(expr[i]);
    }
    //Join all threads and merge the result
    for(int i = 0; i < nThreads;i++){
      threads[i].join();
      // std::cout << "joined " << i << std::endl;
      // int count = 0;
      for(int j = 0; j < mBins.size(); j++){
        mBins[j] += partialSum[mBins.size()*i+j];
        // count += partialSum[mBins.size()*i+j];
      }
      // std::cout << "thread " << i << " added " << count << " elements" << std::endl;
    }

  }


  TH1I createTH1I(const char* name = "", const char* title ="") const{
    TH1I hist(name, title, nBins(), mMin, mMax);
    size_t nEntries = 0;
    for(int i = 0; i < mBins.size(); i++){
      hist.SetBinContent(i, mBins[i]);
      nEntries += mBins[i];
    }
    hist.SetEntries(nEntries);
    return hist;
  }

};
}
#endif
