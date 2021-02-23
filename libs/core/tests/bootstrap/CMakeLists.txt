# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(bootstrap_core_tests
	src/main.cpp
	)
include_config(bootstrap_core_tests ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(bootstrap_core_tests PRIVATE ycore)
set_target_properties(bootstrap_core_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(bootstrap_core_tests PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
add_custom_target(bootstrap
	COMMAND bootstrap_core_tests
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../data
	COMMENT "Bootstrapping core tests"
	VERBATIM)
