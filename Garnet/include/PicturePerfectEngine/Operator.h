#ifndef PICTUREPERFECTENGINE__OPERATOR__H__
#define PICTUREPERFECTENGINE__OPERATOR__H__

#include <map>

#include <Common/IntTypes.h>

#include <PicturePerfectEngine/Program.h>
#include <PicturePerfectEngine/RegisterFile.h>

namespace PicturePerfect
{

/**
 *
 */
class Operator
{
public :
	virtual ~Operator() {};

	typedef Program::value_type  ValueType;
	enum ResultType
	{
		RES_EXIT,
		RES_NEXT,
		RES_ERROR,
		RES_MAX
	};

	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) = 0;
};

class OperatorMap : public std::map<Operator::ValueType, Operator*>
{
public :
	virtual ~OperatorMap();

	void  clear();
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__OPERATOR__H__
