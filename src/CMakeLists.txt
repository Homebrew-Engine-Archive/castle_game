cmake_minimum_required(VERSION 2.6)


include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS}
)

set (SRCS
  sdl_utils.cpp
  atlas.cpp
  gamemap.cpp
  geometry.cpp
  gm1.cpp
  button.cpp
  network.cpp
  sdl_init.cpp
  tgx.cpp
  gamereactor.cpp
  loadingscreen.cpp
  font.cpp
  textrenderer.cpp
  renderer.cpp
  window.cpp
  gamescreen.cpp
  rootscreen.cpp
  menu_main.cpp
  menu_combat.cpp
  main.cpp
  rw.cpp
  surface.cpp
  collection.cpp
  filesystem.cpp
)

find_package (Boost 1.54.0 REQUIRED COMPONENTS filesystem)
find_package (Boost 1.47.0 REQUIRED COMPONENTS date_time system)

set (BOOST_LIBS boost_system boost_filesystem boost_date_time)

add_executable (a.out ${SRCS})

target_link_libraries (a.out
  ${BOOST_LIBS}
  ${SDL2_LIBRARIES}
)