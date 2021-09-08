project(Birdy3d_engine)

add_library(Birdy3d_engine SHARED)

add_subdirectory(src)
add_subdirectory(vendor)

target_link_libraries(Birdy3d_engine glfw GL X11 pthread Xrandr Xi dl assimp freetype)

add_custom_command(
    TARGET Birdy3d_engine POST_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/libBirdy3d_engine.so ${CMAKE_BINARY_DIR}/out/bin
)

add_custom_target(
    Birdy3d_copy_shaders
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/out/shaders
)

add_dependencies(Birdy3d_engine Birdy3d_copy_shaders)
