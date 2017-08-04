#ifndef ALI_O2_COMPONENTS_VERTEX_COVARIANCE_H
#define ALI_O2_COMPONENTS_VERTEX_COVARIANCE_H
#include "../Datatypes/CovarianceMatrix.h"
#include "../Macros/Definition.h"
namespace ecs {
namespace vertex {
  using Cov_t = CovarianceMatrix<component_f,6>;
  DEFINE_COMPONENT(Covariance, Cov_t);
}
}
#endif
