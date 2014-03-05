cmake_minimum_required(VERSION 2.6)

project(openhold)

if (UNIX OR (WIN32 AND MINGW))
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++0x -W -Wall -Wextra -Wunreachable-code")
  set (SDL_INCLUDE_DIR "/usr/include/SDL2")
  set (SDL_LIBRARY "/usr/libs/")
endif ()

find_package (Boost REQUIRED COMPONENTS date_time system)
find_package (SDL REQUIRED)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${SDL_INCLUDE_DIR}/)

set (SRCS
  gamemap.cpp	 
  gm1.cpp	    
  button.cpp
  network.cpp   
  sdl_init.cpp  
  tgx.cpp
  gamereactor.cpp  
  loadingscreen.cpp  
  renderer.cpp  
  window.cpp
  gamescreen.cpp	 
  rootscreen.cpp
  main_menu_screen.cpp
  main.cpp	    
  rw.cpp	  
  surface.cpp
  streamingtexture.cpp)

set (BOOST_LIBS boost_system)
set (SDL_LIBS SDL2)

add_executable (a.out ${SRCS})

target_link_libraries (a.out 
  ${BOOST_LIBS}
  ${SDL_LIBS})