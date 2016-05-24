
echo off

set VCVER=%1
set BOOSTVER=%2
set OPENCVVER=%3

echo "VC Version: %VCVER%"
echo "Boost Version: %BOOSTVER%"
echo "OpenCV Version: %OPENCVVER%"

mklink libboost_filesystem-mt-gd.lib boost\libboost_filesystem-%VCVER%-mt-gd-%BOOSTVER%.lib
mklink libboost_filesystem-mt.lib boost\libboost_filesystem-%VCVER%-mt-%BOOSTVER%.lib
mklink opencv_cored.lib opencv\opencv_core%OPENCVVER%d.lib
mklink opencv_highguid.lib opencv\opencv_highgui%OPENCVVER%d.lib
mklink opencv_imgprocd.lib opencv\opencv_imgproc%OPENCVVER%d.lib
mklink opencv_core.lib opencv\opencv_core%OPENCVVER%.lib
mklink opencv_highgui.lib opencv\opencv_highgui%OPENCVVER%.lib
mklink opencv_imgproc.lib opencv\opencv_imgproc%OPENCVVER%.lib
