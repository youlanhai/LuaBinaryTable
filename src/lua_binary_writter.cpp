//
//  lua_binary_writter.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//
#include "lua_binary_types.h"
#include "lua_binary_table.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cassert>

namespace
{
    using namespace LuaBinaryTable;
    
    class BinaryWriter
    {
    public:
        BinaryWriter()
        : data_(0)
        , p_(0)
        , end_(0)
        {
            
        }
        
        void destroy()
        {
            free(data_);
            data_ = p_ = end_ = 0;
        }
        
        bool full() const { return p_ >= end_; }
        size_t capacity() const { return end_ - data_; }
        size_t size() const { return p_ - data_; }
        const char *data() const { return data_; }
        char* data() { return data_; }
        
        void reserve(size_t capacity)
        {
            if(capacity > this->capacity())
            {
                size_t size = this->size();
                
                if(data_ != nullptr)
                {
                    data_ = (char*)std::realloc(data_, capacity);
                }
                else
                {
                    data_ = (char*)std::malloc(capacity);
                }
                
                p_ = data_ + size;
                end_ = data_ + capacity;
            }
        }
        
        void ensure(size_t size)
        {
            if(p_ + size > end_)
            {
                size_t newCapacity = std::max(this->size() + size, capacity() << 1);
                newCapacity = std::max((size_t)8, newCapacity);
                reserve(newCapacity);
            }
        }
        
        template<typename T>
        void writeNumber(T v)
        {
            ensure(sizeof(v));
            memcpy(p_, &v, sizeof(T));
            p_ += sizeof(T);
        }
        
        void writeBytes(const char *data, size_t length)
        {
            ensure(length);
            memcpy(p_, data_, length);
            p_ += length;
        }
        
        void writeType(Type type)
        {
            writeNumber((uint8_t)type);
        }
        
    private:
        char*       data_;
        char*       p_;
        char*       end_;
    };
    
    void writeInteger(BinaryWriter &stream, int64_t value)
    {
        if(value < 0)
        {
            if(value >= std::numeric_limits<int8_t>::min())
            {
                stream.writeType(T_INT8);
                stream.writeNumber((int8_t)value);
            }
            else if(value >= std::numeric_limits<int16_t>::min())
            {
                stream.writeType(T_INT16);
                stream.writeNumber((int16_t)value);
            }
            else if(value >= std::numeric_limits<int32_t>::min())
            {
                stream.writeType(T_INT32);
                stream.writeNumber((int32_t)value);
            }
            else
            {
                stream.writeType(T_INT64);
                stream.writeNumber(value);
            }
        }
        else
        {
            if(value == 0)
            {
                stream.writeType(T_ZERO);
            }
            else if(value == 1)
            {
                stream.writeType(T_ONE);
            }
            else if(value <= std::numeric_limits<int8_t>::max())
            {
                stream.writeType(T_INT8);
                stream.writeNumber((int8_t)value);
            }
            else if(value <= std::numeric_limits<int16_t>::max())
            {
                stream.writeType(T_INT16);
                stream.writeNumber((int16_t)value);
            }
            else if(value <= std::numeric_limits<int32_t>::max())
            {
                stream.writeType(T_INT32);
                stream.writeNumber((int32_t)value);
            }
            else
            {
                stream.writeType(T_INT64);
                stream.writeNumber(value);
            }
        }
    }
    
    class StringTable
    {
    public:
        StringTable(lua_State *L)
        : size_(0)
        {
            handle_ = lua_gettop(L);
            lua_newtable(L);
            
            index2string_ = lua_gettop(L);
            lua_newtable(L);
        }
        
        void destroy(lua_State *L)
        {
            lua_remove(L, index2string_);
            lua_remove(L, handle_);
            
            index2string_ = 0;
            handle_ = 0;
        }
        
        int storeString(lua_State *L)
        {
            lua_rawget(L, handle_);
            if(lua_isnil(L, -1))
            {
                lua_pop(L, 1); // pop nil
                ++size_;
                
                lua_pushvalue(L, -1);
                lua_pushnumber(L, size_);
                lua_rawset(L, handle_);
                
                lua_pushvalue(L, -1);
                lua_rawseti(L, index2string_, size_);
                
                return size_;
            }
            else
            {
                int index = lua_tonumber(L, -1);
                lua_pop(L, 1);
                return index;
            }
        }
        
