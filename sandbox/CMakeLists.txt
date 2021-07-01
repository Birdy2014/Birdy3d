cmake_minimum_required(VERSION 3.9)
project(sandbox)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../engine)

add_executable(sandbox ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(sandbox PRIVATE
    ${ENGINE_DIR}/src
    ${ENGINE_DIR}/vendor/glad/include
)

target_link_directories(sandbox PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sandbox Birdy3d)

add_custom_command(
    TARGET sandbox PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${ENGINE_DIR}/build/libBirdy3d.so ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
    TARGET sandbox POST_BUILD
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/out
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_DIR}/build/shaders ${CMAKE_BINARY_DIR}/out/shaders
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/libBirdy3d.so ${CMAKE_BINARY_DIR}/out/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/sandbox ${CMAKE_BINARY_DIR}/out/bin
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/out/ressources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/ressources ${CMAKE_BINARY_DIR}/out/ressources
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set_target_properties(sandbox PROPERTIES DEBUG_POSTFIX "")
set_target_properties(sandbox PROPERTIES RELEASE_POSTFIX "")
