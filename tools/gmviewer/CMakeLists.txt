include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (SRCS
  main.cpp
  model.cpp
)

set(TARGET "gm.out")

add_executable (${TARGET} ${SRCS})

target_link_libraries (${TARGET} ${BOOST_LIBS} ${SDL2_LIBRARIES})