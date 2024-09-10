# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/posix" REGULAR_EXPRESSION "src/posix/")
source_group("src/windows" REGULAR_EXPRESSION "src/windows/")
source_group("include" REGULAR_EXPRESSION "include/")
add_library(Y_base STATIC
	include/yttrium/base/buffer.h
	include/yttrium/base/buffer_appender.h
	include/yttrium/base/exceptions.h
	include/yttrium/base/flags.h
	include/yttrium/base/logger.h
	src/buffer.cpp
	src/buffer_memory.cpp
	src/buffer_memory.h
	src/logger.cpp
	src/main.cpp
	src/ring_log.cpp
	src/ring_log.h
	src/virtual_memory.h
	)
target_include_directories(Y_base PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_base PRIVATE Seir::base fmt::fmt Threads::Threads)
if(WIN32)
	target_compile_definitions(Y_base PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(Y_base PRIVATE
		src/windows/error.cpp
		src/windows/error.h
		src/windows/virtual_memory.cpp
		)
else()
	target_sources(Y_base PRIVATE
		src/posix/error.cpp
		src/posix/error.h
		src/posix/virtual_memory.cpp
		)
endif()
seir_target(Y_base FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_base EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
