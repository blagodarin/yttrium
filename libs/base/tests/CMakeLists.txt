# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_base
	src/buffer.cpp
	src/buffer_appender.cpp
	src/flags.cpp
	src/logger.cpp
	src/main.cpp
	src/numeric.cpp
	src/string.cpp
	)
target_include_directories(test_base PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_base PRIVATE Y_base)
set_target_properties(test_base PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME base COMMAND test_base)
