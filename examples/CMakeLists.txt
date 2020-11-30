cmake_minimum_required(VERSION 2.8)

find_package(OpenGL REQUIRED)

if(NOT EMSCRIPTEN)
	# glfw
	add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)

	# glew
	# add_definitions(-DGLEW_STATIC)
	# add_subdirectory(libs/glew EXCLUDE_FROM_ALL)
endif()

# glm
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)

# examples
add_subdirectory(glfw_gl3)
