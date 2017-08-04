#ifndef EXPT_HISTOGRAM_HPP
#define EXPT_HISTOGRAM_HPP
#include <vector>
#include <TH1I.h>
#include <omp.h>
#include "types.hpp"

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
    mBins[index_of(value  )]++;
  }

  template<typename ExprT>
  void Fill(const Expression<ExprT> &base_expression){
    const ExprT& expr = GetSubtype(base_expression);
    constexpr int Width = vec::vectorized_type<typename ExprT::Type>::Width;
    int nThreads;
    std::vector<int> partialSum;
    #pragma omp parallel
    {
      nThreads = omp_get_num_threads();
      const int ithread = omp_get_thread_num();
      #pragma omp single
      {
      partialSum = std::vector<int>(nThreads*mBins.size(),0);
      }
    #pragma omp for schedule(static)
    for(int i = 0; i <  expr.size()/Width; i++){
      auto vec_element = expr.get_vec(i);
      //not the most effective loop, we could do the bin computation in vector form too, but it works(tm)
      for(int j = 0; j < Width; j++){
        partialSum[mBins.size()*ithread + index_of(vec_element[j])]++;
      }
    }
    }
    for(int i = 0; i < nThreads;i++){
      for(int j = 0; j < mBins.size(); j++){
        //This could also be implemented more efficiently and elegantly with OpenMP reduction clauses.
        mBins[j] += partialSum[mBins.size()*i+j];
      }
    }
    for(int i = (expr.size()/Width)*Width;i < expr.size(); i++){
      Fill(expr[i]);
    }
  }


  TH1I createTH1I(const char* name = "", const char* title ="") const{
    TH1I hist(name, title, nBins(), mMin, mMax);
    for(int i = 0; i < mBins.size(); i++){
      hist.SetBinContent(i, mBins[i]);
    }
    return hist;
  }

};

#endif
