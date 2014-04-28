include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (TARGET "testshaders.out")

set (SRCS
  main.cpp
  shaderprogram.cpp
  animationsheet.cpp
  palettesheet.cpp
  glerror.cpp
)

set (SHARED
  ../game/surface.cpp
  ../game/sdl_utils.cpp
  ../game/gm1reader.cpp
  ../game/gm1entryreader.cpp
  ../game/gm1palette.cpp
  ../game/gm1.cpp
  ../game/tgx.cpp
)

add_definitions (-DGLEW_STATIC)

find_package (OpenGL REQUIRED gl)

set (GLEW_LIBRARIES "GLEW")

include_directories (${OPENGL_INCLUDE_DIRS})

add_executable (${TARGET} ${SRCS} ${SHARED})

target_link_libraries (${TARGET} ${BOOST_LIBRARIES} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})