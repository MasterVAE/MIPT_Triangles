cmake_minimum_required(VERSION 3.25)

project(TriaglesIntersaction)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(triangles_strict_debug INTERFACE)
option(ENABLE_STRICT_DEBUG "Enable _GLIBCXX_DEBUG and Adress/UB sanitizers" OFF)

add_library(triangles_compile_flags INTERFACE)

add_executable(triangles_exec
    src/graphic.cpp
    src/main.cpp
    src/triangles.cpp
)

target_include_directories(triangles_exec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_compile_options(triangles_compile_flags INTERFACE
    $<$<CONFIG:DEBUG>:-Wall;>
    $<$<CONFIG:RELEASE:-02>
)

if(ENABLE_STRICT_DEBUG)
    target_compile_definitions(triangles_strict_debug INTERFACE _GLIBCXX_DEBUG)
    target_compile_options(triangles_strict_debug INTERFACE 
        -fsanitize=address,undefined -fno-omit-frame-pointer
    )
    target_link_options(triangles_strict_debug INTERFACE 
        -fsanitize=address,undefined
    )
endif()
