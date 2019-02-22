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

#define LUABT_VERSION 1

namespace LuaBinaryTable
{

    enum Type
    {
        T_EOF       = 0, // end of file
        T_NIL       = 1,
        T_TRUE      = 2,
        T_FALSE     = 3,
        T_ZERO      = 4,
        T_ONE       = 5,
        T_INT8      = 6,
        T_INT16     = 7,
        T_INT32     = 8,
        T_INT64     = 9,
        T_FLOAT     = 10,
        T_DOUBLE    = 11,
        T_STRING0   = 12,
        T_STRING8   = 13,
        T_STRING16  = 14,
        T_STRING32  = 15,
        T_ARRAY0    = 16,
        T_ARRAY8    = 17,
        T_ARRAY16   = 18,
        T_ARRAY32   = 19,
        T_TABLE0    = 20,
        T_TABLE8    = 21,
        T_TABLE16   = 22,
        T_TABLE32   = 23,
        T_MAX       = 24,
    };

    typedef uint16_t    TStringLength;
    typedef uint32_t    TStringPoolLength;

    const uint16_t  MAGIC_NUMBER = 0x7462; // "bt"
}


#endif /* lua_binary_types_h */
