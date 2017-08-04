#ifndef ALI_O2_COMPONENTS_TRACK_COVARIANCE_H
#define ALI_O2_COMPONENTS_TRACK_COVARIANCE_H

#include "../Macros/Definition.h"
#include "../Datatypes/CovarianceMatrix.h"

namespace ecs {
namespace track {
  using Cov_t = CovarianceMatrix<component_f,15>;
  DEFINE_COMPONENT(Covariance, Cov_t);
}
}
#endif
