cmake_minimum_required(VERSION 2.8)

project(flexui)

# yoga
file(GLOB_RECURSE yogacore_SRC deps/yoga/yoga/*.cpp)
add_library(yogacore STATIC ${yogacore_SRC})
target_include_directories(yogacore PUBLIC deps/yoga)
set_target_properties(yogacore PROPERTIES CXX_STANDARD 11)

# flexui
file(GLOB_RECURSE flexui_SRC flexui/*.cpp)
add_executable(flexui ${flexui_SRC})
target_include_directories(flexui PUBLIC .)
set_target_properties(flexui PROPERTIES CXX_STANDARD 11)
target_link_libraries(flexui PUBLIC yogacore)