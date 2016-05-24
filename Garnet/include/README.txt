Make symbolic links to boost and OpenCV directories.
(Administrator rightes required.)

Target directories may vary depending on boost/OpenCV versions.

1. Open cmd.exe with the Administrator rights.

  Right click cmd.exe icon and select "open as an Administrator".

2. Execute these commands.

  mklink /D boost ..\_3rdparty\boost_1_55_0\boost
  mklink /D opencv ..\_3rdparty\opencv-2.4.9\build\include\opencv
  mklink /D opencv2 ..\_3rdparty\opencv-2.4.9\build\include\opencv2

3. If you want to delete the links, use these commands.

  rmdir boost
  rmdir opencv
  rmdir opencv2

