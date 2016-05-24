#include <PicturePerfectEngine/RegisterFile.h>
#include <PicturePerfectEngine/PicturePerfectEngine.h>

//---------------------------------------------------------
//  RegisterFile
//---------------------------------------------------------
PicturePerfect::RegisterFile::RegisterFile()
: Storage()
{
}

PicturePerfect::RegisterFile::~RegisterFile()
{
}

PicturePerfect::RegisterFilePtr PicturePerfect::RegisterFile::createInstance(const PicturePerfect::PicturePerfectEngine* ppe)
{
	PicturePerfect::RegisterFile* ptr = new PicturePerfect::RegisterFile();
	ptr->setImageSize(ppe->getImageSize());
	return PicturePerfect::RegisterFilePtr(ptr);
}
