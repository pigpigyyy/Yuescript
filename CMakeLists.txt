project(yue CXX)
cmake_minimum_required(VERSION 3.1)

SET(LUA_LIBDIR ${LUA_INCDIR}/../lib " " ${LUA_INCDIR}/../../lib)

find_package(Lua)
IF(LUA_FOUND)
	SET(LUA_LIBRARIES "${LUA_LIBRARY}")
ELSE(LUA_FOUND)
	MESSAGE(STATUS "Lua: using information from luarocks")
	SET(LUA_INCLUDE_DIR "${LUA_INCDIR}")
	GET_FILENAME_COMPONENT(LUA_EXEC_NAME ${LUA} NAME_WE)
	IF(LUA_EXEC_NAME STREQUAL "luajit")
		FIND_LIBRARY(LUA_LIBRARIES
			NAMES luajit libluajit
			PATHS ${LUA_LIBDIR}
			NO_DEFAULT_PATH)
	ELSEIF(LUA_EXEC_NAME MATCHES "lua.*")
		FIND_LIBRARY(LUA_LIBRARIES
			NAMES lua lua54 lua53 lua52 lua51 liblua liblua54 liblua53 liblua52 liblua51 liblua5.4 liblua5.3 liblua5.2 liblua5.1
			PATHS ${LUA_LIBDIR}
			NO_DEFAULT_PATH)
	ENDIF(LUA_EXEC_NAME STREQUAL "luajit")
ENDIF(LUA_FOUND)

MESSAGE(STATUS "Lua: " ${LUA})
MESSAGE(STATUS "Lua include: " ${LUA_INCLUDE_DIR})
MESSAGE(STATUS "Lua library: ${LUA_LIBRARIES}")

enable_language(CXX)
include_directories(src src/3rdParty ${LUA_INCLUDE_DIR})
add_definitions(-std=c++17 -O3 -fPIC)

add_library(libyue MODULE src/yuescript/ast.cpp src/yuescript/parser.cpp src/yuescript/yue_parser.cpp src/yuescript/yue_compiler.cpp src/yuescript/yuescript.cpp)
set_target_properties(libyue PROPERTIES PREFIX "")
set_target_properties(libyue PROPERTIES OUTPUT_NAME "yue")
target_link_libraries(libyue ${LUA_LIBRARIES})

find_package(Threads REQUIRED)
add_executable(yue src/yuescript/ast.cpp src/yuescript/yue_compiler.cpp src/yuescript/yue_parser.cpp src/yuescript/yuescript.cpp src/yuescript/parser.cpp src/yue.cpp)
target_link_libraries(yue ${LUA_LIBRARIES} Threads::Threads)
IF(CMAKE_DL_LIBS)
	target_link_libraries(yue ${CMAKE_DL_LIBS})
ENDIF(CMAKE_DL_LIBS)
IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_options(yue PRIVATE -lstdc++fs)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

install(CODE "")

