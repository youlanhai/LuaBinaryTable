//
//  lua_binary_reader.h
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_reader_h
#define lua_binary_reader_h

#include <cstring>

namespace LuaBinaryTable
{
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
        
        const char* readBytes(size_t length)
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
}

#endif /* lua_binary_reader_h */
