#include <iostream>
#include <stdint.h>
#include <vector>
#include <array>
//#include <numeric>
#include <iterator>

#include <boost/format.hpp>

#include <Common/Text.h>
#include <Common/GetTime.h>

#include <Garnet/Garnet.h>
#include <Garnet/Tree.h>
#include <Garnet/Coloring.h>
#include <Garnet/IVGraph.h>
#include <Garnet/uniteTrees.h>
#include <Garnet/TopologicalSort.h>
#include <Garnet/RemoveIntrons.h>
#include <Garnet/IVGraphProcessor.h>

#include <Common/PrintRange.h>

const std::array<const char*, 3> _treesource = {
	"(VAdd(SubtractV(VAdd(SubtractV(Red0)(VDiff(VSubtract(VAlgebraicSum(VAlgebraicProd(VAdd(BinarizeInvV(SobelY3x3(Smooth3x3(SubtractV(BinarizeV(ModAvgV(VLogicalProd(VNegate(Diff(Erode3x3(Dilate3x3(AvgPixel(SelectV(Binarize(Median3x3(VAlgebraicSum(BinarizeInvV(Blue0)(Add(ProjectionY(BinarizeV(Binarize(VSubtract(Const_0.16666)(Gray0)))(Gray0)))(Const_0.16666)))(Const_0.16666))))(Gray0)(Const_0.16666)(Red0)))))(Green0)))(Green0))(Gray0))(Gray0))(Green0))))(AlgebraicSum(Threshold(VAdd(VSubtract(AlgebraicSum(Smooth3x3(Select(VLogicalSum(Binarize(SobelX3x3(Median3x3(SobelY3x3(BinarizeInv(MinPixel(MaxPixel(Gray0))))))))(Green0))(Red0)))(Const_0.16666))(Red0))(Gray0)))(AvgPixel(MultiplyV(FillV(Laplacian3x3(BinarizeInvV(VAlgebraicSum(LogicalProd(Sharpen3x3(ModAvgV(VAdd(VAlgebraicProd(MultiplyV(Erode3x3(LowPass(Diff(MaxPixel(SobelX3x3(Const_0.16666)))(Const_0.16666))(Red0)))(Gray0))(Red0))(Red0))(Gray0)))(MaxPixel(BinarizeV(BinarizeInvV(VSubtract(VSubtract(AlgebraicSum(VNegate(Sobel3x3(Const_0.16666)))(SubtractV(FillV(MaxPixel(ProjectionY(Diff(Blue0)(Const_0.16666)))))(BoundedProd(Laplacian3x3(Const_0.16666))(Blue0))))(Blue0))(Green0))(Green0))(Blue0))))(LowPass(VAlgebraicSum(SobelY3x3(Sharpen3x3(ThresholdInv(Gray0))))(LogicalProd(ThresholdInvV(BinarizeInv(AddV(LogicalSum(Green0)(VNegate(AddV(VLogicalProd(Const_0.16666)(Green0))(Green0))))(Median3x3(SobelY3x3(AlgebraicSum(SubtractV(Sharpen3x3(LogicalProd(FillV(Const_0.16666))(Diff(MaxPixel(LogicalProd(Sharpen3x3(Green0))(Red0)))(Const_0.16666))))(Green0))(Gray0))))))(Green0))(Const_0.16666)))(Green0)))(Red0))))(Const_0.16666)))))(VDiff(VAlgebraicSum(Select(HighPass(Add(SelectV(HighPass(VAlgebraicProd(BoundedProd(VSubtract(MultiplyV(ModAvgV(VLogicalProd(AddV(AlgebraicSum(Green0)(VAdd(Subtract(Const_0.16666)(VAdd(Blue0)(Const_0.16666)))(Red0)))(Gray0))(Const_0.16666))(Blue0))(Gray0))(Gray0))(Const_0.16666))(Gray0))(VAlgebraicSum(Smooth3x3(ThresholdInv(Threshold(BinarizeInv(VSubtract(Diff(MultiplyV(MaxPixel(AddV(Sharpen3x3(AlgebraicProd(Add(Sharpen3x3(Const_0.16666))(Threshold(SubtractV(ThresholdInv(VLogicalProd(Gray0)(AlgebraicProd(Add(SubtractV(Blue0)(Blue0))(VAlgebraicProd(ThresholdV(VAlgebraicSum(Gray0)(Green0))(Red0))(Gray0)))(Blue0))))(Gray0))))(Blue0)))(Blue0)))(Const_0.16666))(Blue0))(Const_0.16666))))))(Red0)))(Red0)(Blue0)(Const_0.16666))(Green0))(HighPass(ThresholdInv(Negate(Diff(VAlgebraicProd(SubtractV(SelectV(Add(VDiff(Sharpen3x3(Blue0))(Const_0.16666))(Gray0))(Const_0.16666)(Green0)(Gray0))(Green0))(Const_0.16666))(Red0))))(BinarizeInvV(SobelY3x3(SelectV(Add(Red0)(Subtract(VSubtract(Const_0.16666)(Smooth3x3(HighPass(Threshold(VSubtract(Green0)(Green0)))(Blue0))))(Blue0)))(Green0)(Red0)(Blue0)))(Green0))))(Red0))(Gray0))(Const_0.16666)))(Green0))(Gray0))(Const_0.16666))(Const_0.16666)))(LogicalSum(Median3x3(VLogicalSum(SelectV(AddV(VLogicalSum(Diff(Diff(Subtract(HighPass(ThresholdV(FillV(Smooth3x3(BinarizeInvV(Laplacian3x3(Const_0.16666))(Const_0.16666))))(Red0))(Green0))(Const_0.16666))(Gray0))(Blue0))(Red0))(Gray0))(Blue0)(Gray0)(Red0))(AlgebraicProd(MultiplyV(Negate(AddV(Add(Diff(ThresholdInv(Laplacian3x3(BinarizeInvV(ThresholdV(BoundedProd(VSubtract(ThresholdInv(ProjectionX(BinarizeInv(VAlgebraicSum(VAlgebraicSum(Const_0.16666)(Green0))(Red0)))))(Blue0))(Green0))(Green0))(Blue0))))(Const_0.16666))(Gray0))(ProjectionX(VAdd(ThresholdV(Negate(SubtractV(SubtractV(MinPixel(SobelX3x3(ProjectionY(LogicalSum(Blue0)(Const_0.16666)))))(Blue0))(Blue0)))(Red0))(Const_0.16666)))))(Blue0))(Green0))))(Blue0)))(Const_0.16666))(AlgebraicProd(MultiplyV(SobelX3x3(BoundedProd(Binarize(Green0))(VSubtract(VAdd(Green0)(VLogicalSum(VAdd(Add(ProjectionX(Blue0))(Blue0))(Gray0))(ThresholdInv(Green0))))(Green0))))(Green0))(Blue0)))",
	"(LogicalSum(AlgebraicProd(Dilate3x3(VLogicalProd(Binarize(AlgebraicProd(LogicalSum(MultiplyV(VAdd(ThresholdInvV(VAdd(VAlgebraicSum(ModAvgV(VAlgebraicSum(ProjectionY(VAlgebraicProd(Median3x3(Dilate3x3(VAlgebraicProd(Gray0)(FillV(Sharpen3x3(ProjectionX(SubtractV(SobelY3x3(Red0))(Green0))))))))(Blue0)))(Gray0))(Green0))(Green0))(Red0))(Blue0))(MinPixel(Smooth3x3(SobelY3x3(LogicalSum(Green0)(Laplacian3x3(VAlgebraicProd(Select(AddV(BoundedProd(HighPass(BinarizeInvV(Blue0)(Green0))(Red0))(Blue0))(Red0))(Blue0))(Blue0))))))))(Subtract(Green0)(ProjectionY(SubtractV(Sharpen3x3(Dilate3x3(SubtractV(Median3x3(MaxPixel(FillV(SelectV(Laplacian3x3(Gray0))(Red0)(Blue0)(Blue0)))))(Blue0))))(Red0)))))(LogicalProd(VSubtract(Red0)(Blue0))(Negate(Erode3x3(AlgebraicProd(VLogicalProd(MinPixel(Select(Erode3x3(BinarizeInv(MultiplyV(Green0)(LogicalProd(MinPixel(AlgebraicSum(MaxPixel(SobelX3x3(ThresholdInvV(ThresholdV(VDiff(SobelY3x3(ModAvgV(ThresholdV(Gray0)(ThresholdInvV(VAlgebraicProd(Add(MinPixel(ModAvgV(Red0)(MultiplyV(BinarizeInv(Subtract(LowPass(VLogicalSum(ModAvgV(Green0)(Blue0))(Red0))(Blue0))(Red0)))(Gray0))))(Blue0))(Green0))(Green0)))(Red0)))(Erode3x3(BinarizeV(VLogicalSum(VNegate(Median3x3(AlgebraicSum(VSubtract(BinarizeInvV(ThresholdV(VAlgebraicProd(VAlgebraicProd(ThresholdV(Blue0)(LogicalProd(BinarizeInv(Blue0))(Red0)))(Gray0))(Blue0))(Blue0))(Blue0))(Gray0))(Green0))))(AddV(Sharpen3x3(VDiff(VAlgebraicProd(MaxPixel(FillV(AlgebraicProd(ThresholdInvV(SelectV(AlgebraicProd(Gray0)(Select(SobelX3x3(SobelY3x3(BinarizeInv(Subtract(Add(Green0)(Blue0))(Gray0)))))(Red0)))(Blue0)(Gray0)(Blue0))(Blue0))(Red0))))(HighPass(Smooth3x3(Sharpen3x3(Laplacian3x3(LogicalSum(VDiff(ProjectionX(VSubtract(LogicalProd(SobelX3x3(VLogicalProd(Blue0)(ModAvgV(Red0)(Blue0))))(Blue0))(Green0)))(Red0))(Green0)))))(Gray0)))(Red0)))(Blue0)))(Gray0))))(Blue0))(Gray0))))(Blue0)))(Gray0)))))(Green0)))(SubtractV(Dilate3x3(SobelY3x3(SelectV(MaxPixel(VLogicalProd(Binarize(Sobel3x3(MinPixel(Binarize(Diff(MinPixel(VAlgebraicSum(Blue0)(Blue0)))(Gray0))))))(Blue0)))(Gray0)(Green0)(Gray0))))(Green0)))(Blue0))))))(Red0)))(VLogicalProd(ProjectionX(LogicalProd(MinPixel(BinarizeInvV(BinarizeInv(BoundedProd(VNegate(LogicalSum(BinarizeInvV(Sobel3x3(Negate(Blue0)))(SobelY3x3(VLogicalProd(LogicalProd(BoundedProd(Negate(VSubtract(BoundedProd(AlgebraicProd(Sobel3x3(ModAvgV(Green0)(Blue0)))(Gray0))(Blue0))(Red0)))(Red0))(Green0))(Red0))))(SobelY3x3(Select(ProjectionY(ThresholdV(BinarizeV(AddV(AddV(VAdd(Subtract(Gray0)(VLogicalSum(Sharpen3x3(ModAvgV(AddV(VDiff(Laplacian3x3(Median3x3(Smooth3x3(Green0))))(ProjectionX(LogicalSum(FillV(AddV(Add(MultiplyV(Gray0)(Erode3x3(LowPass(BinarizeInvV(Gray0)(AvgPixel(ThresholdV(BinarizeInvV(VAlgebraicProd(LogicalProd(Laplacian3x3(Diff(BinarizeV(Gray0)(Gray0))(Gray0)))(Red0))(Blue0))(Gray0))(Gray0))))(Blue0))))(Red0))(Red0)))(Gray0))))(Blue0))(Green0)))(Green0)))(Red0))(Green0))(Red0))(Blue0))(Green0)))(Green0)))))(Blue0)))(VLogicalSum(SobelY3x3(ProjectionX(VAlgebraicProd(VAlgebraicSum(Subtract(VNegate(Select(SobelX3x3(AddV(VLogicalProd(VDiff(Gray0)(Binarize(VAlgebraicProd(SobelX3x3(FillV(Gray0)))(Gray0))))(Red0))(Blue0)))(Gray0)))(Blue0))(Green0))(Red0))))(Green0))))(Red0)))(Gray0))))(Red0))(Dilate3x3(ModAvgV(SelectV(Binarize(AlgebraicProd(Erode3x3(VLogicalProd(SubtractV(Binarize(Subtract(SubtractV(AlgebraicSum(Red0)(Red0))(Gray0))(ThresholdV(AlgebraicSum(MultiplyV(MaxPixel(Negate(ProjectionX(Erode3x3(Green0)))))(Smooth3x3(FillV(Erode3x3(Sobel3x3(VAlgebraicProd(BoundedProd(LogicalProd(Add(ModAvgV(AlgebraicProd(Gray0)(BinarizeInv(MultiplyV(Gray0)(ThresholdInvV(Gray0)(Negate(Gray0))))))(Blue0))(Red0))(Red0))(Blue0))(Red0)))))))(Gray0))(Sharpen3x3(LogicalSum(LogicalSum(SubtractV(BoundedProd(AvgPixel(Smooth3x3(Select(AlgebraicProd(VLogicalSum(VNegate(MaxPixel(VAlgebraicProd(Blue0)(SobelY3x3(VNegate(Blue0))))))(SobelY3x3(Select(VDiff(Subtract(ThresholdInvV(Negate(AvgPixel(VLogicalSum(ThresholdInvV(BinarizeInvV(Green0)(AlgebraicSum(BinarizeInv(VAdd(AddV(AvgPixel(BinarizeV(VAlgebraicSum(AlgebraicSum(LogicalSum(LogicalProd(Gray0)(Gray0))(Blue0))(Gray0))(Red0))(SelectV(Select(VDiff(Red0)(BoundedProd(Gray0)(ProjectionY(Green0))))(Dilate3x3(LowPass(Dilate3x3(Blue0))(MultiplyV(LogicalSum(Subtract(LowPass(VNegate(HighPass(Blue0)(Green0)))(Red0))(Green0))(Blue0))(Gray0)))))(Gray0)(Blue0)(Red0))))(Red0))(Blue0)))(Blue0)))(Gray0))(Red0))))(Red0))(Green0))(Gray0))(Green0))))(VAlgebraicProd(Red0)(AddV(SubtractV(Subtract(Binarize(Blue0))(ModAvgV(LowPass(FillV(VAlgebraicProd(VAlgebraicSum(BinarizeV(Green0)(Red0))(Blue0))(Blue0)))(Red0))(Blue0)))(Blue0))(Blue0))))(Green0))))(Green0))(Red0))(Gray0))(Blue0))))))(LogicalSum(ThresholdV(VLogicalProd(BinarizeV(AlgebraicSum(Diff(Laplacian3x3(Median3x3(Sobel3x3(Binarize(Gray0)))))(Gray0))(ModAvgV(Sharpen3x3(BinarizeV(Sobel3x3(ThresholdInvV(AlgebraicSum(Erode3x3(MinPixel(Blue0)))(Blue0))(Gray0)))(Green0)))(Green0)))(Gray0))(Green0))(Green0))(Gray0)))(Subtract(Sharpen3x3(SobelY3x3(Laplacian3x3(Diff(VLogicalProd(Erode3x3(Red0))(LowPass(SubtractV(Green0)(AvgPixel(Green0)))(AlgebraicProd(Red0)(SobelY3x3(VLogicalProd(AlgebraicSum(Diff(SelectV(VSubtract(VAlgebraicSum(Select(Red0)(Gray0))(Gray0))(Blue0))(Blue0)(Red0)(Red0))(Gray0))(Blue0))(Red0))))))(Blue0)))))(Red0))))(VDiff(Diff(BinarizeV(LogicalSum(MultiplyV(LowPass(AvgPixel(AlgebraicSum(ProjectionX(MultiplyV(Subtract(Select(SobelX3x3(Red0))(VLogicalProd(Smooth3x3(AlgebraicSum(Red0)(BoundedProd(AlgebraicProd(LowPass(Select(BinarizeV(Laplacian3x3(LogicalProd(Erode3x3(VDiff(SelectV(VAlgebraicSum(MultiplyV(BinarizeInv(VAlgebraicSum(VAdd(Gray0)(Green0))(Red0)))(Blue0))(Green0))(Red0)(Red0)(Blue0))(Blue0)))(Gray0)))(BinarizeInv(Gray0)))(Blue0))(Gray0))(Red0))(Green0))))(Gray0)))(Blue0))(Green0)))(Blue0)))(Gray0))(Green0))(Sobel3x3(Red0)))(VLogicalSum(VSubtract(Red0)(ModAvgV(BoundedProd(VNegate(AlgebraicSum(SubtractV(FillV(SobelX3x3(Red0)))(VSubtract(VAdd(SobelX3x3(Red0))(Gray0))(Gray0)))(Red0)))(Blue0))(Blue0)))(Red0)))(Red0))(AlgebraicProd(HighPass(Median3x3(HighPass(MinPixel(ThresholdV(ModAvgV(Median3x3(SelectV(Add(Green0)(Blue0))(Green0)(Red0)(Gray0)))(Gray0))(Blue0)))(Blue0)))(Blue0))(Red0)))))(Subtract(VNegate(Laplacian3x3(Laplacian3x3(Blue0))))(LogicalProd(VDiff(VAlgebraicSum(VAlgebraicSum(BoundedProd(BinarizeInvV(MaxPixel(BinarizeInv(MultiplyV(SobelX3x3(ThresholdV(Sharpen3x3(ProjectionY(SubtractV(Red0)(Subtract(Gray0)(Green0)))))(Gray0)))(Gray0))))(Gray0))(ThresholdInvV(SobelX3x3(ModAvgV(Binarize(VAlgebraicProd(AlgebraicProd(ModAvgV(VDiff(LowPass(ThresholdInvV(LogicalProd(Sobel3x3(Dilate3x3(Erode3x3(Laplacian3x3(MaxPixel(Blue0))))))(Blue0))(Red0))(Green0))(Gray0))(Red0))(Gray0))(Blue0)))(Blue0)))(Green0)))(Green0))(Blue0))(ThresholdInvV(Green0)(Sharpen3x3(VLogicalSum(AlgebraicProd(ThresholdV(SelectV(SelectV(FillV(BinarizeV(ThresholdV(Sobel3x3(LowPass(Red0)(Select(ProjectionY(VDiff(SelectV(VAlgebraicProd(ProjectionY(FillV(Subtract(Laplacian3x3(Dilate3x3(Green0)))(Blue0))))(Red0))(Green0)(Red0)(Green0))(Red0)))(Green0))))(VAlgebraicProd(FillV(MaxPixel(Smooth3x3(ProjectionX(BinarizeInvV(VDiff(LogicalSum(VLogicalProd(AddV(SobelX3x3(Sobel3x3(FillV(Gray0))))(Blue0))(Blue0))(Red0))(Blue0))(Green0))))))(Green0)))(Green0)))(Blue0)(Select(HighPass(LogicalProd(ProjectionY(Sobel3x3(ThresholdV(VDiff(LowPass(Smooth3x3(AddV(AvgPixel(LogicalProd(Dilate3x3(ThresholdV(VAlgebraicSum(ThresholdV(Laplacian3x3(Green0))(ProjectionY(VSubtract(VLogicalSum(ThresholdV(FillV(Select(AlgebraicSum(Add(Blue0)(Red0))(Blue0))(Gray0)))(Red0))(Blue0))(Blue0))))(Green0))(Green0)))(Green0)))(Blue0)))(Blue0))(LogicalSum(Select(AvgPixel(Red0))(Green0))(Blue0)))(Red0))))(Blue0))(Median3x3(SelectV(SobelX3x3(LogicalSum(Select(MultiplyV(LogicalSum(Select(Subtract(VAlgebraicSum(VAdd(Dilate3x3(Erode3x3(BoundedProd(Sharpen3x3(VLogicalProd(Erode3x3(AddV(Green0)(Gray0)))(Blue0)))(Blue0))))(Green0))(Gray0))(Gray0))(Red0))(Gray0))(Blue0))(Green0))(Blue0)))(SubtractV(MultiplyV(FillV(AlgebraicProd(VSubtract(VAdd(Binarize(LowPass(MultiplyV(SobelY3x3(ThresholdInvV(Select(Erode3x3(Subtract(Red0)(Red0)))(Blue0))(Red0)))(Green0))(Gray0)))(Green0))(Red0))(Blue0)))(Blue0))(Red0))(Blue0)(Red0))))(BinarizeV(MaxPixel(LowPass(LogicalProd(Diff(ThresholdInvV(ThresholdInvV(Select(AvgPixel(Add(Gray0)(Green0)))(Blue0))(Gray0))(Red0))(Erode3x3(Sobel3x3(Sobel3x3(HighPass(Red0)(SobelY3x3(Green0)))))))(Red0))(Red0)))(Gray0)))(ProjectionY(Subtract(SobelX3x3(ProjectionY(BoundedProd(BinarizeV(HighPass(ThresholdInvV(LowPass(FillV(AlgebraicSum(Add(AvgPixel(VLogicalSum(Green0)(Gray0)))(Gray0))(Blue0)))(Gray0))(Blue0))(Red0))(Blue0))(Green0))))(Red0))))(Red0)(Red0)(Green0))(Red0))(Green0))(Green0)))))(Blue0)))(Erode3x3(LogicalProd(AvgPixel(VNegate(VLogicalSum(VSubtract(AvgPixel(Laplacian3x3(AlgebraicProd(VSubtract(Add(Gray0)(Blue0))(Gray0))(Blue0))))(ThresholdV(ThresholdV(LogicalSum(HighPass(LogicalSum(HighPass(Median3x3(Median3x3(Smooth3x3(SubtractV(Add(VLogicalSum(Subtract(LogicalProd(HighPass(Red0)(Red0))(Green0))(Green0))(Blue0))(Gray0))(Red0)))))(Blue0))(MaxPixel(AlgebraicProd(VDiff(VSubtract(MinPixel(ProjectionX(SobelX3x3(LowPass(VLogicalProd(Subtract(Gray0)(Gray0))(Green0))(Blue0)))))(Green0))(Green0))(Green0))))(Gray0))(AvgPixel(Negate(Binarize(AvgPixel(Subtract(BoundedProd(Smooth3x3(Select(SobelY3x3(ProjectionX(SelectV(Blue0)(Red0)(Gray0)(Green0))))(Green0)))(Gray0))(Gray0)))))))(Red0))(MultiplyV(HighPass(BinarizeInvV(ModAvgV(AlgebraicSum(Median3x3(HighPass(VDiff(AlgebraicProd(SobelY3x3(Laplacian3x3(Gray0)))(Red0))(Blue0))(Red0)))(Green0))(Blue0))(Green0))(Green0))(Gray0))))(Blue0))))(Laplacian3x3(ProjectionX(Laplacian3x3(Binarize(ProjectionY(AvgPixel(Gray0)))))))))(BinarizeInvV(Binarize(BinarizeV(MultiplyV(MinPixel(VDiff(VLogicalSum(AlgebraicSum(VSubtract(MultiplyV(FillV(Smooth3x3(Green0)))(Blue0))(Green0))(Red0))(VNegate(MaxPixel(MultiplyV(Smooth3x3(MinPixel(Negate(FillV(Erode3x3(ModAvgV(VDiff(AlgebraicSum(Red0)(ThresholdInvV(ProjectionX(VAlgebraicProd(Dilate3x3(BinarizeV(VAlgebraicSum(MaxPixel(Blue0))(Blue0))(Blue0)))(Blue0)))(Green0)))(Blue0))(Gray0)))))))(MaxPixel(Dilate3x3(Blue0)))))))(Gray0)))(Smooth3x3(AlgebraicSum(BinarizeInv(AddV(VDiff(Smooth3x3(LowPass(Gray0)(BinarizeInv(AddV(Sharpen3x3(BinarizeInvV(Gray0)(Blue0)))(Green0)))))(Red0))(Green0)))(Gray0))))(Blue0)))(VAlgebraicProd(Sobel3x3(Negate(VAlgebraicProd(ProjectionX(VLogicalSum(VAlgebraicProd(LogicalSum(BinarizeInvV(AlgebraicSum(Median3x3(AddV(BinarizeV(LowPass(VAdd(Sobel3x3(ThresholdV(MultiplyV(AlgebraicProd(MinPixel(ProjectionY(Blue0)))(Blue0))(Gray0))(Red0)))(Green0))(LogicalSum(Erode3x3(Red0))(MultiplyV(ThresholdV(Blue0)(ThresholdInvV(SobelX3x3(AlgebraicProd(Dilate3x3(VLogicalSum(SobelX3x3(AddV(Blue0)(SubtractV(ThresholdInvV(AlgebraicProd(Gray0)(Binarize(VAlgebraicSum(ModAvgV(BoundedProd(Diff(MultiplyV(BinarizeV(Red0)(Green0))(Green0))(Gray0))(Green0))(Green0))(Green0))))(Gray0))(Gray0))))(Red0)))(Blue0)))(Blue0)))(Blue0))))(Gray0))(Red0)))(Red0))(Gray0))(Green0))(Blue0))(Red0)))(Smooth3x3(VDiff(AddV(SubtractV(Sharpen3x3(Binarize(ProjectionX(Subtract(ModAvgV(Smooth3x3(Blue0))(Green0))(Red0)))))(Gray0))(Blue0))(Green0))))))(MaxPixel(LogicalSum(ThresholdV(BinarizeV(Smooth3x3(ThresholdV(Add(FillV(Negate(Median3x3(SobelX3x3(AlgebraicSum(VAlgebraicSum(SelectV(Green0)(Sharpen3x3(VAdd(Green0)(Blue0)))(Blue0)(Red0))(Green0))(Red0))))))(Gray0))(Red0)))(Green0))(Blue0))(Green0))))))(VLogicalProd(Sharpen3x3(LogicalProd(Binarize(BinarizeInvV(ThresholdV(MinPixel(Red0))(BinarizeInv(MinPixel(ProjectionX(LowPass(Laplacian3x3(MaxPixel(VAlgebraicProd(ThresholdV(FillV(AlgebraicProd(LogicalProd(Negate(VNegate(Smooth3x3(SobelY3x3(Red0)))))(Gray0))(Red0)))(Red0))(Red0))))(AvgPixel(VLogicalProd(BinarizeInv(SelectV(ThresholdV(LowPass(SobelY3x3(ProjectionY(MultiplyV(BinarizeV(BinarizeInv(SelectV(AlgebraicSum(Sharpen3x3(SobelX3x3(Negate(Diff(Blue0)(Green0)))))(Gray0))(Gray0)(Gray0)(Green0)))(Gray0))(Red0))))(Blue0))(Blue0))(Gray0)(Blue0)(Blue0)))(Blue0))))))))(AvgPixel(AlgebraicProd(AlgebraicProd(Sharpen3x3(Smooth3x3(Green0)))(ModAvgV(Select(MinPixel(ThresholdV(Binarize(VAlgebraicSum(ProjectionY(Red0))(Blue0)))(Red0)))(Gray0))(AlgebraicProd(VAdd(HighPass(SobelY3x3(Select(SobelX3x3(ProjectionY(ThresholdV(MinPixel(AvgPixel(Gray0)))(Gray0))))(Gray0)))(Gray0))(Gray0))(Gray0))))(Red0)))))(VDiff(MaxPixel(LowPass(VAdd(Binarize(Blue0))(MaxPixel(Select(Select(Dilate3x3(Red0))(Dilate3x3(SobelX3x3(Binarize(VLogicalProd(LowPass(BinarizeInv(Blue0))(Gray0))(Green0))))))(Gray0))))(Blue0)))(Red0))))(FillV(ProjectionX(SelectV(AvgPixel(Red0))(Green0)(Blue0)(Gray0))))))))",
	"(LogicalSum(AlgebraicProd(Dilate3x3(VLogicalProd(Binarize(AlgebraicProd(LogicalSum(MultiplyV(VAdd(ThresholdInvV(VAdd(VAlgebraicSum(ModAvgV(VAlgebraicSum(Laplacian3x3(HighPass(VLogicalSum(BinarizeInvV(LowPass(SobelY3x3(Negate(VDiff(MinPixel(Negate(MultiplyV(Smooth3x3(Gray0))(Red0))))(Red0))))(Green0))(AddV(AlgebraicProd(ThresholdV(Erode3x3(Median3x3(AlgebraicSum(Erode3x3(HighPass(Diff(BinarizeInvV(Red0)(Gray0))(VAlgebraicSum(VDiff(Red0)(MultiplyV(Select(SobelX3x3(VSubtract(Red0)(BinarizeInv(VSubtract(SobelY3x3(AddV(Blue0)(Green0)))(Red0)))))(Gray0))(Green0)))(Green0)))(Sobel3x3(Subtract(Sobel3x3(BinarizeInvV(Median3x3(Add(ProjectionX(VDiff(LowPass(AddV(Red0)(Green0))(Gray0))(Red0)))(Gray0)))(Blue0)))(Red0)))))(Gray0))))(Gray0))(Red0))(BinarizeInv(Negate(Diff(FillV(LowPass(Diff(VAlgebraicProd(SobelY3x3(SobelX3x3(SubtractV(Gray0)(Blue0))))(Gray0))(Blue0))(Gray0)))(Blue0))))))(Red0))(Gray0)))(Gray0))(Green0))(Green0))(Red0))(Blue0))(MinPixel(ProjectionY(Subtract(BoundedProd(Negate(AlgebraicSum(ThresholdV(BoundedProd(Green0)(Binarize(BinarizeV(SubtractV(Select(ModAvgV(Gray0)(Green0))(Gray0))(Blue0))(Gray0))))(Blue0))(Gray0)))(Gray0))(Green0)))))(Gray0))(LogicalProd(VSubtract(Red0)(Blue0))(Negate(Erode3x3(AlgebraicProd(VLogicalProd(MinPixel(Select(Erode3x3(BinarizeInv(MultiplyV(Green0)(LogicalProd(MinPixel(AlgebraicSum(MaxPixel(AlgebraicProd(VLogicalProd(MinPixel(LowPass(MultiplyV(LowPass(VLogicalProd(Median3x3(FillV(SobelY3x3(Gray0))))(Red0))(Blue0))(Blue0))(Gray0)))(Gray0))(Green0)))(Blue0)))(Gray0)))))(Green0)))(ProjectionY(Sobel3x3(VDiff(ModAvgV(FillV(SobelX3x3(BinarizeV(Select(VAlgebraicProd(AlgebraicSum(Blue0)(Green0))(Red0))(Green0))(Red0))))(Red0))(Blue0)))))(Blue0))))))(Red0)))(VLogicalProd(ProjectionX(LogicalProd(MinPixel(BinarizeInvV(LogicalSum(VAlgebraicProd(ThresholdInvV(Smooth3x3(Erode3x3(VAdd(BinarizeInv(LogicalProd(BinarizeV(Smooth3x3(Gray0))(Red0))(Green0)))(Green0))))(Green0))(Blue0))(Blue0))(VLogicalSum(BoundedProd(VAlgebraicProd(Sobel3x3(VLogicalSum(VDiff(MaxPixel(VAlgebraicProd(Diff(BoundedProd(Binarize(Blue0))(Red0))(Gray0))(Sharpen3x3(Smooth3x3(VLogicalProd(MaxPixel(LowPass(BoundedProd(ProjectionY(VSubtract(MaxPixel(AddV(Green0)(Gray0)))(Blue0)))(Blue0))(Green0)))(Gray0))))))(Gray0))(Gray0)))(Gray0))(Green0))(Green0))))(Binarize(VAlgebraicProd(VSubtract(Erode3x3(MultiplyV(ProjectionY(ThresholdV(Sobel3x3(VSubtract(SobelY3x3(Gray0))(Red0)))(Gray0)))(Blue0)))(Gray0))(Gray0)))))(FillV(MaxPixel(AlgebraicProd(BoundedProd(Gray0)(AddV(SobelX3x3(Negate(BinarizeInv(BinarizeV(Negate(Blue0))(Blue0)))))(Green0)))(Green0)))))))(Red0))(Dilate3x3(ModAvgV(SelectV(Binarize(AlgebraicProd(Erode3x3(VLogicalProd(SubtractV(Binarize(Subtract(SubtractV(AlgebraicSum(Red0)(Red0))(Gray0))(ThresholdV(AlgebraicSum(MultiplyV(VSubtract(ProjectionX(SubtractV(Dilate3x3(LogicalSum(Red0)(Erode3x3(BinarizeInv(HighPass(SelectV(Diff(SelectV(AlgebraicProd(Red0)(Blue0))(Gray0)(BinarizeV(Dilate3x3(VDiff(VAlgebraicProd(BinarizeInvV(VSubtract(Add(AddV(Laplacian3x3(MinPixel(Red0)))(Red0))(Blue0))(Red0))(Red0))(Blue0))(Green0)))(Gray0))(Gray0))(Gray0))(Gray0)(Gray0)(Green0))(Red0))))))(Red0)))(Red0))(Smooth3x3(Gray0)))(Gray0))(Sharpen3x3(LogicalSum(LogicalSum(SubtractV(BoundedProd(AvgPixel(Smooth3x3(Select(AlgebraicProd(VLogicalSum(VNegate(MaxPixel(VAlgebraicProd(Blue0)(SobelY3x3(VNegate(Blue0))))))(SobelY3x3(Select(Sharpen3x3(Binarize(ProjectionY(VLogicalProd(BinarizeV(LogicalProd(VLogicalSum(VLogicalSum(SelectV(ThresholdInvV(MultiplyV(VLogicalSum(Select(HighPass(SubtractV(Smooth3x3(ProjectionY(LowPass(Laplacian3x3(MultiplyV(AddV(MaxPixel(Red0))(Gray0))(Gray0)))(Blue0))))(Green0))(Gray0))(Red0))(Blue0))(Red0))(Gray0))(Blue0)(Blue0)(Blue0))(Gray0))(Gray0))(Red0))(Blue0))(Add(Binarize(Median3x3(ThresholdV(Add(Blue0)(LowPass(VSubtract(VAlgebraicSum(Laplacian3x3(BinarizeInv(Green0)))(Gray0))(Gray0))(Red0)))(Red0))))(Green0))))))(Green0))))(Blue0))(Green0))))(Green0))(Red0))(Gray0))(Blue0))))))(Red0))(Subtract(Sharpen3x3(Laplacian3x3(Add(VAlgebraicProd(VNegate(Erode3x3(VAlgebraicSum(LowPass(VLogicalSum(AddV(ThresholdV(Red0)(Green0))(Blue0))(Green0))(Gray0))(Blue0))))(Green0))(Red0))))(SobelY3x3(BinarizeInvV(AlgebraicProd(Sharpen3x3(MinPixel(BinarizeInv(SobelX3x3(SobelX3x3(ModAvgV(AvgPixel(ThresholdV(Gray0)(Negate(SelectV(AvgPixel(Binarize(ThresholdInvV(ProjectionX(BinarizeV(VLogicalProd(SelectV(VNegate(Subtract(Median3x3(Diff(ModAvgV(Erode3x3(BinarizeV(MaxPixel(Red0))(Blue0)))(Blue0))(Gray0)))(Green0)))(Green0)(Green0)(Red0))(Blue0))(Green0)))(Gray0))))(Green0)(Green0)(Blue0)))))(Blue0)))))))(Red0))(Green0))))))(VDiff(Diff(BinarizeV(LogicalSum(MultiplyV(LowPass(AvgPixel(AlgebraicSum(ProjectionX(MultiplyV(Subtract(Select(SubtractV(VDiff(ThresholdV(Sobel3x3(HighPass(Sharpen3x3(VLogicalProd(Negate(BoundedProd(FillV(Green0))(Green0)))(Green0)))(Red0)))(Red0))(Blue0))(Green0))(ModAvgV(Median3x3(AvgPixel(Add(LowPass(Sobel3x3(LogicalProd(SobelY3x3(MaxPixel(SobelY3x3(Green0))))(Red0)))(Blue0))(Gray0))))(Green0)))(Blue0))(Green0)))(Blue0)))(Gray0))(Green0))(Sobel3x3(Red0)))(SubtractV(VSubtract(ProjectionY(BoundedProd(Sharpen3x3(Sobel3x3(VAdd(Blue0)(LogicalSum(Subtract(BinarizeInvV(VLogicalSum(SelectV(Blue0)(Blue0)(BoundedProd(Dilate3x3(ModAvgV(ThresholdInvV(SobelY3x3(BinarizeInv(Dilate3x3(Dilate3x3(LogicalSum(BinarizeInv(Laplacian3x3(Select(Gray0)(Red0))))(Red0))))))(Red0))(Red0)))(ProjectionY(AddV(SobelY3x3(ProjectionX(Sobel3x3(Gray0))))(Laplacian3x3(AlgebraicSum(Erode3x3(Blue0))(AddV(Erode3x3(Laplacian3x3(HighPass(Median3x3(Sharpen3x3(Sobel3x3(Green0))))(Gray0))))(Green0)))))))(Red0))(Blue0))(Red0))(Red0))(LowPass(ProjectionX(VSubtract(AddV(AvgPixel(LogicalSum(Dilate3x3(SobelY3x3(ProjectionX(Red0))))(Select(Diff(FillV(MinPixel(Binarize(BoundedProd(Red0)(HighPass(VAdd(LowPass(FillV(ThresholdInvV(VDiff(Laplacian3x3(VAdd(Sobel3x3(BoundedProd(Red0)(Blue0)))(Blue0)))(Blue0))(Red0)))(Red0))(Blue0))(Green0))))))(Green0))(Blue0))))(Red0))(Gray0)))(Red0))))))(Gray0)))(Gray0))(LogicalSum(VAdd(FillV(SubtractV(AddV(Erode3x3(Select(VAlgebraicSum(Sobel3x3(Erode3x3(Sharpen3x3(LowPass(Dilate3x3(VAlgebraicProd(VLogicalSum(ThresholdV(Blue0)(Green0))(Blue0))(Red0)))(Gray0)))))(Blue0))(Red0)))(Blue0))(Blue0)))(Blue0))(Red0))))(Red0))(Erode3x3(LogicalSum(Diff(BinarizeInv(SubtractV(AlgebraicProd(Blue0)(VLogicalProd(MaxPixel(SobelX3x3(Negate(Red0))))(Median3x3(Gray0))))(Gray0)))(Smooth3x3(Diff(HighPass(MultiplyV(Diff(Laplacian3x3(Dilate3x3(LogicalSum(Blue0)(Sobel3x3(LowPass(Smooth3x3(SobelX3x3(VLogicalProd(VDiff(Green0)(Red0))(Blue0))))(Blue0))))))(Green0))(Blue0))(Blue0))(Red0))))(MinPixel(SubtractV(ProjectionY(BinarizeInv(Green0)))(Gray0))))))))(Subtract(VNegate(Laplacian3x3(Laplacian3x3(Blue0))))(LogicalProd(VDiff(VAlgebraicSum(VAlgebraicSum(Laplacian3x3(Select(Green0)(Blue0)))(Green0))(Blue0))(ThresholdInvV(Green0)(Sharpen3x3(VLogicalSum(BinarizeV(SobelY3x3(AvgPixel(SelectV(Sharpen3x3(Binarize(Blue0)))(VLogicalProd(VLogicalProd(FillV(Select(VSubtract(BinarizeInv(ModAvgV(LowPass(VNegate(SobelY3x3(VLogicalProd(AvgPixel(Green0))(Sharpen3x3(VLogicalSum(Blue0)(Blue0))))))(Gray0))(Gray0)))(Blue0))(Blue0)))(VAlgebraicProd(FillV(HighPass(Sharpen3x3(Gray0))(Gray0)))(Gray0)))(Red0))(Red0)(Green0))))(Gray0))(Green0)))))(Blue0)))(Erode3x3(LogicalProd(AvgPixel(VNegate(VLogicalSum(VSubtract(AvgPixel(Laplacian3x3(AlgebraicProd(VSubtract(Add(Gray0)(Blue0))(Gray0))(Blue0))))(ThresholdV(ThresholdV(LogicalSum(HighPass(LogicalSum(ProjectionY(BoundedProd(SubtractV(MaxPixel(ThresholdInvV(VLogicalSum(ProjectionY(MaxPixel(AddV(VAlgebraicProd(SobelY3x3(VAlgebraicProd(HighPass(ThresholdV(Red0)(Red0))(Blue0))(Gray0)))(Blue0))(Red0))))(Gray0))(Blue0)))(Blue0))(VLogicalSum(VAdd(LogicalProd(Sharpen3x3(MultiplyV(Sobel3x3(AlgebraicSum(LogicalSum(BinarizeInv(MaxPixel(Red0)))(Green0))(Red0)))(Green0)))(Red0))(Gray0))(Blue0))))(MaxPixel(AlgebraicProd(VDiff(Sobel3x3(Blue0))(LowPass(BinarizeV(ThresholdV(Dilate3x3(Green0))(BoundedProd(Smooth3x3(SelectV(Median3x3(MinPixel(ThresholdV(Laplacian3x3(Green0))(Green0))))(Gray0)(Blue0)(Gray0)))(Gray0)))(Blue0))(Blue0)))(Green0))))(Gray0))(AvgPixel(Gray0)))(VNegate(Select(VNegate(Laplacian3x3(VSubtract(Erode3x3(Dilate3x3(Dilate3x3(Green0))))(Subtract(SobelX3x3(VLogicalSum(SobelX3x3(SubtractV(Gray0)(Gray0)))(Red0)))(Gray0)))))(Red0))))(MultiplyV(Sobel3x3(BoundedProd(Median3x3(Dilate3x3(AlgebraicProd(ProjectionX(VLogicalProd(BoundedProd(SobelX3x3(AddV(VAlgebraicProd(SubtractV(Negate(MinPixel(Blue0)))(Green0))(Green0))(Red0)))(Green0))(Blue0)))(Red0))))(Red0)))(Erode3x3(AlgebraicProd(LowPass(AvgPixel(VAlgebraicProd(ThresholdV(Erode3x3(Laplacian3x3(SobelX3x3(LogicalProd(Green0)(Gray0)))))(Green0))(Green0)))(Gray0))(Gray0))))))(Blue0))))(VAlgebraicSum(Sharpen3x3(BinarizeInvV(VAdd(Sharpen3x3(BinarizeV(AddV(Smooth3x3(SobelX3x3(Select(BinarizeInv(ProjectionX(ProjectionY(Blue0))))(Gray0))))(Blue0))(Blue0)))(Gray0))(Gray0)))(Gray0))))(BinarizeInvV(Binarize(BinarizeV(MultiplyV(MinPixel(VDiff(VLogicalSum(AlgebraicSum(VSubtract(MultiplyV(Median3x3(Subtract(AvgPixel(Erode3x3(Sobel3x3(ProjectionY(FillV(MultiplyV(ModAvgV(VLogicalSum(Red0)(VAlgebraicProd(ThresholdInvV(ProjectionY(VAlgebraicProd(BinarizeInv(VDiff(SobelX3x3(AddV(Diff(HighPass(VLogicalProd(Add(VLogicalProd(Blue0)(Gray0))(Green0))(BinarizeV(FillV(VAlgebraicSum(VAlgebraicSum(VLogicalProd(FillV(Diff(ProjectionY(MinPixel(SelectV(Blue0)(Gray0)(Blue0)(Red0))))(Gray0)))(Green0))(Gray0))(Red0)))(Red0)))(Gray0))(Green0))(AddV(Erode3x3(Erode3x3(Diff(SubtractV(VAdd(Sharpen3x3(ThresholdInvV(VLogicalProd(LogicalProd(Green0)(Red0))(AlgebraicProd(Select(AlgebraicSum(Green0)(Red0))(Binarize(MinPixel(Gray0))))(Blue0)))(Red0)))(Blue0))(Blue0))(Red0))))(Green0))))(Gray0)))(SobelY3x3(SobelY3x3(LogicalProd(BinarizeInvV(Sharpen3x3(Dilate3x3(BinarizeInv(LogicalSum(VLogicalSum(HighPass(Median3x3(ThresholdV(VAlgebraicSum(LogicalSum(SobelX3x3(ThresholdV(Blue0)(SubtractV(Diff(VLogicalSum(Gray0)(ProjectionX(Blue0)))(Red0))(Green0))))(Blue0))(Gray0))(Green0)))(Gray0))(Gray0))(Red0)))))(Gray0))(ThresholdInvV(Erode3x3(VNegate(Gray0)))(Negate(ModAvgV(SubtractV(SobelY3x3(Dilate3x3(MinPixel(ThresholdV(ModAvgV(Negate(Green0))(Blue0))(Gray0)))))(Red0))(Gray0)))))))))(SubtractV(Median3x3(AlgebraicProd(Green0)(SelectV(Gray0)(Laplacian3x3(VLogicalProd(Select(Gray0)(Smooth3x3(Binarize(SobelX3x3(Subtract(Red0)(FillV(Smooth3x3(VLogicalSum(VDiff(BinarizeInvV(Laplacian3x3(Sharpen3x3(Sobel3x3(VDiff(ModAvgV(VAlgebraicProd(SobelY3x3(MultiplyV(VSubtract(HighPass(Green0)(Gray0))(Gray0))(Red0)))(MinPixel(ProjectionY(SelectV(ProjectionX(ThresholdV(Binarize(Smooth3x3(Diff(LogicalProd(SobelY3x3(Green0))(Gray0))(Blue0))))(Blue0)))(Green0)(Red0)(Blue0)))))(Red0))(Gray0)))))(Green0))(Blue0))(SobelX3x3(SobelX3x3(VAlgebraicProd(VAdd(VLogicalSum(ThresholdInvV(VNegate(SubtractV(Green0)(VLogicalProd(VLogicalProd(VAlgebraicSum(ProjectionX(Red0))(Laplacian3x3(SobelX3x3(Laplacian3x3(Diff(Dilate3x3(Sharpen3x3(Sobel3x3(Diff(Blue0)(Blue0)))))(Blue0))))))(Red0))(Blue0))))(Gray0))(Green0))(Red0))(Red0))))))))))))(Green0)))(Green0)(Blue0))))(ThresholdInvV(SelectV(VAdd(MaxPixel(SelectV(Sobel3x3(LogicalSum(ProjectionY(Negate(AlgebraicProd(Green0)(Blue0))))(Red0)))(Gray0)(Red0)(Blue0)))(Red0))(Gray0)(Green0)(Red0))(Gray0))))(Blue0)))(Gray0))(LogicalSum(BoundedProd(AvgPixel(SubtractV(Select(MinPixel(BinarizeInvV(SubtractV(VLogicalSum(MultiplyV(Red0)(Gray0))(Green0))(Blue0))(Gray0)))(SelectV(VAlgebraicSum(HighPass(Laplacian3x3(Laplacian3x3(Smooth3x3(LogicalSum(VLogicalSum(Diff(LowPass(Blue0)(Blue0))(Gray0))(Green0))(Gray0)))))(Gray0))(Green0))(Green0)(Green0)(Red0)))(Green0)))(Gray0))(Red0))))))))(Green0)))(Blue0))(Green0))(Red0))(HighPass(VAdd(BinarizeInv(BinarizeInvV(Binarize(VNegate(Diff(AddV(Blue0)(ThresholdV(Binarize(VLogicalProd(BoundedProd(Select(Blue0)(Red0))(Red0))(Gray0)))(Green0)))(Green0))))(Green0)))(Red0))(LowPass(VSubtract(BinarizeInv(ThresholdV(MultiplyV(Laplacian3x3(VAlgebraicSum(Subtract(Red0)(Sharpen3x3(Red0)))(Blue0)))(Blue0))(Blue0)))(Gray0))(Green0))))(Gray0)))(MinPixel(LogicalSum(VAdd(HighPass(SelectV(SubtractV(Negate(Add(Select(MinPixel(Diff(LowPass(Dilate3x3(ThresholdV(Sobel3x3(AddV(VAlgebraicSum(Green0)(AddV(ThresholdV(ProjectionX(Binarize(VDiff(Binarize(AvgPixel(SelectV(VSubtract(Smooth3x3(Green0))(Blue0))(Gray0)(Blue0)(Green0))))(Green0))))(Red0))(Red0)))(Blue0)))(Blue0)))(Red0))(Gray0)))(Red0))(Gray0)))(Green0))(Gray0)(Red0)(Blue0))(Red0))(Green0))(Add(Gray0)(Binarize(Median3x3(Select(VAlgebraicSum(MultiplyV(Green0)(SelectV(MultiplyV(Diff(LogicalProd(Red0)(Gray0))(Red0))(Green0))(Blue0)(Gray0)(Blue0)))(Green0))(Blue0))))))))(Blue0)))(VAlgebraicProd(Sobel3x3(Negate(Dilate3x3(BinarizeInvV(ModAvgV(HighPass(Median3x3(SobelX3x3(Median3x3(AlgebraicSum(Green0)(SubtractV(Add(AddV(ProjectionX(ModAvgV(BinarizeInv(Diff(BinarizeInv(AddV(SobelY3x3(Median3x3(ThresholdInvV(Blue0)(Red0))))(Red0)))(Gray0)))(Gray0)))(Blue0))(Red0))(Blue0))))))(Red0))(Green0))(Blue0)))))(MaxPixel(LogicalSum(ThresholdV(BinarizeV(Smooth3x3(ThresholdV(LogicalProd(SubtractV(MultiplyV(ThresholdInvV(Binarize(BoundedProd(LogicalProd(VNegate(Red0))(SelectV(Subtract(Gray0)(AlgebraicSum(AddV(VAlgebraicProd(VAlgebraicProd(VLogicalSum(AlgebraicProd(Erode3x3(MinPixel(ProjectionX(LogicalSum(SobelY3x3(LogicalSum(VNegate(Smooth3x3(Gray0)))(Blue0)))(Red0)))))(Blue0))(Gray0))(LowPass(VAlgebraicProd(HighPass(Gray0)(Red0))(Red0))(Green0)))(Red0))(Gray0))(Gray0)))(Gray0)(Blue0)(Red0)))(Gray0)))(Blue0))(VLogicalSum(VSubtract(Blue0)(SobelY3x3(VAlgebraicProd(VDiff(VLogicalSum(Smooth3x3(VAlgebraicSum(BinarizeInvV(ModAvgV(VSubtract(Green0)(ModAvgV(BinarizeInvV(BinarizeInv(Red0))(Red0))(Red0)))(Green0))(Green0))(Red0)))(Green0))(Green0))(Red0))))(Gray0)))(Blue0))(Green0))(Red0)))(Green0))(Blue0))(Green0))))))(VLogicalProd(Sharpen3x3(LogicalProd(Negate(Laplacian3x3(SobelX3x3(VNegate(LogicalProd(Subtract(ProjectionY(VLogicalProd(VLogicalProd(MultiplyV(Red0)(Red0))(Blue0))(Red0)))(Blue0))(Blue0))))))(VDiff(MaxPixel(AlgebraicProd(Laplacian3x3(MinPixel(AvgPixel(SelectV(SelectV(Green0)(LowPass(BinarizeInv(LowPass(VAdd(Red0)(Red0))(Green0)))(MinPixel(ThresholdInvV(Gray0)(SubtractV(MaxPixel(ModAvgV(AlgebraicProd(VLogicalProd(ProjectionY(Erode3x3(BinarizeV(Subtract(Negate(Sobel3x3(ModAvgV(VSubtract(AlgebraicProd(LogicalProd(Green0)(Red0))(Blue0))(Gray0))(Blue0))))(Red0))(Gray0))))(Red0))(Gray0))(Red0)))(Green0)))))(Red0)(LowPass(SelectV(BinarizeV(Blue0)(BinarizeInvV(ModAvgV(VLogicalSum(Red0)(ProjectionY(Erode3x3(LogicalProd(MultiplyV(Blue0)(Red0))(Blue0)))))(Blue0))(Green0)))(Gray0)(Red0)(Blue0))(Blue0)))(Subtract(Negate(Diff(Blue0)(HighPass(Add(Sobel3x3(Sharpen3x3(Subtract(Red0)(LowPass(Green0)(Blue0)))))(SelectV(VAlgebraicSum(Green0)(ProjectionX(VNegate(LowPass(Green0)(Blue0)))))(Red0)(Gray0)(Green0)))(Red0))))(Gray0))(Gray0)(Red0)))))(Blue0)))(VSubtract(Sobel3x3(HighPass(Red0)(VLogicalProd(VAlgebraicSum(SobelX3x3(ProjectionX(Gray0)))(VAlgebraicSum(Median3x3(BinarizeInv(SobelY3x3(ProjectionY(VLogicalProd(Laplacian3x3(Diff(Gray0)(Gray0)))(Gray0))))))(Gray0)))(Gray0))))(Gray0)))))(FillV(ProjectionX(SelectV(VAdd(MinPixel(Add(SobelX3x3(Binarize(VAlgebraicSum(Red0)(LogicalSum(MinPixel(Binarize(Binarize(Red0))))(AvgPixel(Subtract(Blue0)(Negate(AddV(ThresholdV(VAlgebraicSum(ThresholdV(VSubtract(VNegate(Binarize(Blue0)))(Gray0))(Gray0))(Red0))(Blue0))(Gray0)))))))))(Green0)))(Blue0))(Green0)(Blue0)(Gray0))))))))",
};

