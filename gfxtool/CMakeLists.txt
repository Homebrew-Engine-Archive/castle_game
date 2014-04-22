project (gfxtool)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

set (SHARED
  ../game/tgx.cpp
  ../game/surface.cpp
  ../game/sdl_utils.cpp
)

set(TARGET "gfxtool.out")

add_executable (${TARGET} ${SRCS} ${SHARED})

target_link_libraries (${TARGET} ${BOOST_SYSTEM} ${BOOST_} ${SDL2_LIBRARIES})