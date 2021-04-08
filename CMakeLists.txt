cmake_minimum_required(VERSION 2.8)

project(flexui)

add_subdirectory(deps)

# flexui
file(GLOB_RECURSE flexui_SRC
	include/flexui/*.hpp
	src/flexui/*.cpp
)
add_library(flexui ${flexui_SRC})
target_include_directories(flexui PUBLIC include)
target_link_libraries(flexui PUBLIC yogacore freetype)

option(FLEXUI_BUILD_EXAMPLES "Build flexui examples" ON)
if(FLEXUI_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
