//
//  lua_binary_writer.h
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_writer_h
#define lua_binary_writer_h

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
        
        void writeChar(char ch)
        {
            ensure(1);
            *p_++ = ch;
        }
        
        void writeBytes(const char *data, size_t length)
        {
            ensure(length);
            memcpy(p_, data, length);
            p_ += length;
        }
        
        void writeString(const char *str)
        {
            writeBytes(str, strlen(str));
        }
        
        void writeType(Type type)
        {
            writeNumber((uint8_t)type);
        }
        
        void writeIndent(int n)
        {
            if(n > 0)
            {
                ensure(n);
                for(int i = 0; i < n; ++i)
                {
                    *p_++ = '\t';
                }
            }
        }
        
    private:
        char*       data_;
        char*       p_;
        char*       end_;
    };
}

#endif /* lua_binary_writer_h */
