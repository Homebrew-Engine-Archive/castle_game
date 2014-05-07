project(stockade)
cmake_minimum_required(VERSION 2.6)

######################
# C++ compiler flags #
######################

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include (CheckCXXCompilerFlag)

macro (cxx_set_flag flag result)
  check_cxx_compiler_flag (${flag} ${result})
  if (${result})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif ()
endmacro (cxx_set_flag)

cxx_set_flag ("-O0" HAS_FLAG_OPTIMIZATION)
cxx_set_flag ("-pedantic" HAS_FLAG_PEDANTIC_WARNINGS)
cxx_set_flag ("-Wall" HAS_FLAG_WALL_WARNINGS)
cxx_set_flag ("-Wextra" HAS_FLAG_EXTRA_WARNINGS)
cxx_set_flag ("-Wunreachable-code" HAS_FLAG_UNREACHABLE_CODE)
cxx_set_flag ("-fstrict-aliasing" HAS_FLAG_STRICT_ALIASING)
cxx_set_flag ("-Wno-unused-parameter" CXX_NO_UNUSED_PARAMETER)

###################
# Include section #
###################

include_directories (${PROJECT_SOURCE_DIR})

include (FindPkgConfig)

pkg_search_module (SDL2 REQUIRED sdl2)
include_directories (${SDL2_INCLUDE_DIRS})

pkg_search_module (SDL2_TTF REQUIRED SDL2_ttf)
include_directories (${SDL2_TTF_INCLUDE_DIRS})

## see http://wiki.libsdl.org/SDL_SetMainReady
add_definitions (-DSDL_MAIN_HANDLED)

## Version is choosen by fair dice roll (we may support even an older ones)
## TODO check it
find_package (Boost 1.46.0 REQUIRED COMPONENTS system iostreams)

## Here we are waiting for Boost.Filesystem v3 interface
find_package (Boost 1.54.0 REQUIRED COMPONENTS filesystem)

set (BOOST_LIBRARIES boost_system boost_filesystem boost_iostreams)

add_subdirectory (game)
add_subdirectory (gmtool)
add_subdirectory (gfxtool)