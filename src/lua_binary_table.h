//
//  lua_binary_table.hpp
//  LuaBinaryTable
//
//  Created by youlanhai on 15/12/27.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef lua_binary_table_hpp
#define lua_binary_table_hpp

#ifdef __cplusplus
extern "C"
{
#endif
    
#include "lua.h"
    
bool parseTable(lua_State *L, const char *data, size_t length);
    
struct BinaryData
{
    char *data;
    size_t length;
};
BinaryData* writeTable(lua_State*L, size_t &length);
void freeBinaryData(BinaryData *p);
    
#ifdef __cplusplus
}
#endif

#endif /* lua_binary_table_hpp */