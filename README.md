# playground
Code playground with stuff that wants to be a real boy (read: boost contributions). Current interest is threads, executors and their perfect scoping but anything can happen and it gets messy at times.

## getting started (Visual Studio 2017 example)
```
cmake -G "Visual Studio 15 2017 Win64"
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
msbuild /p:Configuration=Release playground.sln
```

We're cross-platform with all the usual compilers supported with emphasis on c++14 and the future.
