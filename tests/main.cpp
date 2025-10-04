#include <gtest/gtest.h>
#include "lua_c_lib/luac.hpp"

TEST(LuaC, GetVariables)
{

  lua_State *L = luaL_newstate();
  ASSERT_NE(L, nullptr) << "error lua state is NULL";
  luaL_openlibs(L);
  int errcode = luaL_loadfile(L, "scripts/get_variables.lua");
  ASSERT_EQ(errcode, LUA_OK) << "error loading lua script";
  errcode = lua_pcall(L, 0, 0, 0);
  ASSERT_EQ(errcode, LUA_OK) << "error lua_pcall";

  lua_Number lua_var_num = -1;
  errcode = luac_get_number(L, "Lua_num", &lua_var_num);
  EXPECT_EQ(errcode, 0);
  EXPECT_EQ(lua_var_num, 321);


  int lua_var_bool = -1;
  errcode = luac_get_boolean(L, "Lua_bool", &lua_var_bool);
  EXPECT_EQ(errcode, 0);
  EXPECT_EQ(lua_var_bool, false);

  const char *lua_var_str = NULL;
  errcode = luac_get_string(L, "Lua_str", &lua_var_str);
  EXPECT_EQ(errcode, 0);
  EXPECT_STREQ(lua_var_str, "hello string c");

  // assert that no extra items are left on lua stack
  int stack_top = lua_gettop(L);
  EXPECT_EQ(stack_top,0);

  lua_close(L);
}

TEST(LuaC, SetVariables)
{

  lua_State *L = luaL_newstate();
  ASSERT_NE(L, nullptr) << "error lua state is NULL";
  luaL_openlibs(L);
  int errcode = luaL_loadfile(L, "scripts/set_variables.lua");
  ASSERT_EQ(errcode, LUA_OK) << "error loading lua script";

  luac_set_number(L, "C_num", 123);
  luac_set_boolean(L, "C_bool", true);
  luac_set_string(L, "C_str", "c string hello");

  errcode = lua_pcall(L, 0, 0, 0);
  ASSERT_EQ(errcode, LUA_OK) << "error lua_pcall";

  lua_Number lua_var_num = -1;
  errcode = luac_get_number(L, "C_num", &lua_var_num);
  EXPECT_EQ(errcode, 0);
  EXPECT_EQ(lua_var_num, 123);

  int lua_var_bool = -1;
  errcode = luac_get_boolean(L, "C_bool", &lua_var_bool);
  EXPECT_EQ(errcode, 0);
  EXPECT_EQ(lua_var_bool, true);

  const char *lua_var_str = NULL;
  errcode = luac_get_string(L, "C_str", &lua_var_str);
  EXPECT_EQ(errcode, 0);
  EXPECT_STREQ(lua_var_str, "c string hello");


  // assert that no extra items are left on lua stack
  int stack_top = lua_gettop(L);
  EXPECT_EQ(stack_top,0);

  lua_close(L);
}

TEST(LuaC, CallLuaFunctionManual)
{

  lua_State *L = luaL_newstate();
  ASSERT_NE(L, nullptr) << "error lua state is NULL";
  luaL_openlibs(L);
  int errcode = luaL_loadfile(L, "scripts/lua_function.lua");
  ASSERT_EQ(errcode, LUA_OK) << "error loading lua script";
  errcode = lua_pcall(L, 0, 0, 0);
  ASSERT_EQ(errcode, LUA_OK) << "error lua_pcall";

  // first ask lua to push function on stack
  int var_type = lua_getglobal(L, "Vec3_len");
  ASSERT_EQ(var_type, LUA_TFUNCTION);
  // arg x
  lua_pushnumber(L, 2);
  // arg y
  lua_pushnumber(L, 3);
  // arg z
  lua_pushnumber(L, 6);
  // 3 arguments and 1 return value
  lua_pcall(L, 3, 1, 0);
  // return value is now on stack
  lua_Number ret = lua_tonumber(L, -1);
  lua_pop(L, 1);
  EXPECT_DOUBLE_EQ(ret, 7.0);

  // assert that no extra items are left on lua stack
  int stack_top = lua_gettop(L);
  EXPECT_EQ(stack_top,0);

  lua_close(L);
}

TEST(LuaC, CallLuaFunction)
{

  lua_State *L = luaL_newstate();
  ASSERT_NE(L, nullptr) << "error lua state is NULL";
  luaL_openlibs(L);
  int errcode = luaL_loadfile(L, "scripts/lua_function.lua");
  ASSERT_EQ(errcode, LUA_OK) << "error loading lua script";
  errcode = lua_pcall(L, 0, 0, 0);
  ASSERT_EQ(errcode, LUA_OK) << "error lua_pcall";

  lua_Number return_val = -1;
  int params_type[] = {LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER};
  errcode = luac_call_function(L, "Vec3_len", LUA_TNUMBER, &return_val, params_type, LUAC_COUNTOF(params_type), 2.0, 3.0, 6.0);

  EXPECT_EQ(errcode,0);
  EXPECT_DOUBLE_EQ(return_val, 7.0);

  // assert that no extra items are left on lua stack
  int stack_top = lua_gettop(L);
  EXPECT_EQ(stack_top,0);

  lua_close(L);
}


int sign_function(lua_State* L){
  // first get the params from stack
  lua_Number param_1 = lua_tonumber(L,-1);
  // calculate result
  lua_Number result = param_1 / abs(param_1);
  // push the return value to stack
  lua_pushnumber(L,result);
  // the number of return values
  return 1;
}

TEST(LuaC, CallCFunction)
{

  lua_State *L = luaL_newstate();
  ASSERT_NE(L, nullptr) << "error lua state is NULL";
  luaL_openlibs(L);
  int errcode = luaL_loadfile(L, "scripts/lua_c_function.lua");
  ASSERT_EQ(errcode, LUA_OK) << "error loading lua script";
  
  luac_set_function(L,"C_sign",&sign_function);

  errcode = lua_pcall(L, 0, 0, 0);
  ASSERT_EQ(errcode, LUA_OK) << "error lua_pcall";

  lua_Number function_return_val = 0xDEADBEEF;
  errcode = luac_get_number(L,"Output",&function_return_val);
  EXPECT_EQ(errcode,0);
  EXPECT_DOUBLE_EQ(function_return_val,1.0);
  // assert that no extra items are left on lua stack
  int stack_top = lua_gettop(L);
  EXPECT_EQ(stack_top,0);

  lua_close(L);
}