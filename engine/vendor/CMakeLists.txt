target_sources(Birdy3d_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c
)

target_include_directories(Birdy3d_engine SYSTEM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_image
)

find_package(OpenGL REQUIRED)
target_link_libraries(Birdy3d_engine OpenGL::GL)

include(FetchContent)

find_package(Freetype)
if (FREETYPE_FOUND)
    target_include_directories(Birdy3d_engine PRIVATE ${FREETYPE_INCLUDE_DIRS})
else()
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY   https://github.com/freetype/freetype
        GIT_TAG          VER-2-11-0
        CMAKE_CACHE_ARGS "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON"
    )

    FetchContent_MakeAvailable(freetype)
endif()
target_link_libraries(Birdy3d_engine freetype)

find_path(LIBRARY_GLM glm)
if(LIBRARY_GLM STREQUAL "LIBRARY_GLM-NOTFOUND")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        master
    )

    FetchContent_MakeAvailable(glm)
    target_include_directories(Birdy3d_engine PUBLIC ${glm_SOURCE_DIR})
else()
    target_include_directories(Birdy3d_engine PUBLIC ${LIBRARY_GLM})
endif()

# TODO: uncomment find_library and change GIT_TAG once GLFW 3.4 is released
#find_library(LIBRARY_GLFW glfw)
#if (NOT LIBRARY_GLFW)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    find_library(LIBRARY_WAYLAND NAMES wayland-client)
    if (LIBRARY_WAYLAND)
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
        target_compile_definitions(Birdy3d_engine PRIVATE _GLFW_WAYLAND)
    endif()

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG        master
    )

    FetchContent_MakeAvailable(glfw)
#endif()
target_link_libraries(Birdy3d_engine glfw)

find_library(LIBRARY_ASSIMP assimp)
if (NOT LIBRARY_ASSIMP)
    set(ASSIMP_BUILD_TESTS CACHE INTERNAL OFF)
    set(BUILD_SHARED_LIBS CACHE INTERNAL OFF)
    set(ASSIMP_BUILD_ASSIMP_TOOLS CACHE INTERNAL OFF)

    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG        v5.0.1
    )

    FetchContent_MakeAvailable(assimp)
endif()
target_link_libraries(Birdy3d_engine assimp)

find_library(LIBRARY_FMT fmt)
if(NOT LIBRARY_FMT)
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(fmt)
endif()
target_link_libraries(Birdy3d_engine fmt)
