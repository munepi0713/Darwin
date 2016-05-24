#ifndef PICTUREPERFECTENGINE__OPCODES__H__
#define PICTUREPERFECTENGINE__OPCODES__H__

//       OpCode                         Number      Operands

#define  OPCODE_End                           (0)     //  
                                           
#define  OPCODE_SetImageSize                  (1)     //  SrcX(Const), SrcY(Const)
#define  OPCODE_SetSizeFromImage              (2)     //  Src(S)
                                           
#define  OPCODE_PrintHash_V1D                 (10)    //  Src
#define  OPCODE_PrintHash_I1B                 (14)    //  Src
#define  OPCODE_PrintHash_I3B                 (15)    //  Src
                                           
#define  OPCODE_V1D_Var                       (20)    //  Dst(V1D)
#define  OPCODE_I1B_Var                       (24)    //  Dst(I1B)
#define  OPCODE_I3B_Var                       (25)    //  Dst(I3B)
#define  OPCODE_S_Var                         (29)    //  Dst(S)
                                           
#define  OPCODE_V1D_Immediate                 (30)    //  Dst(V1D), Immediate(V1D)
#define  OPCODE_S_Immediate                   (39)    //  Dst(S), Immediate(S)
                                           
#define  OPCODE_V1D_MoveFrom_V1Ds             (40)    //  Dst Src(V1Ds)
#define  OPCODE_I1B_MoveFrom_I1Bs             (44)    //  Dst Src(I1Bs)
#define  OPCODE_I3B_MoveFrom_I3Bs             (45)    //  Dst Src(I3Bs)
                                           
#define  OPCODE_V1Ds_MoveTo_V1D               (50)    //  Dst(V1Ds) Src
#define  OPCODE_I1Bs_MoveTo_I1B               (54)    //  Dst(I1Bs) Src
#define  OPCODE_I3Bs_MoveTo_I3B               (55)    //  Dst(I3Bs) Src
                                           
#define  OPCODE_V1D_Zero                      (60)    //  Dst
#define  OPCODE_I1B_Zero                      (64)    //  Dst
#define  OPCODE_I3B_Zero                      (65)    //  Dst
                                           
#define  OPCODE_V1D_Copy_V1D                  (70)    //  Dst Src
#define  OPCODE_I1B_Copy_I1B                  (74)    //  Dst Src
                                           
#define  OPCODE_I1B_Load_S                    (84)    //  Dst(I1B), Src(S)
#define  OPCODE_I3B_Load_S                    (85)    //  Dst(I3B), Src(S)
                                           
#define  OPCODE_Save_I1B_S                    (94)    //  Src1(I1B), Src2(S)
#define  OPCODE_Save_I3B_S                    (95)    //  Src2(I3B), Src2(S)
                                           
#define  OPCODE_I1B_Split1st_I3B              (100)   //  Dst Src
#define  OPCODE_I1B_Split2nd_I3B              (101)   //  Dst Src
#define  OPCODE_I1B_Split3rd_I3B              (102)   //  Dst Src
                                           
#define  OPCODE_I3B_BGR2HSV_I3B               (110)   //  Dst Src
#define  OPCODE_I3B_BGR2YUV_I3B               (111)   //  Dst Src
#define  OPCODE_I1B_BGR2Gray_I3B              (112)   //  Dst Src
                                           
#define  OPCODE_I1B_Dilate3x3_I1B             (120)   //  Dst Src
#define  OPCODE_I1B_Erode3x3_I1B              (121)   //  Dst Src
#define  OPCODE_I1B_Thin_I1B                  (122)   //  Dst Src
                                           
#define  OPCODE_I1B_Laplacian3x3_I1B          (123)   //  Dst Src
#define  OPCODE_I1B_Laplacian2nd3x3_I1B       (124)   //  Dst Src
#define  OPCODE_I1B_Median3x3_I1B             (125)   //  Dst Src
#define  OPCODE_I1B_Sharpen3x3_I1B            (126)   //  Dst Src
#define  OPCODE_I1B_Smooth3x3_I1B             (127)   //  Dst Src
#define  OPCODE_I1B_Sobel3x3_I1B              (128)   //  Dst Src
#define  OPCODE_I1B_SobelX3x3_I1B             (129)   //  Dst Src
#define  OPCODE_I1B_SobelY3x3_I1B             (130)   //  Dst Src
                                           
#define  OPCODE_I1B_Negate_I1B                (140)   //  Dst Src
#define  OPCODE_I1B_ProjectionX_I1B           (141)   //  Dst Src
#define  OPCODE_I1B_ProjectionY_I1B           (142)   //  Dst Src
#define  OPCODE_I1B_Connection4_I1B           (143)   //  Dst Src
#define  OPCODE_I1B_Connection8_I1B           (144)   //  Dst Src
#define  OPCODE_I1B_Outline4_I1B              (145)   //  Dst Src
#define  OPCODE_I1B_Outline8_I1B              (146)   //  Dst Src
#define  OPCODE_I1B_Segment_I1B               (147)   //  Dst Src
                                           
