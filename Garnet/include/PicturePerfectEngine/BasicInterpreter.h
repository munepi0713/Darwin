#ifndef PICTUREPERFECTENGINE__BASIC_INTERPRETER__H__
#define PICTUREPERFECTENGINE__BASIC_INTERPRETER__H__


#include <PicturePerfectEngine/Interpreter.h>


namespace PicturePerfect
{

class BasicInterpreter : public Interpreter
{
public :
	struct Decoder {
		virtual void  execute(const strings& tokens, std::ostream& ous) = 0;
	};
	typedef std::map<std::string, Decoder*> Decoders;

	BasicInterpreter();
	virtual ~BasicInterpreter();
	virtual void  execute(std::istream& ins, std::ostream& ous, Storage& storage);
	
private :
	Decoders _decoders;
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__BASIC_INTERPRETER__H__
