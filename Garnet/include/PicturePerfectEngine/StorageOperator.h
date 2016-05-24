#ifndef PICTUREPERFECTENGINE__STORAGE_OPERATOR__H__
#define PICTUREPERFECTENGINE__STORAGE_OPERATOR__H__

#include <PicturePerfectEngine/Operator.h>
#include <PicturePerfectEngine/Storage.h>

namespace PicturePerfect
{

/**
 *
 */
class StorageOperator : public Operator
{
public :
	void            setStorage(Storage& storage);
	Storage&        getStorage() { return *_storage; }
	const Storage&  getStorage() const  { return *_storage; }
private :
	Storage* _storage;
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__STORAGE_OPERATOR__H__
