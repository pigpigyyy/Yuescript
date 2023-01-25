project(yue CXX)
cmake_minimum_required(VERSION 3.1)

SET(LUA_LIBDIR ${LUA_INCDIR}/../lib " " ${LUA_INCDIR}/../../lib)

find_package(Lua)
if (LUA_FOUND)
	SET(LUA_LIBRARIES "${LUA_LIBRARY}")
else ()
	MESSAGE(STATUS "Lua: using information from luarocks")
	SET(LUA_INCLUDE_DIR "${LUA_INCDIR}")
	GET_FILENAME_COMPONENT(LUA_EXEC_NAME ${LUA} NAME_WE)
	if (LUA_EXEC_NAME STREQUAL "luajit")
		FIND_LIBRARY(LUA_LIBRARIES
			NAMES luajit libluajit
			PATHS ${LUA_LIBDIR}
			NO_DEFAULT_PATH)
	elseif ()
		FIND_LIBRARY(LUA_LIBRARIES
			NAMES lua lua54 lua53 lua52 lua51 liblua liblua54 liblua53 liblua52 liblua51 liblua5.4 liblua5.3 liblua5.2 liblua5.1
			PATHS ${LUA_LIBDIR}
			NO_DEFAULT_PATH)
	endif()
endif()

message(STATUS "Lua: " ${LUA})
message(STATUS "Lua include: " ${LUA_INCLUDE_DIR})
message(STATUS "Lua library: ${LUA_LIBRARIES}")

enable_language(CXX)
include_directories(src src/3rdParty ${LUA_INCLUDE_DIR})
add_definitions(-std=c++17 -O3 -fPIC)

add_library(libyue MODULE
	src/yuescript/ast.cpp
	src/yuescript/parser.cpp
	src/yuescript/yue_parser.cpp
	src/yuescript/yue_compiler.cpp
	src/yuescript/yuescript.cpp
)
set_target_properties(libyue PROPERTIES PREFIX "")
set_target_properties(libyue PROPERTIES OUTPUT_NAME "yue")
target_link_libraries(libyue ${LUA_LIBRARIES})

add_executable(yue
	src/yuescript/ast.cpp
	src/yuescript/yue_compiler.cpp
	src/yuescript/yue_parser.cpp
	src/yuescript/yuescript.cpp
	src/yuescript/parser.cpp
	src/yue.cpp
)

target_sources(yue PRIVATE
	src/3rdParty/efsw/Debug.cpp
	src/3rdParty/efsw/DirectorySnapshot.cpp
	src/3rdParty/efsw/DirectorySnapshotDiff.cpp
	src/3rdParty/efsw/DirWatcherGeneric.cpp
	src/3rdParty/efsw/FileInfo.cpp
	src/3rdParty/efsw/FileSystem.cpp
	src/3rdParty/efsw/FileWatcher.cpp
	src/3rdParty/efsw/FileWatcherCWrapper.cpp
	src/3rdParty/efsw/FileWatcherGeneric.cpp
	src/3rdParty/efsw/FileWatcherImpl.cpp
	src/3rdParty/efsw/Log.cpp
	src/3rdParty/efsw/Mutex.cpp
	src/3rdParty/efsw/String.cpp
	src/3rdParty/efsw/System.cpp
	src/3rdParty/efsw/Thread.cpp
	src/3rdParty/efsw/Watcher.cpp
	src/3rdParty/efsw/WatcherGeneric.cpp
)

if (WIN32)
	target_sources(yue PRIVATE
		src/3rdParty/efsw/platform/win/FileSystemImpl.cpp
		src/3rdParty/efsw/platform/win/MutexImpl.cpp
		src/3rdParty/efsw/platform/win/SystemImpl.cpp
		src/3rdParty/efsw/platform/win/ThreadImpl.cpp
	)
else ()
	target_sources(yue PRIVATE
		src/3rdParty/efsw/platform/posix/FileSystemImpl.cpp
		src/3rdParty/efsw/platform/posix/MutexImpl.cpp
		src/3rdParty/efsw/platform/posix/SystemImpl.cpp
		src/3rdParty/efsw/platform/posix/ThreadImpl.cpp
	)
endif()

if (APPLE)
	target_sources(yue PRIVATE
		src/3rdParty/efsw/FileWatcherFSEvents.cpp
		src/3rdParty/efsw/FileWatcherKqueue.cpp
		src/3rdParty/efsw/WatcherFSEvents.cpp
		src/3rdParty/efsw/WatcherKqueue.cpp
	)

	if (NOT CMAKE_SYSTEM_VERSION GREATER 9)
		target_compile_definitions(yue PRIVATE EFSW_FSEVENTS_NOT_SUPPORTED)
	endif()
elseif (WIN32)
	target_sources(yue PRIVATE
		src/3rdParty/efsw/FileWatcherWin32.cpp
		src/3rdParty/efsw/WatcherWin32.cpp
	)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_sources(yue PRIVATE
		src/3rdParty/efsw/FileWatcherInotify.cpp
		src/3rdParty/efsw/WatcherInotify.cpp
	)

	if (NOT EXISTS "/usr/include/sys/inotify.h" AND NOT EXISTS "/usr/local/include/sys/inotify.h")
		target_compile_definitions(yue PRIVATE EFSW_INOTIFY_NOSYS)
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	target_sources(yue PRIVATE
		src/3rdParty/efsw/FileWatcherKqueue.cpp
		src/3rdParty/efsw/WatcherKqueue.cpp
	)
endif()

if (MSVC)
	target_compile_definitions(yue PRIVATE _SCL_SECURE_NO_WARNINGS)
else ()
	target_compile_options(yue PRIVATE -Wall -Wno-long-long -fPIC)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	target_compile_definitions(yue PRIVATE DEBUG)
elseif (${CMAKE_BUILD_TYPE} MATCHES "Release")
	target_compile_definitions(yue PRIVATE NDEBUG)
endif()

if(CMAKE_DL_LIBS)
	target_link()
	SET(YUE_DEP_LIBS ${LUA_LIBRARIES} " dl")
else ()
	SET(YUE_DEP_LIBS ${LUA_LIBRARIES})
endif()

find_package(Threads REQUIRED)
if (APPLE)
	set(MAC_LIBS "-framework CoreFoundation" "-framework CoreServices")
	target_link_libraries(yue PRIVATE ${YUE_DEP_LIBS} ${MAC_LIBS} Threads::Threads)
elseif (NOT (${CMAKE_SYSTEM_NAME} MATCHES "Haiku") AND NOT WIN32)
	target_link_libraries(yue PRIVATE ${YUE_DEP_LIBS} Threads::Threads)
else ()
	target_link_libraries(yue PRIVATE ${YUE_DEP_LIBS})
endif()
	
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_options(yue PRIVATE -lstdc++fs)
endif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

install(CODE "")

