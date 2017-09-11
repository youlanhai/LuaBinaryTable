cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(LuaBinaryTable)

set(LUA_PATH lua-5.1.5/src)

######################################################################
## lua library
######################################################################
set(TARGET_NAME lua)
set(SOURCE_FILES 
	${LUA_PATH}/lapi.c
	${LUA_PATH}/lauxlib.c
	${LUA_PATH}/lbaselib.c
	${LUA_PATH}/lcode.c
	${LUA_PATH}/ldblib.c
	${LUA_PATH}/ldebug.c
	${LUA_PATH}/ldo.c
	${LUA_PATH}/ldump.c
	${LUA_PATH}/lfunc.c
	${LUA_PATH}/lgc.c
	${LUA_PATH}/linit.c
	${LUA_PATH}/liolib.c
	${LUA_PATH}/llex.c
	${LUA_PATH}/lmathlib.c
	${LUA_PATH}/lmem.c
	${LUA_PATH}/loadlib.c
	${LUA_PATH}/lobject.c
	${LUA_PATH}/lopcodes.c
	${LUA_PATH}/loslib.c
	${LUA_PATH}/lparser.c
	${LUA_PATH}/lstate.c
	${LUA_PATH}/lstring.c
	${LUA_PATH}/lstrlib.c
	${LUA_PATH}/ltable.c
	${LUA_PATH}/ltablib.c
	${LUA_PATH}/ltm.c
	${LUA_PATH}/lua.c
	${LUA_PATH}/luac.c
	${LUA_PATH}/lundump.c
	${LUA_PATH}/lvm.c
	${LUA_PATH}/lzio.c
	${LUA_PATH}/print.c
)

add_library(${TARGET_NAME} ${SOURCE_FILES})

######################################################################
## luac
######################################################################
set(TARGET_NAME luac)

include_directories(${LUA_PATH})
add_executable(${TARGET_NAME} ${LUA_PATH}/luac.c)
target_link_libraries(${TARGET_NAME} lua)
######################################################################
## LuaBinaryTable library
######################################################################

set(TARGET_NAME LuaBinaryTable)

set(SOURCE_FILES 
	src/lua_binary_types.h
	src/lua_binary_reader.h
	src/lua_binary_reader.cpp
	src/lua_binary_table.h
	src/lua_binary_table.cpp
	src/lua_binary_writer.h
	src/lua_binary_writer.cpp
)

include_directories(${LUA_PATH})

add_library(${TARGET_NAME} ${SOURCE_FILES})

######################################################################
## test
######################################################################
set(TARGET_NAME ltest)

include_directories(${LUA_PATH} src)

add_executable(${TARGET_NAME} test/test.cpp)
target_link_libraries(${TARGET_NAME} lua LuaBinaryTable)

######################################################################
## main
######################################################################
set(TARGET_NAME luabt)

include_directories(${LUA_PATH} src)

add_executable(${TARGET_NAME} test/main.cpp)
target_link_libraries(${TARGET_NAME} lua LuaBinaryTable)

install(TARGETS luac ltest luabt RUNTIME DESTINATION "bin")
