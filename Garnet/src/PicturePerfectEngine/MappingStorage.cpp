//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#include <PicturePerfectEngine/MappingStorage.h>

#include <PicturePerfectEngine/Exception.h>

//---------------------------------------------------------
//  MappingStorage
//---------------------------------------------------------
namespace {

uint32_t convertToRealAddress(const PicturePerfect::MappingStorage::Block& block, uint32_t address) {
    return block.real + (address - block.logical);
}

}

PicturePerfect::MappingStorage::MappingStorage(const Blocks& blocks)
: _blocks(blocks) {}

void  PicturePerfect::MappingStorage::clear() {
    // Actual data won't be cleared.
    _blocks.clear();
}

PicturePerfect::DataSlot*  PicturePerfect::MappingStorage::getSlot(uint32_t address) {
    auto block = findBlock(address);
    if (block == _blocks.end()) {
        PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Logical address 0x%08x is not mapped.") % address).str());
    }
    return block->storage->getSlot(convertToRealAddress(*block, address));
}

const PicturePerfect::DataSlot*  PicturePerfect::MappingStorage::getSlot(uint32_t address) const {
    auto block = findBlock(address);
    if (block == _blocks.end()) {
        PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Logical address 0x%08x is not mapped.") % address).str());
    }
    return block->storage->getSlot(convertToRealAddress(*block, address));
}

cv::Size  PicturePerfect::MappingStorage::getImageSize() const {
    assert(!_blocks.empty());
    return _blocks.front().storage->getImageSize();
}

uint32_t  PicturePerfect::MappingStorage::getNumSlots() const {
    uint32_t num = 0;
    for (auto it = _blocks.begin(); it != _blocks.end(); it++) {
        num += it->storage->getNumSlots();
    }
    return num;
}

PicturePerfect::MappingStorage::Blocks::iterator PicturePerfect::MappingStorage::findBlock(uint32_t address) {
    auto it = _blocks.begin();
    while (it != _blocks.end()) {
        if (address >= it->logical && address < it->logical + it->size) {
            break;//return it;
        }
        it++;
    }
    return it;
    //PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Logical address 0x%08x is not mapped.") % address).str());
}

PicturePerfect::MappingStorage::Blocks::const_iterator PicturePerfect::MappingStorage::findBlock(uint32_t address) const {
    auto it = _blocks.cbegin();
    while (it != _blocks.cend()) {
        if (address >= it->logical && address < it->logical + it->size) {
            break;//return it;
        }
        it++;
    }
    return it;
    //PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Logical address 0x%08x is not mapped.") % address).str());
}


PicturePerfect::StoragePtr PicturePerfect::MappingStorage::createInstance(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        PICTUREPERFECT_THROW(InvalidStorageOperationException, "MappingStorage requires at least one mapping block.");
    }

    return PicturePerfect::StoragePtr(new PicturePerfect::MappingStorage(blocks));
}
