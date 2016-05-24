
#include <PicturePerfectEngine/Exception.h>

#include <Garnet/IVPairArchive.h>

namespace {
	void validateAddress(uint32_t address) {
		if ((address & ~PicturePerfect::ADDR_MASK_IVPAIR)) {
			PICTUREPERFECT_THROW(AddressOutOfRangeException, (boost::format("0x%08x is out of range for IVPairArchive.").str()));
		}
	}
}

Garnet::IVPairArchive::IVPairArchive(PicturePerfect::ArchivePtr archive)
: _archive(archive)
{

}

Garnet::IVPairArchive::~IVPairArchive()
{

}

const Garnet::IVPair  Garnet::IVPairArchive::pairAt(uint32_t address) const
{
	validateAddress(address);
	return IVPair(_archive, address);
}
Garnet::IVPair  Garnet::IVPairArchive::pairAt(uint32_t address)
{
	validateAddress(address);
	return IVPair(_archive, address);
}
Garnet::IVPair  Garnet::IVPairArchive::getPair(uint32_t address) const
{
	validateAddress(address);
	return IVPair(_archive, address);
}
void  Garnet::IVPairArchive::setPair(uint32_t address, const IVPair& value)
{
	validateAddress(address);
	auto dst = pairAt(address);
	dst.image() = value.image();
	dst.value() = value.value();
	dst.array() = value.array();
}

Garnet::IVPairArchivePtr Garnet::IVPairArchive::createInstance(const cv::Size& imageSize)
{
	return IVPairArchivePtr(new IVPairArchive(PicturePerfect::Archive::createInstance(imageSize)));
}
