# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_ion
	src/main.cpp
	src/reader.cpp
	src/writer.cpp
	)
target_include_directories(test_ion SYSTEM PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(test_ion PRIVATE Y_base Y_ion Y_storage)
set_target_properties(test_ion PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME ion COMMAND test_ion)
