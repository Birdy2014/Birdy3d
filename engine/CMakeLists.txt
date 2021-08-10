project(Birdy3d)

add_library(Birdy3d SHARED)

add_subdirectory(src)
add_subdirectory(vendor)

target_link_libraries(Birdy3d glfw GL X11 pthread Xrandr Xi dl assimp freetype)

add_custom_command(
    TARGET Birdy3d POST_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/libBirdy3d.so ${CMAKE_BINARY_DIR}/out/bin
)

add_custom_target(
    copy_shaders
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/out/shaders
)

add_dependencies(Birdy3d copy_shaders)

set_target_properties(Birdy3d PROPERTIES DEBUG_POSTFIX "")
set_target_properties(Birdy3d PROPERTIES RELEASE_POSTFIX "")
