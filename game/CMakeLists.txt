include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  camera.cpp
  collection.cpp
  color.cpp
  console.cpp
  creature.cpp
  creaturestate.cpp
  direction.cpp
  engine.cpp
  filesystem.cpp
  font.cpp
  fontmanager.cpp
  gamescreen.cpp
  gamemap.cpp
  gm1.cpp
  gm1entryreader.cpp
  gm1palette.cpp
  gm1reader.cpp
  gm1writer.cpp
  graphicsmanager.cpp
  ingamemenu.cpp
  loadingscreen.cpp
  main.cpp
  menu_combat.cpp
  menu_main.cpp
  network.cpp
  point.cpp
  range.cpp
  rect.cpp
  renderer.cpp
  softwarerenderengine.cpp
  screenmanager.cpp
  simulationmanager.cpp
  simulationcontext.cpp
  sdl_utils.cpp
  sdlrenderengine.cpp
  surface.cpp
  surface_view.cpp
  surface_blur.cpp
  textarea.cpp
  textlayout.cpp
  textlayoutitem.cpp
  tgx.cpp
)

set (TARGET "a.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2GFX_LIBRARY})