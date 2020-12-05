cmake_minimum_required(VERSION 2.8)

project(flexui)

add_subdirectory(deps/freetype2)

# yoga
file(GLOB_RECURSE yogacore_SRC deps/yoga/yoga/*.cpp)
add_library(yogacore STATIC ${yogacore_SRC})
target_include_directories(yogacore PUBLIC deps/yoga)

# flexui
file(GLOB_RECURSE flexui_SRC
	include/flexui/*.hpp
	src/flexui/*.cpp
)
add_library(flexui ${flexui_SRC})
target_include_directories(flexui PUBLIC include)
target_include_directories(flexui PRIVATE ${freetype_SOURCE_DIR}/include)
target_link_libraries(flexui PUBLIC yogacore freetype)

# TODO: add flags
add_subdirectory(examples)