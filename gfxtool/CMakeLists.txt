include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories ("../src/")

set (SRCS
  main.cpp
)

set(TARGET "gfx.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBS} ${SDL2_LIBRARIES})