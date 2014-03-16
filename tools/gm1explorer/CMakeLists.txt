include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${SDL2_INCLUDE_DIRS})
include_directories (../../src)

set (SRCS
  ../../src/atlas.cpp
  ../../src/surface.cpp
  ../../src/rw.cpp
  ../../src/geometry.cpp
  ../../src/gm1.cpp
  ../../src/tgx.cpp
  main.cpp
)

add_executable (gm1.out ${SRCS})

find_package (Boost 1.47.0 REQUIRED COMPONENTS system)

set (BOOST_LIBRARIES boost_system)

target_link_libraries (gm1.out
  ${SDL2_LIBRARIES}
  ${BOOST_LIBRARIES}
)
