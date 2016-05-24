
// Avoid annoying VC's warning.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif//_CRT_SECURE_NO_WARNINGS

#include <fstream>

#include <Common/Path.h>
#include <Common/Text.h>

#include <PicturePerfectEngine/Engine.h>
#include <PicturePerfectEngine/SakuraEngine.h>

namespace impl
{
std::wstring _baseDirectory;
}


//---------------------------------------------------------
//---------------------------------------------------------

PicturePerfect::PicturePerfectEngine::PicturePerfectEngine()
{
}

PicturePerfect::PicturePerfectEngine::~PicturePerfectEngine()
{
}


/**
 *  Destroy instance.
 */
void  PicturePerfect::PicturePerfectEngine::destroy()
{
    delete this;
}

void      PicturePerfect::PicturePerfectEngine::setBaseDirectory(const std::wstring& dir)
{
	impl::_baseDirectory = dir;
}
const std::wstring& PicturePerfect::PicturePerfectEngine::getBaseDirectory()
{
	return impl::_baseDirectory;
}
std::wstring PicturePerfect::PicturePerfectEngine::resolvePath(const std::wstring& path)
{
	return Path::resolvePath(path, impl::_baseDirectory);
}

void PicturePerfect::PicturePerfectEngine::configure(std::istream& ins)
{
	while ( !ins.eof() && ins.good() ) {
		std::string line;
		std::getline(ins, line);
		line = Text::removeAfter(Text::trimLeft(line), "#");
		if ( line.empty() ) continue;
			
		strings tokens = parse(line);
		if ( Text::equalsWithoutCase(tokens[0], "Size") ) {
			if ( tokens.size() < 3 ) {
				PICTUREPERFECT_THROW(NumberOfArgsException, "Size command requires 3 arguments.");
			}
			//this->setImageSize(cvSize(strtoul(tokens[1].c_str(), 0, 0), strtoul(tokens[2].c_str(), 0, 0)));
		}
	}
}

/**
 *  Dummy rehearsal routine.
 */
void PicturePerfect::PicturePerfectEngine::rehearse(const cv::Size& imageSize)
{
	(void)imageSize;
}

/**
 *  Create instance of specified implementation type.
 */
PicturePerfect::PicturePerfectEnginePtr PicturePerfect::PicturePerfectEngine::createInstance(const std::string& type)
{
    strings tokens = parse(type);
	bool openCL;

#if defined(GARNET_ENABLE_OPENCL__)
	openCL = true;
	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		if ( Text::equals(*it, "ocl") ) {
			openCL = true;
		}
		else if ( Text::equals(*it, "noocl") ) {
			openCL = false;
		}
	}
#else
	openCL = false;
#endif

	PicturePerfectEngine* ppe = new Sakura::Engine(openCL);

	return PicturePerfectEnginePtr(ppe);
}
