cmake_minimum_required(VERSION 3.2)
project(gpusat)

include_directories(~/Ba/htd/include)
include_directories(~/Ba/jsoncpp/jsoncpp/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

if (DEFINED ENV{AMDAPPSDKROOT})
    # add amd opencl libraries if AMDAPPSDKROOT environment variable is set
    include_directories("$ENV{AMDAPPSDKROOT}/include")
    link_directories("$ENV{AMDAPPSDKROOT}/lib/x86_64")
elseif (DEFINED ENV{CUDA_PATH})
    # add nvidia opencl libraries if CUDA_PATH environment variable is set
    include_directories("$ENV{CUDA_PATH}/include")
    link_directories("$ENV{CUDA_PATH}/lib/x64")
endif ()

if (DEBUG)
    # compiler flags
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O0 -Wno-ignored-attributes")
    # linker flags
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    add_definitions(-DDEBUG)
elseif (UNIX)
    # compiler flags
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wno-ignored-attributes")
    # linker flags
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif ()

find_library(HTD_LIB htd ~/Ba/htd/lib)
add_library(htd SHARED IMPORTED)
set_target_properties(htd PROPERTIES IMPORTED_LOCATION ${HTD_LIB})

find_library(HTD_IO_LIB htd_io ~/Ba/htd/lib)
add_library(htd_io SHARED IMPORTED)
set_target_properties(htd_io PROPERTIES IMPORTED_LOCATION ${HTD_IO_LIB})

find_library(JSONCPP_LIB jsoncpp ~/Ba/jsoncpp/jsoncpp/build/debug/src/lib_json)
add_library(jsoncpp STATIC IMPORTED)
set_target_properties(jsoncpp PROPERTIES IMPORTED_LOCATION ${JSONCPP_LIB})

file(GLOB gpusat_SRC "src/*.cpp" "src/*/*.cpp")
add_executable(gpusat ${gpusat_SRC})
target_link_libraries(gpusat OpenCL htd htd_io jsoncpp)
