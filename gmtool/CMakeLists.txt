include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
  model.cpp
)

set (SHARED
  ../game/gm1.cpp
  ../game/gm1reader.cpp
  ../game/surface.cpp
  ../game/sdl_utils.cpp
  ../game/geometry.cpp
  ../game/rw.cpp
  ../game/tgx.cpp
)

set(TARGET "gm.out")

add_executable (${TARGET} ${SRCS} ${SHARED})

target_link_libraries (${TARGET} ${BOOST_LIBS} ${SDL2_LIBRARIES})