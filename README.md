# EyeLogicLSL
This README contains a quick guide on how to get eyelogiclsl set up and working on your machine.

## Download binaries
There is a pre-built windows package of the LSL plugin. Contact info@eyelogic.de to obtain that package.

## Compile from source
In case you prefer to build from source you can also compile EyeLogicLSL yourself. 

### prerequisites
* Windows - either 32 bit or 64 bit
* liblsl - [download](https://github.com/sccn/liblsl/releases) or [build it yourself](https://labstreaminglayer.readthedocs.io/dev/lib_dev.html#build-liblsl) 
* [cmake](https://cmake.org/download/)
* Visual Studio

EyeLogicLSL and this installation process ( both for 32bit and 64bit versions ) has been tested and is known to work for

* Windows 10 - 64 bit
* liblsl 1.15
* cmake 3.12 and later
* Visual Studio 17 & 19

### installation steps
This repository contains an installation script that streamlines the installation process, however you may compile through Visual Studio directly or use any other generator of your choosing by passing the appropriate cmake options. The following steps and comments apply to all installation pathways. All divergent steps will be described in the subsequent subsections.

#### preparation
* download or clone this repository
* install cmake
* install Visual Studio
* have your unpacked/self-built liblsl directory somewhere on disk - note where it is located (it should contain directories include/ bin/ and lib/)
* either copy the content of the into EyeLogicLSL/dependencies/lsl **OR** open EyeLogicLSL/install.bat ( typically through, right-click-->Edit ) and edit such that the variable lsldir points to that directory.

From hereon: Remove all \< and \> from the actual calls. Choose Win32 when buildingHi  for a 32 bit target machine, choose x64 when building for a 64 bit target machine. If not specifically determined otherwise EyeLogicLSL.exe and all necesary .dll files will be copied to EyeLogicLSL/install/\<Win32 or x64\>/Release. You can either start EyeLogicLSL from there or copy all contents of the folder to a location of your choosing and start from there.

### install script
* open 'Developer Comand Prompt for Visual Studio XX'
* within the comand prompt navigate to the top-most directory of EyeLogicLSL
* call:

  install -A \<Win32 or x64\>

### Visual Studio
In order to generate the Visual Studio project you first need to generate one using cmake. Afterwards you can open the generated *.sln file in Visual Studio and install EyeLogicLSL from there.

#### generation: command prompt
* open the Windows 'Command Prompt'
* within the comand prompt navigate to the top-most directory of EyeLogicLSL
* call the following commands in order:
  * mkdir build
  * cd build
  * call:

    cmake ../src -A \<Win32 or x64\> -DLSL_DIR=<path to your liblsl directory>

#### installation
* open EyeLogicLSL.sln in EyeLogicLSL/build this will open the project in Visual Studio
* build the INSTALL target in Release mode
