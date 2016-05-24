#ifndef PICTURE_PERFECT_OPEN_CV__H__
#define PICTURE_PERFECT_OPEN_CV__H__

#include <stdint.h>

#include <cassert>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>

#include <PicturePerfectEngine/Engine.h>

namespace Sakura
{

/**
 *
 */
class Engine : public PicturePerfect::PicturePerfectEngine
{
public :
 	Engine(bool enableOpenCL = true);
	virtual ~Engine();

	// Execution
	virtual void execute(const PicturePerfect::ExecutablePtr executable, std::map<uint32_t, PicturePerfect::StoragePtr>& storages, InfoContainer& info);
	virtual void rehearse(const cv::Size& imageSize);

	// Utility
	virtual uint16_t           getOpCode(const std::string& opname) const;
	virtual uint16_t           getMaxOpCode() const;
	virtual const std::string& getOpName(uint16_t opcode) const;
	virtual uint16_t           getNumOperands(uint16_t opcode) const;
	virtual const Statistics&  getStatistics(uint16_t opcode) const { return _statistics[opcode]; }

protected :
	static  void  initialize(const std::string& filter = "*");
	static  void  finalize();

private :
	PicturePerfect::ArchivePtr  _registers;
	std::vector<Statistics>     _statistics;
};

}//end of namespace Sakura

#endif//PICTURE_PERFECT_OPEN_CV__H__
