# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
source_group("include/memory" REGULAR_EXPRESSION "/include/yttrium/memory/")
source_group("include/utils" REGULAR_EXPRESSION "/include/yttrium/utils/")
source_group("src/logger" REGULAR_EXPRESSION "/src/logger/")
source_group("src/memory" REGULAR_EXPRESSION "/src/memory/")
source_group("src/platform" REGULAR_EXPRESSION "/src/platform/")
source_group("src/platform/posix" REGULAR_EXPRESSION "/src/platform/posix/")
source_group("src/platform/windows" REGULAR_EXPRESSION "/src/platform/windows/")
source_group("src/utils" REGULAR_EXPRESSION "/src/utils/")
add_library(ycore STATIC
	include/yttrium/exceptions.h
	include/yttrium/logger.h
	include/yttrium/memory/buffer.h
	include/yttrium/memory/buffer_appender.h
	include/yttrium/utils/clock.h
	include/yttrium/utils/flags.h
	include/yttrium/utils/numeric.h
	include/yttrium/utils/string.h
	src/logger/logger.cpp
	src/logger/ring_log.cpp
	src/logger/ring_log.h
	src/main.cpp
	src/memory/buffer.cpp
	src/memory/buffer_memory.cpp
	src/memory/buffer_memory.h
	src/memory/raw.h
	src/platform/virtual_memory.h
	src/utils/string.cpp
	)
include_config(ycore ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ycore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ycore PRIVATE primal fmt::fmt Threads::Threads)
set_target_properties(ycore PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(WIN32)
	target_compile_definitions(ycore PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(ycore PRIVATE
		src/platform/windows/com.cpp
		src/platform/windows/com.h
		src/platform/windows/error.cpp
		src/platform/windows/error.h
		src/platform/windows/virtual_memory.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set_property(SOURCE src/platform/windows/com.cpp APPEND PROPERTY COMPILE_OPTIONS
			/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
			)
	endif()
else()
	target_sources(ycore PRIVATE
		src/platform/posix/error.cpp
		src/platform/posix/error.h
		src/platform/posix/virtual_memory.cpp
		)
endif()
primal_install(ycore EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