typedef std::vector<uint16_t>   SExpString;
template <typename String>
class SExpTraits {
public :
	typedef typename String::value_type  ValueType;
	static const ValueType L_BRACKET;
	static const ValueType R_BRACKET;
	static const ValueType SPACE;
};
const SExpTraits<std::string>::ValueType SExpTraits<std::string>::L_BRACKET = '(';
const SExpTraits<std::string>::ValueType SExpTraits<std::string>::R_BRACKET = ')';
const SExpTraits<std::string>::ValueType SExpTraits<std::string>::SPACE     = ' ';

const SExpTraits<SExpString>::ValueType SExpTraits<SExpString>::L_BRACKET = 0xfffeu;
const SExpTraits<SExpString>::ValueType SExpTraits<SExpString>::R_BRACKET = 0xffffu;
const SExpTraits<SExpString>::ValueType SExpTraits<SExpString>::SPACE     = 0x0000u;

template <typename Graph, typename String, typename Map>
Graph createGraphFromSExpressionEx(const String& sexp, const Map& words)
//template <typename Graph, typename String>
//Graph createGraphFromSExpressionEx(const String& sexp)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename String::value_type Char;
	typedef typename SExpTraits<String> Traits;
	
	std::stack<Vertex> s;
    Graph g;
    Vertex v, vc;
    String token;
    int16_t depth = -1;
