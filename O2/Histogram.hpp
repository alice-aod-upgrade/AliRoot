#ifndef O2_EXPRESSION_TEMPLATES_HISTOGRAM_H
#define O2_EXPRESSION_TEMPLATES_HISTOGRAM_H
#include <vector>
#include <TH1I.h>
#include <omp.h>
#include <compute_arrays>

namespace o2{
namespace expression_templates{

class Histogram{
  double mMin;
  double mMax;
  double mDelta;
  std::vector<int> mBins;
public:
  ///Construct a 1D histogram by specifying the amount of bins to use and range that should be binned.
  /// nBins does not include the automatically generated under- and over-flow bins.
  Histogram(int nBins, double min, double max) : mMin(min), mMax(max), mDelta((max-min)/nBins), mBins(nBins+2,0){}
  Histogram() : Histogram(0,0,0){}

  ///Returns the amount of bins. Excluding under- and over-flow bin.
  size_t nBins() const{
    return mBins.size()-2;
  }

  ///Returns the bin index corresponding to the given value.
  /// index 0 is the undeflow bin, index nBins()+1 is the overflow bin.
  int index_of(double value) const{
    int shifted = (value-mMin)/mDelta;
    shifted += 1;
    return shifted < 0? 0 : (shifted >= mBins.size() ? mBins.size()-1 : shifted);
  }

  ///Add a single element to the histogram.
  void Fill(double value){
    mBins[index_of(value)]++;
  }

  ///Adds all the values produced by the expression base_expression to the histogram.
  ///This method is much faster than adding single values at a time
  /// because it is vectorized and parallelized with OpenMP.
  ///
  /// In order to prevent data races, each thread creates their own histogram to
  /// be filled with their data, after which the resulting histograms are
  /// merged sequentially on the main thread.
  template<typename ExprT>
  void Fill(const Expression<ExprT> &base_expression){
    const ExprT& expr = GetSubtype(base_expression);
    constexpr int Width = vec::vectorized_type<typename ExprT::Type>::Width;
    int nThreads;
    std::vector<int> partialSum; ///we will use this vector to store the thread-private histograms.
    #pragma omp parallel
    {
      nThreads = omp_get_num_threads();
      const int ithread = omp_get_thread_num();
      #pragma omp single
      {
      //Let one of the threads resize partialSum to have enough space for all of the threads.
      partialSum = std::vector<int>(nThreads*mBins.size(),0);
      }
      //we loop over all the vector elements, adding the remaining scalar
      //elements at the end of this function.
      #pragma omp for schedule(static)
      for(int i = 0; i <  expr.size()/Width; i++){
        auto vec_element = expr.get_vec(i);
        // sequentially add each scalar element of vec_element
        // not the most effective loop, we could do the bin computation in vector form too, but it works
        for(int j = 0; j < Width; j++){
          partialSum[mBins.size()*ithread + index_of(vec_element[j])]++;
        }
      }
    }//end parallel block
    //Now sequentially add the intermediate results to mBins.
    for(int i = 0; i < nThreads;i++){
      for(int j = 0; j < mBins.size(); j++){
        //This could also be implemented more efficiently and elegantly with OpenMP reduction clauses.
        // (for big nThreads)
        mBins[j] += partialSum[mBins.size()*i+j];
      }
    }
    //handle the scalar elements of expr that did not fit into a vector register
    for(int i = (expr.size()/Width)*Width;i < expr.size(); i++){
      Fill(expr[i]);
    }
  }

  ///Creates a TH1I containing a copy of our data with the given name and title.
  ///n.b. This creates a new TH1I so it will automatically be added to any open
  /// TTree!
  TH1I createTH1I(const char* name = "", const char* title ="") const{
    TH1I hist(name, title, nBins(), mMin, mMax);
    for(int i = 0; i < mBins.size(); i++){
      hist.SetBinContent(i, mBins[i]);
    }
    return hist;
  }

};

}
}
#endif
