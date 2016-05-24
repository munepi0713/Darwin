/**
*  NGSA2Strategy is an implemenation of Strategy used with
*  UniversalEngine.
*  This is based on [1].
*  [1] K. Deb, et al. "A Fast and Elitist Multiobjective
*      Genetic Algorithm: NSGA-II," IEEE Trans. on Evolutionary
*      Computation, Vol.6, No.2, April 2002.
*/
#ifndef GARNET__NSGA2_STRATEGY__H__
#define GARNET__NSGA2_STRATEGY__H__

#include <Garnet/StrategyCatalog.h>

namespace Garnet {

namespace NSGA2Strategy {

extern const StrategyFactory& factory;

}//end of namespace NSGA2Strategy

}// end of namespace Garnet


#endif//GARNET__NSGA2_STRATEGY__H__

