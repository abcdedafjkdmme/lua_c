cmake_minimum_required(VERSION 3.5)
project(lua_c_test)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Google Test
SET(INSTALL_GTEST OFF CACHE BOOL "Install Gtest")
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lua_c_test main.cpp)
target_link_libraries(lua_c_test PUBLIC GTest::gtest_main)
target_link_libraries(lua_c_test PUBLIC lua_c_lib)
include(GoogleTest)
gtest_discover_tests(lua_c_test)


# copy scripts
add_custom_target(copy-runtime-files-test ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${CMAKE_CURRENT_BINARY_DIR}/scripts
    DEPENDS lua_c_example)