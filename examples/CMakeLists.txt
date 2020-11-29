cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

find_package(SDL2)

if(SDL2_FOUND)
	add_subdirectory(sdl_gl3)
else()
	message("SDL not found")
endif()
