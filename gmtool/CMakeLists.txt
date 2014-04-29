cmake_minimum_required (VERSION 2.6)
project (gmtool)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
  entrymode.cpp
  dumpmode.cpp
  rendermode.cpp
  hexcolor.cpp
  headermode.cpp
  palettemode.cpp
  listmode.cpp
  packmode.cpp
  unpackmode.cpp
  rw.cpp
  renderer.cpp
)

set (RENDERERS
  renderers/bitmap.cpp
  renderers/tgxrenderer.cpp
)

set (SHARED
  ../game/gm1.cpp
  ../game/gm1reader.cpp
  ../game/gm1entryreader.cpp
  ../game/gm1palette.cpp
  ../game/surface.cpp
  ../game/sdl_utils.cpp
  ../game/tgx.cpp
)

set(TARGET "gmtool.out")

find_package (Boost 1.46 REQUIRED COMPONENTS program_options)
set (BOOST_PROGRAM_OPTIONS boost_program_options)

include (FindPkgConfig)
pkg_search_module (SDL2_IMAGE SDL2_image)

include_directories (${SDL2_IMAGE_INCLUDE_DIR})

include (CheckCXXSourceRuns)

set (CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES};${SDL2_IMAGE_LIBRARIES};${SDL2_LIBRARIES}")

check_cxx_source_runs ("
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
int main() {
    unsigned char magic[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    SDL_RWops *src = SDL_RWFromConstMem(magic, sizeof(magic));
    int result = IMG_isPNG(src);
    SDL_RWclose(src);
    return(result != 1);
}
" HAVE_SDL2_IMAGE_PNG)

if (HAVE_SDL2_IMAGE_PNG)
  message (STATUS "PNG support was found in SDL2_image")
  set (RENDERERS "${RENDERERS};renderers/pngrenderer.cpp")
else ()
  set (HAVE_SDL2_IMAGE_PNG 0)
endif ()

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/config_${PROJECT_NAME}.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config_${PROJECT_NAME}.h)

add_executable (${TARGET} ${SRCS} ${SHARED} ${RENDERERS})

target_link_libraries (${TARGET} ${BOOST_PROGRAM_OPTIONS} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})