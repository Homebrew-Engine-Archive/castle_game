cmake_minimum_required (VERSION 2.6)
project (corelib)

set (TARGET ${TGXLIB})

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

add_library (${TARGET} STATIC ${SOURCES})
