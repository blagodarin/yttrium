# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_core
	src/buffer.cpp
	src/buffer_appender.cpp
	src/flags.cpp
	src/logger.cpp
	src/package.cpp
	src/reader.cpp
	src/source.cpp
	src/temporary_file.cpp
	src/test_utils.cpp
	src/utils_numeric.cpp
	src/utils_string.cpp
	)
include_config(test_core ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_core PRIVATE ycore ytest)
set_target_properties(test_core PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME core COMMAND test_core)
