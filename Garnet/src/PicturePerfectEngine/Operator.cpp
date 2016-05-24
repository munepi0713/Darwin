
#include <PicturePerfectEngine/Operator.h>

//---------------------------------------------------------
//---------------------------------------------------------

PicturePerfect::OperatorMap::~OperatorMap()
{
    clear();
}

void  PicturePerfect::OperatorMap::clear()
{
	for (OperatorMap::iterator it = begin(); it != end(); it++) {
		delete it->second;
	}
	std::map<Operator::ValueType, Operator*>::clear();
}
