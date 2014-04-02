project (gmtool)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

set (SHARED
  ../game/gm1.cpp
  ../game/gm1reader.cpp
  ../game/surface.cpp
  ../game/sdl_utils.cpp
  ../game/geometry.cpp
  ../game/tgx.cpp
)

set(TARGET "gmtool.out")

add_executable (${TARGET} ${SRCS} ${SHARED})

find_package (Boost 1.46 REQUIRED COMPONENTS program_options)

set (BOOST_PROGRAM_OPTIONS boost_program_options)

target_link_libraries (${TARGET} ${BOOST_PROGRAM_OPTIONS} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES})