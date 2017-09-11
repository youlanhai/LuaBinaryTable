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

#include <sys/time.h>

#include "lua_binary_table.h"

#include "lua_binary_reader.h"
#include "lua_binary_writter.h"

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

#define DO_TEST(EXP) if(!(EXP)){std::cout << "Test Failed: " << #EXP << std::endl; abort(); }

static int64_t getTimeMs()
{
    timeval val;
    gettimeofday(&val, 0);
    
    return int64_t(val.tv_sec) * 1000 + val.tv_usec / 1000;
}

static bool readFile(std::string &data, const std::string &path)
{
    FILE *fp = fopen(path.c_str(), "rb");
    if(fp == 0)
    {
        std::cout << "Failed to open file:" << path << std::endl;
        return false;
    }
    
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    data.resize(length);
    
    if(length > 0)
    {
        fread(&data[0], length, 1, fp);
    }
    
    fclose(fp);
    return true;
}

static bool saveFile(const char *path, const char *data, size_t length)
{
    FILE *fp = fopen(path, "wb");
    if(fp == 0)
    {
        std::cout << "Failed to open file:" << path << std::endl;
        return false;
    }
    
    fwrite(data, length, 1, fp);
    fclose(fp);
    return true;
}


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
    
    lua_pushnumber(L, std::numeric_limits<int16_t>::max());
    lua_setfield(L, -2, "int16");
    
    lua_pushnumber(L, std::numeric_limits<int16_t>::min());
    lua_setfield(L, -2, "-int16");
    
    lua_pushnumber(L, std::numeric_limits<int>::max());
    lua_setfield(L, -2, "int32");
    
    lua_pushnumber(L, std::numeric_limits<int>::min());
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
#if 0
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
#else
    saveFile("test.dat", data->data, data->length);
#endif
    
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
    DO_TEST(lua_tonumber(L, -1) == std::numeric_limits<int16_t>::max());
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "-int16");
    DO_TEST(lua_tonumber(L, -1) == std::numeric_limits<int16_t>::min());
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "int32");
    DO_TEST(lua_tonumber(L, -1) == std::numeric_limits<int>::max());
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "-int32");
    DO_TEST(lua_tonumber(L, -1) == std::numeric_limits<int>::min());
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

void testEfficiency(lua_State *L, const char *binaryTablePath, const char *luaBinaryPath)
{
    std::cout << "Test Efficiency: " << binaryTablePath << " vs " << luaBinaryPath << std::endl;

    std::string content;
    const int nTestCase = 100;
    int64_t start, end;
    
    // load binary table
    if(!readFile(content, binaryTablePath))
    {
        return;
    }
    std::cout << "file size:" << content.size() << std::endl;
    
    // test parse binary table
    start = getTimeMs();
    for(int i = 0; i < nTestCase; ++i)
    {
        assert(1 == parseBinaryTable(L, content.c_str(), content.length()));
        lua_pop(L, 1);
    }
    end = getTimeMs();
    std::cout << "parse binary use time: " << end - start << std::endl;
    
    
    
    // load lua file
    if(!readFile(content, luaBinaryPath))
    {
        return;
    }
    std::cout << "file size:" << content.size() << std::endl;
    
    lua_getfield(L, LUA_GLOBALSINDEX, "loadstring");
    assert(lua_isfunction(L, -1));
    lua_pushlstring(L, content.data(), content.size());
    
    // test load lua table
    start = getTimeMs();
    for(int i = 0; i < nTestCase; ++i)
    {
        lua_pushvalue(L, -2); // loadstring
        lua_pushvalue(L, -2); // content
        assert(0 == lua_pcall(L, 1, 1, 0)); //loadstring(content)
        assert(lua_isfunction(L, -1) && "Invalid input file");
        
        assert(0 == lua_pcall(L, 0, 1, 0)); // execute the chunk
        assert(lua_istable(L, -1) && "Invalid input file");
        
        lua_pop(L, 1);
    }
    end = getTimeMs();
    std::cout << "lua dostring use time:" << end - start << std::endl;
}

int main(int argc, char **argv)
{
    testReaderWriter();
    
    lua_State *L = lua_open();
    luaL_openlibs(L);
    
    lua_pushcfunction(L, luaopen_BinaryTable);
    lua_call(L, 0, 0);
    
    testBinaryTable(L);
    if(argc >= 3)
    {
        testEfficiency(L, argv[1], argv[2]);
    }
    else
    {
        std::cout << "usage : ltest path/to/binary/table path/to/lua/binary" << std::endl;
    }
    
    lua_close(L);
    return 0;
}
