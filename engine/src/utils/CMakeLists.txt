target_sources(Birdy3d_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FPPlayerController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Identifier.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimitiveGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Stacktrace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Unicode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serializer/Adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serializer/Json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serializer/PointerRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serializer/Types.cpp
)
