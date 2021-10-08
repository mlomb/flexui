cmake_minimum_required(VERSION 2.8.12)

add_executable(glfw_gl3 main.cpp)
target_link_libraries(glfw_gl3 flexui)

if(EMSCRIPTEN)
	target_link_libraries(glfw_gl3 "-s WASM=1")
	target_link_libraries(glfw_gl3 "-s USE_GLFW=3")
	target_link_libraries(glfw_gl3 "-s USE_WEBGL2=1")
	target_link_libraries(glfw_gl3 "-s FULL_ES3=1")
	target_link_libraries(glfw_gl3 "-o index.html")
	target_link_libraries(glfw_gl3 "--shell-file \"${CMAKE_CURRENT_SOURCE_DIR}\"/template.html")
	
	# flags here are very aggressive
	# trying to reduce the final WASM size
	target_link_libraries(glfw_gl3 "-Oz")
	target_link_libraries(glfw_gl3 "-fno-rtti")
	target_link_libraries(glfw_gl3 "--closure 1")
	target_link_libraries(glfw_gl3 "-flto")
	
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(glfw_gl3 "-s DEMANGLE_SUPPORT=1")
	endif()
else()
	target_link_libraries(glfw_gl3 ${OPENGL_LIBRARIES} glfw libglew_static)
endif()