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

static int LuaBinaryTable_parse(lua_State *L)
{
    size_t length;
    const char *str = luaL_checklstring(L, 1, &length);
    
    return parseBinaryTable(L, str, length);
}

static int LuaBinaryTable_write(lua_State *L)
{
    if(lua_gettop(L) != 1)
    {
        luaL_error(L, "1 argument expected, but %d was given", lua_gettop(L));
    }
    
    BinaryData *ret = writeBinaryTable(L, 1);
    if(!ret)
    {
        return 0;
    }
    
    lua_pushlstring(L, ret->data, ret->length);
    freeBinaryData(ret);
    return 1;
}

static int LuaBinaryTable_parseFromFile(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    FILE *fp = fopen(filename, "rb");
    if(!fp)
    {
        lua_pushnil(L);
        lua_pushstring(L, "Failed open input file.");
        return 2;
    }
    
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* buffer = new char[length];
    fread(buffer, length, 1, fp);
    
    int ret = parseBinaryTable(L, buffer, length);
    fclose(fp);
    delete [] buffer;
    
    if(ret == 0)
    {
        lua_pushnil(L);
        lua_pushstring(L, "Failed parse data");
        return 2;
    }
    
    return 1;
}

static int LuaBinaryTable_writeToFile(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);
    
    BinaryData *ret = writeBinaryTable(L, 2);
    if(!ret)
    {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "Failed to serialize data.");
        return 2;
    }
    
    FILE *fp = fopen(filename, "wb");
    if(!fp)
    {
        freeBinaryData(ret);
        
        lua_pushboolean(L, 0);
        lua_pushstring(L, "Failed open input file.");
        return 2;
    }
    
    fwrite(ret->data, ret->length, 1, fp);
    fclose(fp);
    freeBinaryData(ret);
    
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg binaryTableLib[] = {
    {"parse", LuaBinaryTable_parse},
    {"write", LuaBinaryTable_write},
    {"parseFromFile", LuaBinaryTable_parseFromFile},
    {"writeToFile", LuaBinaryTable_writeToFile},
    {0, 0},
};

extern "C" int luaopen_BinaryTable(lua_State *L)
{
    luaL_register(L, "BinaryTable", binaryTableLib);
    return 1;
}

