cmake_minimum_required (VERSION 2.6)
project (corelib)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SUBDIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set (SOURCES "")

foreach (DIR ${SUBDIRS})
  include_directories (${DIR})
  file (GLOB SUBDIR_SOURCES ${DIR}/*.cpp)
  set (SOURCES ${SOURCES} ${SUBDIR_SOURCES})
endforeach(DIR)

set (TARGET "corelib.a")

add_library (${TARGET} ${SOURCES})

#target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2GFX_LIBRARY})