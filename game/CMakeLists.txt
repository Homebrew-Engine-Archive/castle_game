include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  collection.cpp
  debugconsole.cpp
  direction.cpp
  engine.cpp
  exception.cpp
  filesystem.cpp
  fontmanager.cpp
  gamescreen.cpp
  gamemap.cpp
  gm1.cpp
  gm1entryreader.cpp
  gm1palette.cpp
  gm1reader.cpp
  gm1writer.cpp
  loadingscreen.cpp
  main.cpp
  menu_combat.cpp
  menu_main.cpp
  network.cpp
  renderer.cpp
  rw.cpp
  screenmanager.cpp
  simulationmanager.cpp
  sdl_utils.cpp
  surface.cpp
  textrenderer.cpp
  tgx.cpp
)

set (TARGET "a.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})