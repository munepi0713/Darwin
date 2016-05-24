#ifndef GARNET_IV_PAIR_ARCHIVE__H__
#define GARNET_IV_PAIR_ARCHIVE__H__

#include <memory>

#include <Common/IntTypes.h>

#include <PicturePerfectEngine/Archive.h>

namespace Garnet {

class IVPairArchive;
typedef std::shared_ptr<IVPairArchive> IVPairArchivePtr;

/**
 *  A data type which represents IVPair.
 *  It's named "pair", however, actually it is a structure containing more than two data inside.
 */
class IVPair
{
public :
	typedef cv::Mat                        IType;
	typedef double                         VType;
	typedef PicturePerfect::Vector<double> AType;

	IVPair(const IVPair& x) : _archive(x._archive), _index(x._index) {}
	IVPair(PicturePerfect::ArchivePtr archive, uint32_t index) : _archive(archive), _index(index) {}

	const IType& image() const { return _archive->imageAt(PicturePerfect::ADDR_I1B + _index); }
	      IType& image()       { return _archive->imageAt(PicturePerfect::ADDR_I1B + _index); }
	void         loadImage(const std::string& filename) { _archive->loadImage(PicturePerfect::ADDR_I1B + _index, filename); }
	void         saveImage(const std::string& filename) { _archive->saveImage(PicturePerfect::ADDR_I1B + _index, filename); }
	const IType& colorImage() const { return _archive->imageAt(PicturePerfect::ADDR_I3B + _index); }
          IType& colorImage()       { return _archive->imageAt(PicturePerfect::ADDR_I3B + _index); }
	void         loadColorImage(const std::string& filename) { _archive->loadImage(PicturePerfect::ADDR_I3B + _index, filename); }
	void         saveColorImage(const std::string& filename) { _archive->saveImage(PicturePerfect::ADDR_I3B + _index, filename); }
	const VType& value() const { return _archive->valueAt(PicturePerfect::ADDR_V1D + _index); }
	      VType& value()       { return _archive->valueAt(PicturePerfect::ADDR_V1D + _index); }
	const AType& array() const { return _archive->arrayAt(PicturePerfect::ADDR_A1D + _index); }
	      AType& array()       { return _archive->arrayAt(PicturePerfect::ADDR_A1D + _index); }

	template <typename I, typename V, typename A>
	void load(const I& ivalue, const V& vvalue, const A& avalue) {
		_archive->imageAt(ADDR_I1B + _index) = ivalue;
		_archive->valueAt(ADDR_V1D + _index) = vvalue;
		_archive->arrayAt(ADDR_A1D + _index) = avalue;
	}
	template <typename V, typename A>
	void load(const std::string& iname, const V& vvalue, const A& avalue) {
		_archive->loadImage(ADDR_I1B + _index, iname);
		_archive->valueAt(ADDR_V1D + _index) = vvalue;
		_archive->arrayAt(ADDR_A1D + _index) = avalue;
	}
	template <typename I, typename V, typename A>
	void loadColor(const I& ivalue, const V& vvalue, const A& avalue) {
		_archive->imageAt(ADDR_I3B + _index) = ivalue;
		_archive->valueAt(ADDR_V1D + _index) = vvalue;
		_archive->arrayAt(ADDR_A1D + _index) = avalue;
	}
	template <typename V, typename A>
	void loadColor(const std::string& iname, const V& vvalue, const A& avalue) {
		_archive->loadImage(ADDR_I3B + _index, iname);
		_archive->valueAt(ADDR_V1D + _index) = vvalue;
		_archive->arrayAt(ADDR_A1D + _index) = avalue;
	}

private :
	IVPair() : _archive(0), _index(0) {}

	PicturePerfect::ArchivePtr _archive;
	uint32_t                   _index;
};

/**
*  Archive to contain IVPairs.
*  This class requires for any addresses to have usage bit, not like as Storage/Archive allow usage-less address.
*/
class IVPairArchive
{
public :
	IVPairArchive(const IVPairArchive& x);
	virtual ~IVPairArchive();

	const IVPair           pairAt(uint32_t address) const;
	IVPair                 pairAt(uint32_t address);
	IVPair                 getPair(uint32_t address) const;
	void                   setPair(uint32_t address, const IVPair& value);

	PicturePerfect::ArchivePtr getArchive() { return _archive; }

	static IVPairArchivePtr createInstance(const cv::Size& imageSize);

private :
	IVPairArchive(PicturePerfect::ArchivePtr archive);

	PicturePerfect::ArchivePtr _archive;
};

}// end of namespace Garnet


#endif//GARNET_IV_PAIR_ARCHIVE__H__

