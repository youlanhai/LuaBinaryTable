//
//  lua_binary_types.h
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_types_h
#define lua_binary_types_h

#include <cstdint>

namespace LuaBinaryTable
{

    enum Type
    {
        T_EOF, // end of file
        T_NIL,
        T_TRUE,
        T_FALSE,
        T_ZERO,
        T_ONE,
        T_INT8,
        T_INT16,
        T_INT32,
        T_INT64,
        T_FLOAT,
        T_DOUBLE,
        T_STRING0,
        T_STRING8,
        T_STRING16,
        T_STRING32,
        T_ARRAY0,
        T_ARRAY8,
        T_ARRAY16,
        T_ARRAY32,
        T_TABLE0,
        T_TABLE8,
        T_TABLE16,
        T_TABLE32,
        T_MAX,
    };

    typedef uint16_t    TStringLength;
    typedef uint32_t    TStringPoolLength;
}


#endif /* lua_binary_types_h */
