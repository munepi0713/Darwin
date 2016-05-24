#ifndef GARNET__DICTIONARY__H__
#define GARNET__DICTIONARY__H__

#include <string>
#include <cstdint>
#include <numeric>

#include <PicturePerfectEngine/Storage.h>

#define NUM_PP_PER_FILTER (3)
#define MAX_INPUTS  (6)
#define MAKE_PP_INDEX(ivIndex, ppType)  ((ivIndex) * NUM_PP_PER_FILTER + (ppType))
#define MAKE_PP_INDEX_I(ivIndex)  MAKE_PP_INDEX((ivIndex), 0)
#define MAKE_PP_INDEX_V(ivIndex)  MAKE_PP_INDEX((ivIndex), 1)
#define MAKE_PP_INDEX_A(ivIndex)  MAKE_PP_INDEX((ivIndex), 2)
#define _0i  MAKE_PP_INDEX(0, 0)
#define _0v  MAKE_PP_INDEX(0, 1)
#define _0a  MAKE_PP_INDEX(0, 2)
#define _1i  MAKE_PP_INDEX(1, 0)
#define _1v  MAKE_PP_INDEX(1, 1)
#define _1a  MAKE_PP_INDEX(1, 2)
#define _2i  MAKE_PP_INDEX(2, 0)
#define _2v  MAKE_PP_INDEX(2, 1)
#define _2a  MAKE_PP_INDEX(2, 2)
#define _3i  MAKE_PP_INDEX(3, 0)
#define _3v  MAKE_PP_INDEX(3, 1)
#define _3a  MAKE_PP_INDEX(3, 2)
#define _4i  MAKE_PP_INDEX(4, 0)
#define _4v  MAKE_PP_INDEX(4, 1)
#define _4a  MAKE_PP_INDEX(4, 2)
#define _5i  MAKE_PP_INDEX(5, 0)
#define _5v  MAKE_PP_INDEX(5, 1)
#define _5a  MAKE_PP_INDEX(5, 2)
#define _NULL (std::numeric_limits<uint32_t>::max())

#define _V1D  (PicturePerfect::ADDR_V1D  >> 24)
#define _I1D  (PicturePerfect::ADDR_I1D  >> 24)
#define _I3D  (PicturePerfect::ADDR_I3D  >> 24)
#define _I1B  (PicturePerfect::ADDR_I1B  >> 24)
#define _I3B  (PicturePerfect::ADDR_I3B  >> 24)
#define _S    (PicturePerfect::ADDR_S    >> 24)
#define _A1D  (PicturePerfect::ADDR_A1D  >> 24)
#define _SV1D (PicturePerfect::ADDR_SV1D >> 24)
#define _SI1D (PicturePerfect::ADDR_SI1D >> 24)
#define _SI3D (PicturePerfect::ADDR_SI3D >> 24)
#define _SI1B (PicturePerfect::ADDR_SI1B >> 24)
#define _SI3B (PicturePerfect::ADDR_SI3B >> 24)
#define _SS   (PicturePerfect::ADDR_SS   >> 24)
#define _SA1D (PicturePerfect::ADDR_SA1D >> 24)
#define _RV1D (PicturePerfect::ADDR_RV1D >> 24)
#define _RI1D (PicturePerfect::ADDR_RI1D >> 24)
#define _RI3D (PicturePerfect::ADDR_RI3D >> 24)
#define _RI1B (PicturePerfect::ADDR_RI1B >> 24)
#define _RI3B (PicturePerfect::ADDR_RI3B >> 24)
#define _RA1D (PicturePerfect::ADDR_RA1D >> 24)
#define IS_SRC(x) (getStorageUsage((x) << 24) == PicturePerfect::USAGE_SRC)
#define IS_RES(x) (getStorageUsage((x) << 24) == PicturePerfect::USAGE_RES)
#define _END  (std::numeric_limits<uint32_t>::max())

namespace Garnet
{

struct PPInstruction
{
    std::string  label;             // 
	uint32_t     outputType;        //
    uint32_t     input[MAX_INPUTS]; // 
};

struct IVInstruction
{
    std::string   iv;
    PPInstruction pp[3]; // 0:Image, 1:Value, 2:Array
};

class Dictionary
{
public :
	static const IVInstruction& findByIVName(const std::string& label);
private :
	Dictionary();
	static Dictionary* getInstance();

	typedef std::map<std::string, const Garnet::IVInstruction*> IVInstructionMap;
	IVInstructionMap _ivInstructionMap;
};

}//end of namespace Garnet

#endif//GARNET__DICTIONARY__H__
