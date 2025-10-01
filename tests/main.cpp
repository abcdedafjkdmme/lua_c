#include <gtest/gtest.h>

extern "C"
{
#include "lua_c_lib/luac.h"
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);

  lua_State *L = luaL_newstate();
  if (L == NULL)
  {
    printf("[LUA_C] ERROR: lua state is NULL");
    FAIL();
  }
  luaL_openlibs(L);

  int errcode = luaL_loadfile(L, "scripts/main.lua");
  if (errcode != LUA_OK)
  {
    luac_log_error(L, errcode, "luaL_loadfile");
    FAIL();
  }

  luac_set_number(L, "C_num", 123);
  luac_set_boolean(L, "C_bool", 1);
  luac_set_string(L, "C_str", "c string hello");

  errcode = lua_pcall(L, 0, 0, 0);
  if (errcode != LUA_OK)
  {
    luac_log_error(L, errcode, "luaL_pcall");
    FAIL();
  }

  lua_Number lua_var_num = -1;
  errcode = luac_get_number(L, "Lua_num", &lua_var_num);
  assert(errcode == 0);
  printf("[LUA_C] Lua_num is %lf \n", lua_var_num);

  int lua_var_bool = -1;
  errcode = luac_get_boolean(L, "Lua_bool", &lua_var_bool);
  assert(errcode == 0);
  printf("[LUA_C] Lua_bool is %s \n", lua_var_bool ? "true" : "false");

  const char *lua_var_str = NULL;
  errcode = luac_get_string(L, "Lua_str", &lua_var_str);
  assert(errcode == 0);
  printf("[LUA_C] Lua_str is %s \n", lua_var_str);

  // call lua funciton

  lua_close(L);
}
