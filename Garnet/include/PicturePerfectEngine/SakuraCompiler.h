#ifndef PICTURE_PERFECT_ENGINE__SAKUA_COMPILER__H__
#define PICTURE_PERFECT_ENGINE__SAKUA_COMPILER__H__

#include <stdint.h>

#include <cassert>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>

#include <PicturePerfectEngine/Compiler.h>

namespace Sakura
{

/**
 *
 */
class Compiler : public PicturePerfect::Compiler
{
public :
	Compiler(bool enableOpenCL = true);
	virtual ~Compiler();

	// Execution
	virtual PicturePerfect::ExecutablePtr compile(std::istream& ins, InfoContainer& info);
};

}//end of namespace Sakura

#endif//PICTURE_PERFECT_ENGINE__SAKUA_COMPILER__H__
