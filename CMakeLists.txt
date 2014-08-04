project(stockade)
cmake_minimum_required(VERSION 2.8)

######################
# C++ compiler flags #
######################

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb3")

include (CheckCXXCompilerFlag)

macro (cxx_set_flag flag result)
  check_cxx_compiler_flag (${flag} ${result})
  if (${result})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif ()
endmacro (cxx_set_flag)

cxx_set_flag ("-Og" HAS_FLAG_OPTIMIZATION)
if (NOT ${HAS_FLAG_OPTIMIZATION})
  cxx_set_flag ("-O0" HAS_FLAG_OPTIMIZATION)
endif ()

cxx_set_flag ("-Wpedantic" HAS_FLAG_PEDANTIC_WARNINGS)
cxx_set_flag ("-Wall" HAS_FLAG_ALL_WARNINGS)
cxx_set_flag ("-Wextra" HAS_FLAG_EXTRA_WARNINGS)
cxx_set_flag ("-Wunreachable-code" HAS_FLAG_UNREACHABLE_CODE)
cxx_set_flag ("-fstrict-aliasing" HAS_FLAG_STRICT_ALIASING)
cxx_set_flag ("-Wno-unused-parameter" HAS_NO_UNUSED_PARAMETER)
cxx_set_flag ("-Wno-unused-function" HAS_NO_UNUSED_FUNCTION)

###################
# Include section #
###################

include_directories (${PROJECT_SOURCE_DIR})

include (FindSDL2)
find_package (SDL2 REQUIRED)
include_directories (${SDL2_INCLUDE_DIR})

include (FindSDL2_image)
find_package (SDL2_image REQUIRED)
include_directories (${SDL2IMAGE_INCLUDE_DIR})

## see http://wiki.libsdl.org/SDL_SetMainReady
add_definitions (-DSDL_MAIN_HANDLED)

## Version was choosen by fair dice roll (we may support even an older ones)
## TODO check it
find_package (Boost 1.46.0 REQUIRED COMPONENTS system iostreams)

## Here we are waiting for Boost.Filesystem v3 interface
find_package (Boost 1.54.0 REQUIRED COMPONENTS filesystem)

set (BOOST_LIBRARIES boost_system boost_filesystem boost_iostreams)

set (CORELIB core)
set (TGXLIB tgx)
set (GM1LIB gm1)
set (GMTOOLBIN gmtool.out)
set (GFXTOOLBIN gfxtool.out)

add_subdirectory (core)
add_subdirectory (tgx)
add_subdirectory (gm1)
add_subdirectory (gmtool)
add_subdirectory (gfxtool)
