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
target_include_directories(Birdy3d_engine PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(Birdy3d_engine ${FREETYPE_LIBRARIES})

find_path(LIBRARY_GLM glm)
target_include_directories(Birdy3d_engine PUBLIC ${LIBRARY_GLM})

find_library(LIBRARY_GLFW glfw)
target_link_libraries(Birdy3d_engine ${LIBRARY_GLFW})

find_library(LIBRARY_ASSIMP assimp)
target_link_libraries(Birdy3d_engine ${LIBRARY_ASSIMP})

find_library(LIBRARY_FMT fmt)
target_link_libraries(Birdy3d_engine ${LIBRARY_FMT})
