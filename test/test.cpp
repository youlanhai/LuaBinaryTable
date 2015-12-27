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

#include "lua_binary_reader.h"
#include "lua_binary_writter.h"

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

#define DO_TEST(EXP) if(!(EXP)){std::cout << "Test Failed: " << #EXP << std::endl; abort(); }

void testReaderWriter()
{
    std::cout << "testReaderWriter start..." << std::endl;
    
    LuaBinaryTable::BinaryWriter writer;
    writer.writeType(LuaBinaryTable::T_TRUE);
    writer.writeNumber<int8_t>(127);
    writer.writeNumber<int8_t>(-128);
    writer.writeNumber<int16_t>(32767);
    writer.writeNumber<int16_t>(-32768);
    writer.writeNumber<int32_t>(0x7fffffff);
    writer.writeNumber<int32_t>(0xffffffff);
    writer.writeNumber<float>(3.14f);
    writer.writeNumber<double>(3.14159);
    writer.writeBytes("Hello world", 5);
    
    LuaBinaryTable::BinaryReader reader(writer.data(), writer.size());
    
    DO_TEST(reader.readNumber<uint8_t>() == LuaBinaryTable::T_TRUE);
    DO_TEST(reader.readNumber<int8_t>() == 127);
    DO_TEST(reader.readNumber<int8_t>() == -128);
    DO_TEST(reader.readNumber<int16_t>() == 32767);
    DO_TEST(reader.readNumber<int16_t>() == -32768);
    DO_TEST(reader.readNumber<int32_t>() == 0x7fffffff);
    DO_TEST(reader.readNumber<int32_t>() == 0xffffffff);
    DO_TEST(reader.readNumber<float>() == 3.14f);
    DO_TEST(reader.readNumber<double>() == 3.14159);
    DO_TEST(strncmp(reader.readBytes(5), "Hello", 5) == 0);
    DO_TEST(reader.empty());
    
    writer.destroy();
    
    std::cout << "testReaderWriter end." << std::endl;
}

void testBinaryTable(lua_State *L)
{
    int top = lua_gettop(L);
    
    lua_newtable(L); // create root table
    lua_pushstring(L, "Hello world!");
    lua_setfield(L, -2, "string");
    
    lua_pushstring(L, "");
    lua_setfield(L, -2, "empty string");
    
    lua_newtable(L);
    lua_setfield(L, -2, "empty table");
    
    lua_pushnumber(L, 127);
    lua_setfield(L, -2, "int8");
    
    lua_pushnumber(L, -128);
    lua_setfield(L, -2, "-int8");
    
    lua_pushnumber(L, 0x7fff);
    lua_setfield(L, -2, "int16");
    
    lua_pushnumber(L, 0xffff);
    lua_setfield(L, -2, "-int16");
    
    lua_pushnumber(L, 0x7fffffff);
    lua_setfield(L, -2, "int32");
    
    lua_pushnumber(L, 0xffffffff);
    lua_setfield(L, -2, "-int32");
    
    lua_pushnumber(L, 123456.789);
    lua_setfield(L, -2, "float");
    
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "true");
    
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "false");
    
    lua_newtable(L);
    lua_pushnumber(L, 12);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, 34);
    lua_rawseti(L, -2, 2);
    lua_setfield(L, -2, "array");
    
    BinaryData *data = writeBinaryTable(L, 1);
    assert(data);
    lua_pop(L, 1); // pop root table
    
    std::cout << "binary size:" << data->length << std::endl;
    std::cout << "binary data:";
    for(int i = 0; i < data->length; ++i)
    {
        std::cout << (int)data->data[i] << ", ";
        if(i % 8 == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    
    DO_TEST(1 == parseBinaryTable(L, data->data, data->length));
    
    lua_getfield(L, -1, "string");
    DO_TEST(strcmp(lua_tostring(L, -1), "Hello world!") == 0);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "empty string");
    DO_TEST(lua_strlen(L, -1) == 0);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "empty table");
    DO_TEST(lua_objlen(L, -1) == 0);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "int8");
    DO_TEST(lua_tonumber(L, -1) == 127);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "-int8");
    DO_TEST(lua_tonumber(L, -1) == -128);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "int16");
    DO_TEST(lua_tonumber(L, -1) == 0x7fff);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "-int16");
    DO_TEST(lua_tonumber(L, -1) == 0xffff);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "int32");
    DO_TEST(lua_tonumber(L, -1) == 0x7fffffff);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "-int32");
    DO_TEST(lua_tonumber(L, -1) == 0xffffffff);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "float");
    lua_Number val = lua_tonumber(L, -1);
    DO_TEST(fabs(val - 123456.789) < 0.001);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "true");
    DO_TEST(lua_toboolean(L, -1) == true);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "false");
    DO_TEST(lua_toboolean(L, -1) == false);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "array");
    DO_TEST(lua_objlen(L, -1) == 2);
    lua_pop(L, 1);
    
    lua_pop(L, 1); // pop the parse result.
    DO_TEST(top == lua_gettop(L));
    
    freeBinaryData(data);
}

int main(int argc, char **argv)
{
    testReaderWriter();
    
    lua_State *L = lua_open();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, luaopen_BinaryTable);
    lua_call(L, 0, 0);
    
    testBinaryTable(L);
    
    lua_close(L);
    return 0;
}
