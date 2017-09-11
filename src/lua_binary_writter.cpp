//
//  lua_binary_writter.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "lua_binary_writter.h"
#include "lua_binary_table.h"

#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cassert>

namespace LuaBinaryTable
{
    BinaryWriter::BinaryWriter()
    : data_(0)
    , p_(0)
    , end_(0)
    {
        
    }
    
    void BinaryWriter::destroy()
    {
        free(data_);
        data_ = p_ = end_ = 0;
    }
    
    void BinaryWriter::reserve(size_t capacity)
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
    
    void BinaryWriter::ensure(size_t size)
    {
        if(p_ + size > end_)
        {
            size_t newCapacity = std::max(this->size() + size, capacity() << 1);
            newCapacity = std::max((size_t)8, newCapacity);
            reserve(newCapacity);
        }
    }
}

namespace
{
    using namespace LuaBinaryTable;
    
    
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
        , cacheSize_(0)
        {
            lua_newtable(L);
            handle_ = lua_gettop(L);
            
            lua_newtable(L);
            index2string_ = lua_gettop(L);
        }
        
        void destroy(lua_State *L)
        {
            lua_remove(L, index2string_);
            lua_remove(L, handle_);
            
            index2string_ = 0;
            handle_ = 0;
        }
        
        size_t storeString(lua_State *L, int idx)
        {
            assert(lua_type(L, idx) == LUA_TSTRING);
            
            lua_pushvalue(L, idx);
            lua_rawget(L, handle_);
            if(lua_isnil(L, -1))
            {
                lua_pop(L, 1); // pop nil
                ++size_;
                cacheSize_ += lua_strlen(L, idx);
                
                lua_pushvalue(L, idx);
                lua_pushinteger(L, size_);
                lua_rawset(L, handle_);
                
                lua_pushvalue(L, idx);
                lua_rawseti(L, index2string_, (int)size_);
                
                return size_;
            }
            else
            {
                size_t index = (size_t)lua_tointeger(L, -1);
                lua_pop(L, 1);
                return index;
            }
        }
        
        void write(lua_State *L, BinaryWriter &stream) const
        {
            stream.writeNumber((TStringPoolLength)size_);
            for(size_t i = 1; i <= size_; ++i)
            {
                lua_rawgeti(L, index2string_, (int)i);
                
                size_t length;
                const char *str = lua_tolstring(L, -1, &length);
                stream.writeNumber((TStringLength)length);
                stream.writeBytes(str, length);
                
                lua_pop(L, 1);
            }
        }
        
        size_t getCacheSize() const { return cacheSize_; }
        
    private:
        int     handle_;
        int     index2string_;
        size_t  size_;
        size_t  cacheSize_;
    };
    
    void writeFloat(BinaryWriter &stream, double value)
    {
        stream.writeType(T_DOUBLE);
        stream.writeNumber(value);
    }
    
    size_t countTableLength(lua_State *L, int index)
    {
        int idx = index > 0 ? index : index - 1;
        size_t n = 0;
        lua_pushnil(L);
        while(lua_next(L, idx) != 0)
        {
            lua_pop(L, 1); // pop the value.
            ++n;
        }
        return n;
    }
    
    void writeLength(BinaryWriter &stream, Type type0, size_t length)
    {
        if(length == 0)
        {
            stream.writeType(type0);
        }
        else if(length < std::numeric_limits<uint8_t>::min())
        {
            stream.writeType(Type(type0 + 1));
            stream.writeNumber((uint8_t)length);
        }
        else if(length < std::numeric_limits<uint16_t>::min())
        {
            stream.writeType(Type(type0 + 2));
            stream.writeNumber((uint16_t)length);
        }
        else
        {
            stream.writeType(Type(type0 + 3));
            stream.writeNumber((uint32_t)length);
        }
    }
    
    bool writeValue(lua_State *L, int index, StringTable &strTable, BinaryWriter &stream)
    {
        int type = lua_type(L, index);
        switch (type)
        {
            case LUA_TNIL:
            {
                stream.writeType(T_NIL);
                break;
            }
                
            case LUA_TBOOLEAN:
            {
                if(lua_toboolean(L, index))
                {
                    stream.writeType(T_TRUE);
                }
                else
                {
                    stream.writeType(T_FALSE);
                }
                break;
            }
                
            case LUA_TNUMBER:
            {
                lua_Number v = lua_tonumber(L, index);
                lua_Number nearV = round(v);
                
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
                if(lua_strlen(L, index) == 0)
                {
                    stream.writeType(T_STRING0);
                }
                else
                {
                    size_t strIndex = strTable.storeString(L, index);
                    assert(strIndex != 0);
                    writeLength(stream, T_STRING0, strIndex);
                }
                break;
            }
            case LUA_TTABLE:
            {
                size_t length = countTableLength(L, index);
                if(length == 0)
                {
                    stream.writeType(T_TABLE0);
                }
                else if(length == lua_objlen(L, index))
                {
                    // the table is an array.
                    writeLength(stream, T_ARRAY0, length);
                    for(size_t i = 1; i <= length; ++i)
                    {
                        lua_rawgeti(L, index, (int)i);
                        if(!writeValue(L, -1, strTable, stream))
                        {
                            return false;
                        }
                        lua_pop(L, 1);
                    }
                }
                else
                {
                    // the table is a hash table.
                    writeLength(stream, T_TABLE0, length);
                    
                    int idx = index > 0 ? index : index - 1;
                    lua_pushnil(L);
                    while(lua_next(L, idx) != 0)
                    {
                        if(!writeValue(L, -2, strTable, stream) ||
                           !writeValue(L, -1, strTable, stream))
                        {
                            return false;
                        }
                        lua_pop(L, 1); // pop the value
                    }
                }
                break;
            }
                
            default:
                return false;
        }
        return true;
    }
}

extern "C" BinaryData* writeBinaryTable(lua_State*L, int idx)
{
    int top = lua_gettop(L);
    if(idx < 0)
    {
        idx = top + idx + 1;
    }
    
    StringTable strTable(L);
    BinaryWriter dataWriter;
    
    if(!writeValue(L, idx, strTable, dataWriter))
    {
        lua_settop(L, top);
        
        strTable.destroy(L);
        dataWriter.destroy();
        return 0;
    }
    
    BinaryWriter stream;
    stream.reserve(strTable.getCacheSize() + dataWriter.size());
    
    strTable.write(L, stream);
    strTable.destroy(L);
    assert(lua_gettop(L) == top);
    
    stream.writeBytes(dataWriter.data(), dataWriter.size());
    dataWriter.destroy();
    
    BinaryData *ret = new BinaryData();
    ret->data = stream.data();
    ret->length = stream.size();
    return ret;
}

extern "C" void freeBinaryData(BinaryData *p)
{
    free(p->data);
    delete p;
}
