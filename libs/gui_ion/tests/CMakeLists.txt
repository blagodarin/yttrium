# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(tests)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(test_gui_ion
	src/gui_loading.cpp
	src/ion_utils.cpp
	src/key.cpp
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE src/gui_loading.cpp src/key.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-disabled-optimization)
endif()
include_config(test_gui_ion ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_gui_ion PRIVATE yapplication ycore yengine ygui_ion ytest)
set_target_properties(test_gui_ion PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME gui_ion COMMAND test_gui_ion)
