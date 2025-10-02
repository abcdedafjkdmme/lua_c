#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdbool.h>


#define LUAC_COUNTOF(arr)  (sizeof(arr) / sizeof((arr)[0]))

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
  const char *errstr = lua_tostring(L, -1);
  const char *errcode_str = luac_errcode_to_str(errcode);
  printf("[LUA_C] ERROR: %s DESCRIPTION: %s ERRRCODE: %s \n", errmsg, errstr, errcode_str);
}

int luac_get_number(lua_State *L, const char *var_name, lua_Number *result)
{
  int var_type = lua_getglobal(L, var_name);
  if (var_type != LUA_TNUMBER)
  {
    lua_pop(L, 1);
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
    lua_pop(L, 1);
    return -1;
  }
  *result = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return 0;
}

int luac_get_lstring(lua_State *L, const char *var_name, const char **result, size_t *result_len)
{
  int var_type = lua_getglobal(L, var_name);
  if (var_type != LUA_TSTRING)
  {
    lua_pop(L, 1);
    return -1;
  }

  *result = lua_tolstring(L, -1, result_len);
  lua_pop(L, 1);
  return 0;
}

int luac_get_string(lua_State *L, const char *var_name, const char **result)
{
  return luac_get_lstring(L, var_name, result, NULL);
}

void luac_set_number(lua_State *L, const char *var_name, lua_Number value)
{
  lua_pushnumber(L, value);
  lua_setglobal(L, var_name);
}

void luac_set_boolean(lua_State *L, const char *var_name, int value)
{
  lua_pushboolean(L, value);
  lua_setglobal(L, var_name);
}

void luac_set_string(lua_State *L, const char *var_name, const char *value)
{
  lua_pushstring(L, value);
  lua_setglobal(L, var_name);
}

void luac_set_lstring(lua_State *L, const char *var_name, const char *value, size_t value_len)
{
  lua_pushlstring(L, value, value_len);
  lua_setglobal(L, var_name);
}
void luac_set_function(lua_State* L,const char* func_name, lua_CFunction func){
  lua_pushcfunction(L,func);
  lua_setglobal(L,func_name);
}

typedef enum
{
  OK,
  FUNC_DOESNT_EXIST,
  INVALID_PARAM_TYPE,
  INVALID_RETURN_TYPE,
  PCALL_ERR,
  RETURN_TYPE_MISMATCH,
} luac_call_function_errcode;

/**
 * @param return_type one of LUA_TNUMBER, LUA_TBOOLEAN, LUA_TSTRING
 * @warning only supports 1 return value from lua function
 */
luac_call_function_errcode luac_call_function(lua_State *L, const char *func_name, int return_type, void *return_val, int params_type[], int num_params, ...)
{

  // first ask lua to push function on stack
  int var_type = lua_getglobal(L, func_name);
  if (var_type != LUA_TFUNCTION)
  {
    lua_pop(L, 1);
    return FUNC_DOESNT_EXIST;
  }

  va_list args;
  va_start(args, num_params);
  for (int i = 0; i < num_params; i++)
  {
    int param_type = params_type[i];
    if (param_type == LUA_TNUMBER)
    {
      lua_Number param = va_arg(args, lua_Number);
      lua_pushnumber(L, param);
    }
    else if (param_type == LUA_TBOOLEAN)
    {
      int param = va_arg(args, int);
      lua_pushboolean(L, param);
    }
    else if (param_type == LUA_TSTRING)
    {
      const char *param = va_arg(args, char *);
      lua_pushstring(L, param);
    }
    else
    {
      va_end(args);
      // since (i) number of params were pushed to lua stack before reaching this format error, (i) number of elements are popped from lua stack to cleanup
      lua_pop(L, i);
      // then pop the function
      lua_pop(L,1);
      return INVALID_PARAM_TYPE;
    }
  }
  va_end(args);

  int num_return_values = 1;
  int errcode = lua_pcall(L, num_params, num_return_values, 0);
  if (errcode != LUA_OK)
  {
    return PCALL_ERR;
  }

  int actual_return_type = lua_type(L, -1);
  if (actual_return_type != return_type)
  {
    // first pop the return value
    lua_pop(L, 1);
    // then pop the function
    lua_pop(L, 1);
    return RETURN_TYPE_MISMATCH;
  }

  if (actual_return_type == LUA_TNUMBER)
  {
    lua_Number ret = lua_tonumber(L, -1);
    *(lua_Number *)return_val = ret;
  }
  else if (actual_return_type == LUA_TBOOLEAN)
  {
    int ret = lua_toboolean(L, -1);
    *(int *)return_val = ret;
  }
  else if (actual_return_type == LUA_TNUMBER)
  {
    const char *ret = lua_tostring(L, -1);
    *(const char **)return_val = ret;
  }
  else
  {
    // first pop the return value
    lua_pop(L, 1);
    // then pop the function
    lua_pop(L, 1);
    return INVALID_RETURN_TYPE;
  }
  lua_pop(L, 1);
  return OK;
}