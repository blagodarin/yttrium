# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_engine
	src/gui_loading.cpp
	src/key.cpp
	src/resource_loader.cpp
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE src/gui_loading.cpp src/key.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-disabled-optimization)
endif()
include_config(test_engine ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_engine PRIVATE yapplication ycore yengine ytest)
set_target_properties(test_engine PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(test_engine PRIVATE
		/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
		)
	target_link_options(test_engine PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
add_test(NAME engine COMMAND test_engine)
