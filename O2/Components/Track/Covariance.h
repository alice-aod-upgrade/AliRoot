#ifndef ALI_O2_COMPONENTS_TRACK_COVARIANCE_H
#define ALI_O2_COMPONENTS_TRACK_COVARIANCE_H

#include "../Macros/Definition.h"
#include "../Datatypes/CovarianceMatrix.h"

namespace o2{
namespace ecs {
namespace track {
  using Cov_t = o2::ecs::CovarianceMatrix<float,15>;
  DEFINE_COMPONENT(Covariance, Cov_t);
}
}
}
#endif
