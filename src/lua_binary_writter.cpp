//
//  lua_binary_writter.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//
#include "lua_binary_types.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace LuaBinaryTable
{
    
    class BinaryWriter
    {
    public:
        BinaryWriter()
        : data_(0)
        , p_(0)
        , end_(0)
        {
            
        }
        
        bool full() const { return p_ >= end_; }
        size_t capacity() const { return end_ - data_; }
        size_t size() const { return p_ - data_; }
        const char *data() const { return data_; }
        
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
            return v;
        }
        
        template<typename T>
        void writeString(const char *data, size_t length)
        {
            writeNumber((T)length);
            
            ensure(length);
            memcpy(p_, data_, length);
            p_ += length;
        }
        
    private:
        char*       data_;
        char*       p_;
        char*       end_;
    };
}