//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#include <PicturePerfectEngine/Archive.h>

#include <boost/format.hpp>

#include <opencv2/opencv.hpp>
#if defined(GARNET_ENABLE_OPENCL__)
#include <opencv2/ocl/ocl.hpp>
#endif

#include <Common/Text.h>
#include <Common/Parse.h>

#include <PicturePerfectEngine/Exception.h>
#include <PicturePerfectEngine/Engine.h>

namespace
{
using namespace PicturePerfect;

void processArchiveScript(std::istream& ins, PicturePerfect::Archive& archive, bool isLoading)
{
	while ( !ins.eof() ) {
		std::string line;
		std::getline(ins, line);
		line = Text::removeAfter(Text::trimLeft(line), "#");
		if ( line.empty() ) continue;

		strings tokens = parse(line);
		if ( tokens.size() < 2 ) {
			PICTUREPERFECT_THROW(NumberOfArgsException, "Not enough number of token: " + line);
		}

		// Decode address.
		uint32_t address = PicturePerfect::Storage::decodeAddress(tokens[0]);
		uint32_t storageUsage = getStorageUsageType(address);
		uint32_t storageType  = getStorageType(address);

		// Parse argument.
		if ( isLoading ) {
			if ( storageUsage == USAGE_SRC ) {
				if ( tokens[1][0] == '@' ) {
					switch ( storageType ) {
					case TYPE_I1B :
					case TYPE_I3B :
					case TYPE_I1Bg :
					case TYPE_I3Bg :
						archive.loadImage(address, Text::toMultiByte(PicturePerfectEngine::resolvePath(Text::toWideChar(tokens[1].substr(1)))));
						break;
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
				else if ( Text::equals(tokens[1], "__BLACK__") ) {
					switch ( storageType ) {
					case TYPE_V1D :
					case TYPE_V1Dg :
						archive.setValue(address, 0);
						break;
					case TYPE_I1B :
					case TYPE_I3B :
						archive.imageAt(address) = cv::Scalar(0, 0, 0);
						break;
#if defined(GARNET_ENABLE_OPENCL__)
					case TYPE_SI1Bg :
					case TYPE_SI3Bg :
						archive.at<cv::ocl::oclMat>(address).setTo(cv::Scalar(0, 0, 0));
						break;
#endif
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
				else if ( Text::equals(tokens[1], "__WHITE__") ) {
					switch ( storageType ) {
					case TYPE_V1D :
					case TYPE_V1Dg :
						archive.setValue(address, 0);
						break;
					case TYPE_I1B :
					case TYPE_I3B :
						archive.imageAt(address) = cv::Scalar(255, 255, 255);
						break;
#if defined(GARNET_ENABLE_OPENCL__)
					case TYPE_SI1Bg :
					case TYPE_SI3Bg :
						archive.at<cv::ocl::oclMat>(address).setTo(cv::Scalar(255, 255, 255));
						break;
#endif
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
				else if ( Text::equals(tokens[1], "__EMPTY__") ) {
					switch ( storageType ) {
					case TYPE_A1D :
					case TYPE_V1Dg :
						archive.arrayAt(address).clear();
						break;
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
				else {
					switch ( storageType ) {
					case TYPE_V1D :
					case TYPE_V1Dg :
						archive.setValue(address, strtod(tokens[1].c_str(), 0));
						break;
					case TYPE_S :
						archive.setString(address, Text::toWideChar(tokens[1]));
						break;
					case TYPE_A1D :
						{
							Vector<double>& v = archive.arrayAt(address);
							v.clear();
							for (auto i = 1u; i < tokens.size(); i++) {
								v.push_back(strtod(tokens[i].c_str(), 0));
							}
						}
						break;
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
			}
			else if ( storageUsage == USAGE_RES ) {
				// Create entry by accessing here.
				archive.getSlot(address);
			}
		}
		else {
			if ( storageUsage == USAGE_RES ) {
				if ( tokens[1][0] == '@' ) {
					switch ( storageType ) {
					case TYPE_I1B :
					case TYPE_I3B :
					case TYPE_I1Bg :
					case TYPE_I3Bg :
						archive.saveImage(address, Text::toMultiByte(PicturePerfectEngine::resolvePath(Text::toWideChar(tokens[1].substr(1)))));
						break;
					default :
						PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("Command \"%s\" failed due to invalid storage access at 0x%x. ") % line % address).str());
					}
				}
			}
		}
	}
}
}//end of local namespace

//---------------------------------------------------------
//  Archive
//---------------------------------------------------------
PicturePerfect::Archive::Archive(StoragePtr storage)
: StorageDecorator(storage) {
}

PicturePerfect::Archive::~Archive() {
}

void  PicturePerfect::Archive::clear() {
	getStorage()->clear();
}

cv::Size  PicturePerfect::Archive::getImageSize() const {
	return getStorage()->getImageSize();
}

uint32_t  PicturePerfect::Archive::getNumSlots() const {
    return getStorage()->getNumSlots();
}

PicturePerfect::DataSlot*  PicturePerfect::Archive::getSlot(uint32_t address) {
    return getStorage()->getSlot(address);
}

const PicturePerfect::DataSlot*  PicturePerfect::Archive::getSlot(uint32_t address) const {
	return getStorage()->getSlot(address);
}

const double& PicturePerfect::Archive::valueAt(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_V1D :
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_V1Dg :
#endif
		return at<double>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

double& PicturePerfect::Archive::valueAt(uint32_t address)
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_V1D :
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_V1Dg :
#endif
		return at<double>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

double PicturePerfect::Archive::getValue(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_V1D :
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_V1Dg :
#endif
		return at<double>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::setValue(uint32_t address, double value)
{
	// Set the value to a buffer pointed by the address.
	switch (getStorageType(address)) {
	case TYPE_V1D :
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_V1Dg :
#endif
		at<double>(address) = value;
		return;
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}

}

const cv::Mat& PicturePerfect::Archive::imageAt(uint32_t address) const
{
	// Get the image from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
	case TYPE_I3B :
		return at<cv::Mat>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

cv::Mat& PicturePerfect::Archive::imageAt(uint32_t address)
{
	// Get the image from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
	case TYPE_I3B :
		return at<cv::Mat>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const cv::Mat& PicturePerfect::Archive::getImage(uint32_t address) const
{
	// Get the image from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
	case TYPE_I3B :
		return at<cv::Mat>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::loadImage(uint32_t address, const std::string& filename)
{
	// Set the value to a buffer pointed by the address.
	cv::Mat src;
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
		src = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		break;
	case TYPE_I3B :
		src = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
		break;
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_I1Bg :
		src = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		break;
	case TYPE_I3Bg :
		src = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
		break;
#endif
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
	if ( src.empty() ) {
		PICTUREPERFECT_THROW(FileOpenFailureException, (boost::format(" loading from file %s.") % filename).str());
	}

	cv::Mat tmp;
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
	case TYPE_I3B :
		cv::resize(src, at<cv::Mat>(address), getImageSize());
		return;
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_I1Bg :
	case TYPE_I3Bg :
		cv::resize(src, tmp, getImageSize());
		at<cv::ocl::oclMat>(address).upload(tmp);
		return;
#endif
	}

}

void PicturePerfect::Archive::saveImage(uint32_t address, const std::string& filename)
{
	// Set the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_I1B :
	case TYPE_I3B :
		if ( !cv::imwrite(filename, at<cv::Mat>(address)) || cvGetErrStatus() != CV_NO_ERR ) {
			PICTUREPERFECT_THROW(FileOpenFailureException, (boost::format(" saving to file %s. (CVStatus=%d)") % filename % cvGetErrStatus()).str());
		}
		return;
#if defined(GARNET_ENABLE_OPENCL__)
	case TYPE_I1Bg :
	case TYPE_I3Bg :
		{
			cv::Mat tmp;
			at<cv::ocl::oclMat>(address).download(tmp);
			if ( !cv::imwrite(filename, tmp) || cvGetErrStatus() != CV_NO_ERR ) {
				PICTUREPERFECT_THROW(FileOpenFailureException, (boost::format(" saving to file %s. (CVStatus=%d)") % filename % cvGetErrStatus()).str());
			}
		}
		return;
#endif
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const std::wstring& PicturePerfect::Archive::stringAt(uint32_t address) const
{
	// Get the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_S :
		return at<std::wstring>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

std::wstring& PicturePerfect::Archive::stringAt(uint32_t address)
{
	// Get the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_S :
		return at<std::wstring>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const std::wstring& PicturePerfect::Archive::getString(uint32_t address) const
{
	// Get the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_S :
		return at<std::wstring>(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::setString(uint32_t address, const std::wstring& value)
{
	// Set the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_S :
		at<std::wstring>(address) = value;
		return;
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}

}

const Vector<double>& PicturePerfect::Archive::arrayAt(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<double> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

Vector<double>& PicturePerfect::Archive::arrayAt(uint32_t address)
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<double> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const Vector<double>& PicturePerfect::Archive::getArray(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<double> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::setArray(uint32_t address, const Vector<double>& value)
{
	// Set the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		at<Vector<double> >(address) = value;
		return;
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const Vector<uint32_t>& PicturePerfect::Archive::listAt(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<uint32_t> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

Vector<uint32_t>& PicturePerfect::Archive::listAt(uint32_t address)
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<uint32_t> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

const Vector<uint32_t>& PicturePerfect::Archive::getList(uint32_t address) const
{
	// Get the value from a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		return at<Vector<uint32_t> >(address);
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::setList(uint32_t address, const Vector<uint32_t>& value)
{
	// Set the value to a buffer pointed by the address.
	switch ( getStorageType(address) ) {
	case TYPE_A1D :
		at<Vector<uint32_t> >(address) = value;
		return;
	default :
		PICTUREPERFECT_THROW(InvalidStorageAccessException, (boost::format("%s failed due to invalid storage access at 0x%x. ") % __FUNCTION__ % address).str());
	}
}

void PicturePerfect::Archive::load(std::istream& ins)
{
	processArchiveScript(ins, *this, true);
}

void PicturePerfect::Archive::save(std::istream& ins)
{
	processArchiveScript(ins, *this, false);
}

PicturePerfect::ArchivePtr PicturePerfect::Archive::createInstance(const cv::Size& imageSize)
{
	PicturePerfect::Archive* ptr = new PicturePerfect::Archive(PicturePerfect::Storage::createInstance(imageSize));
	return PicturePerfect::ArchivePtr(ptr);
}

PicturePerfect::ArchivePtr PicturePerfect::Archive::createInstance(StoragePtr storage)
{
	PicturePerfect::Archive* ptr = new PicturePerfect::Archive(storage);
	return PicturePerfect::ArchivePtr(ptr);
}
