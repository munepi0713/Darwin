//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#include <PicturePerfectEngine/MemoryStorage.h>

#include <boost/format.hpp>

#include <PicturePerfectEngine/Vector.h>

//---------------------------------------------------------
//  MemoryStorage
//---------------------------------------------------------

PicturePerfect::MemoryStorage::MemoryStorage(const cv::Size& imageSize)
: _size(imageSize) {}

void  PicturePerfect::MemoryStorage::clear() {
    for (DataMap::iterator it = _registers.begin(); it != _registers.end(); it++) {
        delete it->second;
    }
    _registers.clear();
}

const PicturePerfect::DataSlot*  PicturePerfect::MemoryStorage::getSlot(uint32_t address) const {
    DataSlot* entry = _registers.find(address) != _registers.end() ? _registers.at(address) : 0;
    return entry;
}

PicturePerfect::DataSlot*  PicturePerfect::MemoryStorage::getSlot(uint32_t address) {
    // Get entry.
    DataSlot* entry = _registers.find(address) != _registers.end() ? _registers.at(address) : 0;

    // Check specified entry is free.
    if (!entry) {
        // Create new entity for id.
        switch (getStorageType(address)) {
        case TYPE_V1D:
            entry = new DataSlotT<double>(0);
            break;
        case TYPE_I1D:
            entry = new DataSlotT<cv::Mat>(cv::Mat(_size, CV_64FC1));
            break;
        case TYPE_I3D:
            entry = new DataSlotT<cv::Mat>(cv::Mat(_size, CV_64FC3));
            break;
        case TYPE_I1B:
            entry = new DataSlotT<cv::Mat>(cv::Mat(_size, CV_8UC1));
            break;
        case TYPE_I3B:
            entry = new DataSlotT<cv::Mat>(cv::Mat(_size, CV_8UC3));
            break;
        case TYPE_S:
            entry = new DataSlotT<std::wstring>(L"");
            break;
        case TYPE_A1D:
            entry = new DataSlotT<PicturePerfect::Vector<double> >(PicturePerfect::Vector<double>());
            break;
        case TYPE_L:
            entry = new DataSlotT<PicturePerfect::Vector<uint32_t> >(PicturePerfect::Vector<uint32_t>());
            break;
        case TYPE_V1Dg:
            entry = new DataSlotT<double>(0);
            break;
#if defined(GARNET_ENABLE_OPENCL__)
        case TYPE_I1Dg:
            entry = new DataSlotT<cv::ocl::oclMat>(cv::ocl::oclMat(_size, CV_64FC1));
            break;
        case TYPE_I3Dg:
            entry = new DataSlotT<cv::ocl::oclMat>(cv::ocl::oclMat(_size, CV_64FC3));
            break;
        case TYPE_I1Bg:
            entry = new DataSlotT<cv::ocl::oclMat>(cv::ocl::oclMat(_size, CV_8UC1));
            break;
        case TYPE_I3Bg:
            entry = new DataSlotT<cv::ocl::oclMat>(cv::ocl::oclMat(_size, CV_8UC3));
            break;
#endif
        }

        if (entry == 0) {
            throw (boost::format("Storage at 0x%x is not supported in its datatype.") % address).str();
        }

        // Save the entity.
        _mutex.lock();
        _registers[address] = entry;
        _mutex.unlock();
    }

    return entry;
}

uint32_t  PicturePerfect::MemoryStorage::getNumSlots() const {
    return (uint32_t)_registers.size();
}
