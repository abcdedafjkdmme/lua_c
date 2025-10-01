cmake_minimum_required(VERSION 3.5)
project(lua_c_example)

add_executable(lua_c_example main.c)

target_link_libraries(lua_c_example PUBLIC lua_c_lib)

add_custom_target(copy-runtime-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${CMAKE_CURRENT_BINARY_DIR}/scripts
    DEPENDS lua_c_example)

# add packaging
install(TARGETS lua_c_example RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION bin/)
