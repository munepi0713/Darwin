/**
*  An implemenation strategy for UniversalEngine,
*  which uses a single unified fitness even if
*  individual could generate mulitple scores.
*  Unifying operation can be specified by
*/
#ifndef GARNET__EXPERIMENTAL_STRATEGY__H__
#define GARNET__EXPERIMENTAL_STRATEGY__H__

#include <Garnet/StrategyCatalog.h>

namespace Garnet {

namespace ExperimentalStrategy {

extern const StrategyCatalog& catalog;

}//end of namespace ExperimentalStrategy

}// end of namespace Garnet

#endif//GARNET__EXPERIMENTAL_STRATEGY__H__

