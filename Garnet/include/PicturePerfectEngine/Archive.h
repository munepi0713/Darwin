//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef PICTURE_PERFECT_ENGINE__ARCHIVE__H__
#define PICTURE_PERFECT_ENGINE__ARCHIVE__H__

#include <iostream>
#include <memory>

#include <boost/shared_ptr.hpp>

#include <opencv/cv.h>
#include <PicturePerfectEngine/Vector.h>
#include <PicturePerfectEngine/Storage.h>

namespace PicturePerfect
{

class PicturePerfectEngine;
typedef std::shared_ptr<PicturePerfectEngine> PicturePerfectEnginePtr;

class Archive;
typedef std::shared_ptr<Archive> ArchivePtr;

/**
 *           Class-> - S R
 *  xxxAt() const  : Y Y Y
 *  xxxAt()        : Y N Y
 *  getXxx() const : Y Y Y
 *  setXxx() const : Y N Y
 *  loadImage()    : Y N Y
 *  saveImage()    : Y Y Y
 */
class  Archive : public StorageDecorator
{
public :
	Archive(const Archive& x) : StorageDecorator(x.getStorage()) {}
	virtual ~Archive();

	// Interface from Storage.
    virtual void             clear();
    virtual cv::Size         getImageSize() const;
    virtual uint32_t         getNumSlots() const;
    virtual DataSlot*        getSlot(uint32_t address);
    virtual const DataSlot*  getSlot(uint32_t address) const;

	// Interface of this class.
	const double&           valueAt(uint32_t address) const;
	double&                 valueAt(uint32_t address);
	double                  getValue(uint32_t address) const;
    void                    setValue(uint32_t address, double value);

	const cv::Mat&          imageAt(uint32_t address) const;
	cv::Mat&                imageAt(uint32_t address);
	const cv::Mat&          getImage(uint32_t address) const;
	void                    loadImage(uint32_t address, const std::string& filename);
	void                    saveImage(uint32_t address, const std::string& filename);

	const std::wstring&     stringAt(uint32_t address) const;
	std::wstring&           stringAt(uint32_t address);
    const std::wstring&     getString(uint32_t address) const;
    void                    setString(uint32_t address, const std::wstring& value);

	const Vector<double>&   arrayAt(uint32_t address) const;
	Vector<double>&         arrayAt(uint32_t address);
	const Vector<double>&   getArray(uint32_t address) const;
    void                    setArray(uint32_t address, const Vector<double>& value);

	const Vector<uint32_t>& listAt(uint32_t address) const;
	Vector<uint32_t>&       listAt(uint32_t address);
	const Vector<uint32_t>& getList(uint32_t address) const;
    void                    setList(uint32_t address, const Vector<uint32_t>& value);		

	void                    load(std::istream& ins);
	void                    save(std::istream& ins);

	static ArchivePtr createInstance(const cv::Size& imageSize);
	static ArchivePtr createInstance(StoragePtr storage);

protected :
	Archive(StoragePtr storage);
};

}

#endif//PICTURE_PERFECT_ENGINE__ARCHIVE__H__
