project(sandbox)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)

add_executable(sandbox ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_include_directories(sandbox PRIVATE Birdy3d)

target_link_directories(sandbox PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sandbox Birdy3d)

add_custom_command(
    TARGET sandbox POST_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/sandbox ${CMAKE_BINARY_DIR}/out/bin
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/out/ressources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/ressources ${CMAKE_BINARY_DIR}/out/ressources
)

set_target_properties(sandbox PROPERTIES DEBUG_POSTFIX "")
set_target_properties(sandbox PROPERTIES RELEASE_POSTFIX "")
