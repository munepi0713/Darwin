#ifndef PICTURE_PERFECT_ENGINE__PROGRAM__H__
#define PICTURE_PERFECT_ENGINE__PROGRAM__H__

#include <vector>
#include <iterator>

#include <Common/IntTypes.h>

namespace PicturePerfect
{

class Program : public std::vector<uint16_t>
{
public :
	typedef std::vector<uint16_t>::value_type  ValueType;
};

typedef Program::const_iterator  ProgramIterator;
typedef std::back_insert_iterator<Program>  ProgramWriter;

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__PROGRAM__H__
