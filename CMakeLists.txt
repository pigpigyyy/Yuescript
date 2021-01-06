project ( moonp CXX )
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
		NAMES lua lua54 lua53 lua52 lua51 liblua liblua54 liblua53 liblua52 liblua51
		PATHS ${LUA_INCDIR}/../lib ${LUA_INCDIR}/../../lib
		NO_DEFAULT_PATH)
ENDIF()
MESSAGE(STATUS "Lua library: ${LUA_LIBRARIES}")

enable_language( CXX )
include_directories( src ${LUA_INCLUDE_DIR} )
add_definitions( -std=c++17 -O3 )

add_library( moonp MODULE src/MoonP/ast.cpp src/MoonP/parser.cpp src/MoonP/moon_parser.cpp src/MoonP/moon_compiler.cpp src/MoonP/moonplus.cpp)
set_target_properties( moonp PROPERTIES PREFIX "")
target_link_libraries( moonp ${LUA_LIBRARIES} )

add_custom_command(TARGET moonp
	POST_BUILD
	COMMAND make release
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(PROGRAMS bin/release/moonp DESTINATION bin)

