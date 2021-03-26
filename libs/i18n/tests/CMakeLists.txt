# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_i18n
	src/translation.cpp
	)
include_config(test_i18n ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_i18n PRIVATE ycore yi18n ystorage ytest)
set_target_properties(test_i18n PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME i18n COMMAND test_i18n)
