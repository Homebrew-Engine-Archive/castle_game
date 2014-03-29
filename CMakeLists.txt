project(stockade)
cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include (CheckCXXCompilerFlag)

macro (cxx_set_flag flag result)
  check_cxx_compiler_flag (${flag} ${result})
  if (${result})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif ()
endmacro (cxx_set_flag)

cxx_set_flag ("-O0" CXX_OPTIMIZATION)
cxx_set_flag ("-pedantic" CXX_PEDANTIC_WARNINGS)
cxx_set_flag ("-Wall" CXX_WALL_WARNINGS)
cxx_set_flag ("-Wextra" CXX_EXTRA_WARNINGS)
cxx_set_flag ("-Wunreachable-code" CXX_UNREACHABLE_CODE)
# cxx_set_flag ("-Wno-unused-parameter" CXX_NO_UNUSED_PARAMETER)

include (FindPkgConfig)
pkg_search_module (SDL2 REQUIRED sdl2)

include_directories (${SDL2_INCLUDE_DIRS})

add_definitions (-DSDL_MAIN_HANDLED)

find_package (Boost 1.54.0 REQUIRED COMPONENTS filesystem)
find_package (Boost 1.47.0 REQUIRED COMPONENTS date_time system)

set (BOOST_LIBS boost_system boost_filesystem boost_date_time)

add_subdirectory (castle)
add_subdirectory (gmtool)
add_subdirectory (gfxtool)
