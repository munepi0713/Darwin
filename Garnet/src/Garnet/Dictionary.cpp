
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#include <Garnet/Dictionary.h>


namespace impl {
Garnet::IVInstruction _ivInstructions[] = {
	{ "Zero", {
		{ "I1B_Zero", _I1B, { _NULL } },
		{ "V1D_Zero", _V1D, { _NULL } },
		{ "A1D_Zero", _A1D, { _NULL } },
	} },
	{ "PassI", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _NULL } }, // Intensional intron
		{ "Move", _A1D, { _NULL } }, // Intensional intron
	} },
	{ "PassV", {
		{ "Move", _I1B, { _NULL } }, // Intensional intron
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _NULL } }, // Intensional intron
	} },
	{ "PassA", {
		{ "Move", _I1B, { _NULL } }, // Intensional intron
		{ "Move", _V1D, { _NULL } }, // Intensional intron
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Split3rd", {
		{ "I1B_Split3rd_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Split2nd", {
		{ "I1B_Split2nd_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Split1st", {
		{ "I1B_Split1st_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Red", {
		{ "I1B_Split3rd_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Green", {
		{ "I1B_Split2nd_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Blue", {
		{ "I1B_Split1st_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Gray", {
		{ "I1B_BGR2Gray_I3B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BGR2HSV", {
		{ "I3B_BGR2HSV_I3B", _I3B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BGR2YUV", {
		{ "I3B_BGR2YUV_I3B", _I3B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Dilate3x3", {
		{ "I1B_Dilate3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Erode3x3", {
		{ "I1B_Erode3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Thin", {
		{ "I1B_Thin_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Laplacian3x3", {
		{ "I1B_Laplacian3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Laplacian2nd3x3", {
		{ "I1B_Laplacian2nd3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Median3x3", {
		{ "I1B_Median3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Sharpen3x3", {
		{ "I1B_Sharpen3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Smooth3x3", {
		{ "I1B_Smooth3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Sobel3x3", {
		{ "I1B_Sobel3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "SobelX3x3", {
		{ "I1B_SobelX3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "SobelY3x3", {
		{ "I1B_SobelY3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Negate", {
		{ "I1B_Negate_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ProjectionX", {
		{ "I1B_ProjectionX_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ProjectionY", {
		{ "I1B_ProjectionY_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Connection4", {
		{ "I1B_Connection4_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Connection8", {
		{ "I1B_Connection8_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Outline4", {
		{ "I1B_Outline4_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Outline8", {
		{ "I1B_Outline8_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Threshold", {
		{ "I1B_Threshold_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ThresholdInv", {
		{ "I1B_ThresholdInv_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Binarize", {
		{ "I1B_Binarize_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BinarizeInv", {
		{ "I1B_BinarizeInv_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Add", {
		{ "I1B_Add_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AlgebraicProd", {
		{ "I1B_AlgebraicProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AlgebraicSum", {
		{ "I1B_AlgebraicSum_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BoundedProd", {
		{ "I1B_BoundedProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Diff", {
		{ "I1B_Diff_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "LogicalProd", {
		{ "I1B_LogicalProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "LogicalSum", {
		{ "I1B_LogicalSum_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Subtract", {
		{ "I1B_Subtract_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "HighPass", {
		{ "I1B_HighPass_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "LowPass", {
		{ "I1B_LowPass_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ThresholdV", {
		{ "I1B_ThresholdV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ThresholdInvV", {
		{ "I1B_ThresholdInvV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BinarizeV", {
		{ "I1B_BinarizeV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BinarizeInvV", {
		{ "I1B_BinarizeInvV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BigBlob4", {
		{ "I1B_BigBlob4_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BigBlob8", {
		{ "I1B_BigBlob8_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AddV", {
		{ "I1B_AddV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ModAvgV", {
		{ "I1B_ModAvgV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MultiplyV", {
		{ "I1B_MultiplyV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "SubtractV", {
		{ "I1B_SubtractV_I1B_V1D", _I1B, { _0i, _1v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "FillV", {
		{ "I1B_FillV_V1D", _I1B, { _0v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NoiseV", {
		{ "I1B_NoiseV_V1D", _I1B, { _0v, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AvgPixel", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_AvgPixel_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MaxPixel", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MaxPixel_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MinPixel", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MinPixel_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "FGArea", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_FGArea_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BGArea", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_BGArea_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "FGAreaV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_FGAreaV_I1B_V1D", _V1D, { _0i, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "BGAreaV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_BGAreaV_I1B_V1D", _V1D, { _0i, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MassCenterBX", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MassCenterBX_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MassCenterBY", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MassCenterBY_I1B", _V1D, { _0i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "PSNR", {
		{ "I1B_Zero", _I1B, { _NULL } },
		{ "V1D_PSNR_I1B_I1B", _V1D, { _0i, _1i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MaskPSNR", {
		{ "I1B_Zero", _I1B, { _NULL } },
		{ "V1D_MaskPSNR_I1B_I1B_I1B", _V1D, { _0i, _1i, _2i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NoExtremeMaskPSNR", {
		{ "I1B_Zero", _I1B, { _NULL } },
		{ "V1D_NoExtremeMaskPSNR_I1B_I1B_I1B", _V1D, { _0i, _1i, _2i, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Distance", {
		{ "I1B_Zero", _I1B, { _NULL } },
		{ "V1D_Distance_V1D_V1D_V1D_V1D", _V1D, { _0v, _1v, _2v, _3v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VNegate", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VNegate_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VAdd", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VAdd_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VAlgebraicProd", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VAlgebraicProd_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VAlgebraicSum", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VAlgebraicSum_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VDiff", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VDiff_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VLogicalProd", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VLogicalProd_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VLogicalSum", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VLogicalSum_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VSubtract", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VSubtract_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Select", {
		{ "I1B_Select_I1B_I1B_V1D_V1D", _I1B, { _0i, _1i, _0v, _1v, _NULL } },
		{ "V1D_Select_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "SelectV", {
		{ "I1B_Select_I1B_I1B_V1D_V1D", _I1B, { _0i, _1i, _2v, _3v, _NULL } },
		{ "V1D_Select_V1D_V1D_V1D_V1D", _V1D, { _0v, _1v, _2v, _3v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },

	{ "VAddV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VAddV_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AAddV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "A1D_AAddV_A1D_V1D", _A1D, { _0a, _1v, _NULL } },
	} },
	{ "ASubtractV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "A1D_ASubtractV_A1D_V1D", _A1D, { _0a, _1v, _NULL } },
	} },
	{ "Sqrt", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_Sqrt_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VDivV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VDivV_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "ADivV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "A1D_ADivV_A1D_V1D", _A1D, { _0a, _1v, _NULL } },
	} },
	{ "VMultiplyV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VMultiplyV_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "PercentV", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_PercentV_V1D_V1D", _V1D, { _0v, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_NumA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "SumA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_SumA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "RMSA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_RMSA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MSEA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MSEA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MinA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MinA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "MaxA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_MaxA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumGreaterA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_NumGreaterA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumGreaterEqA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_NumGreaterEqA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumLessA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_NumLessA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumLessEqA", {
		{ "I1B_Mov_I1Be", _I1B, { _0i, _NULL } },
		{ "V1D_NumLessEqA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "NumEqA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_NumEqA_A1D_V1D", _V1D, { _0a, _1v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "AAbsolute", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "A1D_AAbsolute_A1D", _A1D, { _0a, _NULL } },
	} },
	{ "AverageA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_AverageA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "StdevA", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_StdevA_A1D", _V1D, { _0a, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Invert", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_Invert_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VSignFlip", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VSignFlip_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "Adjust", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_Adjust_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VExp", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VExp_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "VInvertC", {
		{ "Move", _I1B, { _0i, _NULL } },
		{ "V1D_VInvertC_V1D", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },

	// PT-ACTIT filters
	{ "__", {
		{ "I1B_Smooth3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_M", {
		{ "I1B_Dilate3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_m", {
		{ "I1B_Erode3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_d", {
		{ "I1B_Sobel3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_E", {
		{ "I1B_LightEdge3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_e", {
		{ "I1B_DarkEdge3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
#if 0
	{ "_T", {
		{ "I1B_ThresholdV _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_t", {
		{ "I1B_ThresholdInvV _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
#endif
	{ "_S", {
		{ "I1B_LargeArea_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_s", {
		{ "I1B_SmallArea_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_i", {
		{ "I1B_Negate_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_G", {
		{ "I1B_Gamma _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_z", {
		{ "I1B_Prewitt3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_g", {
		{ "I1B_Laplacian2nd3x3_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
#if 0
	{ "_F", {
		{ "I1B_HighPass _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_f", {
		{ "I1B_LowPass _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_P", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_p", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_R", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_r", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_C", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_c", {
		{ "I1B_Blob _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_N", {
		{ "I1B_BinarizeV _I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
#endif
	{ "_n", {
		{ "I1B_Binarize_I1B", _I1B, { _0i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_L", {
		{ "I1B_LogicalSum_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_l", {
		{ "I1B_LogicalProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_A", {
		{ "I1B_AlgebraicSum_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_a", {
		{ "I1B_AlgebraicProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_B", {
		{ "I1B_Add_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_b", {
		{ "I1B_BoundedProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_u", {
		{ "I1B_DrasticSum_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_U", {
		{ "I1B_DrasticProd_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
	{ "_D", {
		{ "I1B_Diff_I1B_I1B", _I1B, { _0i, _1i, _NULL } },
		{ "Move", _V1D, { _0v, _NULL } },
		{ "Move", _A1D, { _0a, _NULL } },
	} },
};


static boost::shared_ptr<Garnet::Dictionary> _instance;

}

Garnet::Dictionary::Dictionary()
{
	for (auto it = impl::_ivInstructions; it != impl::_ivInstructions + sizeof(impl::_ivInstructions)/sizeof(impl::_ivInstructions[0]); it++) {
		_ivInstructionMap.insert(std::make_pair(it->iv, it));
	}
}

Garnet::Dictionary* Garnet::Dictionary::getInstance()
{
	if ( !impl::_instance ) {
		impl::_instance.reset(new Garnet::Dictionary());
	}
	return impl::_instance.get();
}

const Garnet::IVInstruction& Garnet::Dictionary::findByIVName(const std::string& label)
{
	IVInstructionMap::iterator it = getInstance()->_ivInstructionMap.find(label);
	if ( it == getInstance()->_ivInstructionMap.end() ) {
		throw (boost::format("[%s] Node '%s' is not found in the dictionary.") % __FUNCTION__ % label).str();
	}
	return *it->second;
}
