#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2019 Sergei Blagodarin.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(_src
	src/console.cpp
	src/resource_loader.cpp
	src/utf8.h
	)

add_library(yengine ${_src})
source_group("src" FILES ${_src})

include_config(yengine ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yengine PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yengine PRIVATE yaudio ycore Threads::Threads)
set_property(TARGET yengine PROPERTY FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_property(TARGET yengine PROPERTY DEFINE_SYMBOL Y_ENGINE_EXPORT)
	target_compile_definitions(yengine INTERFACE Y_ENGINE_IMPORT)
endif()
if(WIN32)
	find_package(Y3_freetype REQUIRED)
	target_compile_definitions(yengine PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	target_link_libraries(yengine PRIVATE Y3::freetype)
else()
	find_package(Freetype REQUIRED)
	target_link_libraries(yengine PRIVATE Freetype::Freetype)
endif()

y_sources(yengine "include"
	include/yttrium/application.h
	include/yttrium/console.h
	include/yttrium/key.h
	include/yttrium/resource_loader.h
	include/yttrium/window.h
	)

y_sources(yengine "include/gui"
	include/yttrium/gui/font.h
	include/yttrium/gui/gui.h
	include/yttrium/gui/text_capture.h
	)

y_sources(yengine "include/renderer"
	include/yttrium/renderer/manager.h
	include/yttrium/renderer/material.h
	include/yttrium/renderer/mesh.h
	include/yttrium/renderer/modifiers.h
	include/yttrium/renderer/pass.h
	include/yttrium/renderer/program.h
	include/yttrium/renderer/texture.h
	include/yttrium/renderer/textured_rect.h
	)

y_sources(yengine "src/application"
	src/application/application.cpp
	src/application/application.h
	src/application/key_codes.h
	src/application/window.cpp
	src/application/window.h
	src/application/window_backend.h
	src/application/window_callbacks.h
	)
if(Y_RENDERER_OPENGL)
	y_sources(yengine "src/application"
		src/application/opengl.h
		)
endif()

if(UNIX)
	if(Y_RENDERER_OPENGL)
		y_sources(yengine "src/application/x11"
			src/application/x11/application.cpp
			src/application/x11/application.h
			src/application/x11/glx.cpp
			src/application/x11/glx.h
			src/application/x11/window.cpp
			src/application/x11/window.h
			)
		target_include_directories(yengine SYSTEM PRIVATE ${X11_INCLUDE_DIR})
		target_link_libraries(yengine PRIVATE ${X11_X11_LIB})
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			# Xlib macros (DefaultScreen, RootWindow) produce old-style-cast warnings.
			set_property(SOURCE src/application/x11/application.cpp src/application/x11/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-old-style-cast)
		endif()
	else()
		y_sources(yengine "src/application/xcb"
			src/application/xcb/application.cpp
			src/application/xcb/application.h
			src/application/xcb/window.cpp
			src/application/xcb/window.h
			)
		target_link_libraries(yengine PRIVATE xcb xcb-image xcb-xkb xkbcommon xkbcommon-x11)
		if(Y_RENDERER_VULKAN)
			target_compile_definitions(yengine PRIVATE VK_USE_PLATFORM_XCB_KHR)
		endif()
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			# https://bugs.freedesktop.org/show_bug.cgi?id=74080
			set_property(SOURCE src/application/xcb/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-keyword-macro)
		endif()
	endif()
elseif(WIN32)
	y_sources(yengine "src/application/windows"
		src/application/windows/application.cpp
		src/application/windows/application.h
		src/application/windows/window.cpp
		src/application/windows/window.h
		)
	if(Y_RENDERER_OPENGL)
		y_sources(yengine "src/application/windows"
			src/application/windows/wgl.cpp
			src/application/windows/wgl.h
			)
	endif()
endif()

y_sources(yengine "src/gui"
	src/gui/actions.cpp
	src/gui/actions.h
	src/gui/cursor.h
	src/gui/font.cpp
	src/gui/gui.cpp
	src/gui/gui.h
	src/gui/key_lookup.cpp
	src/gui/key_lookup.h
	src/gui/layout.cpp
	src/gui/layout.h
	src/gui/screen.cpp
	src/gui/screen.h
	src/gui/widget_data.cpp
	src/gui/widget_data.h
	src/gui/widget.cpp
	src/gui/widget.h
	)

y_sources(yengine "src/gui/ion"
	src/gui/ion/loader.cpp
	src/gui/ion/loader.h
	)

y_sources(yengine "src/gui/logic"
	src/gui/logic/line_editor.cpp
	src/gui/logic/line_editor.h
	)

y_sources(yengine "src/gui/widgets"
	src/gui/widgets/button.cpp
	src/gui/widgets/button.h
	src/gui/widgets/canvas.cpp
	src/gui/widgets/canvas.h
	src/gui/widgets/image.cpp
	src/gui/widgets/image.h
	src/gui/widgets/input.cpp
	src/gui/widgets/input.h
	src/gui/widgets/label.cpp
	src/gui/widgets/label.h
	)

y_sources(yengine "src/model"
	src/model/mesh_data.cpp
	src/model/mesh_data.h
	)

y_sources(yengine "src/model/formats"
	src/model/formats/obj.cpp
	src/model/formats/obj.h
	)

y_sources(yengine "src/renderer"
	src/renderer/backend.h
	src/renderer/builtin.cpp
	src/renderer/builtin.h
	src/renderer/debug_renderer.cpp
	src/renderer/debug_renderer.h
	src/renderer/debug_texture.cpp
	src/renderer/debug_texture.h
	src/renderer/material.cpp
	src/renderer/material.h
	src/renderer/modifiers.cpp
	src/renderer/pass.cpp
	src/renderer/pass.h
	src/renderer/renderer.cpp
	src/renderer/renderer.h
	src/renderer/texture.cpp
	src/renderer/texture.h
	)

if(Y_RENDERER_OPENGL)
	y_sources(yengine "src/renderer/_opengl"
		src/renderer/_opengl/api.h
		src/renderer/_opengl/gl.cpp
		src/renderer/_opengl/gl.h
		src/renderer/_opengl/mesh.h
		src/renderer/_opengl/program.cpp
		src/renderer/_opengl/program.h
		src/renderer/_opengl/renderer.cpp
		src/renderer/_opengl/renderer.h
		src/renderer/_opengl/texture.cpp
		src/renderer/_opengl/texture.h
		src/renderer/_opengl/version.h
		src/renderer/_opengl/wrappers.cpp
		src/renderer/_opengl/wrappers.h
		)
	add_yrc_sources(yengine
		src/renderer/_opengl/2d_fs.glsl
		src/renderer/_opengl/2d_vs.glsl
		)
	source_group("src\\renderer\\_opengl" FILES
		src/renderer/_opengl/2d_fs.glsl
		src/renderer/_opengl/2d_vs.glsl
		)
	target_link_libraries(yengine PRIVATE OpenGL::GL)
	if(WIN32)
		find_path(GLEXT_INCLUDE_DIR GL/glext.h)
		find_path(WGLEXT_INCLUDE_DIR GL/wglext.h)
		mark_as_advanced(GLEXT_INCLUDE_DIR WGLEXT_INCLUDE_DIR)
		if(NOT GLEXT_INCLUDE_DIR)
			message(SEND_ERROR "Unable to find <GL/glext.h>")
		endif()
		if(NOT WGLEXT_INCLUDE_DIR)
			message(SEND_ERROR "Unable to find <GL/wglext.h>")
		endif()
		target_include_directories(yengine SYSTEM PRIVATE ${GLEXT_INCLUDE_DIR} ${WGLEXT_INCLUDE_DIR})
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE src/renderer/_opengl/gl.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	endif()
elseif(Y_RENDERER_VULKAN)
	y_sources(yengine "src/renderer/_vulkan"
		src/renderer/_vulkan/buffer.cpp
		src/renderer/_vulkan/buffer.h
		src/renderer/_vulkan/context.cpp
		src/renderer/_vulkan/context.h
		src/renderer/_vulkan/depth_buffer.cpp
		src/renderer/_vulkan/depth_buffer.h
		src/renderer/_vulkan/handles.cpp
		src/renderer/_vulkan/handles.h
		src/renderer/_vulkan/helpers.cpp
		src/renderer/_vulkan/helpers.h
		src/renderer/_vulkan/mesh.cpp
		src/renderer/_vulkan/mesh.h
		src/renderer/_vulkan/program.cpp
		src/renderer/_vulkan/program.h
		src/renderer/_vulkan/renderer.cpp
		src/renderer/_vulkan/renderer.h
		src/renderer/_vulkan/swapchain.cpp
		src/renderer/_vulkan/swapchain.h
		src/renderer/_vulkan/texture.cpp
		src/renderer/_vulkan/texture.h
		src/renderer/_vulkan/wrappers.cpp
		src/renderer/_vulkan/wrappers.h
		)
	add_yglslc_sources(yengine
		FRAGMENT src/renderer/_vulkan/2d_fs.glsl
		VERTEX src/renderer/_vulkan/2d_vs.glsl
		)
	source_group("src\\renderer\\_vulkan" FILES
		src/renderer/_vulkan/2d_fs.glsl
		src/renderer/_vulkan/2d_vs.glsl
		)
	target_link_libraries(yengine PRIVATE Vulkan::Vulkan)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# VK_NULL_HANDLE is defined to be 0 and is used for both pointers and integers.
		target_compile_options(yengine PRIVATE -Wno-zero-as-null-pointer-constant)
	endif()
else()
	y_sources(yengine "src/renderer/_null"
		src/renderer/_null/renderer.cpp
		src/renderer/_null/renderer.h
		)
endif()

install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS yengine EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
