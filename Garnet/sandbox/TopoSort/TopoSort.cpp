#include <iostream>
#include <stdint.h>
#include <vector>
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

using namespace Garnet;

int main(int argc, const char* argv[])
{
	typedef IVGraph::vertex_descriptor IVVertex;

	std::vector<Tree> trees;
//	trees.push_back(Tree("(I(A(B)(C(D)(E(F)))))"));
//	trees.push_back(Tree("(Z(Y(X(W(J(H(G(C(D)(E(F)(G(P)(Q(R))))))))(L(M(N(O))))))))"));
//	trees.push_back(Tree("(U(T(S(L(M(N(O)))))))"));
	trees.push_back(Tree("(VAdd(SubtractV(VAdd(SubtractV(Red0)(VDiff(VSubtract(VAlgebraicSum(VAlgebraicProd(VAdd(BinarizeInvV(SobelY3x3(Smooth3x3(SubtractV(BinarizeV(ModAvgV(VLogicalProd(VNegate(Diff(Erode3x3(Dilate3x3(AvgPixel(SelectV(Binarize(Median3x3(VAlgebraicSum(BinarizeInvV(Blue0)(Add(ProjectionY(BinarizeV(Binarize(VSubtract(Const_0.16666)(Gray0)))(Gray0)))(Const_0.16666)))(Const_0.16666))))(Gray0)(Const_0.16666)(Red0)))))(Green0)))(Green0))(Gray0))(Gray0))(Green0))))(AlgebraicSum(Threshold(VAdd(VSubtract(AlgebraicSum(Smooth3x3(Select(VLogicalSum(Binarize(SobelX3x3(Median3x3(SobelY3x3(BinarizeInv(MinPixel(MaxPixel(Gray0))))))))(Green0))(Red0)))(Const_0.16666))(Red0))(Gray0)))(AvgPixel(MultiplyV(FillV(Laplacian3x3(BinarizeInvV(VAlgebraicSum(LogicalProd(Sharpen3x3(ModAvgV(VAdd(VAlgebraicProd(MultiplyV(Erode3x3(LowPass(Diff(MaxPixel(SobelX3x3(Const_0.16666)))(Const_0.16666))(Red0)))(Gray0))(Red0))(Red0))(Gray0)))(MaxPixel(BinarizeV(BinarizeInvV(VSubtract(VSubtract(AlgebraicSum(VNegate(Sobel3x3(Const_0.16666)))(SubtractV(FillV(MaxPixel(ProjectionY(Diff(Blue0)(Const_0.16666)))))(BoundedProd(Laplacian3x3(Const_0.16666))(Blue0))))(Blue0))(Green0))(Green0))(Blue0))))(LowPass(VAlgebraicSum(SobelY3x3(Sharpen3x3(ThresholdInv(Gray0))))(LogicalProd(ThresholdInvV(BinarizeInv(AddV(LogicalSum(Green0)(VNegate(AddV(VLogicalProd(Const_0.16666)(Green0))(Green0))))(Median3x3(SobelY3x3(AlgebraicSum(SubtractV(Sharpen3x3(LogicalProd(FillV(Const_0.16666))(Diff(MaxPixel(LogicalProd(Sharpen3x3(Green0))(Red0)))(Const_0.16666))))(Green0))(Gray0))))))(Green0))(Const_0.16666)))(Green0)))(Red0))))(Const_0.16666)))))(VDiff(VAlgebraicSum(Select(HighPass(Add(SelectV(HighPass(VAlgebraicProd(BoundedProd(VSubtract(MultiplyV(ModAvgV(VLogicalProd(AddV(AlgebraicSum(Green0)(VAdd(Subtract(Const_0.16666)(VAdd(Blue0)(Const_0.16666)))(Red0)))(Gray0))(Const_0.16666))(Blue0))(Gray0))(Gray0))(Const_0.16666))(Gray0))(VAlgebraicSum(Smooth3x3(ThresholdInv(Threshold(BinarizeInv(VSubtract(Diff(MultiplyV(MaxPixel(AddV(Sharpen3x3(AlgebraicProd(Add(Sharpen3x3(Const_0.16666))(Threshold(SubtractV(ThresholdInv(VLogicalProd(Gray0)(AlgebraicProd(Add(SubtractV(Blue0)(Blue0))(VAlgebraicProd(ThresholdV(VAlgebraicSum(Gray0)(Green0))(Red0))(Gray0)))(Blue0))))(Gray0))))(Blue0)))(Blue0)))(Const_0.16666))(Blue0))(Const_0.16666))))))(Red0)))(Red0)(Blue0)(Const_0.16666))(Green0))(HighPass(ThresholdInv(Negate(Diff(VAlgebraicProd(SubtractV(SelectV(Add(VDiff(Sharpen3x3(Blue0))(Const_0.16666))(Gray0))(Const_0.16666)(Green0)(Gray0))(Green0))(Const_0.16666))(Red0))))(BinarizeInvV(SobelY3x3(SelectV(Add(Red0)(Subtract(VSubtract(Const_0.16666)(Smooth3x3(HighPass(Threshold(VSubtract(Green0)(Green0)))(Blue0))))(Blue0)))(Green0)(Red0)(Blue0)))(Green0))))(Red0))(Gray0))(Const_0.16666)))(Green0))(Gray0))(Const_0.16666))(Const_0.16666)))(LogicalSum(Median3x3(VLogicalSum(SelectV(AddV(VLogicalSum(Diff(Diff(Subtract(HighPass(ThresholdV(FillV(Smooth3x3(BinarizeInvV(Laplacian3x3(Const_0.16666))(Const_0.16666))))(Red0))(Green0))(Const_0.16666))(Gray0))(Blue0))(Red0))(Gray0))(Blue0)(Gray0)(Red0))(AlgebraicProd(MultiplyV(Negate(AddV(Add(Diff(ThresholdInv(Laplacian3x3(BinarizeInvV(ThresholdV(BoundedProd(VSubtract(ThresholdInv(ProjectionX(BinarizeInv(VAlgebraicSum(VAlgebraicSum(Const_0.16666)(Green0))(Red0)))))(Blue0))(Green0))(Green0))(Blue0))))(Const_0.16666))(Gray0))(ProjectionX(VAdd(ThresholdV(Negate(SubtractV(SubtractV(MinPixel(SobelX3x3(ProjectionY(LogicalSum(Blue0)(Const_0.16666)))))(Blue0))(Blue0)))(Red0))(Const_0.16666)))))(Blue0))(Green0))))(Blue0)))(Const_0.16666))(AlgebraicProd(MultiplyV(SobelX3x3(BoundedProd(Binarize(Green0))(VSubtract(VAdd(Green0)(VLogicalSum(VAdd(Add(ProjectionX(Blue0))(Blue0))(Gray0))(ThresholdInv(Green0))))(Green0))))(Green0))(Blue0)))"));
	trees.push_back(Tree("(LogicalSum(AlgebraicProd(Dilate3x3(VLogicalProd(Binarize(AlgebraicProd(LogicalSum(MultiplyV(VAdd(ThresholdInvV(VAdd(VAlgebraicSum(ModAvgV(VAlgebraicSum(ProjectionY(VAlgebraicProd(Median3x3(Dilate3x3(VAlgebraicProd(Gray0)(FillV(Sharpen3x3(ProjectionX(SubtractV(SobelY3x3(Red0))(Green0))))))))(Blue0)))(Gray0))(Green0))(Green0))(Red0))(Blue0))(MinPixel(Smooth3x3(SobelY3x3(LogicalSum(Green0)(Laplacian3x3(VAlgebraicProd(Select(AddV(BoundedProd(HighPass(BinarizeInvV(Blue0)(Green0))(Red0))(Blue0))(Red0))(Blue0))(Blue0))))))))(Subtract(Green0)(ProjectionY(SubtractV(Sharpen3x3(Dilate3x3(SubtractV(Median3x3(MaxPixel(FillV(SelectV(Laplacian3x3(Gray0))(Red0)(Blue0)(Blue0)))))(Blue0))))(Red0)))))(LogicalProd(VSubtract(Red0)(Blue0))(Negate(Erode3x3(AlgebraicProd(VLogicalProd(MinPixel(Select(Erode3x3(BinarizeInv(MultiplyV(Green0)(LogicalProd(MinPixel(AlgebraicSum(MaxPixel(SobelX3x3(ThresholdInvV(ThresholdV(VDiff(SobelY3x3(ModAvgV(ThresholdV(Gray0)(ThresholdInvV(VAlgebraicProd(Add(MinPixel(ModAvgV(Red0)(MultiplyV(BinarizeInv(Subtract(LowPass(VLogicalSum(ModAvgV(Green0)(Blue0))(Red0))(Blue0))(Red0)))(Gray0))))(Blue0))(Green0))(Green0)))(Red0)))(Erode3x3(BinarizeV(VLogicalSum(VNegate(Median3x3(AlgebraicSum(VSubtract(BinarizeInvV(ThresholdV(VAlgebraicProd(VAlgebraicProd(ThresholdV(Blue0)(LogicalProd(BinarizeInv(Blue0))(Red0)))(Gray0))(Blue0))(Blue0))(Blue0))(Gray0))(Green0))))(AddV(Sharpen3x3(VDiff(VAlgebraicProd(MaxPixel(FillV(AlgebraicProd(ThresholdInvV(SelectV(AlgebraicProd(Gray0)(Select(SobelX3x3(SobelY3x3(BinarizeInv(Subtract(Add(Green0)(Blue0))(Gray0)))))(Red0)))(Blue0)(Gray0)(Blue0))(Blue0))(Red0))))(HighPass(Smooth3x3(Sharpen3x3(Laplacian3x3(LogicalSum(VDiff(ProjectionX(VSubtract(LogicalProd(SobelX3x3(VLogicalProd(Blue0)(ModAvgV(Red0)(Blue0))))(Blue0))(Green0)))(Red0))(Green0)))))(Gray0)))(Red0)))(Blue0)))(Gray0))))(Blue0))(Gray0))))(Blue0)))(Gray0)))))(Green0)))(SubtractV(Dilate3x3(SobelY3x3(SelectV(MaxPixel(VLogicalProd(Binarize(Sobel3x3(MinPixel(Binarize(Diff(MinPixel(VAlgebraicSum(Blue0)(Blue0)))(Gray0))))))(Blue0)))(Gray0)(Green0)(Gray0))))(Green0)))(Blue0))))))(Red0)))(VLogicalProd(ProjectionX(LogicalProd(MinPixel(BinarizeInvV(BinarizeInv(BoundedProd(VNegate(LogicalSum(BinarizeInvV(Sobel3x3(Negate(Blue0)))(SobelY3x3(VLogicalProd(LogicalProd(BoundedProd(Negate(VSubtract(BoundedProd(AlgebraicProd(Sobel3x3(ModAvgV(Green0)(Blue0)))(Gray0))(Blue0))(Red0)))(Red0))(Green0))(Red0))))(SobelY3x3(Select(ProjectionY(ThresholdV(BinarizeV(AddV(AddV(VAdd(Subtract(Gray0)(VLogicalSum(Sharpen3x3(ModAvgV(AddV(VDiff(Laplacian3x3(Median3x3(Smooth3x3(Green0))))(ProjectionX(LogicalSum(FillV(AddV(Add(MultiplyV(Gray0)(Erode3x3(LowPass(BinarizeInvV(Gray0)(AvgPixel(ThresholdV(BinarizeInvV(VAlgebraicProd(LogicalProd(Laplacian3x3(Diff(BinarizeV(Gray0)(Gray0))(Gray0)))(Red0))(Blue0))(Gray0))(Gray0))))(Blue0))))(Red0))(Red0)))(Gray0))))(Blue0))(Green0)))(Green0)))(Red0))(Green0))(Red0))(Blue0))(Green0)))(Green0)))))(Blue0)))(VLogicalSum(SobelY3x3(ProjectionX(VAlgebraicProd(VAlgebraicSum(Subtract(VNegate(Select(SobelX3x3(AddV(VLogicalProd(VDiff(Gray0)(Binarize(VAlgebraicProd(SobelX3x3(FillV(Gray0)))(Gray0))))(Red0))(Blue0)))(Gray0)))(Blue0))(Green0))(Red0))))(Green0))))(Red0)))(Gray0))))(Red0))(Dilate3x3(ModAvgV(SelectV(Binarize(AlgebraicProd(Erode3x3(VLogicalProd(SubtractV(Binarize(Subtract(SubtractV(AlgebraicSum(Red0)(Red0))(Gray0))(ThresholdV(AlgebraicSum(MultiplyV(MaxPixel(Negate(ProjectionX(Erode3x3(Green0)))))(Smooth3x3(FillV(Erode3x3(Sobel3x3(VAlgebraicProd(BoundedProd(LogicalProd(Add(ModAvgV(AlgebraicProd(Gray0)(BinarizeInv(MultiplyV(Gray0)(ThresholdInvV(Gray0)(Negate(Gray0))))))(Blue0))(Red0))(Red0))(Blue0))(Red0)))))))(Gray0))(Sharpen3x3(LogicalSum(LogicalSum(SubtractV(BoundedProd(AvgPixel(Smooth3x3(Select(AlgebraicProd(VLogicalSum(VNegate(MaxPixel(VAlgebraicProd(Blue0)(SobelY3x3(VNegate(Blue0))))))(SobelY3x3(Select(VDiff(Subtract(ThresholdInvV(Negate(AvgPixel(VLogicalSum(ThresholdInvV(BinarizeInvV(Green0)(AlgebraicSum(BinarizeInv(VAdd(AddV(AvgPixel(BinarizeV(VAlgebraicSum(AlgebraicSum(LogicalSum(LogicalProd(Gray0)(Gray0))(Blue0))(Gray0))(Red0))(SelectV(Select(VDiff(Red0)(BoundedProd(Gray0)(ProjectionY(Green0))))(Dilate3x3(LowPass(Dilate3x3(Blue0))(MultiplyV(LogicalSum(Subtract(LowPass(VNegate(HighPass(Blue0)(Green0)))(Red0))(Green0))(Blue0))(Gray0)))))(Gray0)(Blue0)(Red0))))(Red0))(Blue0)))(Blue0)))(Gray0))(Red0))))(Red0))(Green0))(Gray0))(Green0))))(VAlgebraicProd(Red0)(AddV(SubtractV(Subtract(Binarize(Blue0))(ModAvgV(LowPass(FillV(VAlgebraicProd(VAlgebraicSum(BinarizeV(Green0)(Red0))(Blue0))(Blue0)))(Red0))(Blue0)))(Blue0))(Blue0))))(Green0))))(Green0))(Red0))(Gray0))(Blue0))))))(LogicalSum(ThresholdV(VLogicalProd(BinarizeV(AlgebraicSum(Diff(Laplacian3x3(Median3x3(Sobel3x3(Binarize(Gray0)))))(Gray0))(ModAvgV(Sharpen3x3(BinarizeV(Sobel3x3(ThresholdInvV(AlgebraicSum(Erode3x3(MinPixel(Blue0)))(Blue0))(Gray0)))(Green0)))(Green0)))(Gray0))(Green0))(Green0))(Gray0)))(Subtract(Sharpen3x3(SobelY3x3(Laplacian3x3(Diff(VLogicalProd(Erode3x3(Red0))(LowPass(SubtractV(Green0)(AvgPixel(Green0)))(AlgebraicProd(Red0)(SobelY3x3(VLogicalProd(AlgebraicSum(Diff(SelectV(VSubtract(VAlgebraicSum(Select(Red0)(Gray0))(Gray0))(Blue0))(Blue0)(Red0)(Red0))(Gray0))(Blue0))(Red0))))))(Blue0)))))(Red0))))(VDiff(Diff(BinarizeV(LogicalSum(MultiplyV(LowPass(AvgPixel(AlgebraicSum(ProjectionX(MultiplyV(Subtract(Select(SobelX3x3(Red0))(VLogicalProd(Smooth3x3(AlgebraicSum(Red0)(BoundedProd(AlgebraicProd(LowPass(Select(BinarizeV(Laplacian3x3(LogicalProd(Erode3x3(VDiff(SelectV(VAlgebraicSum(MultiplyV(BinarizeInv(VAlgebraicSum(VAdd(Gray0)(Green0))(Red0)))(Blue0))(Green0))(Red0)(Red0)(Blue0))(Blue0)))(Gray0)))(BinarizeInv(Gray0)))(Blue0))(Gray0))(Red0))(Green0))))(Gray0)))(Blue0))(Green0)))(Blue0)))(Gray0))(Green0))(Sobel3x3(Red0)))(VLogicalSum(VSubtract(Red0)(ModAvgV(BoundedProd(VNegate(AlgebraicSum(SubtractV(FillV(SobelX3x3(Red0)))(VSubtract(VAdd(SobelX3x3(Red0))(Gray0))(Gray0)))(Red0)))(Blue0))(Blue0)))(Red0)))(Red0))(AlgebraicProd(HighPass(Median3x3(HighPass(MinPixel(ThresholdV(ModAvgV(Median3x3(SelectV(Add(Green0)(Blue0))(Green0)(Red0)(Gray0)))(Gray0))(Blue0)))(Blue0)))(Blue0))(Red0)))))(Subtract(VNegate(Laplacian3x3(Laplacian3x3(Blue0))))(LogicalProd(VDiff(VAlgebraicSum(VAlgebraicSum(BoundedProd(BinarizeInvV(MaxPixel(BinarizeInv(MultiplyV(SobelX3x3(ThresholdV(Sharpen3x3(ProjectionY(SubtractV(Red0)(Subtract(Gray0)(Green0)))))(Gray0)))(Gray0))))(Gray0))(ThresholdInvV(SobelX3x3(ModAvgV(Binarize(VAlgebraicProd(AlgebraicProd(ModAvgV(VDiff(LowPass(ThresholdInvV(LogicalProd(Sobel3x3(Dilate3x3(Erode3x3(Laplacian3x3(MaxPixel(Blue0))))))(Blue0))(Red0))(Green0))(Gray0))(Red0))(Gray0))(Blue0)))(Blue0)))(Green0)))(Green0))(Blue0))(ThresholdInvV(Green0)(Sharpen3x3(VLogicalSum(AlgebraicProd(ThresholdV(SelectV(SelectV(FillV(BinarizeV(ThresholdV(Sobel3x3(LowPass(Red0)(Select(ProjectionY(VDiff(SelectV(VAlgebraicProd(ProjectionY(FillV(Subtract(Laplacian3x3(Dilate3x3(Green0)))(Blue0))))(Red0))(Green0)(Red0)(Green0))(Red0)))(Green0))))(VAlgebraicProd(FillV(MaxPixel(Smooth3x3(ProjectionX(BinarizeInvV(VDiff(LogicalSum(VLogicalProd(AddV(SobelX3x3(Sobel3x3(FillV(Gray0))))(Blue0))(Blue0))(Red0))(Blue0))(Green0))))))(Green0)))(Green0)))(Blue0)(Select(HighPass(LogicalProd(ProjectionY(Sobel3x3(ThresholdV(VDiff(LowPass(Smooth3x3(AddV(AvgPixel(LogicalProd(Dilate3x3(ThresholdV(VAlgebraicSum(ThresholdV(Laplacian3x3(Green0))(ProjectionY(VSubtract(VLogicalSum(ThresholdV(FillV(Select(AlgebraicSum(Add(Blue0)(Red0))(Blue0))(Gray0)))(Red0))(Blue0))(Blue0))))(Green0))(Green0)))(Green0)))(Blue0)))(Blue0))(LogicalSum(Select(AvgPixel(Red0))(Green0))(Blue0)))(Red0))))(Blue0))(Median3x3(SelectV(SobelX3x3(LogicalSum(Select(MultiplyV(LogicalSum(Select(Subtract(VAlgebraicSum(VAdd(Dilate3x3(Erode3x3(BoundedProd(Sharpen3x3(VLogicalProd(Erode3x3(AddV(Green0)(Gray0)))(Blue0)))(Blue0))))(Green0))(Gray0))(Gray0))(Red0))(Gray0))(Blue0))(Green0))(Blue0)))(SubtractV(MultiplyV(FillV(AlgebraicProd(VSubtract(VAdd(Binarize(LowPass(MultiplyV(SobelY3x3(ThresholdInvV(Select(Erode3x3(Subtract(Red0)(Red0)))(Blue0))(Red0)))(Green0))(Gray0)))(Green0))(Red0))(Blue0)))(Blue0))(Red0))(Blue0)(Red0))))(BinarizeV(MaxPixel(LowPass(LogicalProd(Diff(ThresholdInvV(ThresholdInvV(Select(AvgPixel(Add(Gray0)(Green0)))(Blue0))(Gray0))(Red0))(Erode3x3(Sobel3x3(Sobel3x3(HighPass(Red0)(SobelY3x3(Green0)))))))(Red0))(Red0)))(Gray0)))(ProjectionY(Subtract(SobelX3x3(ProjectionY(BoundedProd(BinarizeV(HighPass(ThresholdInvV(LowPass(FillV(AlgebraicSum(Add(AvgPixel(VLogicalSum(Green0)(Gray0)))(Gray0))(Blue0)))(Gray0))(Blue0))(Red0))(Blue0))(Green0))))(Red0))))(Red0)(Red0)(Green0))(Red0))(Green0))(Green0)))))(Blue0)))(Erode3x3(LogicalProd(AvgPixel(VNegate(VLogicalSum(VSubtract(AvgPixel(Laplacian3x3(AlgebraicProd(VSubtract(Add(Gray0)(Blue0))(Gray0))(Blue0))))(ThresholdV(ThresholdV(LogicalSum(HighPass(LogicalSum(HighPass(Median3x3(Median3x3(Smooth3x3(SubtractV(Add(VLogicalSum(Subtract(LogicalProd(HighPass(Red0)(Red0))(Green0))(Green0))(Blue0))(Gray0))(Red0)))))(Blue0))(MaxPixel(AlgebraicProd(VDiff(VSubtract(MinPixel(ProjectionX(SobelX3x3(LowPass(VLogicalProd(Subtract(Gray0)(Gray0))(Green0))(Blue0)))))(Green0))(Green0))(Green0))))(Gray0))(AvgPixel(Negate(Binarize(AvgPixel(Subtract(BoundedProd(Smooth3x3(Select(SobelY3x3(ProjectionX(SelectV(Blue0)(Red0)(Gray0)(Green0))))(Green0)))(Gray0))(Gray0)))))))(Red0))(MultiplyV(HighPass(BinarizeInvV(ModAvgV(AlgebraicSum(Median3x3(HighPass(VDiff(AlgebraicProd(SobelY3x3(Laplacian3x3(Gray0)))(Red0))(Blue0))(Red0)))(Green0))(Blue0))(Green0))(Green0))(Gray0))))(Blue0))))(Laplacian3x3(ProjectionX(Laplacian3x3(Binarize(ProjectionY(AvgPixel(Gray0)))))))))(BinarizeInvV(Binarize(BinarizeV(MultiplyV(MinPixel(VDiff(VLogicalSum(AlgebraicSum(VSubtract(MultiplyV(FillV(Smooth3x3(Green0)))(Blue0))(Green0))(Red0))(VNegate(MaxPixel(MultiplyV(Smooth3x3(MinPixel(Negate(FillV(Erode3x3(ModAvgV(VDiff(AlgebraicSum(Red0)(ThresholdInvV(ProjectionX(VAlgebraicProd(Dilate3x3(BinarizeV(VAlgebraicSum(MaxPixel(Blue0))(Blue0))(Blue0)))(Blue0)))(Green0)))(Blue0))(Gray0)))))))(MaxPixel(Dilate3x3(Blue0)))))))(Gray0)))(Smooth3x3(AlgebraicSum(BinarizeInv(AddV(VDiff(Smooth3x3(LowPass(Gray0)(BinarizeInv(AddV(Sharpen3x3(BinarizeInvV(Gray0)(Blue0)))(Green0)))))(Red0))(Green0)))(Gray0))))(Blue0)))(VAlgebraicProd(Sobel3x3(Negate(VAlgebraicProd(ProjectionX(VLogicalSum(VAlgebraicProd(LogicalSum(BinarizeInvV(AlgebraicSum(Median3x3(AddV(BinarizeV(LowPass(VAdd(Sobel3x3(ThresholdV(MultiplyV(AlgebraicProd(MinPixel(ProjectionY(Blue0)))(Blue0))(Gray0))(Red0)))(Green0))(LogicalSum(Erode3x3(Red0))(MultiplyV(ThresholdV(Blue0)(ThresholdInvV(SobelX3x3(AlgebraicProd(Dilate3x3(VLogicalSum(SobelX3x3(AddV(Blue0)(SubtractV(ThresholdInvV(AlgebraicProd(Gray0)(Binarize(VAlgebraicSum(ModAvgV(BoundedProd(Diff(MultiplyV(BinarizeV(Red0)(Green0))(Green0))(Gray0))(Green0))(Green0))(Green0))))(Gray0))(Gray0))))(Red0)))(Blue0)))(Blue0)))(Blue0))))(Gray0))(Red0)))(Red0))(Gray0))(Green0))(Blue0))(Red0)))(Smooth3x3(VDiff(AddV(SubtractV(Sharpen3x3(Binarize(ProjectionX(Subtract(ModAvgV(Smooth3x3(Blue0))(Green0))(Red0)))))(Gray0))(Blue0))(Green0))))))(MaxPixel(LogicalSum(ThresholdV(BinarizeV(Smooth3x3(ThresholdV(Add(FillV(Negate(Median3x3(SobelX3x3(AlgebraicSum(VAlgebraicSum(SelectV(Green0)(Sharpen3x3(VAdd(Green0)(Blue0)))(Blue0)(Red0))(Green0))(Red0))))))(Gray0))(Red0)))(Green0))(Blue0))(Green0))))))(VLogicalProd(Sharpen3x3(LogicalProd(Binarize(BinarizeInvV(ThresholdV(MinPixel(Red0))(BinarizeInv(MinPixel(ProjectionX(LowPass(Laplacian3x3(MaxPixel(VAlgebraicProd(ThresholdV(FillV(AlgebraicProd(LogicalProd(Negate(VNegate(Smooth3x3(SobelY3x3(Red0)))))(Gray0))(Red0)))(Red0))(Red0))))(AvgPixel(VLogicalProd(BinarizeInv(SelectV(ThresholdV(LowPass(SobelY3x3(ProjectionY(MultiplyV(BinarizeV(BinarizeInv(SelectV(AlgebraicSum(Sharpen3x3(SobelX3x3(Negate(Diff(Blue0)(Green0)))))(Gray0))(Gray0)(Gray0)(Green0)))(Gray0))(Red0))))(Blue0))(Blue0))(Gray0)(Blue0)(Blue0)))(Blue0))))))))(AvgPixel(AlgebraicProd(AlgebraicProd(Sharpen3x3(Smooth3x3(Green0)))(ModAvgV(Select(MinPixel(ThresholdV(Binarize(VAlgebraicSum(ProjectionY(Red0))(Blue0)))(Red0)))(Gray0))(AlgebraicProd(VAdd(HighPass(SobelY3x3(Select(SobelX3x3(ProjectionY(ThresholdV(MinPixel(AvgPixel(Gray0)))(Gray0))))(Gray0)))(Gray0))(Gray0))(Gray0))))(Red0)))))(VDiff(MaxPixel(LowPass(VAdd(Binarize(Blue0))(MaxPixel(Select(Select(Dilate3x3(Red0))(Dilate3x3(SobelX3x3(Binarize(VLogicalProd(LowPass(BinarizeInv(Blue0))(Gray0))(Green0))))))(Gray0))))(Blue0)))(Red0))))(FillV(ProjectionX(SelectV(AvgPixel(Red0))(Green0)(Blue0)(Gray0))))))))"));
	trees.push_back(Tree("(LogicalSum(AlgebraicProd(Dilate3x3(VLogicalProd(Binarize(AlgebraicProd(LogicalSum(MultiplyV(VAdd(ThresholdInvV(VAdd(VAlgebraicSum(ModAvgV(VAlgebraicSum(Laplacian3x3(HighPass(VLogicalSum(BinarizeInvV(LowPass(SobelY3x3(Negate(VDiff(MinPixel(Negate(MultiplyV(Smooth3x3(Gray0))(Red0))))(Red0))))(Green0))(AddV(AlgebraicProd(ThresholdV(Erode3x3(Median3x3(AlgebraicSum(Erode3x3(HighPass(Diff(BinarizeInvV(Red0)(Gray0))(VAlgebraicSum(VDiff(Red0)(MultiplyV(Select(SobelX3x3(VSubtract(Red0)(BinarizeInv(VSubtract(SobelY3x3(AddV(Blue0)(Green0)))(Red0)))))(Gray0))(Green0)))(Green0)))(Sobel3x3(Subtract(Sobel3x3(BinarizeInvV(Median3x3(Add(ProjectionX(VDiff(LowPass(AddV(Red0)(Green0))(Gray0))(Red0)))(Gray0)))(Blue0)))(Red0)))))(Gray0))))(Gray0))(Red0))(BinarizeInv(Negate(Diff(FillV(LowPass(Diff(VAlgebraicProd(SobelY3x3(SobelX3x3(SubtractV(Gray0)(Blue0))))(Gray0))(Blue0))(Gray0)))(Blue0))))))(Red0))(Gray0)))(Gray0))(Green0))(Green0))(Red0))(Blue0))(MinPixel(ProjectionY(Subtract(BoundedProd(Negate(AlgebraicSum(ThresholdV(BoundedProd(Green0)(Binarize(BinarizeV(SubtractV(Select(ModAvgV(Gray0)(Green0))(Gray0))(Blue0))(Gray0))))(Blue0))(Gray0)))(Gray0))(Green0)))))(Gray0))(LogicalProd(VSubtract(Red0)(Blue0))(Negate(Erode3x3(AlgebraicProd(VLogicalProd(MinPixel(Select(Erode3x3(BinarizeInv(MultiplyV(Green0)(LogicalProd(MinPixel(AlgebraicSum(MaxPixel(AlgebraicProd(VLogicalProd(MinPixel(LowPass(MultiplyV(LowPass(VLogicalProd(Median3x3(FillV(SobelY3x3(Gray0))))(Red0))(Blue0))(Blue0))(Gray0)))(Gray0))(Green0)))(Blue0)))(Gray0)))))(Green0)))(ProjectionY(Sobel3x3(VDiff(ModAvgV(FillV(SobelX3x3(BinarizeV(Select(VAlgebraicProd(AlgebraicSum(Blue0)(Green0))(Red0))(Green0))(Red0))))(Red0))(Blue0)))))(Blue0))))))(Red0)))(VLogicalProd(ProjectionX(LogicalProd(MinPixel(BinarizeInvV(LogicalSum(VAlgebraicProd(ThresholdInvV(Smooth3x3(Erode3x3(VAdd(BinarizeInv(LogicalProd(BinarizeV(Smooth3x3(Gray0))(Red0))(Green0)))(Green0))))(Green0))(Blue0))(Blue0))(VLogicalSum(BoundedProd(VAlgebraicProd(Sobel3x3(VLogicalSum(VDiff(MaxPixel(VAlgebraicProd(Diff(BoundedProd(Binarize(Blue0))(Red0))(Gray0))(Sharpen3x3(Smooth3x3(VLogicalProd(MaxPixel(LowPass(BoundedProd(ProjectionY(VSubtract(MaxPixel(AddV(Green0)(Gray0)))(Blue0)))(Blue0))(Green0)))(Gray0))))))(Gray0))(Gray0)))(Gray0))(Green0))(Green0))))(Binarize(VAlgebraicProd(VSubtract(Erode3x3(MultiplyV(ProjectionY(ThresholdV(Sobel3x3(VSubtract(SobelY3x3(Gray0))(Red0)))(Gray0)))(Blue0)))(Gray0))(Gray0)))))(FillV(MaxPixel(AlgebraicProd(BoundedProd(Gray0)(AddV(SobelX3x3(Negate(BinarizeInv(BinarizeV(Negate(Blue0))(Blue0)))))(Green0)))(Green0)))))))(Red0))(Dilate3x3(ModAvgV(SelectV(Binarize(AlgebraicProd(Erode3x3(VLogicalProd(SubtractV(Binarize(Subtract(SubtractV(AlgebraicSum(Red0)(Red0))(Gray0))(ThresholdV(AlgebraicSum(MultiplyV(VSubtract(ProjectionX(SubtractV(Dilate3x3(LogicalSum(Red0)(Erode3x3(BinarizeInv(HighPass(SelectV(Diff(SelectV(AlgebraicProd(Red0)(Blue0))(Gray0)(BinarizeV(Dilate3x3(VDiff(VAlgebraicProd(BinarizeInvV(VSubtract(Add(AddV(Laplacian3x3(MinPixel(Red0)))(Red0))(Blue0))(Red0))(Red0))(Blue0))(Green0)))(Gray0))(Gray0))(Gray0))(Gray0)(Gray0)(Green0))(Red0))))))(Red0)))(Red0))(Smooth3x3(Gray0)))(Gray0))(Sharpen3x3(LogicalSum(LogicalSum(SubtractV(BoundedProd(AvgPixel(Smooth3x3(Select(AlgebraicProd(VLogicalSum(VNegate(MaxPixel(VAlgebraicProd(Blue0)(SobelY3x3(VNegate(Blue0))))))(SobelY3x3(Select(Sharpen3x3(Binarize(ProjectionY(VLogicalProd(BinarizeV(LogicalProd(VLogicalSum(VLogicalSum(SelectV(ThresholdInvV(MultiplyV(VLogicalSum(Select(HighPass(SubtractV(Smooth3x3(ProjectionY(LowPass(Laplacian3x3(MultiplyV(AddV(MaxPixel(Red0))(Gray0))(Gray0)))(Blue0))))(Green0))(Gray0))(Red0))(Blue0))(Red0))(Gray0))(Blue0)(Blue0)(Blue0))(Gray0))(Gray0))(Red0))(Blue0))(Add(Binarize(Median3x3(ThresholdV(Add(Blue0)(LowPass(VSubtract(VAlgebraicSum(Laplacian3x3(BinarizeInv(Green0)))(Gray0))(Gray0))(Red0)))(Red0))))(Green0))))))(Green0))))(Blue0))(Green0))))(Green0))(Red0))(Gray0))(Blue0))))))(Red0))(Subtract(Sharpen3x3(Laplacian3x3(Add(VAlgebraicProd(VNegate(Erode3x3(VAlgebraicSum(LowPass(VLogicalSum(AddV(ThresholdV(Red0)(Green0))(Blue0))(Green0))(Gray0))(Blue0))))(Green0))(Red0))))(SobelY3x3(BinarizeInvV(AlgebraicProd(Sharpen3x3(MinPixel(BinarizeInv(SobelX3x3(SobelX3x3(ModAvgV(AvgPixel(ThresholdV(Gray0)(Negate(SelectV(AvgPixel(Binarize(ThresholdInvV(ProjectionX(BinarizeV(VLogicalProd(SelectV(VNegate(Subtract(Median3x3(Diff(ModAvgV(Erode3x3(BinarizeV(MaxPixel(Red0))(Blue0)))(Blue0))(Gray0)))(Green0)))(Green0)(Green0)(Red0))(Blue0))(Green0)))(Gray0))))(Green0)(Green0)(Blue0)))))(Blue0)))))))(Red0))(Green0))))))(VDiff(Diff(BinarizeV(LogicalSum(MultiplyV(LowPass(AvgPixel(AlgebraicSum(ProjectionX(MultiplyV(Subtract(Select(SubtractV(VDiff(ThresholdV(Sobel3x3(HighPass(Sharpen3x3(VLogicalProd(Negate(BoundedProd(FillV(Green0))(Green0)))(Green0)))(Red0)))(Red0))(Blue0))(Green0))(ModAvgV(Median3x3(AvgPixel(Add(LowPass(Sobel3x3(LogicalProd(SobelY3x3(MaxPixel(SobelY3x3(Green0))))(Red0)))(Blue0))(Gray0))))(Green0)))(Blue0))(Green0)))(Blue0)))(Gray0))(Green0))(Sobel3x3(Red0)))(SubtractV(VSubtract(ProjectionY(BoundedProd(Sharpen3x3(Sobel3x3(VAdd(Blue0)(LogicalSum(Subtract(BinarizeInvV(VLogicalSum(SelectV(Blue0)(Blue0)(BoundedProd(Dilate3x3(ModAvgV(ThresholdInvV(SobelY3x3(BinarizeInv(Dilate3x3(Dilate3x3(LogicalSum(BinarizeInv(Laplacian3x3(Select(Gray0)(Red0))))(Red0))))))(Red0))(Red0)))(ProjectionY(AddV(SobelY3x3(ProjectionX(Sobel3x3(Gray0))))(Laplacian3x3(AlgebraicSum(Erode3x3(Blue0))(AddV(Erode3x3(Laplacian3x3(HighPass(Median3x3(Sharpen3x3(Sobel3x3(Green0))))(Gray0))))(Green0)))))))(Red0))(Blue0))(Red0))(Red0))(LowPass(ProjectionX(VSubtract(AddV(AvgPixel(LogicalSum(Dilate3x3(SobelY3x3(ProjectionX(Red0))))(Select(Diff(FillV(MinPixel(Binarize(BoundedProd(Red0)(HighPass(VAdd(LowPass(FillV(ThresholdInvV(VDiff(Laplacian3x3(VAdd(Sobel3x3(BoundedProd(Red0)(Blue0)))(Blue0)))(Blue0))(Red0)))(Red0))(Blue0))(Green0))))))(Green0))(Blue0))))(Red0))(Gray0)))(Red0))))))(Gray0)))(Gray0))(LogicalSum(VAdd(FillV(SubtractV(AddV(Erode3x3(Select(VAlgebraicSum(Sobel3x3(Erode3x3(Sharpen3x3(LowPass(Dilate3x3(VAlgebraicProd(VLogicalSum(ThresholdV(Blue0)(Green0))(Blue0))(Red0)))(Gray0)))))(Blue0))(Red0)))(Blue0))(Blue0)))(Blue0))(Red0))))(Red0))(Erode3x3(LogicalSum(Diff(BinarizeInv(SubtractV(AlgebraicProd(Blue0)(VLogicalProd(MaxPixel(SobelX3x3(Negate(Red0))))(Median3x3(Gray0))))(Gray0)))(Smooth3x3(Diff(HighPass(MultiplyV(Diff(Laplacian3x3(Dilate3x3(LogicalSum(Blue0)(Sobel3x3(LowPass(Smooth3x3(SobelX3x3(VLogicalProd(VDiff(Green0)(Red0))(Blue0))))(Blue0))))))(Green0))(Blue0))(Blue0))(Red0))))(MinPixel(SubtractV(ProjectionY(BinarizeInv(Green0)))(Gray0))))))))(Subtract(VNegate(Laplacian3x3(Laplacian3x3(Blue0))))(LogicalProd(VDiff(VAlgebraicSum(VAlgebraicSum(Laplacian3x3(Select(Green0)(Blue0)))(Green0))(Blue0))(ThresholdInvV(Green0)(Sharpen3x3(VLogicalSum(BinarizeV(SobelY3x3(AvgPixel(SelectV(Sharpen3x3(Binarize(Blue0)))(VLogicalProd(VLogicalProd(FillV(Select(VSubtract(BinarizeInv(ModAvgV(LowPass(VNegate(SobelY3x3(VLogicalProd(AvgPixel(Green0))(Sharpen3x3(VLogicalSum(Blue0)(Blue0))))))(Gray0))(Gray0)))(Blue0))(Blue0)))(VAlgebraicProd(FillV(HighPass(Sharpen3x3(Gray0))(Gray0)))(Gray0)))(Red0))(Red0)(Green0))))(Gray0))(Green0)))))(Blue0)))(Erode3x3(LogicalProd(AvgPixel(VNegate(VLogicalSum(VSubtract(AvgPixel(Laplacian3x3(AlgebraicProd(VSubtract(Add(Gray0)(Blue0))(Gray0))(Blue0))))(ThresholdV(ThresholdV(LogicalSum(HighPass(LogicalSum(ProjectionY(BoundedProd(SubtractV(MaxPixel(ThresholdInvV(VLogicalSum(ProjectionY(MaxPixel(AddV(VAlgebraicProd(SobelY3x3(VAlgebraicProd(HighPass(ThresholdV(Red0)(Red0))(Blue0))(Gray0)))(Blue0))(Red0))))(Gray0))(Blue0)))(Blue0))(VLogicalSum(VAdd(LogicalProd(Sharpen3x3(MultiplyV(Sobel3x3(AlgebraicSum(LogicalSum(BinarizeInv(MaxPixel(Red0)))(Green0))(Red0)))(Green0)))(Red0))(Gray0))(Blue0))))(MaxPixel(AlgebraicProd(VDiff(Sobel3x3(Blue0))(LowPass(BinarizeV(ThresholdV(Dilate3x3(Green0))(BoundedProd(Smooth3x3(SelectV(Median3x3(MinPixel(ThresholdV(Laplacian3x3(Green0))(Green0))))(Gray0)(Blue0)(Gray0)))(Gray0)))(Blue0))(Blue0)))(Green0))))(Gray0))(AvgPixel(Gray0)))(VNegate(Select(VNegate(Laplacian3x3(VSubtract(Erode3x3(Dilate3x3(Dilate3x3(Green0))))(Subtract(SobelX3x3(VLogicalSum(SobelX3x3(SubtractV(Gray0)(Gray0)))(Red0)))(Gray0)))))(Red0))))(MultiplyV(Sobel3x3(BoundedProd(Median3x3(Dilate3x3(AlgebraicProd(ProjectionX(VLogicalProd(BoundedProd(SobelX3x3(AddV(VAlgebraicProd(SubtractV(Negate(MinPixel(Blue0)))(Green0))(Green0))(Red0)))(Green0))(Blue0)))(Red0))))(Red0)))(Erode3x3(AlgebraicProd(LowPass(AvgPixel(VAlgebraicProd(ThresholdV(Erode3x3(Laplacian3x3(SobelX3x3(LogicalProd(Green0)(Gray0)))))(Green0))(Green0)))(Gray0))(Gray0))))))(Blue0))))(VAlgebraicSum(Sharpen3x3(BinarizeInvV(VAdd(Sharpen3x3(BinarizeV(AddV(Smooth3x3(SobelX3x3(Select(BinarizeInv(ProjectionX(ProjectionY(Blue0))))(Gray0))))(Blue0))(Blue0)))(Gray0))(Gray0)))(Gray0))))(BinarizeInvV(Binarize(BinarizeV(MultiplyV(MinPixel(VDiff(VLogicalSum(AlgebraicSum(VSubtract(MultiplyV(Median3x3(Subtract(AvgPixel(Erode3x3(Sobel3x3(ProjectionY(FillV(MultiplyV(ModAvgV(VLogicalSum(Red0)(VAlgebraicProd(ThresholdInvV(ProjectionY(VAlgebraicProd(BinarizeInv(VDiff(SobelX3x3(AddV(Diff(HighPass(VLogicalProd(Add(VLogicalProd(Blue0)(Gray0))(Green0))(BinarizeV(FillV(VAlgebraicSum(VAlgebraicSum(VLogicalProd(FillV(Diff(ProjectionY(MinPixel(SelectV(Blue0)(Gray0)(Blue0)(Red0))))(Gray0)))(Green0))(Gray0))(Red0)))(Red0)))(Gray0))(Green0))(AddV(Erode3x3(Erode3x3(Diff(SubtractV(VAdd(Sharpen3x3(ThresholdInvV(VLogicalProd(LogicalProd(Green0)(Red0))(AlgebraicProd(Select(AlgebraicSum(Green0)(Red0))(Binarize(MinPixel(Gray0))))(Blue0)))(Red0)))(Blue0))(Blue0))(Red0))))(Green0))))(Gray0)))(SobelY3x3(SobelY3x3(LogicalProd(BinarizeInvV(Sharpen3x3(Dilate3x3(BinarizeInv(LogicalSum(VLogicalSum(HighPass(Median3x3(ThresholdV(VAlgebraicSum(LogicalSum(SobelX3x3(ThresholdV(Blue0)(SubtractV(Diff(VLogicalSum(Gray0)(ProjectionX(Blue0)))(Red0))(Green0))))(Blue0))(Gray0))(Green0)))(Gray0))(Gray0))(Red0)))))(Gray0))(ThresholdInvV(Erode3x3(VNegate(Gray0)))(Negate(ModAvgV(SubtractV(SobelY3x3(Dilate3x3(MinPixel(ThresholdV(ModAvgV(Negate(Green0))(Blue0))(Gray0)))))(Red0))(Gray0)))))))))(SubtractV(Median3x3(AlgebraicProd(Green0)(SelectV(Gray0)(Laplacian3x3(VLogicalProd(Select(Gray0)(Smooth3x3(Binarize(SobelX3x3(Subtract(Red0)(FillV(Smooth3x3(VLogicalSum(VDiff(BinarizeInvV(Laplacian3x3(Sharpen3x3(Sobel3x3(VDiff(ModAvgV(VAlgebraicProd(SobelY3x3(MultiplyV(VSubtract(HighPass(Green0)(Gray0))(Gray0))(Red0)))(MinPixel(ProjectionY(SelectV(ProjectionX(ThresholdV(Binarize(Smooth3x3(Diff(LogicalProd(SobelY3x3(Green0))(Gray0))(Blue0))))(Blue0)))(Green0)(Red0)(Blue0)))))(Red0))(Gray0)))))(Green0))(Blue0))(SobelX3x3(SobelX3x3(VAlgebraicProd(VAdd(VLogicalSum(ThresholdInvV(VNegate(SubtractV(Green0)(VLogicalProd(VLogicalProd(VAlgebraicSum(ProjectionX(Red0))(Laplacian3x3(SobelX3x3(Laplacian3x3(Diff(Dilate3x3(Sharpen3x3(Sobel3x3(Diff(Blue0)(Blue0)))))(Blue0))))))(Red0))(Blue0))))(Gray0))(Green0))(Red0))(Red0))))))))))))(Green0)))(Green0)(Blue0))))(ThresholdInvV(SelectV(VAdd(MaxPixel(SelectV(Sobel3x3(LogicalSum(ProjectionY(Negate(AlgebraicProd(Green0)(Blue0))))(Red0)))(Gray0)(Red0)(Blue0)))(Red0))(Gray0)(Green0)(Red0))(Gray0))))(Blue0)))(Gray0))(LogicalSum(BoundedProd(AvgPixel(SubtractV(Select(MinPixel(BinarizeInvV(SubtractV(VLogicalSum(MultiplyV(Red0)(Gray0))(Green0))(Blue0))(Gray0)))(SelectV(VAlgebraicSum(HighPass(Laplacian3x3(Laplacian3x3(Smooth3x3(LogicalSum(VLogicalSum(Diff(LowPass(Blue0)(Blue0))(Gray0))(Green0))(Gray0)))))(Gray0))(Green0))(Green0)(Green0)(Red0)))(Green0)))(Gray0))(Red0))))))))(Green0)))(Blue0))(Green0))(Red0))(HighPass(VAdd(BinarizeInv(BinarizeInvV(Binarize(VNegate(Diff(AddV(Blue0)(ThresholdV(Binarize(VLogicalProd(BoundedProd(Select(Blue0)(Red0))(Red0))(Gray0)))(Green0)))(Green0))))(Green0)))(Red0))(LowPass(VSubtract(BinarizeInv(ThresholdV(MultiplyV(Laplacian3x3(VAlgebraicSum(Subtract(Red0)(Sharpen3x3(Red0)))(Blue0)))(Blue0))(Blue0)))(Gray0))(Green0))))(Gray0)))(MinPixel(LogicalSum(VAdd(HighPass(SelectV(SubtractV(Negate(Add(Select(MinPixel(Diff(LowPass(Dilate3x3(ThresholdV(Sobel3x3(AddV(VAlgebraicSum(Green0)(AddV(ThresholdV(ProjectionX(Binarize(VDiff(Binarize(AvgPixel(SelectV(VSubtract(Smooth3x3(Green0))(Blue0))(Gray0)(Blue0)(Green0))))(Green0))))(Red0))(Red0)))(Blue0)))(Blue0)))(Red0))(Gray0)))(Red0))(Gray0)))(Green0))(Gray0)(Red0)(Blue0))(Red0))(Green0))(Add(Gray0)(Binarize(Median3x3(Select(VAlgebraicSum(MultiplyV(Green0)(SelectV(MultiplyV(Diff(LogicalProd(Red0)(Gray0))(Red0))(Green0))(Blue0)(Gray0)(Blue0)))(Green0))(Blue0))))))))(Blue0)))(VAlgebraicProd(Sobel3x3(Negate(Dilate3x3(BinarizeInvV(ModAvgV(HighPass(Median3x3(SobelX3x3(Median3x3(AlgebraicSum(Green0)(SubtractV(Add(AddV(ProjectionX(ModAvgV(BinarizeInv(Diff(BinarizeInv(AddV(SobelY3x3(Median3x3(ThresholdInvV(Blue0)(Red0))))(Red0)))(Gray0)))(Gray0)))(Blue0))(Red0))(Blue0))))))(Red0))(Green0))(Blue0)))))(MaxPixel(LogicalSum(ThresholdV(BinarizeV(Smooth3x3(ThresholdV(LogicalProd(SubtractV(MultiplyV(ThresholdInvV(Binarize(BoundedProd(LogicalProd(VNegate(Red0))(SelectV(Subtract(Gray0)(AlgebraicSum(AddV(VAlgebraicProd(VAlgebraicProd(VLogicalSum(AlgebraicProd(Erode3x3(MinPixel(ProjectionX(LogicalSum(SobelY3x3(LogicalSum(VNegate(Smooth3x3(Gray0)))(Blue0)))(Red0)))))(Blue0))(Gray0))(LowPass(VAlgebraicProd(HighPass(Gray0)(Red0))(Red0))(Green0)))(Red0))(Gray0))(Gray0)))(Gray0)(Blue0)(Red0)))(Gray0)))(Blue0))(VLogicalSum(VSubtract(Blue0)(SobelY3x3(VAlgebraicProd(VDiff(VLogicalSum(Smooth3x3(VAlgebraicSum(BinarizeInvV(ModAvgV(VSubtract(Green0)(ModAvgV(BinarizeInvV(BinarizeInv(Red0))(Red0))(Red0)))(Green0))(Green0))(Red0)))(Green0))(Green0))(Red0))))(Gray0)))(Blue0))(Green0))(Red0)))(Green0))(Blue0))(Green0))))))(VLogicalProd(Sharpen3x3(LogicalProd(Negate(Laplacian3x3(SobelX3x3(VNegate(LogicalProd(Subtract(ProjectionY(VLogicalProd(VLogicalProd(MultiplyV(Red0)(Red0))(Blue0))(Red0)))(Blue0))(Blue0))))))(VDiff(MaxPixel(AlgebraicProd(Laplacian3x3(MinPixel(AvgPixel(SelectV(SelectV(Green0)(LowPass(BinarizeInv(LowPass(VAdd(Red0)(Red0))(Green0)))(MinPixel(ThresholdInvV(Gray0)(SubtractV(MaxPixel(ModAvgV(AlgebraicProd(VLogicalProd(ProjectionY(Erode3x3(BinarizeV(Subtract(Negate(Sobel3x3(ModAvgV(VSubtract(AlgebraicProd(LogicalProd(Green0)(Red0))(Blue0))(Gray0))(Blue0))))(Red0))(Gray0))))(Red0))(Gray0))(Red0)))(Green0)))))(Red0)(LowPass(SelectV(BinarizeV(Blue0)(BinarizeInvV(ModAvgV(VLogicalSum(Red0)(ProjectionY(Erode3x3(LogicalProd(MultiplyV(Blue0)(Red0))(Blue0)))))(Blue0))(Green0)))(Gray0)(Red0)(Blue0))(Blue0)))(Subtract(Negate(Diff(Blue0)(HighPass(Add(Sobel3x3(Sharpen3x3(Subtract(Red0)(LowPass(Green0)(Blue0)))))(SelectV(VAlgebraicSum(Green0)(ProjectionX(VNegate(LowPass(Green0)(Blue0)))))(Red0)(Gray0)(Green0)))(Red0))))(Gray0))(Gray0)(Red0)))))(Blue0)))(VSubtract(Sobel3x3(HighPass(Red0)(VLogicalProd(VAlgebraicSum(SobelX3x3(ProjectionX(Gray0)))(VAlgebraicSum(Median3x3(BinarizeInv(SobelY3x3(ProjectionY(VLogicalProd(Laplacian3x3(Diff(Gray0)(Gray0)))(Gray0))))))(Gray0)))(Gray0))))(Gray0)))))(FillV(ProjectionX(SelectV(VAdd(MinPixel(Add(SobelX3x3(Binarize(VAlgebraicSum(Red0)(LogicalSum(MinPixel(Binarize(Binarize(Red0))))(AvgPixel(Subtract(Blue0)(Negate(AddV(ThresholdV(VAlgebraicSum(ThresholdV(VSubtract(VNegate(Binarize(Blue0)))(Gray0))(Gray0))(Red0))(Blue0))(Gray0)))))))))(Green0)))(Blue0))(Green0)(Blue0)(Gray0))))))))"));
	  
	std::deque<IVVertex> nodes;
	IVGraph iv;
	
	uniteTreesToDag<Tree::GraphType>(trees.begin(), trees.end(), iv, [](const Tree::GraphType::vertex_property_type& src_p, IVGraph::vertex_property_type& dst_p) {
		dst_p.label = src_p.label;
	});

	for (auto i = 0u; i < trees.size(); i++) {
		std::cout << "\nGraph #" << i;
		std::cout << "\n Vertices:  " << boost::num_vertices(trees[i].getGraph());
		std::cout << "\n Depth:     " << getDepth(trees[i].getGraph());
		std::cout << "\n Max Degree:" << getMaxDegree(trees[i].getGraph());
		#if 0
		print_graph(trees[0].getGraph(), get(&Garnet::Tree::VertexProp::label, trees[i].getGraph()));
		#endif
	}

	std::cout << "\n-----" << std::endl;

	#if 0
	print_graph(iv);
	#endif
	std::cout << "\nVertices:  " << boost::num_vertices(iv);
	std::cout << "\nDepth:     " << getDepth(iv);
	std::cout << "\nMax Degree:" << getMaxDegree(iv);

	std::cout << "\n-----" << std::endl;

	#if 0
	print_graph(iv, get(&Garnet::ivgraph_impl::VProp::label, iv));
	#endif

	#if 0
	std::cout << "\n-----" << std::endl;

	boost::topological_sort(iv, std::back_inserter(nodes));
	std::string pgraph = Garnet::getSExpression<std::string>(nodes.back(), iv);
	//std::cout << "\nSExp:      " << pgraph << std::endl;
	Tree t2(pgraph);
	std::cout << "\nVertices:  " << boost::num_vertices(t2.getGraph());
	std::cout << "\nDepth:     " << getDepth(t2.getGraph());
	std::cout << "\nMax Degree:" << getMaxDegree(t2.getGraph());
	#endif

	std::cout << "\n-----" << std::endl;

	std::map<IVVertex, uint32_t> reg;

	typedef boost::adjacency_list<
			boost::listS,        // OutEdgeList
			boost::vecS,         // VertexList
			boost::undirectedS,  // Directed
			dag::VProp<Garnet::IVGraph> // VertexProperty
		> LineGraph;

	{	
		LineGraph l;
		nodes.clear();
		reg.clear();
		auto vertices = boost::vertices(iv);
		std::for_each(vertices.first, vertices.second, [&](IVVertex u) {
			iv[u].color = -1;
		});

		topological_sort(1, iv, std::back_inserter(nodes), reg);
		#if 0
		std::cout << "\n[BGL Method]"
				  << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
						os << boost::format(" %s:%u") % iv[u].label % reg[u];
					})
				  << std::endl;
		#endif

		std::cout << "\n-----" << std::endl;

		auto num_colors = dag::edge_coloring(iv, nodes.cbegin(), nodes.cend(), l);
		#if 0
		print_graph(l, get(&dag::VProp<Garnet::IVGraph>::label, l));
		#endif
		std::cout << "\n" << num_colors << " colors.";
		
	//	boost::graph_traits<LineGraph>::vertices_size_type num_colors;
	//	num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));
	
		#if 0
		std::cout << "\n-----" << std::endl;
		std::cout << "\n[Colored after BGL Method]"
				  << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
						os << boost::format(" %s:%d") % iv[u].label % iv[u].color;
					})
				  << std::endl;
		#endif
	}

	std::cout << "\n=====" << std::endl;

	{
		LineGraph l;
		nodes.clear();
		reg.clear();
		auto vertices = boost::vertices(iv);
		std::for_each(vertices.first, vertices.second, [&](IVVertex u) {
			iv[u].color = -1;
		});

		topological_sort(0, iv, std::back_inserter(nodes), reg);
		#if 0
		std::cout << "\n[Sarker Method]"
				  << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
						os << boost::format(" %s:%u") % iv[u].label % reg[u];
					})
				  << std::endl;
		#endif

		std::cout << "\n-----" << std::endl;
		
		auto num_colors = dag::edge_coloring(iv, nodes.cbegin(), nodes.cend(), l);
		#if 0
		print_graph(l, get(&dag::VProp<Garnet::IVGraph>::label, l));
		#endif
		std::cout << "\n" << num_colors << " colors.";
		
	//	boost::graph_traits<LineGraph>::vertices_size_type num_colors;
	//	num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));

		#if 0
		std::cout << "\n-----" << std::endl;
		std::cout << "\n[Colored after Sarker Method]"
				  << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
						os << boost::format(" %s:%d") % iv[u].label % iv[u].color;
					})
				  << std::endl;
		#endif
	}
	
