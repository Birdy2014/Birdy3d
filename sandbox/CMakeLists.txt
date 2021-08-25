project(Birdy3d_sandbox)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)

add_executable(Birdy3d_sandbox ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_include_directories(Birdy3d_sandbox PRIVATE Birdy3d_engine)

target_link_directories(Birdy3d_sandbox PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Birdy3d_sandbox Birdy3d_engine)

add_custom_command(
    TARGET Birdy3d_sandbox POST_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/Birdy3d_sandbox ${CMAKE_BINARY_DIR}/out/bin
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/ressources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ressources ${CMAKE_BINARY_DIR}/out/ressources
)

set_target_properties(Birdy3d_sandbox PROPERTIES DEBUG_POSTFIX "")
set_target_properties(Birdy3d_sandbox PROPERTIES RELEASE_POSTFIX "")
