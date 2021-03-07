# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(examples)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")

add_executable(3d WIN32
	src/game.cpp
	src/game.h
	src/main.cpp
	src/model.cpp
	src/model.h
	)
target_link_libraries(3d PRIVATE yapplication ycore yengine ygui_ion ymain)
set_target_properties(3d PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(WIN32)
	set_property(TARGET 3d APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
endif()

yttrium_target_package(3d
	OUTPUT 3d.ypq
	INDEX data.index)

primal_install(3d)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/3d.ypq DESTINATION ${CMAKE_INSTALL_DATADIR})
