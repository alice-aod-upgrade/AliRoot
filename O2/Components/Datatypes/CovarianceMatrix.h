#ifndef ALI_O2_COMPONENTS_DATATYPES_COVARIANCE_MATRIX_H
#define ALI_O2_COMPONENTS_DATATYPES_COVARIANCE_MATRIX_H

template<typename T, unsigned N>
struct CovarianceMatrix {
  T mCovariance[N];

public:
  CovarianceMatrix(double Covariance[N]) {
    for (int i = 0; i < N; i++) {
      mCovariance[i] = Covariance[i];
    }
  }
  CovarianceMatrix(float Covariance[N]) {
    for (int i = 0; i < N; i++) {
      mCovariance[i] = Covariance[i];
    }
  }
  CovarianceMatrix() {
    for (int i = 0; i < N; i++) {
      mCovariance[i] = 0;
    }
  }
};

#endif
