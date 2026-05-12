
cmake_minimum_required(VERSION 3.20)

project(SDK)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(SdkDir "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
get_filename_component(RootDir "${SdkDir}/.." ABSOLUTE)
set(ThirdPartyDir "${RootDir}/3rdparty")
set(ModulesDir "${SdkDir}/cmake/modules")

add_subdirectory(lib/sdk)
add_subdirectory(samples)
