//
//  main.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <climits>
#include <cassert>

#include "lua_binary_table.h"

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

void testBinaryTable(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "Hello world!");
    lua_setfield(L, -2, "string");
    
    lua_pushnumber(L, 127);
    lua_setfield(L, -2, "int8");
    
    lua_pushnumber(L, 123456.789);
    lua_setfield(L, -2, "float");
    
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "true");
    
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "false");
    
    BinaryData *data = writeBinaryTable(L, 1);
    assert(data);
    
    std::cout << "binary: ";
    for(int i = 0; i < data->length; ++i)
    {
        std::cout << (int)data->data[i] << ", ";
        if(i % 8 == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    
    freeBinaryData(data);
}

int main(int argc, char **argv)
{
    lua_State *L = lua_open();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, luaopen_BinaryTable);
    lua_call(L, 0, 0);
    
    testBinaryTable(L);
    
    lua_close(L);
    return 0;
}
