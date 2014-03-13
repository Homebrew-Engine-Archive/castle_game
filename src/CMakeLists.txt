cmake_minimum_required(VERSION 2.6)

project(stockade)

if (UNIX OR (WIN32 AND MINGW))
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
endif ()

include (FindPkgConfig)

pkg_search_module (SDL2 REQUIRED sdl2)

find_package (Boost REQUIRED COMPONENTS date_time system)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS})

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
  streamingtexture.cpp)

set (BOOST_LIBS boost_system)

add_executable (a.out ${SRCS})

target_link_libraries (a.out 
  ${BOOST_LIBS}
  ${SDL2_LIBRARIES})