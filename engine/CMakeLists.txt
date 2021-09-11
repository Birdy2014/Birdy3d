project(Birdy3d_engine)

add_library(Birdy3d_engine STATIC)

add_subdirectory(src)
add_subdirectory(vendor)

# TODO: Store this in a variable, if linux, etc.
if(WIN32)
    target_link_libraries(Birdy3d_engine glfw opengl32 assimp freetype)
else()
    target_link_libraries(Birdy3d_engine glfw GL X11 pthread Xrandr Xi dl assimp freetype)
endif()

add_custom_target(
    Birdy3d_copy_shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/out/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out/ressources
)

add_dependencies(Birdy3d_engine Birdy3d_copy_shaders)

set_target_properties(Birdy3d_engine PROPERTIES DEBUG_POSTFIX "")
set_target_properties(Birdy3d_engine PROPERTIES RELEASE_POSTFIX "")
