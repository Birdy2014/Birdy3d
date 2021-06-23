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
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_DIR}/build/shaders ${CMAKE_CURRENT_LIST_DIR}/shaders
)
