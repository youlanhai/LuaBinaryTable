//
//  lua_binary_types.h
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_types_h
#define lua_binary_types_h

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
        T_EMPTY_STR,
        T_INT8,
        T_INT16,
        T_INT32,
        T_INT64,
        T_FLOAT,
        T_DOUBLE,
        T_STRING,
        T_ARRAY,
        T_TABLE,
    };

}

#endif /* lua_binary_types_h */
