
#include <boost/format.hpp>

#include <Common/GetTime.h>

#include <PicturePerfectEngine/SakuraEngine.h>
#include <PicturePerfectEngine/SakuraCompiler.h>

namespace {

static const char* _scriptCpuArchive = 
	"SV1D(0)	 0.5\n"
	"SV1D(1)	 0.4\n"
	"SV1D(2)	 0.3\n"
	"SV1D(3)	 0.2\n"
#if 1
	"SI3B(0)	 __BLACK__\n"
	"SI3B(1)	 __WHITE__\n"
	"SI1B(0)	 __BLACK__\n"
	"SI1B(1)	 __WHITE__\n"
#else
	"SI3B(0)	 @1.jpg\n"
	"SI3B(1)	 @2.jpg\n"
	"SI1B(0)	 @1.jpg\n"
	"SI1B(1)	 @2.jpg\n"
#endif
	"SS(0)    TestString\n"
#if 0
	"RI1B(0)  @I1B_Move_I1B.png\n"
	"RI3B(1)  @I3B_Move_I3B.png\n"
	"RI1B(2)  @I1B_Split1st_I3B.png\n"
	"RI1B(3)  @I1B_Split2nd_I3B.png\n"
	"RI1B(4)  @I1B_Split3rd_I3B.png\n"
	"RI3B(5)  @I3B_BGR2HSV_I3B.png\n"
	"RI3B(6)  @I3B_BGR2YUV_I3B.png\n"
	"RI1B(7)  @I1B_BGR2Gray_I3B.png\n"
	"RI1B(8)  @I1B_Dilate3x3_I1B.png\n"
	"RI1B(9)  @I1B_Erode3x3_I1B.png\n"
	"RI1B(10) @I1B_Thin_I1B.png\n"
	"RI1B(11) @I1B_Laplacian3x3_I1B.png\n"
	"RI1B(12) @I1B_Laplacian2nd3x3_I1B.png\n"
	"RI1B(13) @I1B_Median3x3_I1B.png\n"
	"RI1B(14) @I1B_Sharpen3x3_I1B.png\n"
	"RI1B(15) @I1B_Smooth3x3_I1B.png\n"
	"RI1B(16) @I1B_Sobel3x3_I1B.png\n"
	"RI1B(17) @I1B_SobelX3x3_I1B.png\n"
	"RI1B(18) @I1B_SobelY3x3_I1B.png\n"
	"RI1B(19) @I1B_Negate_I1B.png\n"
	"RI1B(20) @I1B_ProjectionX_I1B.png\n"
	"RI1B(21) @I1B_ProjectionY_I1B.png\n"
	"RI1B(22) @I1B_Connection4_I1B.png\n"
	"RI1B(23) @I1B_Connection8_I1B.png\n"
	"RI1B(24) @I1B_Outline4_I1B.png\n"
	"RI1B(25) @I1B_Outline8_I1B.png\n"
	"RI1B(26) @I1B_Segment_I1B.png\n"
	"RI1B(27) @I1B_Add_I1B_I1B.png\n"
	"RI1B(28) @I1B_Diff_I1B_I1B.png\n"
	"RI1B(29) @I1B_AlgebraicProd_I1B_I1B.png\n"
	"RI1B(30) @I1B_AlgebraicSum_I1B_I1B.png\n"
	"RI1B(31) @I1B_BoundedProd_I1B_I1B.png\n"
	"RI1B(32) @I1B_LogicalProd_I1B_I1B.png\n"
	"RI1B(33) @I1B_LogicalSum_I1B_I1B.png\n"
	"RI1B(34) @I1B_Subtract_I1B_I1B.png\n"
	"RI1B(35) @I1B_HighPass_I1B_V1D.png\n"
	"RI1B(36) @I1B_LowPass_I1B_V1D.png\n"
	"RI1B(37) @I1B_Threshold_I1B.png\n"
	"RI1B(38) @I1B_ThresholdInv_I1B.png\n"
	"RI1B(39) @I1B_ThresholdV_I1B_V1D.png\n"
	"RI1B(40) @I1B_ThresholdInvV_I1B_V1D.png\n"
	"RI1B(41) @I1B_Binarize_I1B.png\n"
	"RI1B(42) @I1B_BinarizeInv_I1B.png\n"
	"RI1B(43) @I1B_BinarizeV_I1B_V1D.png\n"
	"RI1B(44) @I1B_BinarizeInvV_I1B_V1D.png\n"
	"RI1B(45) @I1B_BigBlob4_I1B_V1D.png\n"
	"RI1B(46) @I1B_BigBlob8_I1B_V1D.png\n"
	"RI1B(47) @I1B_AddV_I1B_V1D.png\n"
	"RI1B(48) @I1B_FillV_V1D.png\n"
	"RI1B(49) @I1B_ModAvgV_I1B_V1D.png\n"
	"RI1B(50) @I1B_MultiplyV_I1B_V1D.png\n"
	"RI1B(51) @I1B_SubtractV_I1B_V1D.png\n"
	"RI1B(52) @I1B_NoiseV_V1D.png\n"
	"RI1B(53) @I1B_Select_I1B_I1B_V1D_V1D.png\n"
#endif
	"";

static const char* _scriptCpuExecution =
	"Start\n"
	"V1D_Move_V1D                 RV1D(21), SV1D(0)\n"
	"V1D_AvgPixel_I1B             RV1D(0), SI1B(0)\n"
	"V1D_MaxPixel_I1B             RV1D(1), SI1B(0)\n"
	"V1D_MinPixel_I1B             RV1D(2), SI1B(0)\n"
	"V1D_FGArea_I1B               RV1D(3), SI1B(0)\n"
	"V1D_BGArea_I1B               RV1D(4), SI1B(0)\n"
	"V1D_FGAreaV_I1B_V1D          RV1D(5), SI1B(0), SV1D(0)\n"
	"V1D_BGAreaV_I1B_V1D          RV1D(6), SI1B(0), SV1D(0)\n"
	"V1D_MassCenterBX_I1B         RV1D(7), SI1B(0)\n"
	"V1D_MassCenterBY_I1B         RV1D(8), SI1B(0)\n"
	"V1D_PSNR_I1B_I1B             RV1D(9), SI1B(0), SI1B(1)\n"
	"V1D_VNegate_V1D              RV1D(10), SV1D(0)\n"
	"V1D_VAdd_V1D_V1D             RV1D(11), SV1D(0), SV1D(1)\n"
	"V1D_VAlgebraicProd_V1D_V1D   RV1D(12), SV1D(0), SV1D(1)\n"
	"V1D_VAlgebraicSum_V1D_V1D    RV1D(13), SV1D(0), SV1D(1)\n"
	"V1D_VDiff_V1D_V1D            RV1D(14), SV1D(0), SV1D(1)\n"
	"V1D_VLogicalProd_V1D_V1D     RV1D(15), SV1D(0), SV1D(1)\n"
	"V1D_VLogicalSum_V1D_V1D      RV1D(16), SV1D(0), SV1D(1)\n"
	"V1D_VSubtract_V1D_V1D        RV1D(17), SV1D(0), SV1D(1)\n"
	"V1D_Distance_V1D_V1D_V1D_V1D RV1D(18), SV1D(0), SV1D(1), SV1D(2), SV1D(3)\n"
	"V1D_Select_V1D_V1D           RV1D(19), SV1D(0), SV1D(1)\n"
	"V1D_Select_V1D_V1D_V1D_V1D   RV1D(20), SV1D(0), SV1D(1), SV1D(2), SV1D(3)\n"
	"I1B_Move_I1B                 RI1B(0), SI1B(0)\n"
	"I3B_Move_I3B                 RI3B(1), SI3B(0)\n"
	"I1B_Split1st_I3B             RI1B(2), SI3B(0)\n"
	"I1B_Split2nd_I3B             RI1B(3), SI3B(0)\n"
	"I1B_Split3rd_I3B             RI1B(4), SI3B(0)\n"
	"I3B_BGR2HSV_I3B              RI3B(5), SI3B(0)\n"
	"I3B_BGR2YUV_I3B              RI3B(6), SI3B(0)\n"
	"I1B_BGR2Gray_I3B             RI1B(7), SI3B(0)\n"
	"I1B_Dilate3x3_I1B            RI1B(8), SI1B(0)\n"
	"I1B_Erode3x3_I1B             RI1B(9), SI1B(0)\n"
	"I1B_Thin_I1B                 RI1B(10), SI1B(0)\n"
	"I1B_Laplacian3x3_I1B         RI1B(11), SI1B(0)\n"
	"I1B_Laplacian2nd3x3_I1B      RI1B(12), SI1B(0)\n"
	"I1B_Median3x3_I1B            RI1B(13), SI1B(0)\n"
	"I1B_Sharpen3x3_I1B           RI1B(14), SI1B(0)\n"
	"I1B_Smooth3x3_I1B            RI1B(15), SI1B(0)\n"
	"I1B_Sobel3x3_I1B             RI1B(16), SI1B(0)\n"
	"I1B_SobelX3x3_I1B            RI1B(17), SI1B(0)\n"
	"I1B_SobelY3x3_I1B            RI1B(18), SI1B(0)\n"
	"I1B_Negate_I1B               RI1B(19), SI1B(0)\n"
	"I1B_ProjectionX_I1B          RI1B(20), SI1B(0)\n"
	"I1B_ProjectionY_I1B          RI1B(21), SI1B(0)\n"
	"I1B_Connection4_I1B          RI1B(22), SI1B(0)\n"
	"I1B_Connection8_I1B          RI1B(23), SI1B(0)\n"
	"#I1B_Outline4_I1B             RI1B(24), SI1B(0)\n"
	"#I1B_Outline8_I1B             RI1B(25), SI1B(0)\n"
	"#I1B_Segment_I1B              RI1B(26), SI1B(0)\n"
	"I1B_Add_I1B_I1B              RI1B(27), SI1B(0), SI1B(1)\n"
	"I1B_Diff_I1B_I1B             RI1B(28), SI1B(0), SI1B(1)\n"
	"I1B_AlgebraicProd_I1B_I1B    RI1B(29), SI1B(0), SI1B(1)\n"
	"I1B_AlgebraicSum_I1B_I1B     RI1B(30), SI1B(0), SI1B(1)\n"
	"I1B_BoundedProd_I1B_I1B      RI1B(31), SI1B(0), SI1B(1)\n"
	"I1B_LogicalProd_I1B_I1B      RI1B(32), SI1B(0), SI1B(1)\n"
	"I1B_LogicalSum_I1B_I1B       RI1B(33), SI1B(0), SI1B(1)\n"
	"I1B_Subtract_I1B_I1B         RI1B(34), SI1B(0), SI1B(1)\n"
	"I1B_HighPass_I1B_V1D         RI1B(35), SI1B(0), SV1D(0)\n"
	"I1B_LowPass_I1B_V1D          RI1B(36), SI1B(0), SV1D(0)\n"
	"I1B_Threshold_I1B            RI1B(37), SI1B(0)\n"
	"I1B_ThresholdInv_I1B         RI1B(38), SI1B(0)\n"
	"I1B_ThresholdV_I1B_V1D       RI1B(39), SI1B(0), SV1D(0)\n"
	"I1B_ThresholdInvV_I1B_V1D    RI1B(40), SI1B(0), SV1D(0)\n"
	"I1B_Binarize_I1B             RI1B(41), SI1B(0)\n"
	"I1B_BinarizeInv_I1B          RI1B(42), SI1B(0)\n"
	"I1B_BinarizeV_I1B_V1D        RI1B(43), SI1B(0), SV1D(0)\n"
	"I1B_BinarizeInvV_I1B_V1D     RI1B(44), SI1B(0), SV1D(0)\n"
	"#I1B_BigBlob4_I1B_V1D         RI1B(45), SI1B(0), SV1D(0)\n"
	"#I1B_BigBlob8_I1B_V1D         RI1B(46), SI1B(0), SV1D(0)\n"
	"I1B_AddV_I1B_V1D             RI1B(47), SI1B(0), SV1D(0)\n"
	"I1B_FillV_V1D                RI1B(48), SV1D(0)\n"
	"I1B_ModAvgV_I1B_V1D          RI1B(49), SI1B(0), SV1D(0)\n"
	"I1B_MultiplyV_I1B_V1D        RI1B(50), SI1B(0), SV1D(0)\n"
	"I1B_SubtractV_I1B_V1D        RI1B(51), SI1B(0), SV1D(0)\n"
	"#I1B_NoiseV_V1D               RI1B(52), SV1D(0)\n"
	"I1B_Select_I1B_I1B_V1D_V1D   RI1B(53), SI1B(0), SI1B(1), SV1D(0), SV1D(1)\n"
	"End";

static const char* _scriptGpuArchive =
	"SV1Dg(0)	 0.5\n"
	"SV1Dg(1)	 0.4\n"
	"SV1Dg(2)	 0.3\n"
	"SV1Dg(3)	 0.2\n"
#if 1
	"SI3Bg(0)	 __BLACK__\n"
	"SI3Bg(1)	 __WHITE__\n"
	"SI1Bg(0)	 __BLACK__\n"
	"SI1Bg(1)	 __WHITE__\n"
#else
	"SI3Bg(0)	 @1.jpg\n"
	"SI3Bg(1)	 @2.jpg\n"
	"SI1Bg(0)	 @1.jpg\n"
	"SI1Bg(1)	 @2.jpg\n"
#endif
#if 0
	"RI1Bg(0)  @002_I1B_Move_I1B.png\n"
	"RI3Bg(1)  @002_I3B_Move_I3B.png\n"
	"RI1Bg(2)  @002_I1B_Split1st_I3B.png\n"
	"RI1Bg(3)  @002_I1B_Split2nd_I3B.png\n"
	"RI1Bg(4)  @002_I1B_Split3rd_I3B.png\n"
	"RI3Bg(5)  @002_I3B_BGR2HSV_I3B.png\n"
	"RI3Bg(6)  @002_I3B_BGR2YUV_I3B.png\n"
	"RI1Bg(7)  @002_I1B_BGR2Gray_I3B.png\n"
	"RI1Bg(8)  @002_I1B_Dilate3x3_I1B.png\n"
	"RI1Bg(9)  @002_I1B_Erode3x3_I1B.png\n"
	"RI1Bg(10) @002_I1B_Thin_I1B.png\n"
	"RI1Bg(11) @002_I1B_Laplacian3x3_I1B.png\n"
	"RI1Bg(12) @002_I1B_Laplacian2nd3x3_I1B.png\n"
	"RI1Bg(13) @002_I1B_Median3x3_I1B.png\n"
	"RI1Bg(14) @002_I1B_Sharpen3x3_I1B.png\n"
	"RI1Bg(15) @002_I1B_Smooth3x3_I1B.png\n"
	"RI1Bg(16) @002_I1B_Sobel3x3_I1B.png\n"
	"RI1Bg(17) @002_I1B_SobelX3x3_I1B.png\n"
	"RI1Bg(18) @002_I1B_SobelY3x3_I1B.png\n"
	"RI1Bg(19) @002_I1B_Negate_I1B.png\n"
	"RI1Bg(20) @002_I1B_ProjectionX_I1B.png\n"
	"RI1Bg(21) @002_I1B_ProjectionY_I1B.png\n"
	"RI1Bg(22) @002_I1B_Connection4_I1B.png\n"
	"RI1Bg(23) @002_I1B_Connection8_I1B.png\n"
	"RI1Bg(24) @002_I1B_Outline4_I1B.png\n"
	"RI1Bg(25) @002_I1B_Outline8_I1B.png\n"
	"RI1Bg(26) @002_I1B_Segment_I1B.png\n"
	"RI1Bg(27) @002_I1B_Add_I1B_I1B.png\n"
	"RI1Bg(28) @002_I1B_Diff_I1B_I1B.png\n"
	"RI1Bg(29) @002_I1B_AlgebraicProd_I1B_I1B.png\n"
	"RI1Bg(30) @002_I1B_AlgebraicSum_I1B_I1B.png\n"
	"RI1Bg(31) @002_I1B_BoundedProd_I1B_I1B.png\n"
	"RI1Bg(32) @002_I1B_LogicalProd_I1B_I1B.png\n"
	"RI1Bg(33) @002_I1B_LogicalSum_I1B_I1B.png\n"
	"RI1Bg(34) @002_I1B_Subtract_I1B_I1B.png\n"
	"RI1Bg(35) @002_I1B_HighPass_I1B_V1D.png\n"
	"RI1Bg(36) @002_I1B_LowPass_I1B_V1D.png\n"
	"RI1Bg(37) @002_I1B_Threshold_I1B.png\n"
	"RI1Bg(38) @002_I1B_ThresholdInv_I1B.png\n"
	"RI1Bg(39) @002_I1B_ThresholdV_I1B_V1D.png\n"
	"RI1Bg(40) @002_I1B_ThresholdInvV_I1B_V1D.png\n"
	"RI1Bg(41) @002_I1B_Binarize_I1B.png\n"
	"RI1Bg(42) @002_I1B_BinarizeInv_I1B.png\n"
	"RI1Bg(43) @002_I1B_BinarizeV_I1B_V1D.png\n"
	"RI1Bg(44) @002_I1B_BinarizeInvV_I1B_V1D.png\n"
	"RI1Bg(45) @002_I1B_BigBlob4_I1B_V1D.png\n"
	"RI1Bg(46) @002_I1B_BigBlob8_I1B_V1D.png\n"
	"RI1Bg(47) @002_I1B_AddV_I1B_V1D.png\n"
	"RI1Bg(48) @002_I1B_FillV_V1D.png\n"
	"RI1Bg(49) @002_I1B_ModAvgV_I1B_V1D.png\n"
	"RI1Bg(50) @002_I1B_MultiplyV_I1B_V1D.png\n"
	"RI1Bg(51) @002_I1B_SubtractV_I1B_V1D.png\n"
	"RI1Bg(52) @002_I1B_NoiseV_V1D.png\n"
	"RI1Bg(53) @002_I1B_Select_I1B_I1B_V1D_V1D.png\n"
#endif
	"";

static const char* _scriptGpuExecution =
	"Start\n"
	"#V1Dg_Move_V1Dg                 RV1Dg(21), SV1Dg(0)\n"
	"#V1Dg_AvgPixel_I1Bg             RV1Dg(0), SI1Bg(0)\n"
	"#V1Dg_MaxPixel_I1Bg             RV1Dg(1), SI1Bg(0)\n"
	"#V1Dg_MinPixel_I1Bg             RV1Dg(2), SI1Bg(0)\n"
	"#V1Dg_FGArea_I1Bg               RV1Dg(3), SI1Bg(0)\n"
	"#V1Dg_BGArea_I1Bg               RV1Dg(4), SI1Bg(0)\n"
	"#V1Dg_FGAreaV_I1Bg_V1Dg          RV1Dg(5), SI1Bg(0), SV1Dg(0)\n"
	"#V1Dg_BGAreaV_I1Bg_V1Dg          RV1Dg(6), SI1Bg(0), SV1Dg(0)\n"
	"#V1Dg_MassCenterBX_I1Bg         RV1Dg(7), SI1Bg(0)\n"
	"#V1Dg_MassCenterBY_I1Bg         RV1Dg(8), SI1Bg(0)\n"
	"#V1Dg_PSNR_I1Bg_I1Bg             RV1Dg(9), SI1Bg(0), SI1Bg(1)\n"
	"#V1Dg_VNegate_V1Dg              RV1Dg(10), SV1Dg(0)\n"
	"#V1Dg_VAdd_V1Dg_V1Dg             RV1Dg(11), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VAlgebraicProd_V1Dg_V1Dg   RV1Dg(12), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VAlgebraicSum_V1Dg_V1Dg    RV1Dg(13), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VDiff_V1Dg_V1Dg            RV1Dg(14), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VLogicalProd_V1Dg_V1Dg     RV1Dg(15), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VLogicalSum_V1Dg_V1Dg      RV1Dg(16), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_VSubtract_V1Dg_V1Dg        RV1Dg(17), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_Distance_V1Dg_V1Dg_V1Dg_V1Dg RV1Dg(18), SV1Dg(0), SV1Dg(1), SV1Dg(2), SV1Dg(3)\n"
	"#V1Dg_Select_V1Dg_V1Dg           RV1Dg(19), SV1Dg(0), SV1Dg(1)\n"
	"#V1Dg_Select_V1Dg_V1Dg_V1Dg_V1Dg   RV1Dg(20), SV1Dg(0), SV1Dg(1), SV1Dg(2), SV1Dg(3)\n"
	"I1Bg_Move_I1Bg                 RI1Bg(0), SI1Bg(0)\n"
	"I3Bg_Move_I3Bg                 RI3Bg(1), SI3Bg(0)\n"
	"#I1Bg_Split1st_I3Bg             RI1Bg(2), SI3Bg(0) \n"
	"#I1Bg_Split2nd_I3Bg             RI1Bg(3), SI3Bg(0) \n"
	"#I1Bg_Split3rd_I3Bg             RI1Bg(4), SI3Bg(0) \n"
	"#I3Bg_BGR2HSV_I3Bg              RI3Bg(5), SI3Bg(0) \n"
	"#I3Bg_BGR2YUV_I3Bg              RI3Bg(6), SI3Bg(0) \n"
	"#I1Bg_BGR2Gray_I3Bg             RI1Bg(7), SI3Bg(0) \n"
	"I1Bg_Dilate3x3_I1Bg            RI1Bg(8), SI1Bg(0) \n"
	"I1Bg_Erode3x3_I1Bg             RI1Bg(9), SI1Bg(0) \n"
	"#I1Bg_Thin_I1Bg                 RI1Bg(10), SI1Bg(0)\n"
	"I1Bg_Laplacian3x3_I1Bg         RI1Bg(11), SI1Bg(0)\n"
	"I1Bg_Laplacian2nd3x3_I1Bg      RI1Bg(12), SI1Bg(0)\n"
	"I1Bg_Median3x3_I1Bg            RI1Bg(13), SI1Bg(0)\n"
	"I1Bg_Sharpen3x3_I1Bg           RI1Bg(14), SI1Bg(0)\n"
	"I1Bg_Smooth3x3_I1Bg            RI1Bg(15), SI1Bg(0)\n"
	"#I1Bg_Sobel3x3_I1Bg             RI1Bg(16), SI1Bg(0)\n"
	"#I1Bg_SobelX3x3_I1Bg            RI1Bg(17), SI1Bg(0)\n"
	"#I1Bg_SobelY3x3_I1Bg            RI1Bg(18), SI1Bg(0)\n"
	"#I1Bg_Negate_I1Bg               RI1Bg(19), SI1Bg(0)\n"
	"#I1Bg_ProjectionX_I1Bg          RI1Bg(20), SI1Bg(0)\n"
	"#I1Bg_ProjectionY_I1Bg          RI1Bg(21), SI1Bg(0)\n"
	"#I1Bg_Connection4_I1Bg          RI1Bg(22), SI1Bg(0)\n"
	"#I1Bg_Connection8_I1Bg          RI1Bg(23), SI1Bg(0)\n"
	"#I1Bg_Outline4_I1Bg             RI1Bg(24), SI1Bg(0)\n"
	"#I1Bg_Outline8_I1Bg             RI1Bg(25), SI1Bg(0)\n"
	"#I1Bg_Segment_I1Bg              RI1Bg(26), SI1Bg(0)\n"
	"I1Bg_Add_I1Bg_I1Bg              RI1Bg(27), SI1Bg(0), SI1Bg(1)\n"
	"I1Bg_Diff_I1Bg_I1Bg             RI1Bg(28), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_AlgebraicProd_I1Bg_I1Bg    RI1Bg(29), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_AlgebraicSum_I1Bg_I1Bg     RI1Bg(30), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_BoundedProd_I1Bg_I1Bg      RI1Bg(31), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_LogicalProd_I1Bg_I1Bg      RI1Bg(32), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_LogicalSum_I1Bg_I1Bg       RI1Bg(33), SI1Bg(0), SI1Bg(1)\n"
	"I1Bg_Subtract_I1Bg_I1Bg         RI1Bg(34), SI1Bg(0), SI1Bg(1)\n"
	"#I1Bg_HighPass_I1Bg_V1Dg         RI1Bg(35), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_LowPass_I1Bg_V1Dg          RI1Bg(36), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_Threshold_I1Bg            RI1Bg(37), SI1Bg(0)\n"
	"#I1Bg_ThresholdInv_I1Bg         RI1Bg(38), SI1Bg(0)\n"
	"I1Bg_ThresholdV_I1Bg_V1Dg       RI1Bg(39), SI1Bg(0), SV1Dg(0)\n"
	"I1Bg_ThresholdInvV_I1Bg_V1Dg    RI1Bg(40), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_Binarize_I1Bg             RI1Bg(41), SI1Bg(0)\n"
	"#I1Bg_BinarizeInv_I1Bg          RI1Bg(42), SI1Bg(0)\n"
	"I1Bg_BinarizeV_I1Bg_V1Dg        RI1Bg(43), SI1Bg(0), SV1Dg(0)\n"
	"I1Bg_BinarizeInvV_I1Bg_V1Dg     RI1Bg(44), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_BigBlob4_I1Bg_V1Dg         RI1Bg(45), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_BigBlob8_I1Bg_V1Dg         RI1Bg(46), SI1Bg(0), SV1Dg(0)\n"
	"I1Bg_AddV_I1Bg_V1Dg             RI1Bg(47), SI1Bg(0), SV1Dg(0)\n"
	"I1Bg_FillV_V1Dg                RI1Bg(48), SV1Dg(0)\n"
	"#I1Bg_ModAvgV_I1Bg_V1Dg          RI1Bg(49), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_MultiplyV_I1Bg_V1Dg        RI1Bg(50), SI1Bg(0), SV1Dg(0)\n"
	"I1Bg_SubtractV_I1Bg_V1Dg        RI1Bg(51), SI1Bg(0), SV1Dg(0)\n"
	"#I1Bg_NoiseV_V1Dg               RI1Bg(52), SV1Dg(0)\n"
	"#I1Bg_Select_I1Bg_I1Bg_V1Dg_V1Dg   RI1Bg(53), SI1Bg(0), SI1Bg(1), SV1Dg(0), SV1Dg(1)\n"
	"End";

}//end of local namespace