#define  OPCODE_I1B_Add_I1B_I1B               (200)   //  Dst Src1 Src2
#define  OPCODE_I1B_Diff_I1B_I1B              (201)   //  Dst Src1 Src2
#define  OPCODE_I1B_AlgebraicProd_I1B_I1B     (202)   //  Dst Src1 Src2
#define  OPCODE_I1B_AlgebraicSum_I1B_I1B      (203)   //  Dst Src1 Src2
#define  OPCODE_I1B_BoundedProd_I1B_I1B       (204)   //  Dst Src1 Src2
#define  OPCODE_I1B_LogicalProd_I1B_I1B       (205)   //  Dst Src1 Src2
#define  OPCODE_I1B_LogicalSum_I1B_I1B        (206)   //  Dst Src1 Src2
#define  OPCODE_I1B_Subtract_I1B_I1B          (207)   //  Dst Src1 Src2
                                           
#define  OPCODE_I1B_HighPass_I1B_V1D          (300)   //  Dst Src1 Src2
#define  OPCODE_I1B_LowPass_I1B_V1D           (301)   //  Dst Src1 Src2
                                           
#define  OPCODE_I1B_Threshold_I1B             (302)   //  Dst Src
#define  OPCODE_I1B_ThresholdInv_I1B          (303)   //  Dst Src
#define  OPCODE_I1B_ThresholdV_I1B_V1D        (304)   //  Dst Src1 Src2
#define  OPCODE_I1B_ThresholdInvV_I1B_V1D     (305)   //  Dst Src1 Src2
#define  OPCODE_I1B_Binarize_I1B              (306)   //  Dst Src
#define  OPCODE_I1B_BinarizeInv_I1B           (307)   //  Dst Src
#define  OPCODE_I1B_BinarizeV_I1B_V1D         (308)   //  Dst Src1 Src2
#define  OPCODE_I1B_BinarizeInvV_I1B_V1D      (309)   //  Dst Src1 Src2
                                           
#define  OPCODE_I1B_BigBlob4_I1B_V1D          (310)   //  Dst Src1 Src2
#define  OPCODE_I1B_BigBlob8_I1B_V1D          (311)   //  Dst Src1 Src2
                                            
#define  OPCODE_I1B_AddV_I1B_V1D              (320)   //  Dst Src1 Src2
#define  OPCODE_I1B_FillV_V1D                 (321)   //  Dst Src1 Src2
#define  OPCODE_I1B_ModAvgV_I1B_V1D           (322)   //  Dst Src1 Src2
#define  OPCODE_I1B_MultiplyV_I1B_V1D         (323)   //  Dst Src1 Src2
#define  OPCODE_I1B_SubtractV_I1B_V1D         (324)   //  Dst Src1 Src2
                                           
#define  OPCODE_I1B_NoiseV_V1D                (330)   //  Dst Src
                                           
#define  OPCODE_V1D_AvgPixel_I1B              (400)   //  Dst Src
#define  OPCODE_V1D_MaxPixel_I1B              (401)   //  Dst Src
#define  OPCODE_V1D_MinPixel_I1B              (402)   //  Dst Src
                                           
#define  OPCODE_V1D_FGArea_I1B                (403)   //  Dst Src
#define  OPCODE_V1D_BGArea_I1B                (404)   //  Dst Src
#define  OPCODE_V1D_FGAreaV_I1B_V1D           (405)   //  Dst Src1 Src2
#define  OPCODE_V1D_BGAreaV_I1B_V1D           (406)   //  Dst Src1 Src2
                                           
#define  OPCODE_V1D_MassCenterBX_I1B          (407)   //  Dst Src
#define  OPCODE_V1D_MassCenterBY_I1B          (408)   //  Dst Src
                                           
#define  OPCODE_V1D_PSNR_I1B_I1B              (500)   //  Dst Src1 Src2
                                           
#define  OPCODE_V1D_VNegate_V1D               (600)   //  Dst Src
                                           
#define  OPCODE_V1D_VAdd_V1D_V1D              (601)   //  Dst Src1 Src2
#define  OPCODE_V1D_VAlgebraicProd_V1D_V1D    (602)   //  Dst Src1 Src2
#define  OPCODE_V1D_VAlgebraicSum_V1D_V1D     (603)   //  Dst Src1 Src2
#define  OPCODE_V1D_VDiff_V1D_V1D             (604)   //  Dst Src1 Src2
#define  OPCODE_V1D_VLogicalProd_V1D_V1D      (605)   //  Dst Src1 Src2
#define  OPCODE_V1D_VLogicalSum_V1D_V1D       (606)   //  Dst Src1 Src2
#define  OPCODE_V1D_VSubtract_V1D_V1D         (607)   //  Dst Src1 Src2

#define  OPCODE_V1D_Distance_V1D_V1D_V1D_V1D  (610)   //  Dst Src1 Src2 Src3 Src4

#define  OPCODE_I1B_Select_I1B_I1B_V1D_V1D    (700)   //  Dst Src1 Src2 Src3 Src4
#define  OPCODE_V1D_Select_V1D_V1D            (701)   //  Dst Src1 Src2
#define  OPCODE_V1D_Select_V1D_V1D_V1D_V1D    (702)   //  Dst Src1 Src2 Src3 Src4
                                           
#define  OPCODE_PrintValue_V1D                (900)    //  Src
#define  OPCODE_PrintValue_S                  (909)    //  Src


#define  OPCODE_V1Ds_Var                      (1020)    //  Dst(V1D)
#define  OPCODE_I1Bs_Var                      (1024)    //  Dst(I1B)
#define  OPCODE_I3Bs_Var                      (1025)    //  Dst(I3B)
#define  OPCODE_Ss_Var                        (1029)    //  Dst(S)
                                           

#endif//PICTUREPERFECTENGINE__OPCODES__H__
