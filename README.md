# playground
Code playground with stuff that wants to be a real boy (read: boost contributions). Current interest is threads, executors and their perfect scoping but anything can happen and it gets messy at times. This is cross-platform with all the usual compilers supported and emphasis on c++14 and the next standards.

## getting started with Visual Studio 2015+
```
mkdir x64 && cd x64
cmake -G "Visual Studio 15 2017 Win64" .. #Visual Studio 14 2015 also works fine
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
msbuild /p:Configuration=Release playground.sln
```

## getting started with Linux (Clang 3.8+ GCC 6.3.0+)
```
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make
```
