Make symbolic links to boost and OpenCV directories.
(Administrator rightes required.)

Target directories may vary depending on boost/OpenCV versions.

1. Open cmd.exe with the Administrator rights.

  Right click cmd.exe icon and select "open as an Administrator".

2. Execute these commands.

  mklink opencv_core245.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_core245.dll
  mklink opencv_core245d.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_core245d.dll
  mklink opencv_highgui245.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_highgui245.dll
  mklink opencv_highgui245d.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_highgui245d.dll
  mklink opencv_imgproc245.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_imgproc245.dll
  mklink opencv_imgproc245d.dll ..\_3rdparty\opencv\build\x86\vc11\bin\opencv_imgproc245d.dll


3. If delete the links, use these commands.

  rmdir boost
  rmdir opencv
  rmdir opencv2

