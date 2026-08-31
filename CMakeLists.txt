cmake_minimum_required(VERSION 3.16)
project(S3Sim VERSION 2.1 LANGUAGES CXX)

# S3 - the Speed Scaling Simulator.
#
# This file exists so the simulator can be built on Linux and macOS as well as
# Windows. The Visual Studio solution (S3Sim22.sln) remains the primary Windows
# build and is kept in step with this file; both compile exactly the same sources
# and produce identical simulation output.

set(CMAKE_CXX_STANDARD 17)          # std::filesystem
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# --- the simulator library -------------------------------------------------
# Listed explicitly rather than globbed, so that this file and the .vcxproj can
# be diffed against each other. "WLGenCustom - Copy.cpp" is a stray duplicate on
# disk; the Visual Studio project does not build it and neither do we.
add_library(s3 STATIC
  src/AlphaPowerFunction.cpp
  src/BasicLogger.cpp
  src/Configuration.cpp
  src/CoupledSpeed.cpp
  src/CoupledSpeedWith1Addition.cpp
  src/DES.cpp
  src/FCFS.cpp
  src/FEST.cpp
  src/FSP.cpp
  src/FSPEI.cpp
  src/GlobalsAndTypes.cpp
  src/Job.cpp
  src/LRPT.cpp
  src/LRPTSpeed.cpp
  src/PRandomGen.cpp
  src/PS.cpp
  src/PSBS.cpp
  src/PSE.cpp
  src/PSEPT.cpp
  src/ProbeSetting.cpp
  src/RandomSpeed.cpp
  src/RoundZeroLogger.cpp
  src/SEH.cpp
  src/SEPT.cpp
  src/SERPT.cpp
  src/SJF.cpp
  src/SRPT.cpp
  src/ShadowSpeed.cpp
  src/ShadowSpeedEstimateSize.cpp
  src/SingleSpeed.cpp
  src/WLGenBatchExponential.cpp
  src/WLGenCustom.cpp
  src/WLGenExponential.cpp
  src/WLGenPareto.cpp
  src/WLGenParetoEstimates.cpp
  src/WLGenWeibull.cpp
  src/WorkloadFileWrapper.cpp
  src/yLog.cpp
)
target_include_directories(s3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# GCC and Clang need libstdc++fs / libc++fs explicitly before GCC 9.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_link_libraries(s3 PUBLIC stdc++fs)
endif()

if(MSVC)
  target_compile_definitions(s3 PUBLIC _CRT_SECURE_NO_WARNINGS)
else()
  # The sources carry MSVC #pragma warning directives that other compilers do
  # not understand; silence the resulting noise rather than editing every file.
  target_compile_options(s3 PRIVATE -Wno-unknown-pragmas)
endif()

# --- executables -----------------------------------------------------------
add_executable(DEStest testprojects/DEStest/DEStest.cpp)
target_link_libraries(DEStest PRIVATE s3)

add_executable(PRandTest testprojects/PRandTest/PRandTest.cpp)
target_link_libraries(PRandTest PRIVATE s3)

if(NOT MSVC)
  target_compile_options(DEStest PRIVATE -Wno-unknown-pragmas)
  target_compile_options(PRandTest PRIVATE -Wno-unknown-pragmas)
endif()

# DEStest reads ./config.txt from its working directory and resolves the PATH
# directive relative to it, so put a copy next to the binary for a first run.
configure_file(testprojects/DEStest/config.txt
               ${CMAKE_CURRENT_BINARY_DIR}/config.txt COPYONLY)
