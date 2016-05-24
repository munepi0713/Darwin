

1. Get 3rd party libraries.

Boost:    http://www.boost.org/users/download/
OpenCv:   http://www.opencv.org/downloads.html
GraphCut: http://vision.csd.uwo.ca/code/maxflow-v3.01.src.tar.gz


2. Extract packages.

After extraction, comfirm that the directory structure is as follows;
_3rdparty
  +-- boost_1_53_0
       +-- boost
       +-- b2.exe
       +-- ...
  +-- opencv
       +-- README
       +-- index.rst
       +-- ...
  +-- maxflow-v3.01.src
       +-- block.h
       +-- CHANGES.TXT
       +-- ...

3. Create symbolic links.

Follow instructions in include and lib directories.




bootstrap

b2 runtime-link=static --build-dir=build\x64 --stagedir=stage\x64 address-model=64 -j 9 -sNO_ZLIB=0 -sZLIB_LIBPATH="F:\Study\program\_3rdparty\zlib-1.2.8" -sZLIB_SOURCE="F:\Study\program\_3rdparty\zlib-1.2.8"


b2 runtime-link=static --build-dir=build\x86 --stagedir=stage\x86 address-model=32 -j 9 -sNO_ZLIB=0 -sZLIB_LIBPATH="F:\Study\program\_3rdparty\zlib-1.2.8" -sZLIB_SOURCE="F:\Study\program\_3rdparty\zlib-1.2.8"
