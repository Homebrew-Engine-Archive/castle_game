include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

set (SHARED
  ../game/tgx.cpp
  ../game/rw.cpp
  ../game/surface.cpp
  ../game/sdl_utils.cpp
  ../game/geometry.cpp
  ../game/sdl_init.cpp
)

set(TARGET "gfx.out")

add_executable (${TARGET} ${SRCS} ${SHARED})

target_link_libraries (${TARGET} ${BOOST_LIBS} ${SDL2_LIBRARIES})