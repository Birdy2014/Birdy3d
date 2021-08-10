target_sources(Birdy3d PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c
)

target_include_directories(Birdy3d PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_image
)

include(FetchContent)

find_package(Freetype REQUIRED)
if (FREETYPE_FOUND)
    target_include_directories(Birdy3d PRIVATE ${FREETYPE_INCLUDE_DIRS})
else()
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY   https://github.com/freetype/freetype
        GIT_TAG          VER-2-11-0
        CMAKE_CACHE_ARGS "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON"
    )

    FetchContent_MakeAvailable(freetype)
endif()

find_library(LIBRARY_GLM glm) # FIXME: This never finds glm
if (NOT LIBRARY_GLM)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        0.9.9.8
    )

    FetchContent_MakeAvailable(glm)
    target_include_directories(Birdy3d PUBLIC ${glm_SOURCE_DIR})
endif()

find_library(LIBRARY_GLFW glfw)
if (NOT LIBRARY_GLFW)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG        3.3.4
    )

    FetchContent_MakeAvailable(glfw)
endif()

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
