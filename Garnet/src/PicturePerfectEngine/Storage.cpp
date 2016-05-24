//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#include <PicturePerfectEngine/Storage.h>

#include <boost/format.hpp>

#include <opencv2/opencv.hpp>
#if defined(GARNET_ENABLE_OPENCL__)
#include <opencv2/ocl/ocl.hpp>
#endif

#include <Common/Text.h>

#include <PicturePerfectEngine/Vector.h>
#include <PicturePerfectEngine/Exception.h>
#include <PicturePerfectEngine/MemoryStorage.h>

namespace
{

static std::map<uint32_t, const char*> _dataTypeName;
static std::map<std::string, uint32_t> _type2addr;
static std::set<uint32_t> _addrset;

void initTypeAddrMap()
{
	_dataTypeName[PicturePerfect::DATA_V1D] = "v1d";
	_dataTypeName[PicturePerfect::DATA_I1D] = "i1d";
	_dataTypeName[PicturePerfect::DATA_I3D] = "i3d";
	_dataTypeName[PicturePerfect::DATA_I1B] = "i1b";
	_dataTypeName[PicturePerfect::DATA_I3B] = "i3b";
	_dataTypeName[PicturePerfect::DATA_S]   = "s";
	_dataTypeName[PicturePerfect::DATA_A1D] = "a1d";
	_dataTypeName[PicturePerfect::DATA_L]   = "l";

#pragma warning(push)
#pragma warning(disable: 4127) // Constant condition
#define _ENTRY(name)  do { \
	_type2addr[#name] = PicturePerfect::ADDR_ ## name; \
	_addrset.insert(PicturePerfect::ADDR_ ## name); \
} while (false)
	_ENTRY(V1D);
	_ENTRY(I1D);
	_ENTRY(I3D);
	_ENTRY(I1B);
	_ENTRY(I3B);
	_ENTRY(S);
	_ENTRY(A1D);
	_ENTRY(L);
	_ENTRY(SV1D);
	_ENTRY(SI1D);
	_ENTRY(SI3D);
	_ENTRY(SI1B);
	_ENTRY(SI3B);
	_ENTRY(SS);
	_ENTRY(SA1D);
	_ENTRY(RV1D);
	_ENTRY(RI1D);
	_ENTRY(RI3D);
	_ENTRY(RI1B);
	_ENTRY(RI3B);
	_ENTRY(RA1D);
	_ENTRY(V1Dg);
	_ENTRY(I1Dg);
	_ENTRY(I3Dg);
	_ENTRY(I1Bg);
	_ENTRY(I3Bg);
	_ENTRY(SV1Dg);
	_ENTRY(SI1Dg);
	_ENTRY(SI3Dg);
	_ENTRY(SI1Bg);
	_ENTRY(SI3Bg);
	_ENTRY(RV1Dg);
	_ENTRY(RI1Dg);
	_ENTRY(RI3Dg);
	_ENTRY(RI1Bg);
	_ENTRY(RI3Bg);
#undef _ENTRY
#pragma warning(pop)
}

uint32_t getAddressFromType(const std::string& type)
{
	if (_type2addr.empty()) {
		initTypeAddrMap();
	}

	if (_type2addr.find(type) == _type2addr.end()) {
		return 0;
	}
	else {
		return _type2addr[type];
	}
}

bool isValidAddress(uint32_t address)
{
	if (_addrset.empty()) {
		initTypeAddrMap();
	}

	return _addrset.find(address & 0xFFF00000u) != _addrset.end();
}

}//end of namespace impl

//---------------------------------------------------------
//  Storage
//---------------------------------------------------------

PicturePerfect::Storage::~Storage() {
    clear();
}

uint32_t  PicturePerfect::Storage::decodeAddress(const std::string& exp) {
	// Decode address.
	uint32_t address;
	if (exp[exp.length() - 1] == ')') {
		// Indirect.
		uint32_t type;
		uint32_t subaddr;
		std::string::size_type pos = exp.find_first_of('(');
		if (pos == std::string::npos) {
			PICTUREPERFECT_THROW(InvalidNumberFormatException, exp);
		}
		type = getAddressFromType(exp.substr(0, pos));
		if (type == 0) {
			PICTUREPERFECT_THROW(InvalidNumberFormatException, exp);
		}
		subaddr = strtoul(Text::trimLeft(exp.substr(pos + 1)).c_str(), 0, 0);
		if (subaddr > 0x3FFFFu) {
			PICTUREPERFECT_THROW(AddressOutOfRangeException, exp);
		}
		address = type + subaddr;
	}
	else {
		// Direct number.
		address = strtoul(exp.c_str(), 0, 0);
		if (!isValidAddress(address)) {
			PICTUREPERFECT_THROW(AddressOutOfRangeException, exp);
		}
	}
	return address;
}

bool  PicturePerfect::Storage::exists(uint32_t address) const {
	const DataSlot* slot = getSlot(address);
	return slot != 0;
}

bool  	PicturePerfect::Storage::isValid(uint32_t address) const {
	// Get entry.
	const DataSlot* entry = getSlot(address);
	if (entry == 0) {
		return false;
	}

	// Check specified entry is free.
	if (!entry) {
		// Create new entity for id.
		switch (getStorageType(address)) {
		case TYPE_V1D: {
			auto p = dynamic_cast<const DataSlotT<double>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for V1D at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			break;
		}
		case TYPE_I1D: {
			auto p = dynamic_cast<const DataSlotT<cv::Mat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I1D at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::Mat>().type();
			if (type != CV_64FC1) {
				throw (boost::format("[%s] Invalid format 0x%08x for I1D at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I3D: {
			auto p = dynamic_cast<const DataSlotT<cv::Mat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I3D at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::Mat>().type();
			if (type != CV_64FC3) {
				throw (boost::format("[%s] Invalid format 0x%08x for I3D at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I1B: {
			auto p = dynamic_cast<const DataSlotT<cv::Mat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I1B at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::Mat>().type();
			if (type != CV_8UC1) {
				throw (boost::format("[%s] Invalid format 0x%08x for I1B at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I3B: {
			auto p = dynamic_cast<const DataSlotT<cv::Mat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I3B at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::Mat>().type();
			if (type != CV_8UC3) {
				throw (boost::format("[%s] Invalid format 0x%08x for I3B at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_S: {
			auto p = dynamic_cast<const DataSlotT<std::wstring>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for S at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			entry = new DataSlotT<std::wstring>(L"");
			break;
		}
		case TYPE_A1D: {
			auto p = dynamic_cast<const DataSlotT<PicturePerfect::Vector<double> >*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for A1D at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			break;
		}
		case TYPE_L: {
			auto p = dynamic_cast<const DataSlotT<PicturePerfect::Vector<uint32_t> >*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for L at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			break;
		}
		case TYPE_V1Dg: {
			auto p = dynamic_cast<const DataSlotT<double>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for V1D at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			break;
		}
#if defined(GARNET_ENABLE_OPENCL__)
		case TYPE_I1Dg: {
			auto p = dynamic_cast<const DataSlotT<cv::ocl::oclMat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I1Dg at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::ocl::oclMat>().type();
			if (type != CV_64FC1) {
				throw (boost::format("[%s] Invalid format 0x%08x for I1Dg at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I3Dg: {
			auto p = dynamic_cast<const DataSlotT<cv::ocl::oclMat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I3Dg at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::ocl::oclMat>().type();
			if (type != CV_64FC3) {
				throw (boost::format("[%s] Invalid format 0x%08x for I3Dg at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I1Bg: {
			auto p = dynamic_cast<const DataSlotT<cv::ocl::oclMat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I1Bg at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::ocl::oclMat>().type();
			if (type != CV_8UC1) {
				throw (boost::format("[%s] Invalid format 0x%08x for I1Bg at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
		case TYPE_I3Bg: {
			auto p = dynamic_cast<const DataSlotT<cv::ocl::oclMat>*>(entry);
			if (p == 0) {
				throw (boost::format("[%s] Invalid instance for I3Bg at address 0x%08x.") % __FUNCTION__ % address).str();
			}
			int type = p->at<cv::ocl::oclMat>().type();
			if (type != CV_8UC3) {
				throw (boost::format("[%s] Invalid format 0x%08x for I3Bg at address 0x%08x.") % __FUNCTION__ % type % address).str();
			}
			break;
		}
#endif
		}
	}

	// Pass.
	return true;
}

const char* PicturePerfect::Storage::toDataTypeName(uint32_t dataType) {
	if (_dataTypeName.empty()) {
		initTypeAddrMap();
	}

	return _dataTypeName[getStorageDataType(dataType)];
}

PicturePerfect::StoragePtr PicturePerfect::Storage::createInstance(const cv::Size& imageSize) {
	PicturePerfect::Storage* ptr = new PicturePerfect::MemoryStorage(imageSize);
	return PicturePerfect::StoragePtr(ptr);
}
