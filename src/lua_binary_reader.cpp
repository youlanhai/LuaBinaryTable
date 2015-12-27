//
//  lua_binary_reader.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "lua_binary_types.h"
#include "lua_binary_reader.h"

extern "C"
{
#include "lua.h"
}

#include <cassert>


namespace
{
    using namespace LuaBinaryTable;
    
    class StringTable
    {
    public:
        StringTable()
        : handle_(0)
        , size_(0)
        {}
        
        void destroy(lua_State *L)
        {
            if(handle_ != 0)
            {
                lua_remove(L, handle_);
                handle_ = 0;
            }
        }
        
        bool getString(lua_State *L, int strIndex)
        {
            if(strIndex >0 && strIndex <= size_)
            {
                lua_rawgeti(L, handle_, strIndex);
                return true;
            }
            return false;
        }
        
        bool parse(lua_State *L, BinaryReader &reader)
        {
            size_ = (int)reader.readNumber<TStringIndex>();
            lua_createtable(L, size_, 0);
            handle_ = lua_gettop(L);
            
            for(int i = 0; i < size_; ++i)
            {
                size_t length = reader.readNumber<TStringLength>();
                if(length == 0)
                {
                    return false;
                }
                
                const char *str = reader.readBytes(length);
                if(str == 0)
                {
                    return false;
                }
                
                lua_pushlstring(L, str, length);
                lua_rawseti(L, -2, i + 1);
            }
            return true;
        }
        
        int handle() const { return handle_; }
        int size() const { return size_; }
        
    private:
        int      handle_;
        int      size_;
    };
    
    typedef bool (*PARSE_FUN)(lua_State *L, StringTable &strTable, BinaryReader &reader);
    
    bool parserValue(lua_State *L, StringTable &strTable, BinaryReader &reader);

    bool _parseEOF(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        return false;
    }
    
    bool _parseNil(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnil(L);
        return true;
    }
    
    bool _parseTrue(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushboolean(L, 1);
        return true;
    }
    
    bool _parseFalse(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushboolean(L, 0);
        return true;
    }
    
    bool _parseOne(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, 1);
        return true;
    }
    
    bool _parseZero(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, 0);
        return true;
    }
    
    bool _parseEmptyString(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushlstring(L, "", 0);
        return true;
    }
    
    bool _parseEmptyTable(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_newtable(L);
        return true;
    }

    bool _parseInt8(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<int8_t>());
        return true;
    }
        
    bool _parseInt16(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<int16_t>());
        return true;
    }
        
    bool _parseInt32(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<int32_t>());
        return true;
    }
        
    bool _parseInt64(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<int64_t>());
        return true;
    }
        
    bool _parseFloat(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<float>());
        return true;
    }
        
    bool _parseDouble(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        lua_pushnumber(L, (lua_Number)reader.readNumber<double>());
        return true;
    }

    bool _parseString(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        int index = (int)reader.readNumber<TStringIndex>();
        if(!strTable.getString(L, index))
        {
            return false;
        }
        return true;
    }

    bool _parseArray(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        int size = (int)reader.readNumber<TTableLength>();
        lua_createtable(L, size, 0);
        for(int i = 0; i < size; ++i)
        {
            if(!parserValue(L, strTable, reader))
            {
                return false;
            }
            lua_rawseti(L, -2, i + 1);
        }
        return true;
    }

    bool _parseTable(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        int size = (int)reader.readNumber<TTableLength>();
        lua_createtable(L, 0, size);
        for(int i = 0; i < size; ++i)
        {
            if(!parserValue(L, strTable, reader) ||
               !parserValue(L, strTable, reader))
            {
                return false;
            }
            lua_rawset(L, -3);
        }
        return true;
    }
    
    static PARSE_FUN parsers[T_MAX] = {
        _parseEOF,
        _parseNil,
        _parseTrue,
        _parseFalse,
        _parseZero,
        _parseOne,
        _parseEmptyString,
        _parseEmptyTable,
        _parseInt8,
        _parseInt16,
        _parseInt32,
        _parseInt64,
        _parseFloat,
        _parseDouble,
        _parseString,
        _parseArray,
        _parseTable,
    };
    
    bool parserValue(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        int type = (int)reader.readNumber<uint8_t>();
        return parsers[type](L, strTable, reader);
    }
}

extern "C" int parseBinaryTable(lua_State *L, const char *data, size_t length)
{
    int top = lua_gettop(L);
    
    StringTable strTable;
    BinaryReader reader(data, length);
    
    if(!strTable.parse(L, reader) ||
       !parserValue(L, strTable, reader))
    {
        lua_settop(L, top);
        return 0;
    }
    
    strTable.destroy(L);
    
    int nReturns = lua_gettop(L) - top;
    return nReturns;
}
