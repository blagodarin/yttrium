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
target_link_libraries(3d PRIVATE ycore yengine ymain)
set_target_properties(3d PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(3d PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
if(WIN32)
	set_property(TARGET 3d APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
endif()

add_ypack_target(3d_package
	INDEX data.index
	OUTPUT ${PROJECT_BINARY_DIR}/3d.ypq)
add_dependencies(3d 3d_package)

if(INSTALL_EXAMPLES)
	primal_install(3d)
	install(FILES ${PROJECT_BINARY_DIR}/3d.ypq DESTINATION ${CMAKE_INSTALL_DATADIR})
endif()
