project (gfxtool)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

set(TARGET "gfxtool.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${SDL2_LIBRARY} corelib tgxlib)