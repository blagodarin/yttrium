# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/windows" REGULAR_EXPRESSION "/src/windows/")
source_group("src/x11" REGULAR_EXPRESSION "/src/x11/")
source_group("src/xcb" REGULAR_EXPRESSION "/src/xcb/")
source_group("include" REGULAR_EXPRESSION "/include/")
add_library(yapplication STATIC
	include/yttrium/application/application.h
	include/yttrium/application/id.h
	include/yttrium/application/key.h
	include/yttrium/application/window.h
	src/application.cpp
	src/application.h
	src/key_codes.h
	src/window.cpp
	src/window.h
	src/window_backend.h
	src/window_callbacks.h
	)
include_config(yapplication ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yapplication PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yapplication PUBLIC ycore PRIVATE yimage primal)
set_target_properties(yapplication PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(YTTRIUM_RENDERER STREQUAL "opengl")
	target_sources(yapplication PRIVATE
		src/opengl.h
		)
endif()
if(WIN32)
	target_compile_definitions(yapplication PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_sources(yapplication PRIVATE
		src/windows/application.cpp
		src/windows/application.h
		src/windows/window.cpp
		src/windows/window.h
		)
	if(YTTRIUM_RENDERER STREQUAL "opengl")
		target_sources(yapplication PRIVATE
			src/windows/wgl.cpp
			src/windows/wgl.h
			)
		primal_download("https://www.khronos.org/registry/OpenGL/api/GL/wglext.h" NAME GL/wglext.h)
		primal_download("https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h" NAME KHR/khrplatform.h)
		target_include_directories(yapplication PRIVATE ${PRIMAL_DOWNLOAD_DIR})
	endif()
else()
	if(YTTRIUM_RENDERER STREQUAL "opengl")
		find_package(X11 REQUIRED)
		target_sources(yapplication PRIVATE
			src/x11/application.cpp
			src/x11/application.h
			src/x11/glx.cpp
			src/x11/glx.h
			src/x11/window.cpp
			src/x11/window.h
			)
		target_link_libraries(yapplication PRIVATE X11::X11)
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			# Xlib macros (DefaultScreen, RootWindow) produce old-style-cast warnings.
			set_property(SOURCE src/x11/application.cpp src/x11/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-old-style-cast)
		endif()
	else()
		find_package(X11 REQUIRED COMPONENTS xcb_xkb xkbcommon_X11)
		pkg_check_modules(xcb-image REQUIRED IMPORTED_TARGET xcb-image) # TODO: Remove this dependency.
		target_sources(yapplication PRIVATE
			src/xcb/application.cpp
			src/xcb/application.h
			src/xcb/window.cpp
			src/xcb/window.h
			)
		target_link_libraries(yapplication PRIVATE X11::xcb_xkb X11::xkbcommon_X11 PkgConfig::xcb-image)
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			# https://bugs.freedesktop.org/show_bug.cgi?id=74080
			set_property(SOURCE src/xcb/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-keyword-macro)
		endif()
	endif()
endif()
primal_install(yapplication EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
