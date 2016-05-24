	
// Avoid annoying VC's warning.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Common/Text.h>

#include <PicturePerfectEngine/SakuraExecutionUnits.h>

namespace Sakura
{

static bool _isInitialized = false;
static Sakura::NI  _ni; // Not implemented.
static Sakura::MU  _mu;
static Sakura::VF  _vf;
static Sakura::IF  _if;
static Sakura::AF  _af;

std::vector<InstructionIssue> _unitMap;
std::map<std::string, OpInfo> _opcodeMap;
std::map<uint16_t, std::string> _opcodeMapInv;
std::set<std::string> _voidAliases; // Aliases with no return value.

#if defined(GARNET_ENABLE_OPENCL__)
static Sakura::IFg _ifg;
static std::vector<cv::ocl::Info>       _oclinfo;
#endif

void  initialize(bool enableOpenCL)
{
	if ( _isInitialized ) return;

	bool isOclOk;
#if !defined(GARNET_ENABLE_OPENCL__)
	(void)enableOpenCL;
	isOclOk = false;
#else
	// Initialize OpenCL runtime.
	isOclOk = true;
	if ( enableOpenCL ) {
		int numDevices;
		std::cerr << "Searching OpenCL GPU devices ... ";
		numDevices = cv::ocl::getDevice(_oclinfo, cv::ocl::CVCL_DEVICE_TYPE_GPU);
		if ( numDevices == 0 ) {
			std::cerr << "None.\n";
			std::cerr << "Searching OpenCL CPU devices ... ";
			numDevices = cv::ocl::getDevice(_oclinfo, cv::ocl::CVCL_DEVICE_TYPE_CPU);
			if ( numDevices == 0 ) {
				std::cerr << "None.\n";
				std::cerr << "OpenCL disabled.\n";
				isOclOk = false;
			}
		}
		if ( isOclOk ) {	
			std::cerr << numDevices << " devices detected." << std::endl;
			{//if ( isTrace() ) {
				for (unsigned i = 0; i < _oclinfo.size(); i++) {
					for (unsigned j = 0; j < _oclinfo[i].DeviceName.size(); j++) {
						std::cerr << boost::format("  [%d:%d] %s\n") % i % j % _oclinfo[i].DeviceName[j];
					}
					std::cerr << std::flush;
				}
			}
		}
	}
	else {
		isOclOk = false;
	}
#endif

	// Prepare execution units.
	uint16_t opcode = 0;
	#pragma warning(push)
	#pragma warning(disable:4127)
	#define DECL(unit, index, mnemonic) do {\
			_unitMap.push_back(InstructionIssue((unit), index));\
			_opcodeMap[#mnemonic] = OpInfo(opcode, (uint16_t)Text::countChar(#mnemonic, '_'));\
			_opcodeMapInv[opcode] = #mnemonic;\
			opcode++;\
		} while ( false )

	DECL(_mu, 0, end);
	DECL(_mu, 1, start);
	DECL(_mu, 2, printhash_v1d);
	DECL(_mu, 3, printhash_i1b);
	DECL(_mu, 4, printhash_i3b);
	DECL(_mu, 5, printhash_a1d);
	DECL(_mu, 6, printvalue_v1d);
	DECL(_mu, 7, printvalue_s);
	DECL(_mu, 8, printvalue_a1d);

	DECL(_vf, 0, v1d_zero);
	DECL(_vf, 1, v1d_move_v1d);
	DECL(_vf, 2, v1d_avgpixel_i1b);
	DECL(_vf, 3, v1d_maxpixel_i1b);
	DECL(_vf, 4, v1d_minpixel_i1b);
	DECL(_vf, 5, v1d_fgarea_i1b);
	DECL(_vf, 6, v1d_bgarea_i1b);
	DECL(_vf, 7, v1d_fgareav_i1b_v1d);
	DECL(_vf, 8, v1d_bgareav_i1b_v1d);
	DECL(_vf, 9, v1d_masscenterbx_i1b);
	DECL(_vf, 10, v1d_masscenterby_i1b);
	DECL(_vf, 11, v1d_psnr_i1b_i1b);
	DECL(_vf, 12, v1d_vnegate_v1d);
	DECL(_vf, 13, v1d_vadd_v1d_v1d);
	DECL(_vf, 14, v1d_valgebraicprod_v1d_v1d);
	DECL(_vf, 15, v1d_valgebraicsum_v1d_v1d);
	DECL(_vf, 16, v1d_vdiff_v1d_v1d);
	DECL(_vf, 17, v1d_vlogicalprod_v1d_v1d);
	DECL(_vf, 18, v1d_vlogicalsum_v1d_v1d);
	DECL(_vf, 19, v1d_vsubtract_v1d_v1d);
	DECL(_vf, 20, v1d_distance_v1d_v1d_v1d_v1d);
	DECL(_vf, 21, v1d_select_v1d_v1d);
	DECL(_vf, 22, v1d_select_v1d_v1d_v1d_v1d);
	DECL(_vf, 23, v1d_adjust_v1d);
	DECL(_vf, 24, v1d_averagea_a1d);
	DECL(_vf, 25, v1d_invert_v1d);
	DECL(_vf, 26, v1d_maxa_a1d);
	DECL(_vf, 27, v1d_mina_a1d);
	DECL(_vf, 28, v1d_msea_a1d_v1d);
	DECL(_vf, 29, v1d_numa_a1d);
	DECL(_vf, 30, v1d_numeqa_a1d_v1d);
	DECL(_vf, 31, v1d_numgreatera_a1d_v1d);
	DECL(_vf, 32, v1d_numgreatereqa_a1d_v1d);
	DECL(_vf, 33, v1d_numlessa_a1d_v1d);
	DECL(_vf, 34, v1d_numlesseqa_a1d_v1d);
	DECL(_vf, 35, v1d_percentv_v1d_v1d);
	DECL(_vf, 36, v1d_rmsa_a1d);
	DECL(_vf, 37, v1d_sqrt_v1d);
	DECL(_vf, 38, v1d_stdeva_a1d);
	DECL(_vf, 39, v1d_suma_a1d);
	DECL(_vf, 40, v1d_vaddv_v1d_v1d);
	DECL(_vf, 41, v1d_vdivv_v1d_v1d);
	DECL(_vf, 42, v1d_vmultiplyv_v1d_v1d);
	DECL(_vf, 43, v1d_vsignflip_v1d);
	DECL(_vf, 44, v1d_vexp_v1d);
	DECL(_vf, 45, v1d_vinvertc_v1d);
	DECL(_vf, 46, v1d_maskpsnr_i1b_i1b_i1b);
	DECL(_vf, 47, v1d_noextrememaskpsnr_i1b_i1b_i1b);

	DECL(_if, 0, i1b_zero);
	DECL(_if, 1, i3b_zero);
	DECL(_if, 2, i1b_move_i1b);
	DECL(_if, 3, i3b_move_i3b);
	DECL(_if, 4, i1b_split1st_i3b);
	DECL(_if, 5, i1b_split2nd_i3b);
	DECL(_if, 6, i1b_split3rd_i3b);
	DECL(_if, 7, i3b_bgr2hsv_i3b);
	DECL(_if, 8, i3b_bgr2yuv_i3b);
	DECL(_if, 9, i1b_bgr2gray_i3b);
	DECL(_if, 10, i1b_dilate3x3_i1b);
	DECL(_if, 11, i1b_erode3x3_i1b);
	DECL(_if, 12, i1b_thin_i1b);
	DECL(_if, 13, i1b_laplacian3x3_i1b);
	DECL(_if, 14, i1b_laplacian2nd3x3_i1b);
	DECL(_if, 15, i1b_median3x3_i1b);
	DECL(_if, 16, i1b_sharpen3x3_i1b);
	DECL(_if, 17, i1b_smooth3x3_i1b);
	DECL(_if, 18, i1b_sobel3x3_i1b);
	DECL(_if, 19, i1b_sobelx3x3_i1b);
	DECL(_if, 20, i1b_sobely3x3_i1b);
	DECL(_if, 21, i1b_negate_i1b);
	DECL(_if, 22, i1b_projectionx_i1b);
	DECL(_if, 23, i1b_projectiony_i1b);
	DECL(_if, 24, i1b_connection4_i1b);
	DECL(_if, 25, i1b_connection8_i1b);
	DECL(_if, 26, i1b_outline4_i1b);
	DECL(_if, 27, i1b_outline8_i1b);
	DECL(_if, 28, i1b_segment_i1b);
	DECL(_if, 29, i1b_add_i1b_i1b);
	DECL(_if, 30, i1b_diff_i1b_i1b);
	DECL(_if, 31, i1b_algebraicprod_i1b_i1b);
	DECL(_if, 32, i1b_algebraicsum_i1b_i1b);
	DECL(_if, 33, i1b_boundedprod_i1b_i1b);
	DECL(_if, 34, i1b_logicalprod_i1b_i1b);
	DECL(_if, 35, i1b_logicalsum_i1b_i1b);
	DECL(_if, 36, i1b_subtract_i1b_i1b);
	DECL(_if, 37, i1b_highpass_i1b_v1d);
	DECL(_if, 38, i1b_lowpass_i1b_v1d);
	DECL(_if, 39, i1b_threshold_i1b);
	DECL(_if, 40, i1b_thresholdinv_i1b);
	DECL(_if, 41, i1b_thresholdv_i1b_v1d);
	DECL(_if, 42, i1b_thresholdinvv_i1b_v1d);
	DECL(_if, 43, i1b_binarize_i1b);
	DECL(_if, 44, i1b_binarizeinv_i1b);
	DECL(_if, 45, i1b_binarizev_i1b_v1d);
	DECL(_if, 46, i1b_binarizeinvv_i1b_v1d);
	DECL(_if, 47, i1b_bigblob4_i1b_v1d);
	DECL(_if, 48, i1b_bigblob8_i1b_v1d);
	DECL(_if, 49, i1b_addv_i1b_v1d);
	DECL(_if, 50, i1b_fillv_v1d);
	DECL(_if, 51, i1b_modavgv_i1b_v1d);
	DECL(_if, 52, i1b_multiplyv_i1b_v1d);
	DECL(_if, 53, i1b_subtractv_i1b_v1d);
	DECL(_if, 54, i1b_noisev_v1d);
	DECL(_if, 55, i1b_select_i1b_i1b_v1d_v1d);
	DECL(_if, 56, i1b_mean3x3_i1b);
	DECL(_if, 57, i1b_lightedge3x3_i1b);
	DECL(_if, 58, i1b_darkedge3x3_i1b);
	DECL(_if, 59, i1b_largearea_i1b);
	DECL(_if, 60, i1b_smallarea_i1b);
	DECL(_if, 61, i1b_variance3x3_i1b);
	DECL(_if, 62, i1b_gamma_i1b_v1d);
	DECL(_if, 63, i1b_drasticsum_i1b_i1b);
	DECL(_if, 64, i1b_drasticprod_i1b_i1b);
	DECL(_if, 65, i1b_prewitt3x3_i1b);

	DECL(_af,  0, a1d_zero);
	DECL(_af,  1, a1d_move_a1d);
	DECL(_af,  2, a1d_aabsolute_a1d);
	DECL(_af,  3, a1d_aaddv_a1d_v1d);
	DECL(_af,  4, a1d_adivv_a1d_v1d);
	DECL(_af,  5, a1d_adivv_v1d_a1d);
	DECL(_af,  6, a1d_asubtractv_a1d_v1d);
	DECL(_af,  7, a1d_asubtractv_v1d_a1d);

#if defined(GARNET_ENABLE_OPENCL__)
	if ( isOclOk ) {
		DECL(_ni, 0, v1dg_move_v1d);
		DECL(_ni, 1, i1bg_move_i1b);
		DECL(_ni, 2, i3bg_move_i3b);
		DECL(_ni, 3, v1d_move_v1dg);
		DECL(_ni, 4, i1b_move_i1bg);
		DECL(_ni, 5, i3b_move_i3bg);

		DECL(_ni, 0, v1dg_zero);
		DECL(_ni, 1, v1dg_move_v1dg);
		DECL(_ni, 2, v1dg_avgpixel_i1bg);
		DECL(_ni, 3, v1dg_maxpixel_i1bg);
		DECL(_ni, 4, v1dg_minpixel_i1bg);
		DECL(_ni, 5, v1dg_fgarea_i1bg);
		DECL(_ni, 6, v1dg_bgarea_i1bg);
		DECL(_ni, 7, v1dg_fgareav_i1bg_v1dg);
		DECL(_ni, 8, v1dg_bgareav_i1bg_v1dg);
		DECL(_ni, 9, v1dg_macenterbx_i1bg);
		DECL(_ni, 10, v1dg_macenterby_i1bg);
		DECL(_ni, 11, v1dg_psnr_i1bg_i1bg);
		DECL(_ni, 12, v1dg_vnegate_v1dg);
		DECL(_ni, 13, v1dg_vadd_v1dg_v1dg);
		DECL(_ni, 14, v1dg_valgebraicprod_v1dg_v1dg);
		DECL(_ni, 15, v1dg_valgebraicsum_v1dg_v1dg);
		DECL(_ni, 16, v1dg_vdiff_v1dg_v1dg);
		DECL(_ni, 17, v1dg_vlogicalprod_v1dg_v1dg);
		DECL(_ni, 18, v1dg_vlogicalsum_v1dg_v1dg);
		DECL(_ni, 19, v1dg_vsubtract_v1dg_v1dg);
		DECL(_ni, 20, v1dg_distance_v1dg_v1dg_v1dg_v1dg);
		DECL(_ni, 21, v1dg_select_v1dg_v1dg);
		DECL(_ni, 22, v1dg_select_v1dg_v1dg_v1dg_v1dg);

		DECL(_ifg, 0, i1bg_zero);
		DECL(_ifg, 1, i3bg_zero_i3bg);
		DECL(_ifg, 2, i1bg_move_i1bg);
		DECL(_ifg, 3, i3bg_move_i3bg);
		DECL(_ifg, 4, i1bg_split1st_i3bg);
		DECL(_ifg, 5, i1bg_split2nd_i3bg);
		DECL(_ifg, 6, i1bg_split3rd_i3bg);
		DECL(_ifg, 7, i3bg_bgr2hsv_i3bg);
		DECL(_ifg, 8, i3bg_bgr2yuv_i3bg);
		DECL(_ifg, 9, i1bg_bgr2gray_i3bg);
		DECL(_ifg, 10, i1bg_dilate3x3_i1bg);
		DECL(_ifg, 11, i1bg_erode3x3_i1bg);
		DECL(_ni, 12, i1bg_thin_i1bg);
		DECL(_ifg, 13, i1bg_laplacian3x3_i1bg);
		DECL(_ifg, 14, i1bg_laplacian2nd3x3_i1bg);
		DECL(_ifg, 15, i1bg_median3x3_i1bg);
		DECL(_ifg, 16, i1bg_sharpen3x3_i1bg);
		DECL(_ifg, 17, i1bg_smooth3x3_i1bg);
		DECL(_ifg, 18, i1bg_sobel3x3_i1bg);
		DECL(_ifg, 19, i1bg_sobelx3x3_i1bg);
		DECL(_ifg, 20, i1bg_sobely3x3_i1bg);
		DECL(_ifg, 21, i1bg_negate_i1bg);
		DECL(_ni, 22, i1bg_projectionx_i1bg);
		DECL(_ni, 23, i1bg_projectiony_i1bg);
		DECL(_ni, 24, i1bg_connection4_i1bg);
		DECL(_ni, 25, i1bg_connection8_i1bg);
		DECL(_ni, 26, i1bg_outline4_i1bg);
		DECL(_ni, 27, i1bg_outline8_i1bg);
		DECL(_ni, 28, i1bg_segment_i1bg);
		DECL(_ifg, 29, i1bg_add_i1bg_i1bg);
		DECL(_ifg, 30, i1bg_diff_i1bg_i1bg);
		DECL(_ni, 31, i1bg_algebraicprod_i1bg_i1bg);
		DECL(_ni, 32, i1bg_algebraicsum_i1bg_i1bg);
		DECL(_ni, 33, i1bg_boundedprod_i1bg_i1bg);
		DECL(_ni, 34, i1bg_logicalprod_i1bg_i1bg);
		DECL(_ni, 35, i1bg_logicalsum_i1bg_i1bg);
		DECL(_ifg, 36, i1bg_subtract_i1bg_i1bg);
		DECL(_ni, 37, i1bg_highpa_i1bg_v1dg);
		DECL(_ni, 38, i1bg_lowpa_i1bg_v1dg);
		DECL(_ifg, 39, i1bg_threshold_i1bg);
		DECL(_ifg, 40, i1bg_thresholdinv_i1bg);
		DECL(_ifg, 41, i1bg_thresholdv_i1bg_v1dg);
		DECL(_ifg, 42, i1bg_thresholdinvv_i1bg_v1dg);
		DECL(_ifg, 43, i1bg_binarize_i1bg);
		DECL(_ifg, 44, i1bg_binarizeinv_i1bg);
		DECL(_ifg, 45, i1bg_binarizev_i1bg_v1dg);
		DECL(_ifg, 46, i1bg_binarizeinvv_i1bg_v1dg);
		DECL(_ni, 47, i1bg_bigblob4_i1bg_v1dg);
		DECL(_ni, 48, i1bg_bigblob8_i1bg_v1dg);
		DECL(_ifg, 49, i1bg_addv_i1bg_v1dg);
		DECL(_ifg, 50, i1bg_fillv_v1dg);
		DECL(_ni, 51, i1bg_modavgv_i1bg_v1dg);
		DECL(_ni, 52, i1bg_multiplyv_i1bg_v1dg);
		DECL(_ifg, 53, i1bg_subtractv_i1bg_v1dg);
		DECL(_ni, 54, i1bg_noisev_v1dg);
		DECL(_ifg, 55, i1bg_select_i1bg_i1bg_v1dg_v1dg);
	}
#endif

	#pragma warning(pop)
	#undef DECL

	_voidAliases.insert("printhash");
	_voidAliases.insert("printvalue");

	_isInitialized = true;
}

}//end of namespace Sakura

Sakura::InstructionIssue::InstructionIssue()
: _unit(&_ni), _index(0)
{
}