#if 1
    for (String::const_iterator it = sexp.begin(); it != sexp.end(); it++) {
		Char c = *it;
        switch ( c ) {
        case Traits::L_BRACKET : 
			break;
        case Traits::R_BRACKET : 
			vc = s.top();
			s.pop();
			if ( !s.empty() ) {
				v = s.top();
				boost::add_edge(v, vc, g);
			}
			break;
        default :
			v = boost::add_vertex(g);
			//g[v].label = words.at(token);
			s.push(v);
			break;
		}
    }
#elif 1
//	for_each(sexp.cbegin(), sexp.cend(), [&] (Char c) {
    for (auto it = sexp.cbegin(); it != sexp.cend(); it++) {
		const Char c = *it;
        switch ( c ) {
        case Traits::L_BRACKET : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                //g[v].label = words.at(token);
                s.push(v);
                token.clear();
            }
//			else if ( c == Traits::L_BRACKET ) {
				depth++;
//			}
			break;
        case Traits::SPACE : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                //g[v].label = words.at(token);
                s.push(v);
                token.clear();
            }
			break;
        case Traits::R_BRACKET : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                //g[v].label = words.at(token);
                s.push(v);
                token.clear();
            }
//			if ( c == Traits::R_BRACKET ) {
				vc = s.top();
				s.pop();
				if ( !s.empty() ) {
					v = s.top();
					boost::add_edge(v, vc, g);
				}
				depth--;
