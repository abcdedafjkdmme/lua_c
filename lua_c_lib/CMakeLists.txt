cmake_minimum_required(VERSION 3.5)
project (lua_c_lib)

find_package(Lua)

if(Lua_FOUND AND NOT TARGET Lua::Lua)
  add_library(Lua::Lua INTERFACE IMPORTED)
  set_target_properties(
    Lua::Lua
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
  )
endif()



# Add your C library
add_library(lua_c_lib INTERFACE)
# Link your library to the Lua library
add_dependencies(lua_c_lib liblua) # Ensure Lua is built before your project
target_include_directories(lua_c_lib INTERFACE ${LUA_INCLUDE_DIR} "include")
target_link_libraries(lua_c_lib INTERFACE ${LUA_LIBRARIES})
target_link_libraries(lua_c_lib INTERFACE m)

if(UNIX)
  target_compile_options(lua_c_lib INTERFACE -O0 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -fstack-clash-protection -fstack-protector-strong -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -fsanitize=address -fsanitize=undefined)
  target_link_options(lua_c_lib INTERFACE -fsanitize=address -fsanitize=undefined)
endif()


