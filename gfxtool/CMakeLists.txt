project (gfxtool)

set(TARGET ${GFXTOOLBIN})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
)

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${SDL2_LIBRARY} ${CORELIB} ${TGXLIB})