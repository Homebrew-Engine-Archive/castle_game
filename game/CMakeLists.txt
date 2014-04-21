include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  atlas.cpp
  button.cpp
  collection.cpp
  debugconsole.cpp
  direction.cpp
  engine.cpp
  filesystem.cpp
  font.cpp
  gamemap.cpp
  gamereactor.cpp
  gamescreen.cpp
  geometry.cpp
  gm1.cpp
  loadingscreen.cpp
  main.cpp
  menu_combat.cpp
  menu_main.cpp
  network.cpp
  point.cpp
  renderer.cpp
  sdl_init.cpp
  sdl_utils.cpp
  surface.cpp
  textrenderer.cpp
  tgx.cpp
  screenmanager.cpp
  graphicsmanager.cpp
  widget.cpp
  entityclass.cpp
  gm1reader.cpp
  gm1entryreader.cpp
  gm1palette.cpp
)

set (TARGET "a.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})