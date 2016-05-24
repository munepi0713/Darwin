#ifndef PICTURE_PERFECT_ENGINE__COMPILER__H__
#define PICTURE_PERFECT_ENGINE__COMPILER__H__


#include <string>
#include <iostream>
#include <memory>

#include <PicturePerfectEngine/Executable.h>

namespace PicturePerfect
{

class Compiler;
typedef std::shared_ptr<Compiler> CompilerPtr;

class Compiler
{
public :
	virtual ~Compiler();
	virtual ExecutablePtr compile(std::istream& ins, InfoContainer& info) = 0;
	virtual ExecutablePtr compile(std::istream& ins) {
		InfoContainer dummyInfo;
		return compile(ins, dummyInfo);
	}

	static CompilerPtr  createInstance(const std::string& type = "");
};

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__COMPILER__H__
