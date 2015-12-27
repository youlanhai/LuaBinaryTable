//
//  lua_binary_writter.h
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_writter_h
#define lua_binary_writter_h

#include "lua_binary_types.h"
#include <cstring>

namespace LuaBinaryTable
{
    class BinaryWriter
    {
    public:
        BinaryWriter();
        
        void reserve(size_t capacity);
        void ensure(size_t size);
        void destroy();
        
        bool full() const { return p_ >= end_; }
        size_t capacity() const { return end_ - data_; }
        size_t size() const { return p_ - data_; }
        
        const char *data() const { return data_; }
        char* data() { return data_; }
        
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
            memcpy(p_, data, length);
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
}

#endif /* lua_binary_writter_h */
