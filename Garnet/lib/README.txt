Make symbolic links to boost and OpenCV directories.
(Administrator rightes required.)

Target directories may vary depending on boost/OpenCV versions.

1. Open cmd.exe with the Administrator rights.

  Right click cmd.exe icon and select "open as an Administrator".

2.1 Execute following commands.

  mklink /D boost ..\_3rdparty\boost_1_53_0\stage\lib
  mklink /D opencv ..\_3rdparty\opencv\build\x86\vc11\lib

2.2 Execute following commands.

  mklink libboost_filesystem-mt.lib boost\libboost_filesystem-vc110-mt-1_53.lib
  mklink opencv_core.lib opencv\opencv_core245.lib
  mklink opencv_highgui.lib opencv\opencv_highgui245.lib
  mklink opencv_imgproc.lib opencv\opencv_imgproc245.lib

  mklink libboost_filesystem-mt-gd.lib boost\libboost_filesystem-vc110-mt-gd-1_53.lib
  mklink opencv_cored.lib opencv\opencv_core245d.lib
  mklink opencv_highguid.lib opencv\opencv_highgui245d.lib
  mklink opencv_imgprocd.lib opencv\opencv_imgproc245d.lib

  Or execute following batch command.

  mklinks vc110 1_53 245

3. If delete the links, use these commands.

  rmdir boost
  rmdir opencv
  rm *.lib

