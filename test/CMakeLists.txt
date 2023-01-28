add_executable(test)

set_target_properties(test PROPERTIES DEBUG_POSTFIX "")
set_target_properties(test PROPERTIES RELEASE_POSTFIX "")

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG master
)
FetchContent_MakeAvailable(doctest)
target_include_directories(test PUBLIC ${doctest_SOURCE_DIR}/doctest)

target_include_directories(test PRIVATE Birdy3d_engine)
target_link_libraries(test Birdy3d_engine)

target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_subdirectory(ui)
