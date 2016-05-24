#ifndef PICTUREPERFECTENGINE__INTERPRETER__H__
#define PICTUREPERFECTENGINE__INTERPRETER__H__


#include <string>
#include <iostream>

#include <PicturePerfectEngine/Program.h>
#include <PicturePerfectEngine/Storage.h>


namespace PicturePerfect
{

class Interpreter
{
public :
	virtual ~Interpreter();
	virtual void  execute(std::istream& ins, std::ostream& ous, Storage& storage) = 0;
	virtual void  destroy();

	static Interpreter* createInstance(const std::string& type = "");
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__INTERPRETER__H__
