//
//  lua_binary_table.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "lua_binary_table.h"

extern "C"
{
#include "lauxlib.h"
}

static int luaParseBinaryTable(lua_State *L)
{
    size_t length;
    const char *str = luaL_checklstring(L, 1, &length);
    
    return parseBinaryTable(L, str, length);
}

static int luaWriteBinaryTable(lua_State *L)
{
    BinaryData *ret = writeBinaryTable(L, lua_gettop(L));
    if(!ret)
    {
        return 0;
    }
    lua_pushlstring(L, ret->data, ret->length);
    freeBinaryData(ret);
    return 1;
}

static const luaL_Reg binaryTableLib[] = {
    "parse", luaParseBinaryTable,
    "write", luaWriteBinaryTable,
};

extern "C" int luaopen_BinaryTable(lua_State *L)
{
    luaL_register(L, "BinaryTable", binaryTableLib);
    return 1;
}

