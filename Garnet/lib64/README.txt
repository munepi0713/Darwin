Make symbolic links to boost and OpenCV directories.
(Administrator rightes required.)

Target directories may vary depending on boost/OpenCV versions.

1. Open cmd.exe with the Administrator rights.

  Right click cmd.exe icon and select "open as an Administrator".

2.1 Execute following commands.

  mklink /D boost ..\_3rdparty\boost_1_55_0\stage\x64\lib
  mklink /D opencv ..\_3rdparty\opencv-2.4.9\build\x64\vc12\staticlib

2.2 Execute following commands.

  mklink opencv_core.lib opencv\opencv_core249.lib
  mklink opencv_highgui.lib opencv\opencv_highgui249.lib
  mklink opencv_imgproc.lib opencv\opencv_imgproc249.lib

  mklink opencv_cored.lib opencv\opencv_core249d.lib
  mklink opencv_highguid.lib opencv\opencv_highgui249d.lib
  mklink opencv_imgprocd.lib opencv\opencv_imgproc249d.lib

3. If delete the links, use these commands.

  rmdir boost
  rmdir opencv
  rm *.lib

