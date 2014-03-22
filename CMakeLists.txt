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

cxx_set_flag ("-O0" CXX_OPTIMIZATION_WARN)
cxx_set_flag ("-pedantic" CXX_PEDANTIC_WARN)
cxx_set_flag ("-Wall" CXX_WALL_WARN)
cxx_set_flag ("-Wextra" CXX_EXTRA_WARN)
cxx_set_flag ("-Wunreachable-code" CXX_UNREACHABLE_WARN)

add_subdirectory (src)
add_subdirectory (tools/gmviewer)
