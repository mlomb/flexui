cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/cmake-sdl/cmake/Modules)

find_package(SDL2)
find_package(OpenGL)

if(NOT EMSCRIPTEN)
	# glew
	add_definitions(-DGLEW_STATIC)
	add_subdirectory(libs/glew EXCLUDE_FROM_ALL)
endif()

if(EMSCRIPTEN OR (OpenGL_FOUND AND SDL2_FOUND))
	add_subdirectory(sdl_gl3)
else()
	message("SDL not found")
endif()
