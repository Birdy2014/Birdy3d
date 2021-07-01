cmake_minimum_required(VERSION 3.9)
project(Birdy3d)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Birdy3d SHARED)

add_subdirectory(src)
add_subdirectory(vendor)

target_link_libraries(Birdy3d glfw GL X11 pthread Xrandr Xi dl assimp freetype)

add_custom_command(
    TARGET Birdy3d POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders
)
add_dependencies(Birdy3d copy_assets)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set_target_properties(Birdy3d PROPERTIES DEBUG_POSTFIX "")
set_target_properties(Birdy3d PROPERTIES RELEASE_POSTFIX "")
