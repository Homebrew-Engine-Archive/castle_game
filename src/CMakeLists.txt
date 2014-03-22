cmake_minimum_required(VERSION 2.6)

include (FindPkgConfig)
pkg_search_module (SDL2 REQUIRED sdl2)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS}
)

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
  window.cpp
)

find_package (Boost 1.54.0 REQUIRED COMPONENTS filesystem)
find_package (Boost 1.47.0 REQUIRED COMPONENTS date_time system)

set (BOOST_LIBS boost_system boost_filesystem boost_date_time)

add_executable (a.out ${SRCS})

target_link_libraries (a.out
  ${BOOST_LIBS}
  ${SDL2_LIBRARIES}
)