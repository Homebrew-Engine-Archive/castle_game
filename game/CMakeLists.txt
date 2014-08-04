include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (TARGET ${GAMEBIN})

set (SUBSYSTEMS
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES "")

foreach (DIR ${SUBSYSTEMS})
  include_directories (${DIR})
  file (GLOB SUBSYSTEM_SOURCES ${DIR}/*.cpp)
  set (SOURCES ${SOURCES} ${SUBSYSTEM_SOURCES})
endforeach(DIR)

add_executable (${TARGET} ${SOURCES})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2GFX_LIBRARY} ${GM1LIB} ${TGXLIB} ${CORELIB})