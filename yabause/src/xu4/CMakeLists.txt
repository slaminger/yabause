project(yabause-xu4)

yab_port_start()

find_package(SDL2 COMPONENTS sdl)
find_package(SDL2 REQUIRED)
find_package(OpenGLES)
find_package(PkgConfig REQUIRED)

if (NOT OPENGLES2_FOUND)
	return()
endif (NOT OPENGLES2_FOUND)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PORT_INCLUDE_DIRS ${SDL2_INCLUDE_DIR} ${OPENGLES_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})
set(PORT_LIBRARIES ${SDL2_LIBRARY} ${OPENGLES2_LIBRARIES} ${GLFW_LIBRARIES})

set(yabause_xu4_SOURCES
	main.cpp
)

add_executable(yabause-xu4 ${yabause_xu4_SOURCES})
target_link_libraries(yabause-xu4 yabause ${YABAUSE_LIBRARIES} ${PORT_LIBRARIES})

yab_port_success(yabause-xu4)

