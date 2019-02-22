//
//  main.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include <iostream>
#include <string>
#include <cstring>

#include "lua_binary_table.h"

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

const char *USAGE = "USAGE:\n"
"LuaBinaryTable [-c][-d][-o output] file\n"
"-c         compress mode\n"
"-d         decompress mode\n"
"-o output\n"
"           output file name\n"
;

int usage()
{
    std::cout << USAGE << std::endl;
    return 0;
}

int compressFile(lua_State *L, const std::string &srcFile, const std::string &dstFile)
{
    if(0 != luaL_loadfile(L, srcFile.c_str()))
    {
        std::cout << "Failded to load lua file:" << srcFile << std::endl;
        return 0;
    }
    
    lua_pushlstring(L, srcFile.c_str(), srcFile.length());
    if(0 != lua_pcall(L, 1, 1, 0))
    {
        std::cout << "Failed to excute lua file:" << srcFile << "\n"
            << "error:" << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return 0;
    }
    
    BinaryData *data = writeBinaryTable(L, -1);
    if(data == 0)
    {
        std::cout << "Failed to compress file:" << srcFile << std::endl;
        return 0;
    }
    lua_pop(L, 1);
    
    FILE *file = fopen(dstFile.c_str(), "wb");
    if(file == 0)
    {
        freeBinaryData(data);
        std::cout << "Failed to create file:" << dstFile << std::endl;
        return 0;
    }
    
    fwrite(data->data, data->length, 1, file);
    
    fclose(file);
    freeBinaryData(data);
    return 0;
}

int decompressFile(lua_State *L, const std::string &srcFile, const std::string &dstFile)
{
    lua_getglobal(L, "BinaryTable");
    lua_getfield(L, -1, "dumpToFile");
    lua_getfield(L, -2, "parseFromFile");
    lua_pushlstring(L, srcFile.c_str(), srcFile.size());
    if(0 != lua_pcall(L, 1, 1, 0))
    {
        std::cout << "Failed to excute function 'parseFromFile':" << srcFile << "\n"
            << "error:" << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return 0;
    }
    
    lua_pushlstring(L, dstFile.c_str(), dstFile.size());
    if(0 != lua_pcall(L, 2, 0, 0))
    {
        std::cout << "Failed to excute function 'dumpToFile':" << srcFile << "\n"
        << "error:" << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return 0;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        return usage();
    }
    
    std::string srcFile;
    std::string dstFile;
    
    bool compress = true;
    
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "-c") == 0)
        {
            compress = true;
        }
        else if(strcmp(argv[i], "-d") == 0)
        {
            compress = false;
        }
        else if(strcmp(argv[i], "-o") == 0)
        {
            if(i + 1 >= argc)
            {
                std::cout << "on output file." << std::endl;
                return usage();
            }
            dstFile = argv[i + 1];
            ++i;
        }
        else if(srcFile.empty())
        {
            srcFile = argv[i];
        }
        else
        {
            std::cout << "invalid argument: " << argv[i]  << std::endl;
            return usage();
        }
    }
    
    if(srcFile.empty())
    {
        std::cout << "no input file." << std::endl;
        return usage();
    }
    
    if(dstFile.empty())
    {
        dstFile = srcFile;
        
        size_t pos = dstFile.find_last_of("/\\.");
        if(pos < dstFile.size() && dstFile[pos] == '.')
        {
            dstFile.erase(pos);
        }
        dstFile += ".dat";
    }
    
    lua_State *L = lua_open();
    luaL_openlibs(L);
    lua_pushcfunction(L, luaopen_BinaryTable);
    lua_pcall(L, 0, 0, 0);
    
    int ret = 0;
    if(compress)
    {
        ret = compressFile(L, srcFile, dstFile);
    }
    else
    {
        ret = decompressFile(L, srcFile, dstFile);
    }
    
    lua_close(L);
    return ret;
}
