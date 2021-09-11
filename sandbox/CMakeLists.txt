project(Birdy3d_sandbox)

add_executable(Birdy3d_sandbox ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_include_directories(Birdy3d_sandbox PRIVATE Birdy3d_engine)

target_link_libraries(Birdy3d_sandbox Birdy3d_engine)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ressources)
    add_custom_target(
        Birdy3d_copy_sandbox_ressources
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out/ressources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ressources ${CMAKE_BINARY_DIR}/out/ressources
    )

    add_dependencies(Birdy3d_sandbox Birdy3d_copy_sandbox_ressources)
endif()

set_target_properties(Birdy3d_sandbox PROPERTIES DEBUG_POSTFIX "")
set_target_properties(Birdy3d_sandbox PROPERTIES RELEASE_POSTFIX "")