void Sakura::Engine::rehearse(const cv::Size& imageSize)
{
	std::stringstream ss;
	PicturePerfect::ArchivePtr source;
	PicturePerfect::ArchivePtr result;
	PicturePerfect::ExecutablePtr executable;
	PicturePerfect::CompilerPtr compiler(new Sakura::Compiler());
	PicturePerfect::PicturePerfectEnginePtr engine(new Sakura::Engine());

	// 0. Create objects.
	source = PicturePerfect::Archive::createInstance(imageSize);
	result = PicturePerfect::Archive::createInstance(imageSize);

	// 1. Rehearse CPU units.
	StopWatch watch1;
	ss.str(_scriptCpuArchive);
	ss.clear();
	source->load(ss);

	ss.str(_scriptCpuExecution);
	ss.clear();
	executable = compiler->compile(ss);

	engine->execute(executable, source, result);
	watch1.stop();

	// 2. Rehearse GPU units.
	StopWatch watch2;
	ss.str(_scriptGpuArchive);
	ss.clear();
	source->load(ss);

	ss.str(_scriptGpuExecution);
	ss.clear();
	executable = compiler->compile(ss);

	engine->execute(executable, source, result);
	watch2.stop();

	//
	std::cerr << boost::format("Rehearsal CPU: %.6f[s]\n") % watch1.get();
	std::cerr << boost::format("Rehearsal GPU: %.6f[s]\n") % watch2.get();
	std::cerr << std::endl;
}
