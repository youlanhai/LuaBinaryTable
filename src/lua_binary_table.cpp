//
//  lua_binary_table.cpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "lua_binary_table.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cassert>


extern "C" BinaryData* writeTable(lua_State*L, size_t &length)
{
    return 0;
}

extern "C" void freeBinaryData(BinaryData *p)
{
    free(p->data);
    delete p;
}
