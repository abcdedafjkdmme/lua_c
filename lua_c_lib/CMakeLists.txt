cmake_minimum_required(VERSION 3.5)
project (lua_c_lib)




# Add Lua library
set(LUA_VERSION "lua-5.4.4")
include(ExternalProject)
ExternalProject_Add(liblua
  PREFIX ${CMAKE_BINARY_DIR}/${LUA_VERSION}
  URL https://www.lua.org/ftp/${LUA_VERSION}.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make generic
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE true
  DOWNLOAD_EXTRACT_TIMESTAMP false
)
# Specify where to find Lua includes and libraries after building
ExternalProject_Get_Property(liblua BINARY_DIR)
set(LUA_INCLUDE_DIR ${BINARY_DIR}/src)
set(LUA_LIBRARIES ${BINARY_DIR}/src/liblua.a)





# Add your C library
add_library(lua_c_lib INTERFACE)
# Link your library to the Lua library
add_dependencies(lua_c_lib liblua) # Ensure Lua is built before your project
target_include_directories(lua_c_lib INTERFACE ${LUA_INCLUDE_DIR} "include")
target_link_libraries(lua_c_lib INTERFACE ${LUA_LIBRARIES})
target_link_libraries(lua_c_lib INTERFACE m)
target_compile_options(lua_c_lib INTERFACE -O0 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -fstack-clash-protection -fstack-protector-strong -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -fsanitize=address -fsanitize=undefined)
target_link_options(lua_c_lib INTERFACE -fsanitize=address -fsanitize=undefined)


