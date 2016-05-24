#include <string>
#include <map>
#include <cstdint>

#include <Garnet/Garnet.h>

namespace {

const Garnet::ExecTimeEntry _execTimeDefault[] = {
	{ "end", 1 , }, 
	{ "start", 1 , }, 
	{ "printhash_v1d", 1000 , },
	{ "printhash_i1b", 1000 , }, 
	{ "printhash_i3b", 1000 , }, 
	{ "printhash_a1d", 1000 , }, 
	{ "printvalue_v1d", 1000 , }, 
	{ "printvalue_s", 1000 , }, 
	{ "printvalue_a1d", 1000 , },
	{ "v1d_zero", 1 , }, 
	{ "v1d_move_v1d", 1 , }, 
	{ "v1d_avgpixel_i1b", 383 , }, 
	{ "v1d_maxpixel_i1b", 1551 , }, 
	{ "v1d_minpixel_i1b", 876 , }, 
	{ "v1d_fgarea_i1b", 880 , }, 
	{ "v1d_bgarea_i1b", 880 , }, 
	{ "v1d_fgareav_i1b_v1d", 880 , }, 
	{ "v1d_bgareav_i1b_v1d", 880 , }, 
	{ "v1d_masscenterbx_i1b", 880 , }, 
	{ "v1d_masscenterby_i1b", 880 , }, 
	{ "v1d_psnr_i1b_i1b", 3375 , }, 
	{ "v1d_vnegate_v1d", 8 , }, 
	{ "v1d_vadd_v1d_v1d", 3 , }, 
	{ "v1d_valgebraicprod_v1d_v1d", 4 , }, 
	{ "v1d_valgebraicsum_v1d_v1d", 12 , }, 
	{ "v1d_vdiff_v1d_v1d", 4 , }, 
	{ "v1d_vlogicalprod_v1d_v1d", 6 , }, 
	{ "v1d_vlogicalsum_v1d_v1d", 8 , }, 
	{ "v1d_vsubtract_v1d_v1d", 2 , }, 
	{ "v1d_distance_v1d_v1d_v1d_v1d", 6 , }, 
	{ "v1d_select_v1d_v1d", 6 , }, 
	{ "v1d_select_v1d_v1d_v1d_v1d", 6 , }, 
	{ "v1d_adjust_v1d", 4 , }, 
	{ "v1d_averagea_a1d", 4 , }, 
	{ "v1d_invert_v1d", 2 , }, 
	{ "v1d_maxa_a1d", 5 , }, 
	{ "v1d_mina_a1d", 5 , }, 
	{ "v1d_msea_a1d_v1d", 5 , }, 
	{ "v1d_numa_a1d", 5 , }, 
	{ "v1d_numeqa_a1d_v1d", 5 , }, 
	{ "v1d_numgreatera_a1d_v1d", 5 , }, 
	{ "v1d_numgreatereqa_a1d_v1d", 5 , }, 
	{ "v1d_numlessa_a1d_v1d", 5 , }, 
	{ "v1d_numlesseqa_a1d_v1d", 5 , }, 
	{ "v1d_percentv_v1d_v1d", 5 , }, 
	{ "v1d_rmsa_a1d", 5 , }, 
	{ "v1d_sqrt_v1d", 5 , }, 
	{ "v1d_stdeva_a1d", 5 , }, 
	{ "v1d_suma_a1d", 5 , }, 
	{ "v1d_vaddv_v1d_v1d", 5 , }, 
	{ "v1d_vdivv_v1d_v1d", 5 , }, 
	{ "v1d_vmultiplyv_v1d_v1d", 5 , }, 
	{ "v1d_vsignflip_v1d", 5 , }, 
	{ "v1d_vexp_v1d", 3 , }, 
	{ "v1d_vinvertc_v1d", 4 , }, 
	{ "i1b_zero", 79 , }, 
	{ "i3b_zero", 240 , }, 
	{ "i1b_move_i1b", 50 , }, 
	{ "i3b_move_i3b", 150 , }, 
	{ "i1b_split1st_i3b", 218 , }, 
	{ "i1b_split2nd_i3b", 193 , }, 
	{ "i1b_split3rd_i3b", 512 , }, 
	{ "i3b_bgr2hsv_i3b", 1000 , }, 
	{ "i3b_bgr2yuv_i3b", 1000 , }, 
	{ "i1b_bgr2gray_i3b", 366 , }, 
	{ "i1b_dilate3x3_i1b", 1708 , }, 
	{ "i1b_erode3x3_i1b", 2281 , }, 
	{ "i1b_thin_i1b", 10000 , }, 
	{ "i1b_laplacian3x3_i1b", 3598 , }, 
	{ "i1b_laplacian2nd3x3_i1b", 3600 , }, 
	{ "i1b_median3x3_i1b", 5359 , }, 
	{ "i1b_sharpen3x3_i1b", 5709 , }, 
	{ "i1b_smooth3x3_i1b", 6266 , }, 
	{ "i1b_sobel3x3_i1b", 11950 , }, 
	{ "i1b_sobelx3x3_i1b", 11021 , }, 
	{ "i1b_sobely3x3_i1b", 16498 , }, 
	{ "i1b_negate_i1b", 431 , }, 
	{ "i1b_projectionx_i1b", 3786 , }, 
	{ "i1b_projectiony_i1b", 4524 , }, 
	{ "i1b_connection4_i1b", 10000 , }, 
	{ "i1b_connection8_i1b", 10000 , }, 
	{ "i1b_outline4_i1b", 10000 , }, 
	{ "i1b_outline8_i1b", 10000 , }, 
	{ "i1b_segment_i1b", 20000 , }, 
	{ "i1b_add_i1b_i1b", 137 , }, 
	{ "i1b_diff_i1b_i1b", 138 , }, 
	{ "i1b_algebraicprod_i1b_i1b", 801 , }, 
	{ "i1b_algebraicsum_i1b_i1b", 6153 , }, 
	{ "i1b_boundedprod_i1b_i1b", 2529 , }, 
	{ "i1b_logicalprod_i1b_i1b", 195 , }, 
	{ "i1b_logicalsum_i1b_i1b", 230 , }, 
	{ "i1b_subtract_i1b_i1b", 155 , }, 
	{ "i1b_highpass_i1b_v1d", 77705 , }, 
	{ "i1b_lowpass_i1b_v1d", 99026 , }, 
	{ "i1b_threshold_i1b", 2243 , }, 
	{ "i1b_thresholdinv_i1b", 2063 , }, 
	{ "i1b_thresholdv_i1b_v1d", 170 , }, 
	{ "i1b_thresholdinvv_i1b_v1d", 186 , }, 
	{ "i1b_binarize_i1b", 2413 , }, 
	{ "i1b_binarizeinv_i1b", 1802 , }, 
	{ "i1b_binarizev_i1b_v1d", 196 , }, 
	{ "i1b_binarizeinvv_i1b_v1d", 225 , }, 
	{ "i1b_bigblob4_i1b_v1d", 20000 , }, 
	{ "i1b_bigblob8_i1b_v1d", 20000 , }, 
	{ "i1b_addv_i1b_v1d", 2335 , }, 
	{ "i1b_fillv_v1d", 658 , }, 
	{ "i1b_modavgv_i1b_v1d", 577 , }, 
	{ "i1b_multiplyv_i1b_v1d", 1188 , }, 
	{ "i1b_subtractv_i1b_v1d", 1478 , }, 
	{ "i1b_noisev_v1d", 20000 , }, 
	{ "i1b_select_i1b_i1b_v1d_v1d", 39 , }, 
	{ "a1d_zero", 1 , }, 
	{ "a1d_move_a1d", 1 , }, 
	{ "a1d_aabsolute_a1d", 6 , }, 
	{ "a1d_aaddv_a1d_v1d", 6 , }, 
	{ "a1d_adivv_a1d_v1d", 6 , }, 
	{ "a1d_adivv_v1d_a1d", 6 , }, 
	{ "a1d_asubtractv_a1d_v1d", 6 , }, 
	{ "a1d_asubtractv_v1d_a1d", 6 , }, 
};

std::map<uint16_t, uint32_t>    _opCodeToValue;


}//end of local namespace


void Garnet::initializeExecTimeTable(const PicturePerfect::PicturePerfectEnginePtr engine, const ExecTimeEntry* first, const ExecTimeEntry* last)
{
	_opCodeToValue.clear();
	if ( first == 0 || last == 0 ) {
		first = _execTimeDefault;
		last  = _execTimeDefault + sizeof(_execTimeDefault) / sizeof(_execTimeDefault[0]);
	}
	for (auto it = first; it != last; it++) {
		_opCodeToValue[engine->getOpCode(it->opName)] = it->unitTime;
	}
}

uint32_t Garnet::getOpExecTime(uint16_t opCode)
{
	return _opCodeToValue.at(opCode);
}
