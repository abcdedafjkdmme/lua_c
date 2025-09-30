#include "luac.h"



int main(int argc, char **argv)
{
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        printf("[LUA_C] ERROR: lua state is NULL");
        return -1;
    }
    luaL_openlibs(L);

    int errcode = luaL_loadfile(L, "scripts/main.lua");
    if (errcode != LUA_OK)
    {
        luac_log_error(L, errcode, "luaL_loadfile");
        return -1;
    }

    luac_set_number(L,"C_num",123);
    luac_set_boolean(L,"C_bool",1);
    luac_set_string(L,"C_str","c string hello");

    errcode = lua_pcall(L, 0, 0, 0);
    if (errcode != LUA_OK)
    {
        luac_log_error(L, errcode, "luaL_pcall");
        return -1;
    }

    lua_Number lua_var_num = -1;
    errcode = luac_get_number(L,"Lua_num",&lua_var_num);
    assert(errcode == 0);
    printf("[LUA_C] Lua_num is %lf \n",lua_var_num);

    int lua_var_bool = -1;
    errcode = luac_get_boolean(L,"Lua_bool",&lua_var_bool);
    assert(errcode == 0);
    printf("[LUA_C] Lua_bool is %s \n",lua_var_bool ? "true":"false");

   
    char* lua_var_str = NULL;
    errcode = luac_get_string(L,"Lua_str",&lua_var_str);
    assert(errcode == 0);
    printf("[LUA_C] Lua_str is %s \n",lua_var_str);


    // call lua funciton

    
    lua_close(L);
    return 0;
}