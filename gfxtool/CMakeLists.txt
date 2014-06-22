project (gfxtool)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

set (SHARED
  ../game/tgx.cpp
  ../game/image.cpp
  ../game/imagelocker.cpp
  ../game/palette.cpp
  ../game/sdl_utils.cpp
  ../game/rect.cpp
  ../game/point.cpp
  ../game/color.cpp
)

set(TARGET "gfxtool.out")

add_executable (${TARGET} ${SRCS} ${SHARED})

target_link_libraries (${TARGET} ${SDL2_LIBRARY})