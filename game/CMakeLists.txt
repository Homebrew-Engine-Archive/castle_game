include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SUBSYSTEMS
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES 
  engine.cpp
  main.cpp)

foreach (DIR ${SUBSYSTEMS})
  include_directories (${DIR})
  file (GLOB SUBSYSTEM_SOURCES ${DIR}/*.cpp)
  set (SOURCES ${SOURCES} ${SUBSYSTEM_SOURCES})
endforeach(DIR)

set (TARGET "a.out")

add_executable (${TARGET} ${SOURCES})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2GFX_LIBRARY} corelib.a)