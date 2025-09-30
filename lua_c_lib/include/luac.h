#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>

const char *luac_errcode_to_str(int errcode)
{
  if (errcode == LUA_OK)
  {
    return "OK";
  }
  else if (errcode == LUA_ERRRUN)
  {
    return "RUNTIME ERROR";
  }
  else if (errcode == LUA_ERRMEM)
  {
    return "MEMORY ALLOCATION ERROR";
  }
  else if (errcode == LUA_ERRERR)
  {
    return "ERROR RUNNING MESSAGE HANDLER";
  }
  else if (errcode == LUA_ERRSYNTAX)
  {
    return "SYNTAX ERROR";
  }
  else if (errcode == LUA_YIELD)
  {
    return "THREAD YIELDS";
  }
  else if (errcode == LUA_ERRFILE)
  {
    return "FILE ERROR";
  }
  else
  {
    assert(0);
    return "ERROR: CANNOT PARSE ERRCODE";
  }
}

void luac_dump_stack(lua_State *L)
{
  printf("[LUA_C] dumping stack: \n");
  int top = lua_gettop(L);
  for (int i = 1; i <= top; i++)
  {
    printf("%d\t%s\t", i, luaL_typename(L, i));
    switch (lua_type(L, i))
    {
    case LUA_TNUMBER:
      printf("%g\n", lua_tonumber(L, i));
      break;
    case LUA_TSTRING:
      printf("%s\n", lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN:
      printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
      break;
    case LUA_TNIL:
      printf("%s\n", "nil");
      break;
    default:
      printf("%p\n", lua_topointer(L, i));
      break;
    }
  }
}

void luac_log_error(lua_State *L, int errcode, const char *errmsg)
{
  const char* errstr = lua_tostring(L,-1);
  const char* errcode_str = luac_errcode_to_str(errcode);
  printf("[LUA_C] ERROR: %s DESCRIPTION: %s ERRRCODE: %s \n", errmsg, errstr, errcode_str);
}

int luac_get_number(lua_State *L, const char *var_name, lua_Number *result)
{
  int var_type = lua_getglobal(L, var_name);
  if (var_type != LUA_TNUMBER)
  {
    return -1;
  }
  *result = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return 0;
}

int luac_get_boolean(lua_State *L, const char *var_name, int *result)
{
  int var_type = lua_getglobal(L, var_name);
  if (var_type != LUA_TBOOLEAN)
  {
    return -1;
  }
  *result = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return 0;
}

int luac_get_lstring(lua_State *L, const char *var_name, const char** result,size_t* result_len)
{
  int var_type = lua_getglobal(L, var_name);
  if (var_type != LUA_TSTRING)
  {
    return -1;
  }
  
  *result = lua_tolstring(L, -1, result_len);
  lua_pop(L, 1);
  return 0;
}

int luac_get_string(lua_State *L, const char *var_name, const char** result){
  return luac_get_lstring(L,var_name,result,NULL);
}

void luac_set_number(lua_State *L, const char *var_name, lua_Number value){
  lua_pushnumber(L,value);
  lua_setglobal(L,var_name);
}
void luac_set_boolean(lua_State *L, const char *var_name, int value){
  lua_pushboolean(L,value);
  lua_setglobal(L,var_name);
}
void luac_set_string(lua_State *L, const char *var_name, const char* value){
  lua_pushstring(L,value);
  lua_setglobal(L,var_name);
}
void luac_set_lstring(lua_State *L, const char *var_name, const char* value,size_t value_len){
  lua_pushlstring(L,value,value_len);
  lua_setglobal(L,var_name);
}