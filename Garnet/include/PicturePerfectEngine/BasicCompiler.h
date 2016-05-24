#ifndef PICTUREPERFECTENGINE__BASIC_COMPILER__H__
#define PICTUREPERFECTENGINE__BASIC_COMPILER__H__


#include <string>
#include <iostream>

#include <PicturePerfectEngine/Compiler.h>

namespace PicturePerfect
{

class BasicCompiler : public Compiler
{
public :
	struct Decoder {
		virtual void  execute(const strings& tokens, PicturePerfect::Program& program, PicturePerfect::Storage& storage) = 0;
	};
	typedef std::map<std::string, Decoder*> Decoders;

	BasicCompiler();
	~BasicCompiler();
	virtual void  execute(std::istream& ins, Program& program, Storage& storage);
protected :
	Decoders  _decoders;
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__BASIC_COMPILER__H__
