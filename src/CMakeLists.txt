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
  rw.cpp
  sdl_init.cpp
  sdl_utils.cpp
  surface.cpp
  textrenderer.cpp
  tgx.cpp
  screenmanager.cpp
  resourcemanager.cpp
)

set (TARGET "a.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBS} ${SDL2_LIBRARIES})