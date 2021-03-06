project ( yue CXX )
cmake_minimum_required ( VERSION 3.1 )

MESSAGE(STATUS "Lua: using information from luarocks")

MESSAGE(STATUS "LUA_LIBDIR: " ${LUA_INCDIR}/../lib " " ${LUA_INCDIR}/../../lib )
MESSAGE(STATUS "LUA_INCDIR: " ${LUA_INCDIR})
MESSAGE(STATUS "LUA: " ${LUA})

SET(LUA_EXECUTABLE "${LUA}")
SET(LUA_INCLUDE_DIR "${LUA_INCDIR}")

GET_FILENAME_COMPONENT(LUA_EXEC_NAME ${LUA} NAME_WE)
IF(LUA_EXEC_NAME STREQUAL "luajit")
	FIND_LIBRARY(LUA_LIBRARIES
		NAMES luajit libluajit
		PATHS ${LUA_INCDIR}/../lib ${LUA_INCDIR}/../../lib
		NO_DEFAULT_PATH)
ELSEIF(LUA_EXEC_NAME MATCHES "lua.*")
	FIND_LIBRARY(LUA_LIBRARIES
		NAMES lua lua54 lua53 lua52 lua51 liblua liblua54 liblua53 liblua52 liblua51 liblua5.4 liblua5.3 liblua5.2 liblua5.1
		PATHS ${LUA_INCDIR}/../lib ${LUA_INCDIR}/../../lib
		NO_DEFAULT_PATH)
ENDIF()
MESSAGE(STATUS "Lua library: ${LUA_LIBRARIES}")

enable_language( CXX )
include_directories( src ${LUA_INCLUDE_DIR} )
add_definitions( -std=c++17 -O3 -fPIC )

add_library( libyue MODULE src/yuescript/ast.cpp src/yuescript/parser.cpp src/yuescript/yue_parser.cpp src/yuescript/yue_compiler.cpp src/yuescript/yuescript.cpp)
set_target_properties( libyue PROPERTIES PREFIX "" )
set_target_properties( libyue PROPERTIES OUTPUT_NAME "yue" )
target_link_libraries( libyue ${LUA_LIBRARIES} )

add_executable( yue src/yuescript/ast.cpp src/yuescript/yue_compiler.cpp src/yuescript/yue_parser.cpp src/yuescript/yuescript.cpp src/yuescript/parser.cpp src/yue.cpp )
target_link_libraries( yue ${LUA_LIBRARIES} )
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_options( yue PRIVATE -ldl -lpthread -lstdc++fs )
endif()

install(CODE "")

