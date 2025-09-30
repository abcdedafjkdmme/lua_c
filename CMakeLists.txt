cmake_minimum_required(VERSION 3.5)
project (lua_c_lib)


# Add your C library
add_library(lua_c_lib INTERFACE)

# Link math library
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(lua_c_lib INTERFACE ${MATH_LIBRARY})
endif()

# library specific
target_include_directories(lua_c_lib INTERFACE ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
  target_compile_options(lua_c_lib INTERFACE -O0 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -fstack-clash-protection -fstack-protector-strong -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -fsanitize=address -fsanitize=undefined)
  target_link_options(lua_c_lib INTERFACE -fsanitize=address -fsanitize=undefined)
endif()


# Link your library to the Lua library
find_package(Lua REQUIRED)
# add_dependencies(lua_c_lib liblua) # Ensure Lua is built before your project
target_include_directories(lua_c_lib INTERFACE ${LUA_INCLUDE_DIR})
target_link_libraries(lua_c_lib INTERFACE ${LUA_LIBRARIES})



# add example project
add_subdirectory(examples)

# add tests 
enable_testing()
add_subdirectory(tests)

# add docs
add_subdirectory(docs)


