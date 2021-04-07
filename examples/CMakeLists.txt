cmake_minimum_required(VERSION 2.8)

if(NOT EMSCRIPTEN)
	find_package(OpenGL REQUIRED)

	# glfw
	add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)

	# glew
	add_definitions(-DGLEW_STATIC)
	add_subdirectory(libs/glew EXCLUDE_FROM_ALL)
endif()

# examples
add_subdirectory(glfw_gl3)