//			}
			break;
        default :
            token.push_back(c);
			//continue;
		}
		
    }
#else
	for_each(sexp.begin(), sexp.end(), [&] (Char c) {
        switch ( c ) {
        case Traits::L_BRACKET : 
        case Traits::SPACE : 
        case Traits::R_BRACKET : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                g[v].label = token;
                s.push(v);
                token.clear();
            }
			if ( c == Traits::R_BRACKET ) {
				vc = s.top();
				s.pop();
				if ( !s.empty() ) {
					v = s.top();
					boost::add_edge(v, vc, g);
				}
				depth--;
			}
			else if ( c == Traits::L_BRACKET ) {
				depth++;
			}
            break;
        default :
            token.push_back(c);
        }
    });
#endif
    return g;
}

template <typename Func>
void test(Func func)
{
	using namespace Garnet;
	
	for (auto repeat = 100; repeat < 101; repeat *= 2) {
		std::cout << boost::format("\n\n---- REPEAT=%u -----") % repeat;
		for (auto t = 0u; t < _treesource.size(); t++) {
			std::cout << boost::format("\nTree %u") % t;
			func(_treesource[t], repeat);
		}
	}
}

int main(int argc, const char* argv[])
{
	using namespace Garnet;

	#if 0
	// Swap
	std::cout << "\n\n===== Swap =====";
	test([] (const char* source, uint32_t repeat) {
		uint32_t numVertices;
		Tree treeA(source);
		Tree treeB(source);
		numVertices = treeA.getNumVertices();
		std::cout << boost::format("\n  %u vertices") % numVertices;

		uint32_t accumVerts = 0u;
		StopWatch w;
		for (auto i = 0u; i < repeat; i++) {
			auto numVertsA = treeA.getNumVertices();
			auto numVertsB = treeB.getNumVertices();
			uint32_t posA = randInteger(numVertsA);
			uint32_t posB = randInteger(numVertsB);
			swapSubTrees(treeA, posA, treeB, posB);
			accumVerts += numVertsA + numVertsB;
		}
		w.stop();
		std::cout << boost::format("\n  A: %u vertices") % treeA.getNumVertices()
		          << boost::format("\n  B: %u vertices") % treeB.getNumVertices()
		          << boost::format("\n  %.3f [us]") % (w.get() * 1000000.0 / repeat)
				  << boost::format("\n (%.3f [us/vertex]") % (w.get() * 1000000.0 / accumVerts);
	});
	#endif

	#if 0
	// Sexp --> Tree
	std::cout << "\n\n===== SExp --> Tree =====";
	test([] (const char* source, uint32_t repeat) {
		uint32_t numVertices;
		{
			Tree tree(source);
			numVertices = tree.getNumVertices();
			std::cout << boost::format("\n  %u vertices") % numVertices;
		}
		
		StopWatch w;
		for (auto i = 0u; i < repeat; i++) {
			Tree tree(source);
		}
		w.stop();
		std::cout << boost::format("\n  %.3f [us]") % (w.get() * 1000000.0 / repeat)
				  << boost::format("\n (%.3f [us/vertex]") % (w.get() * 1000000.0 / repeat / numVertices);
	});
	#endif

	#if 0
	// Tree --> Sexp
	std::cout << "\n\n===== Tree --> SExp =====";
	test([] (const char* source, uint32_t repeat) {
		Tree tree(source);
		uint32_t numVertices = tree.getNumVertices();
		std::cout << boost::format("\n  %u vertices") % numVertices;

		StopWatch w;
		for (auto i = 0u; i < repeat; i++) {
			std::string sexp = tree.getSExpression();
		}
		w.stop();
		std::cout << boost::format("\n  %.3f [us]") % (w.get() * 1000000.0 / repeat)
				  << boost::format("\n (%.3f [us/vertex]") % (w.get() * 1000000.0 / repeat / numVertices);
	});
	#endif

	// Sexp --> Tree (2)
	std::cout << "\n\n===== SExp --> Tree (2) =====";
	test([] (const char* source, uint32_t repeat) {
		uint32_t numVertices;
		{
			Tree tree(source);
			numVertices = tree.getNumVertices();
			std::cout << boost::format("\n  %u vertices") % numVertices;
		}
		
		SExpString sexp;
		typedef SExpTraits<SExpString> Traits;
		std::map<std::string, uint16_t> words;
		std::map<SExpString, std::string> toWords;
		std::string orig(source);
		std::string token;
		for (std::string::const_iterator it = orig.begin(); it != orig.end(); it++) {
			switch ( *it ) {
			case '(' : 
			case ' ' : 
			case ')' : 
				if ( !token.empty() ) {
					if (words.find(token) == words.end()) {
						SExpString intToken;
						intToken.push_back(words.size());
						toWords[intToken] = token;
						words[token] = words.size();
					}
					sexp.push_back(words[token]);
					token = "";
				}
				break;
			default :
				token.push_back(*it);
				continue;
			}
			
			if ( *it == ')' ) {
				sexp.push_back(Traits::R_BRACKET);
			}
			else if ( *it == '(' ) {
				sexp.push_back(Traits::L_BRACKET);
			}
		}
		std::cout << '\n' << print_range(sexp.begin(), sexp.end(), "", [](std::ostream& os, uint16_t x) {
			auto y = x;
			if (x == SExpTraits<SExpString>::L_BRACKET) {
				os << '(';
			}
			else if (x == SExpTraits<SExpString>::R_BRACKET) {
				os << ')';
			}
			else {
				os << boost::format("%x") % x;
			}
		});
		
		StopWatch w;
		for (auto i = 0u; i < repeat; i++) {
			//Tree tree(createGraphFromSExpressionEx<Tree::GraphType, std::string>(source));
			Tree tree(createGraphFromSExpressionEx<Tree::GraphType, SExpString>(sexp, toWords));
		}
		w.stop();
		std::cout << boost::format("\n  %.3f [us]") % (w.get() * 1000000.0 / repeat)
				  << boost::format("\n (%.3f [us/vertex]") % (w.get() * 1000000.0 / repeat / numVertices);
	});
	
	return 0;
}
