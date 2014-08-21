cmake_minimum_required (VERSION 2.6)
project (gmtool)

set(TARGET ${GMTOOLBIN})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  ${SRCS}
  main.cpp
  imagewriter.cpp
  table.cpp
  tilepart.cpp
  modes.cpp
)

set (SRCS
  ${SRCS}
  imagewriters/bitmapwriter.cpp
  imagewriters/tgximagewriter.cpp
)

find_package (Boost 1.46 REQUIRED COMPONENTS program_options)
set (BOOST_PROGRAM_OPTIONS boost_program_options)

include (CheckCXXSourceRuns)

set (CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})
set (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${SDL2IMAGE_LIBRARY} ${SDL2_LIBRARY})
set (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})

check_cxx_source_runs ("
#include <SDL.h>
#include <SDL_image.h>
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
  set (SRCS 
    ${SRCS} 
    imagewriters/pngimagewriter.cpp
  )
else ()
  set (HAVE_SDL2_IMAGE_PNG 0)
endif ()

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/config_${PROJECT_NAME}.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config_${PROJECT_NAME}.h)

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_PROGRAM_OPTIONS} ${BOOST_LIBRARIES} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${GM1LIB} ${TGXLIB} ${CORELIB})