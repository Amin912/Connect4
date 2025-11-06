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

# Prints how many positions each search variant visits (see the README).
add_executable(connect4_bench bench/main.cpp)
target_link_libraries(connect4_bench PRIVATE connect4_core)

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

# SDL2 and SDL2_ttf are built from source as static libraries, so the game
# needs no system-wide install and no DLLs shipped next to it.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.9
    GIT_SHALLOW TRUE)

# SDL2_ttf renders the text, using its bundled copy of FreeType.
set(SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.22.0
    GIT_SHALLOW TRUE
    GIT_SUBMODULES external/freetype)

# FreeType still declares compatibility with CMake 3.0, which CMake 4 rejects.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(SDL2 SDL2_ttf)

add_executable(connect4
    app/main.cpp
    app/Button.cpp
    app/Game.cpp
    app/Grid.cpp
    app/Menu.cpp
    app/Object.cpp
    app/Paint.cpp
    app/Text.cpp
    app/Texture.cpp)
target_link_libraries(connect4 PRIVATE
    connect4_core
    SDL2::SDL2main
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static)
if(MSVC)
    target_compile_options(connect4 PRIVATE /W4 /permissive-)
else()
    target_compile_options(connect4 PRIVATE -Wall -Wextra -Wpedantic)
endif()
# A windowed app on Windows: no console window opens next to the game.
set_target_properties(connect4 PROPERTIES WIN32_EXECUTABLE TRUE)

# The fonts are loaded from an assets folder next to the executable.
add_custom_command(TARGET connect4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:connect4>/assets)
