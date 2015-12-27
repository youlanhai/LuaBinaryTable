//
//  lua_binary_reader.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "lua_binary_types.h"

extern "C"
{
#include "lua.h"
}

#include <cstdint>
#include <cassert>
#include <cstring>

namespace
{
    using namespace LuaBinaryTable;
    
    class BinaryReader
    {
    public:
        BinaryReader(const char *data, size_t length)
        : data_(data)
        , p_(data)
        , end_(data + length)
        {
            
        }
        
        template<typename T>
        T readNumber()
        {
            T v = T(0);
            if(p_ + sizeof(T) <= end_)
            {
                memcpy(&v, p_, sizeof(T));
            }
            p_ += sizeof(T);
            return v;
        }
        
        const char* readBytes(size_t &length)
        {
            if(p_ + length <= end_)
            {
                const char *ret = p_;
                p_ += length;
                return ret;
            }
            else
            {
                p_ = end_;
                return 0;
            }
        }
        
        bool empty() const { return p_ >= end_; }
        
        
    private:
        const char*     data_;
        const char*     p_;
        const char*     end_;
    };
   
    
    class StringTable
    {
    public:
        StringTable()
        : handle_(0)
        , size_(0)
        {}
        
        bool getString(lua_State *L, int strIndex)
        {
            if(strIndex >=0 && strIndex < size_)
            {
                lua_rawgeti(L, handle_, strIndex);
                return true;
            }
            return false;
        }
        
        bool parse(lua_State *L, BinaryReader &reader)
        {
            handle_ = lua_gettop(L);
            size_ = (int)reader.readNumber<TStringIndex>();
            lua_createtable(L, size_, 0);
            
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
        
        int index() const { return handle_; }
        int size() const { return size_; }
        
    private:
        int      handle_;
        int      size_;
    };
    
    bool parserValue(lua_State *L, StringTable &strTable, BinaryReader &reader)
    {
        int type = (int)reader.readNumber<uint8_t>();
        switch (type)
        {
            case T_NIL:
                lua_pushnil(L);
                break;
                
            case T_TRUE:
                lua_pushboolean(L, 1);
                break;
                
            case T_FALSE:
                lua_pushboolean(L, 0);
                break;
                
            case T_ZERO:
                lua_pushnumber(L, 0);
                break;
                
            case T_ONE:
                lua_pushnumber(L, 1);
                break;
                
            case T_EMPTY_STRING:
                lua_pushlstring(L, "", 0);
                break;
                
            case T_EMPTY_TABLE:
                lua_newthread(L);
                break;
                
            case T_INT8:
                lua_pushnumber(L, (lua_Number)reader.readNumber<int8_t>());
                break;
                
            case T_INT16:
                lua_pushnumber(L, (lua_Number)reader.readNumber<int16_t>());
                break;
                
            case T_INT32:
                lua_pushnumber(L, (lua_Number)reader.readNumber<int32_t>());
                break;
                
            case T_INT64:
                lua_pushnumber(L, (lua_Number)reader.readNumber<int64_t>());
                break;
                
            case T_FLOAT:
                lua_pushnumber(L, (lua_Number)reader.readNumber<float>());
                break;
                
            case T_DOUBLE:
                lua_pushnumber(L, (lua_Number)reader.readNumber<double>());
                break;
                
            case T_STRING:
            {
                int index = (int)reader.readNumber<TStringIndex>();
                if(!strTable.getString(L, index))
                {
                    return false;
                }
                break;
            }
                
            case T_ARRAY:
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
                break;
            }
                
            case T_TABLE:
            {
                int size = (int)reader.readNumber<TTableLength>();
                lua_createtable(L, 0, size);
                for(int i = 0; i < size; ++i)
                {
                    if(!parserValue(L, strTable, reader) &&
                       !parserValue(L, strTable, reader))
                    {
                        return false;
                    }
                    lua_rawset(L, -3);
                }
                break;
            }
                
            case T_EOF:
                return false;
                
            default:
                assert(false && "shouldn't reach here.");
                return false;
        }
        return true;
    }
}

extern "C" bool parseTable(lua_State *L, const char *data, size_t length)
{
    StringTable strTable;
    BinaryReader reader(data, length);
    
    if(!strTable.parse(L, reader) ||
       !parserValue(L, strTable, reader))
    {
        lua_settop(L, strTable.index());
        return false;
    }
    
    lua_remove(L, strTable.index());
    return true;
}