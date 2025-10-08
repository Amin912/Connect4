cmake_minimum_required(VERSION 3.21)
project(connect4 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CONNECT4_BUILD_GAME "Build the SDL2 game" ON)
option(CONNECT4_BUILD_TESTS "Build the unit tests" ON)

include(FetchContent)

# Game rules and AI. Plain C++ with no SDL dependency, so the tests can
# exercise it without opening a window.
add_library(connect4_core
    core/board.cpp
    core/ai.cpp)
target_include_directories(connect4_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
    target_compile_options(connect4_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(connect4_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CONNECT4_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(doctest)

    add_executable(connect4_tests
        tests/main.cpp
        tests/board_tests.cpp
        tests/ai_tests.cpp)
    target_link_libraries(connect4_tests PRIVATE connect4_core doctest::doctest)
    add_test(NAME connect4_tests COMMAND connect4_tests)
endif()

if(NOT CONNECT4_BUILD_GAME)
    return()
endif()

# SDL2 and SDL2_image are built from source as static libraries, so the game
# needs no system-wide SDL install and no DLLs shipped next to it.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
    GIT_SHALLOW TRUE)

# PNG and JPEG are decoded by SDL_image's bundled stb_image backend, so the
# formats that would need extra libraries are switched off.
set(SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_VENDORED OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.2
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(SDL2 SDL2_image)

add_executable(connect4
    main.cpp
    src/Game.cpp
    src/Grid.cpp
    src/Object.cpp
    src/Texture.cpp)
target_include_directories(connect4 PRIVATE include)
target_link_libraries(connect4 PRIVATE
    SDL2::SDL2main
    SDL2::SDL2-static
    SDL2_image::SDL2_image-static)

# The game loads its images from an assets folder next to the executable.
add_custom_command(TARGET connect4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:connect4>/assets)
set_property(TARGET connect4 PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:connect4>)
