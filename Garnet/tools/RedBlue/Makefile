# For NMAKE

TOP=..\..
TARGET=RedBlue
OBJS=main.obj dSFMT.obj

CXX=cl
CXXFLAGS=/I$(TOP)\External\OpenCV\include /I$(TOP)\External\dSFMT /INCREMENTAL:NO /NOLOGO /LIBPATH:"D:\Munehiro\Study\program\External\opencv_build64\lib\Release" "opencv_core231.lib" "opencv_highgui231.lib" "opencv_imgproc231.lib" /ALLOWISOLATION /DEBUG /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64

dSFMT.obj: $(TOP)\External\dSFMT\dSFMT.c
	$(CXX) $(CXXFLAGS) $@ $?

$(TARGET): $(OBJS)
	$(CXX)
