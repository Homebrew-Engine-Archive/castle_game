include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  atlas.cpp
  button.cpp
  collection.cpp
  debugconsole.cpp
  direction.cpp
  engine.cpp
  entityclass.cpp
  filesystem.cpp
  fontmanager.cpp
  gameexception.cpp
  gamescreen.cpp
  gm1.cpp
  gm1entryreader.cpp
  gm1palette.cpp
  gm1reader.cpp
  loadingscreen.cpp
  main.cpp
  menu_combat.cpp
  menu_main.cpp
  network.cpp
  point.cpp
  renderer.cpp
  rw.cpp
  screenmanager.cpp
  sdl_utils.cpp
  surface.cpp
  textrenderer.cpp
  tgx.cpp
)

set (TARGET "a.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})