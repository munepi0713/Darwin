#ifndef PICTURE_PERFECT_ENGINE__REGISTER_FILE__H__
#define PICTURE_PERFECT_ENGINE__REGISTER_FILE__H__

#include <boost/shared_ptr.hpp>

#include <PicturePerfectEngine/Storage.h>

namespace PicturePerfect
{

class PicturePerfectEngine;
typedef boost::shared_ptr<PicturePerfectEngine> PicturePerfectEnginePtr;

class RegisterFile;
typedef boost::shared_ptr<RegisterFile> RegisterFilePtr;

/**
 *
 */
class  RegisterFile : public Storage
{
public :
    RegisterFile();
	virtual ~RegisterFile();

	static RegisterFilePtr createInstance(const PicturePerfectEngine* ppe);
};

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__REGISTER_FILE__H__
