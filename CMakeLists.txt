cmake_minimum_required(VERSION 3.5)
project (lua_c_lib)


# Add your C library
add_library(lua_c_lib INTERFACE)

# library specific
target_include_directories(lua_c_lib INTERFACE ${CMAKE_SOURCE_DIR}/include)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(LUA_C_LIB_COMPILER_FLAGS -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -fstack-clash-protection -fstack-protector-strong -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -fsanitize=undefined)
    set(LUA_C_LIB_LINKER_FLAGS -fsanitize=undefined)

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND LUA_C_LIB_COMPILER_FLAGS -fsanitize=address)
        list(APPEND LUA_C_LIB_LINKER_FLAGS -fsanitize=address)
    endif()
endif()

target_compile_options(lua_c_lib INTERFACE ${LUA_C_LIB_COMPILER_FLAGS})
target_link_options(lua_c_lib INTERFACE ${LUA_C_LIB_LINKER_FLAGS})

# Link math library
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(lua_c_lib INTERFACE ${MATH_LIBRARY})
endif()

# Link your library to the Lua library
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build as shared library")
add_subdirectory(external/lua)
target_link_libraries(lua_c_lib INTERFACE lua::lib)

# add example project
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


# add tests 
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# add docs
option(BUILD_DOCS "Build docs" ON)
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

# add packaging
include(CPack)
install(TARGETS lua_c_lib
    EXPORT MyLibraryTargets # Exports target information for find_package
    ARCHIVE DESTINATION lib # Library binaries
    LIBRARY DESTINATION lib # Library binaries
    RUNTIME DESTINATION lib # For shared libraries on some systems
)

install(DIRECTORY include/lua_c_lib/
    DESTINATION include # Header files
)