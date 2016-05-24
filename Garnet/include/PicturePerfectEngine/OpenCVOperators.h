#ifndef PICTUREPERFECTENGINE__OPEN_CV_OPERATORS__H__
#define PICTUREPERFECTENGINE__OPEN_CV_OPERATORS__H__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <Common/IntTypes.h>
#include <Common/MD5.h>
#include <Common/Text.h>
#include <PicturePerfectEngine/Operator.h>
#include <PicturePerfectEngine/StorageOperator.h>

namespace PicturePerfect
{
namespace OpenCVOperators
{
//---------------------------------------------------------
//---------------------------------------------------------
#include "PicturePerfectEngine/OpenCVOperators/SetSizeFromImage.h"
#include "PicturePerfectEngine/OpenCVOperators/PrintHash.h"
#include "PicturePerfectEngine/OpenCVOperators/PrintValue.h"
#include "PicturePerfectEngine/OpenCVOperators/Var.h"
#include "PicturePerfectEngine/OpenCVOperators/Immediate.h"
#include "PicturePerfectEngine/OpenCVOperators/Move.h"
#include "PicturePerfectEngine/OpenCVOperators/Zero.h"
#include "PicturePerfectEngine/OpenCVOperators/Copy.h"
#include "PicturePerfectEngine/OpenCVOperators/Load.h"
#include "PicturePerfectEngine/OpenCVOperators/Save.h"

#include "PicturePerfectEngine/OpenCVOperators/Split.h"
#include "PicturePerfectEngine/OpenCVOperators/CvtColor.h"
#include "PicturePerfectEngine/OpenCVOperators/Morphology3x3.h"   // Dilate3x3, Erode3x3
#include "PicturePerfectEngine/OpenCVOperators/KernelFilter3x3.h" // KernelFilter3x3, Sobel3x3
#include "PicturePerfectEngine/OpenCVOperators/Negate.h"          // Negate
#include "PicturePerfectEngine/OpenCVOperators/Projection.h"      // ProjectionX, ProjectionY
#include "PicturePerfectEngine/OpenCVOperators/Connection.h"      // Connection4, Connection8
//#include "PicturePerfectEngine/OpenCVOperators/Outline.h"         // Outline4, Outline8
//#include "PicturePerfectEngine/OpenCVOperators/Segment.h"         // Segment (GraphCut)
#include "PicturePerfectEngine/OpenCVOperators/Arithmetic.h"      // AlgebraicSum, AlgebraicProd, BoundedProd, Diff, LogicalSum, LogicalProd, Sum
#include "PicturePerfectEngine/OpenCVOperators/DFT.h"             // HighPass, LowPass
#include "PicturePerfectEngine/OpenCVOperators/Threshold.h"       // Threshold, ThresholdInv, ThresholdV, ThresholdVInv, Binarize, BinarizeInv, BinarizeV, BinarizeVInv
//#include "PicturePerfectEngine/OpenCVOperators/Blob.h"            // BigBlob4, BigBlob8
#include "PicturePerfectEngine/OpenCVOperators/IArithmeticV.h"        // AddV, FilV, MultiplyV, ModAvgV, SubtractV
//#include "PicturePerfectEngine/OpenCVOperators/NoiseV.h"         // NoiseV
#include "PicturePerfectEngine/OpenCVOperators/VPixel.h"          // AvgPixel, MaxPixel, MinPixel
#include "PicturePerfectEngine/OpenCVOperators/Area.h"          // FGArea, FGAreaV, BGArea, BGAreaV
#include "PicturePerfectEngine/OpenCVOperators/MassCenter.h"     // MassCenterBX, MassCenterBY
#include "PicturePerfectEngine/OpenCVOperators/PSNR.h"           // PSNR
#include "PicturePerfectEngine/OpenCVOperators/VNegate.h"         // VNegate
#include "PicturePerfectEngine/OpenCVOperators/VArithmetic.h"     // VAlgebraicSum, VAlgebraicProd, VDiff, VLogicalSum, VLogicalProd, VSub, VAdd
#include "PicturePerfectEngine/OpenCVOperators/Distance.h"          // Distance
#include "PicturePerfectEngine/OpenCVOperators/Select.h"          // Select, SelectV


/**
 *  Summation of all elements should be 1.0 or 0.0.
 *  If it is larger than 1.0, the image will be brigetened.
 */

extern float _kernel_Smooth3x3[9];
extern float _kernel_Sharpen3x3[9];
extern float _kernel_SobelX3x3[9];
extern float _kernel_SobelY3x3[9];
extern float _kernel_Laplacian3x3[9];
extern float _kernel_Laplacian2nd3x3[9];

}//end of namespace OpenCVOperators
}//end of namespace PicturePerfect

#endif//PICTUREPERFECTENGINE__OPEN_CV_OPERATORS__H__
