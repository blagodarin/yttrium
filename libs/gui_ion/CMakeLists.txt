# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/ion" REGULAR_EXPRESSION "/src/ion/")
source_group("src/logic" REGULAR_EXPRESSION "/src/logic/")
source_group("src/widgets" REGULAR_EXPRESSION "/src/widgets/")
source_group("include" REGULAR_EXPRESSION "/include/yttrium/gui/")
add_library(ygui_ion STATIC
	include/yttrium/gui/ion_gui.h
	src/actions.cpp
	src/actions.h
	src/cursor.h
	src/gui.cpp
	src/gui.h
	src/ion/loader.cpp
	src/ion/loader.h
	src/ion/utils.cpp
	src/ion/utils.h
	src/key_lookup.cpp
	src/key_lookup.h
	src/layout.cpp
	src/layout.h
	src/logic/line_editor.cpp
	src/logic/line_editor.h
	src/screen.cpp
	src/screen.h
	src/widget.cpp
	src/widget.h
	src/widget_data.cpp
	src/widget_data.h
	src/widgets/button.cpp
	src/widgets/button.h
	src/widgets/canvas.cpp
	src/widgets/canvas.h
	src/widgets/image.cpp
	src/widgets/image.h
	src/widgets/input.cpp
	src/widgets/input.h
	src/widgets/label.cpp
	src/widgets/label.h
	)
include_config(ygui_ion ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(ygui_ion PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ygui_ion PRIVATE yapplication yaudio ycore ygui yengine primal)
set_target_properties(ygui_ion PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
primal_install(ygui_ion EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
