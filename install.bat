@echo off

set lsldir=%cd%/dependencies/lsl

echo Compiling EyeLogic LSL
echo Use LSL dependency: %lsldir%

if not exist %lsldir% (
echo LSL directory not found. Edit the path to your LSL directory in install.bat
exit /b 1
)

rmdir /Q /S build > nul
mkdir build
if ERRORLEVEL 1 (
echo ERROR: Cannot create directory build
cd ..
exit /b 1
)

cd build
if ERRORLEVEL 1 (
echo ERROR: directory build does not exist
cd ..
exit /b 1
)
echo Generating Project
cmake ../src -DLSL_DIR=%lsldir% %*
if ERRORLEVEL 1 (
echo ERROR: cmake call failed
cd ..
exit /b 1
)
echo Compiling Project
msbuild eyelogiclsl.sln "-p:Configuration=Release"
if ERRORLEVEL 1 (
echo ERROR: Compile failed, is msbuild installed?
cd ..
exit /b 1
)
cmake --install .
if ERRORLEVEL 1 (
echo ERROR: installation failed
cd ..
exit /b 1
)

cd ..