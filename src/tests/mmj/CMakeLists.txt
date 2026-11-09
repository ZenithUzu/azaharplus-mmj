cmake_minimum_required(VERSION 3.20)
project(mmj_hacks_tests LANGUAGES CXX)

# Small, portable regression target: exercise the real hack tables without needing Qt,
# an Android SDK, a GPU, game files, or the rest of the emulator's dependencies.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(../../../externals/catch2 catch2)

add_executable(mmj_hacks_tests
    ../common/mmj_hacks.cpp
    ../../../src/common/hacks/hack_manager.cpp
    ../../../src/common/hacks/hack_list.cpp
)
target_include_directories(mmj_hacks_tests PRIVATE ../../../src)
target_link_libraries(mmj_hacks_tests PRIVATE Catch2::Catch2WithMain)
enable_testing()
add_test(NAME mmj_hacks COMMAND mmj_hacks_tests)
