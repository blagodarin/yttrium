# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(ymain STATIC include/yttrium/main.h)
target_include_directories(ymain PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(ymain PRIVATE primal)
set_target_properties(ymain PROPERTIES FOLDER "libs" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(WIN32)
	target_sources(ymain PRIVATE src/main_windows.cpp)
else()
	target_sources(ymain PRIVATE src/main.cpp)
endif()
primal_install(ymain EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