        void write(lua_State *L, BinaryWriter &stream)
        {
            stream.writeNumber((TStringIndex)size_);
            for(int i = 1; i <= size_; ++i)
            {
                lua_rawgeti(L, index2string_, i);
                
                size_t length;
                const char *str = lua_tolstring(L, -1, &length);
                stream.writeNumber((TStringLength)length);
                stream.writeBytes(str, length);
                
                lua_pop(L, 1);
            }
        }
        
    private:
        int     handle_;
        int     index2string_;
        int     size_;
    };
    
    void writeFloat(BinaryWriter &stream, double value)
    {
        if(value < 0)
        {
            if(value >= std::numeric_limits<float>::min())
            {
                stream.writeType(T_FLOAT);
                stream.writeNumber((float)value);
            }
            else
            {
                stream.writeType(T_DOUBLE);
                stream.writeNumber(value);
            }
        }
        else
        {
            if(value <= std::numeric_limits<float>::max())
            {
                stream.writeType(T_FLOAT);
                stream.writeNumber((float)value);
            }
            else
            {
                stream.writeType(T_DOUBLE);
                stream.writeNumber(value);
            }
        }
    }
    
    int countTableLength(lua_State *L)
    {
        int n = 0;
        lua_pushnil(L);
        while(lua_next(L, -2) != 0)
        {
            lua_pop(L, 1); // pop the value.
            ++n;
        }
        lua_pop(L, 1); // pop the last key.
        return n;
    }
    
    bool writeValue(lua_State *L, StringTable &strTable, BinaryWriter &stream)
    {
        int type = lua_type(L, -1);
        switch (type)
        {
            case LUA_TNIL:
                stream.writeType(T_NIL);
                break;
                
            case LUA_TBOOLEAN:
                if(lua_toboolean(L, -1))
                {
                    stream.writeType(T_TRUE);
                }
                else
                {
                    stream.writeType(T_FALSE);
                }
                break;
                
            case LUA_TNUMBER:
            {
                lua_Number v = lua_tonumber(L, -1);
                lua_Number nearV = std::round(v);
                
                if(v == nearV)
                {
                    writeInteger(stream, (int64_t)nearV);
                }
                else
                {
                    writeFloat(stream, v);
                }
                break;
            }
            case LUA_TSTRING:
            {
                if(lua_strlen(L, -1) == 0)
                {
                    stream.writeType(T_EMPTY_STRING);
                }
                else
                {
                    int index = strTable.storeString(L);
                    stream.writeType(T_STRING);
                    stream.writeNumber((TStringIndex)index);
                }
                break;
            }
                
            case LUA_TTABLE:
            {
                int length = countTableLength(L);
                if(length == 0)
                {
                    stream.writeType(T_EMPTY_TABLE);
                }
                else if(length == (int)lua_objlen(L, -1))
                {
                    // the table is an array.
                    stream.writeType(T_ARRAY);
                    stream.writeNumber((TTableLength)length);
                    for(int i = 1; i <= length; ++i)
                    {
                        lua_rawgeti(L, -1, i);
                        if(!writeValue(L, strTable, stream))
                        {
                            return false;
                        }
                        lua_pop(L, 1);
                    }
                    break;
                }
                else
                {
                    // the table is a hash table.
                    stream.writeType(T_TABLE);
                    stream.writeNumber((TTableLength)length);
                    
                    lua_pushnil(L);
                    while(lua_next(L, -2) != 0)
                    {
                        lua_pushvalue(L, -2); // duplicate the key at top.
                        if(!writeValue(L, strTable, stream))
                        {
                            return false;
                        }
                        lua_pop(L, 1); // pop the duplicated key
                        if(!writeValue(L, strTable, stream))
                        {
                            return false;
                        }
                        lua_pop(L, 1); // pop the value
                    }
                    lua_pop(L, 1); // pop the last key
                }
                break;
            }
                
            default:
                return false;
        }
        return true;
    }
}

extern "C" BinaryData* writeTable(lua_State*L, size_t &length)
{
    int top = lua_gettop(L);
    
    StringTable strTable(L);
    BinaryWriter dataWriter;
    if(!writeValue(L, strTable, dataWriter))
    {
        lua_settop(L, top);
        return 0;
    }
    
    BinaryWriter stream;
    strTable.write(L, stream);
    strTable.destroy(L);
    assert(lua_gettop(L) == top);
    
    stream.writeBytes(dataWriter.data(), dataWriter.size());
    dataWriter.destroy();
    
    BinaryData *ret = new BinaryData();
    ret->data = stream.data();
    ret->length = stream.size();
    return 0;
}

extern "C" void freeBinaryData(BinaryData *p)
{
    free(p->data);
    delete p;
}
