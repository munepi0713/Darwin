
#ifndef OPEN_CL_OPERATOR__H__
#define OPEN_CL_OPERATOR__H__

#include <PicturePerfectEngine/PicturePerfectOpenCV.h>

namespace OpenCLOperator
{

void initialize();

void finalize();

Operator* getOperator(const std::string& name);


}



#endif//OPEN_CL_OPERATOR__H__
