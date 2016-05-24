/**
*  An implemenation strategy for UniversalEngine,
*  which uses a single unified fitness even if
*  individual could generate mulitple scores.
*  Unifying operation can be specified by
*/
#ifndef GARNET__IVTREE_STRATEGY__H__
#define GARNET__IVTREE_STRATEGY__H__

#include <Garnet/IVPairArchive.h>
#include <Garnet/StrategyCatalog.h>

namespace Garnet {

namespace IVTreeStrategy {

extern const StrategyCatalog& catalog;

class IVTreeIntermediate : public Chromosome::Intermediate {
public:
    std::string          igraph;
    std::string          vgraph;
    std::string          agraph;
    std::vector<IVPair>  iv;     /** A list of IVPairs resulted from evaluaiton. Each result coresponds to a task. */
};
typedef std::shared_ptr<IVTreeIntermediate> IVTreeIntermediatePtr;

}//end of namespace IVTreeStrategy

}// end of namespace Garnet


#endif//GARNET__IVTREE_STRATEGY__H__