#if 0

    Tree  t[] = {
		// [0]
        Tree("(0)"),
		// [1] 
        Tree("(Sink0(X1a(X0a)))"), 
		// [2]
        Tree("(Sink1(X2a(X1a(X0a))))"),
		// [3]
        Tree("(Sink0(f(b)(f(a)(g(b)))))"),
		// [4]
        Tree("(Sink1(f(b)(g(a))))"),
		// [5]
        Tree("(Sink2(f(a)(g(b))))"),
		// [6]
        Tree("(Sink3(x(y)(y(z))))"),

		// [7] Chromosomes 4329
		Tree("(A0a(A1a(A2a(A3a(A4a)(A4b(A5a(A6a(A7a(A8a(A9a(A10a))(A9b)(A9c)(A9d))(A8b))(A7b))(A6b))(A5b)))(A3b))(A2b))(A1b))"),

		// [8] 369 vertices
		Tree("(VAdd(SubtractV(VAdd(SubtractV(Red0)(VDiff(VSubtract(VAlgebraicSum(VAlgebraicProd(VAdd(BinarizeInvV(SobelY3x3(Smooth3x3(SubtractV(BinarizeV(ModAvgV(VLogicalProd(VNegate(Diff(Erode3x3(Dilate3x3(AvgPixel(SelectV(Binarize(Median3x3(VAlgebraicSum(BinarizeInvV(Blue0)(Add(ProjectionY(BinarizeV(Binarize(VSubtract(Const_0.16666)(Gray0)))(Gray0)))(Const_0.16666)))(Const_0.16666))))(Gray0)(Const_0.16666)(Red0)))))(Green0)))(Green0))(Gray0))(Gray0))(Green0))))(AlgebraicSum(Threshold(VAdd(VSubtract(AlgebraicSum(Smooth3x3(Select(VLogicalSum(Binarize(SobelX3x3(Median3x3(SobelY3x3(BinarizeInv(MinPixel(MaxPixel(Gray0))))))))(Green0))(Red0)))(Const_0.16666))(Red0))(Gray0)))(AvgPixel(MultiplyV(FillV(Laplacian3x3(BinarizeInvV(VAlgebraicSum(LogicalProd(Sharpen3x3(ModAvgV(VAdd(VAlgebraicProd(MultiplyV(Erode3x3(LowPass(Diff(MaxPixel(SobelX3x3(Const_0.16666)))(Const_0.16666))(Red0)))(Gray0))(Red0))(Red0))(Gray0)))(MaxPixel(BinarizeV(BinarizeInvV(VSubtract(VSubtract(AlgebraicSum(VNegate(Sobel3x3(Const_0.16666)))(SubtractV(FillV(MaxPixel(ProjectionY(Diff(Blue0)(Const_0.16666)))))(BoundedProd(Laplacian3x3(Const_0.16666))(Blue0))))(Blue0))(Green0))(Green0))(Blue0))))(LowPass(VAlgebraicSum(SobelY3x3(Sharpen3x3(ThresholdInv(Gray0))))(LogicalProd(ThresholdInvV(BinarizeInv(AddV(LogicalSum(Green0)(VNegate(AddV(VLogicalProd(Const_0.16666)(Green0))(Green0))))(Median3x3(SobelY3x3(AlgebraicSum(SubtractV(Sharpen3x3(LogicalProd(FillV(Const_0.16666))(Diff(MaxPixel(LogicalProd(Sharpen3x3(Green0))(Red0)))(Const_0.16666))))(Green0))(Gray0))))))(Green0))(Const_0.16666)))(Green0)))(Red0))))(Const_0.16666)))))(VDiff(VAlgebraicSum(Select(HighPass(Add(SelectV(HighPass(VAlgebraicProd(BoundedProd(VSubtract(MultiplyV(ModAvgV(VLogicalProd(AddV(AlgebraicSum(Green0)(VAdd(Subtract(Const_0.16666)(VAdd(Blue0)(Const_0.16666)))(Red0)))(Gray0))(Const_0.16666))(Blue0))(Gray0))(Gray0))(Const_0.16666))(Gray0))(VAlgebraicSum(Smooth3x3(ThresholdInv(Threshold(BinarizeInv(VSubtract(Diff(MultiplyV(MaxPixel(AddV(Sharpen3x3(AlgebraicProd(Add(Sharpen3x3(Const_0.16666))(Threshold(SubtractV(ThresholdInv(VLogicalProd(Gray0)(AlgebraicProd(Add(SubtractV(Blue0)(Blue0))(VAlgebraicProd(ThresholdV(VAlgebraicSum(Gray0)(Green0))(Red0))(Gray0)))(Blue0))))(Gray0))))(Blue0)))(Blue0)))(Const_0.16666))(Blue0))(Const_0.16666))))))(Red0)))(Red0)(Blue0)(Const_0.16666))(Green0))(HighPass(ThresholdInv(Negate(Diff(VAlgebraicProd(SubtractV(SelectV(Add(VDiff(Sharpen3x3(Blue0))(Const_0.16666))(Gray0))(Const_0.16666)(Green0)(Gray0))(Green0))(Const_0.16666))(Red0))))(BinarizeInvV(SobelY3x3(SelectV(Add(Red0)(Subtract(VSubtract(Const_0.16666)(Smooth3x3(HighPass(Threshold(VSubtract(Green0)(Green0)))(Blue0))))(Blue0)))(Green0)(Red0)(Blue0)))(Green0))))(Red0))(Gray0))(Const_0.16666)))(Green0))(Gray0))(Const_0.16666))(Const_0.16666)))(LogicalSum(Median3x3(VLogicalSum(SelectV(AddV(VLogicalSum(Diff(Diff(Subtract(HighPass(ThresholdV(FillV(Smooth3x3(BinarizeInvV(Laplacian3x3(Const_0.16666))(Const_0.16666))))(Red0))(Green0))(Const_0.16666))(Gray0))(Blue0))(Red0))(Gray0))(Blue0)(Gray0)(Red0))(AlgebraicProd(MultiplyV(Negate(AddV(Add(Diff(ThresholdInv(Laplacian3x3(BinarizeInvV(ThresholdV(BoundedProd(VSubtract(ThresholdInv(ProjectionX(BinarizeInv(VAlgebraicSum(VAlgebraicSum(Const_0.16666)(Green0))(Red0)))))(Blue0))(Green0))(Green0))(Blue0))))(Const_0.16666))(Gray0))(ProjectionX(VAdd(ThresholdV(Negate(SubtractV(SubtractV(MinPixel(SobelX3x3(ProjectionY(LogicalSum(Blue0)(Const_0.16666)))))(Blue0))(Blue0)))(Red0))(Const_0.16666)))))(Blue0))(Green0))))(Blue0)))(Const_0.16666))(AlgebraicProd(MultiplyV(SobelX3x3(BoundedProd(Binarize(Green0))(VSubtract(VAdd(Green0)(VLogicalSum(VAdd(Add(ProjectionX(Blue0))(Blue0))(Gray0))(ThresholdInv(Green0))))(Green0))))(Green0))(Blue0)))"),
		
		
		// [9]
		Tree("(_IVStore0(Add(Median3x3(SelectV(MaxPixel(Red0))(VDiff(VDiff(VSubtract(VLogicalProd(Gray0)(MinPixel(ThresholdInvV(Dilate3x3(VAlgebraicProd(Dilate3x3(LogicalSum(BinarizeInv(ProjectionX(SubtractV(HighPass(AlgebraicSum(BinarizeInvV(LogicalSum(VDiff(BinarizeInv(SobelY3x3(Red0)))(Green0))(Red0))(Red0))(Red0))(Gray0))(SelectV(MaxPixel(Red0))(VDiff(VDiff(VSubtract(VLogicalProd(Gray0)(BinarizeInv(VDiff(SubtractV(Diff(Diff(Sobel3x3(ModAvgV(HighPass(Diff(Add(Red0)(Red0))(Green0))(Green0))(Red0)))(Gray0))(Green0))(Gray0))(Gray0))))(Red0))(Red0))(Gray0))(Green0)(Dilate3x3(VAlgebraicProd(ThresholdInvV(FillV(VDiff(AddV(AvgPixel(SelectV(SobelY3x3(Diff(BoundedProd(Blue0)(Gray0))(Gray0)))(Green0)(Blue0)(Gray0)))(Gray0))(Red0)))(Red0))(Green0)))))))(Red0)))(Gray0)))(Red0))))(Red0))(MaxPixel(Gray0)))(Gray0))(Median3x3(VAdd(BinarizeInv(VDiff(VLogicalSum(MinPixel(Median3x3(MultiplyV(Median3x3(BinarizeInv(SobelY3x3(MultiplyV(Median3x3(Green0))(Green0)))))(Blue0))))(Red0))(Gray0)))(Blue0)))(Red0)))(Gray0)))"),
	};
	IVGraph iv;
	
	// Merge IVTrees into a IVGraph.
	{
		std::vector<Tree> trees;

		trees.push_back(t[4]);
		trees.push_back(t[3]);
		trees.push_back(t[5]);
		trees.push_back(t[6]);
		//trees.push_back(t[7]);
		//trees.push_back(t[8]);
				
		convertIVTreesToIVGraph(trees, iv);
		std::cout << __LINE__ << std::endl;

		auto vertices = boost::vertices(iv);
		std::cout << "\n" << print_range(vertices.first, vertices.second, " ", [&] (std::ostream& os, IVVertex u) {
			os << boost::format(" %s") % iv[u].label;
		}) << std::endl;
		
		print_graph(iv);
		print_graph(iv, get(&Garnet::ivgraph_impl::VProp::label, iv));
	}

	// Topological sort for an aggregated graph
	//   Sethi-Ullman.
	{	
		std::deque<IVVertex> nodes;
		std::map<IVVertex, uint32_t> reg;
		
		StopWatch watch;
		for (int i = 0; i < 1; i++) {
			nodes.clear();
			topological_sort(0, iv, std::back_inserter(nodes), reg);
		}
		watch.stop();

		std::cout << (boost::format("\n[Sarker Method] %.3f us\n") % (watch.get() * 1000000)) << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
			os << boost::format(" %s:%u") % iv[u].label % reg[u];
		}) << std::endl;
	}

	// Topological sort for an aggregated graph
	//   Default topo sort.
	{	
		std::deque<IVGraph::vertex_descriptor> nodes;
	
		topological_sort(1, iv, std::back_inserter(nodes));

		std::cout << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
			os << boost::format(" %s:-") % iv[u].label;
		}) << std::endl;
	}
	
	// Remove introns.
	{
		std::deque<IVGraph::vertex_descriptor> nodes;
		nodes.push_back(0);
		//nodes.push_back(4); // Sink1
		//nodes.push_back(8); // Sink0
		//nodes.push_back(9); // Sink2

		std::cout << boost::format("\n before: v=%u, e=%u.")
				% boost::num_vertices(iv) % boost::num_edges(iv);
		
		#if 0
		remove_introns(iv, nodes.begin(), nodes.end());

		std::cout << boost::format("\n after:  v=%u, e=%u.")
				% boost::num_vertices(iv) % boost::num_edges(iv);
		#endif

		auto vertices = boost::vertices(iv);
		std::cout << "\n" << print_range(vertices.first, vertices.second, " ", [&] (std::ostream& os, IVVertex u) {
			os << boost::format(" %s") % iv[u].label;
		}) << std::endl;

		//std::deque<IVVertex> nodes;
		nodes.clear();
		std::map<IVVertex, uint32_t> reg;
		
		topological_sort(0, iv, std::back_inserter(nodes), reg);

		std::cout << "\n" << print_range(nodes.cbegin(), nodes.cend(), " ", [&] (std::ostream& os, IVVertex u) {
			os << boost::format(" %s:%u") % iv[u].label % reg[u];
		}) << std::endl;
	}

	#if 0
	// Topological sort for an aggregated graph
	{	
		std::deque<IVGraph::vertex_descriptor> nodes;
		
		#if 1
		deep_subtree_first_sort(iv, std::back_inserter(nodes));
		#else
		boost::topological_sort(iv, std::back_inserter(nodes));

	    for (auto it = nodes.cbegin(); it != nodes.cend(); it++) {
			auto u = *it;
			
			auto k = out_degree(u, iv);
			if ( k == 0 ) {
				iv[u].reg = 1; // In Garnet, leaf is not a constant but a load op for constant value.
			}
			else {
				auto r = 0u;
				auto i = 0u;
				auto last = 0u;
				for (auto erange = out_edges(u, iv); erange.first != erange.second; erange.first++) {
					auto temp = iv[(*erange.first).m_target].reg;
					if ( last > temp ) break;
					last = temp;
					temp += k - i;
					if ( r < temp ) r = temp;
					i++;
				}
				iv[u].reg = r;
			}
		}
		
	    for (auto it = nodes.cbegin(); it != nodes.cend(); it++) {
	    	std::cout << boost::format(" %s:%u") % iv[*it].label % iv[*it].reg;
	    }
	    std::cout << std::endl;
		
		// find roots.
		// Sink0=8, Sink1=4, Sink2=9
		
		#if 1
		// Find roots.
		std::vector<IVGraph::vertex_descriptor> roots;
		#if 1
		std::vector<bool> mark(num_vertices(iv), false);
	    for (auto erange = edges(iv); erange.first != erange.second; erange.first++) {
			mark[(*erange.first).m_target] = true;
		}
		for (auto i = 0u; i < mark.size(); i++) {
			if ( !mark[i] ) roots.push_back(i);
		}
		#else
		roots.push_back(8);
		roots.push_back(4);
		roots.push_back(9);
		#endif
		
		#if 1
		std::cout << "\n Roots: ";
	    for (auto it = roots.cbegin(); it != roots.cend(); it++) {
	    	std::cout << boost::format(" %s:%u") % iv[*it].label % iv[*it].reg;
	    }
	    std::cout << std::endl;
		#endif
		
		// Push roots to stack.
		std::sort(roots.begin(), roots.end(), [&iv] (const IVGraph::vertex_descriptor& u, const IVGraph::vertex_descriptor& v) {
			return iv[u].reg < iv[v].reg;
		});
		std::stack<IVGraph::vertex_descriptor> source;
		std::for_each(roots.rbegin(), roots.rend(), [&source] (const IVGraph::vertex_descriptor& u) {
			source.push(u);
		});
		#if 0
		std::for_each(roots.begin(), roots.end(), [&iv] (const IVGraph::vertex_descriptor& u) {
			std::cout << boost::format(" %s:%u") % iv[u].label % iv[u].reg;
		});
		std::cout << std::endl;
		#endif

		// Serialize all nodes in DAG on depth-first basis.
		// Visiting is processed from a child tree which has the greater reg number to one has the less.
		std::set<IVGraph::vertex_descriptor> visited;
		nodes.clear();
		while ( !source.empty() ) {
			auto u = source.top();
			visited.insert(u);
			
			//
			std::vector<IVGraph::vertex_descriptor> children;
			auto erange = out_edges(u, iv);
			std::for_each(erange.first, erange.second, [&children, &visited] (const IVGraph::edge_descriptor& e) {
				if ( visited.find(e.m_target) == visited.end() ) {
					children.push_back(e.m_target);
				}
			});
			if ( children.empty() ) {
				source.pop();
				nodes.push_back(u);
			}
			else {
				std::sort(children.begin(), children.end(), [&iv] (const IVGraph::vertex_descriptor& u, const IVGraph::vertex_descriptor& v) {
					return iv[u].reg < iv[v].reg;
				});
				std::for_each(children.begin(), children.end(), [&source] (const IVGraph::vertex_descriptor& u) {
					source.push(u);
				});
			}
		}
		#else
		nodes.clear();
		nodes.push_back(1); // a
		nodes.push_back(2); // g
		nodes.push_back(0); // b
		nodes.push_back(3); // f
		nodes.push_back(4); // Sink1
		nodes.push_back(5); // g
		nodes.push_back(6); // f
		nodes.push_back(9); // Sink2
		nodes.push_back(7); // f
		nodes.push_back(8); // Sink0
		#endif
		#endif
				
	    for (auto it = nodes.cbegin(); it != nodes.cend(); it++) {
	    	std::cout << boost::format(" %s:%u") % iv[*it].label % reg[*it];
	    }
	    std::cout << std::endl;

		typedef boost::adjacency_list<
				boost::listS,        // OutEdgeList
				boost::vecS,         // VertexList
				boost::undirectedS,  // Directed
				::VProp<Garnet::IVGraph> // VertexProperty
			> LineGraph;
		LineGraph l;
		convertToLineGraph(iv, nodes.begin(), nodes.end(), l);
		
		print_graph(l, get(&VProp<Garnet::IVGraph>::label, l));		
		
		boost::graph_traits<LineGraph>::vertices_size_type num_colors;
		num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));
		{
			int32_t maxDegree = 0;
			for (auto it = vertices(l); it.first != it.second; it.first++) {
				int32_t degree = boost::degree(*it.first, l);
				if ( maxDegree < degree ) maxDegree = degree;
			}
			std::cout << "\n[" __FUNCTION__ "] # of vertices:   " << boost::num_vertices(l);
			std::cout << "\n[" __FUNCTION__ "] # of edges:      " << boost::num_edges(l);
			std::cout << "\n[" __FUNCTION__ "] # of colors:     " << num_colors;
			std::cout << "\n[" __FUNCTION__ "] max # of degree: " << maxDegree;
			std::cout << std::flush;
		}
	}
	#endif
	
	#if 0
	{
		std::vector<Tree> trees;
		IVGraph iv;

		trees.push_back(t[9]);
				
		convertIVTreesToIVGraph(trees, iv);

		PPEGraph pg;
		convertIVGraphToPPEGraph(iv, pg);
		
		std::cout << "\n[PPEGraph]\n" << boost::num_vertices(pg) << "vertices" << std::endl;
	}
	#endif
#endif

	return 0;
}
