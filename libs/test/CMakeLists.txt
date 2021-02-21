# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(ytest STATIC
	include/yttrium/test/utils.h
	src/main.cpp
	src/utils.cpp
	)
include_config(ytest ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ytest PUBLIC include ${PROJECT_BINARY_DIR})
target_link_libraries(ytest PRIVATE ycore)
set_target_properties(ytest PROPERTIES FOLDER "libs" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(ytest PRIVATE
		/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
		)
endif()
