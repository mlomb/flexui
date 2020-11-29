cmake_minimum_required(VERSION 2.8)

add_executable(sdl_gl3 main.cpp)
target_include_directories(sdl_gl3 PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(sdl_gl3 flexui ${SDL2_LIBRARY})
