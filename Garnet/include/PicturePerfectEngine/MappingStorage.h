
#ifndef PICTURE_PERFECT_ENGINE__MAPPING_STORAGE__H__
#define PICTURE_PERFECT_ENGINE__MAPPING_STORAGE__H__

#include <opencv2/opencv.hpp>

#include <PicturePerfectEngine/Storage.h>

namespace PicturePerfect {

//---------------------------------------------------------
//  MappingStorage
//---------------------------------------------------------
class  MappingStorage : public Storage {
public:
    struct Block {
        StoragePtr storage;
        uint32_t   logical;
        uint32_t   real;
        uint32_t   size;

        Block() {}
        Block(uint32_t l, StoragePtr st, uint32_t r, uint32_t s) : storage(st), logical(l), real(r), size(s) {}
        Block(const Block& x) : storage(x.storage), logical(x.logical), real(x.real), size(x.size) {}

        void set(uint32_t l, StoragePtr st, uint32_t r, uint32_t s) {
            storage = st;
            logical = l;
            real = r;
            size = s;
        }
    };
    typedef std::vector<Block> Blocks;

    virtual ~MappingStorage() {}

    virtual void             clear();
    virtual cv::Size         getImageSize() const;
    virtual uint32_t         getNumSlots() const;
    virtual DataSlot*        getSlot(uint32_t address);
    virtual const DataSlot*  getSlot(uint32_t address) const;

    static StoragePtr  createInstance(const Blocks& blocks);

protected:
    Blocks::iterator       findBlock(uint32_t address);
    Blocks::const_iterator findBlock(uint32_t address) const;
private:
    MappingStorage(const std::vector<Block>& blocks);

    Blocks      _blocks;
};

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__MAPPING_STORAGE__H__
