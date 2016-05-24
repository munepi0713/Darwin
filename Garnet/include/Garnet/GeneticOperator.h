#ifndef GARNET__GENETIC_OPERATOR__H__
#define GARNET__GENETIC_OPERATOR__H__

#include <Garnet/Generation.h>

namespace Garnet
{

class GeneticOperator
{
public :
    virtual IndividualList execute(const IndividualList& src) = 0;
};

}// end of namespace Garnet

#endif//GARNET__GENETIC_OPERATOR__H__
