//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#ifndef PICTURE_PERFECT_ENGINE__MEMORY_STORAGE__H__
#define PICTURE_PERFECT_ENGINE__MEMORY_STORAGE__H__

#include <mutex>

#include <opencv2/opencv.hpp>

#include <PicturePerfectEngine/Storage.h>

//---------------------------------------------------------
//  MemoryStorage
//---------------------------------------------------------
namespace PicturePerfect {

/**
*
*/
class  MemoryStorage : public Storage
{
public:
	MemoryStorage(const cv::Size& size);
	virtual ~MemoryStorage();

	virtual void             clear();
	virtual cv::Size         getImageSize() const;
	virtual uint32_t         getNumSlots() const;
	virtual DataSlot*        getSlot(uint32_t address);
	virtual const DataSlot*  getSlot(uint32_t address) const;

private:
	typedef std::map<uint32_t, DataSlot*>  DataMap;

	DataMap             _registers;
	cv::Size            _size;
	mutable std::mutex  _mutex;
};

}//end of namespace PicturePerfect


#endif//PICTURE_PERFECT_ENGINE__MEMORY_STORAGE__H__
