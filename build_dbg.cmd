@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" > nul
msbuild AdcnyDec.sln /p:Configuration=Debug /p:Platform=x64 /p:WindowsTargetPlatformVersion=10.0 /m:4 %*
