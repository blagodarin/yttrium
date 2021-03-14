# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(libs)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/model" REGULAR_EXPRESSION "/src/model/")
source_group("src/model/formats" REGULAR_EXPRESSION "/src/model/formats/")
source_group("src/renderer" REGULAR_EXPRESSION "/src/renderer/")
source_group("src/renderer/null" REGULAR_EXPRESSION "/src/renderer/null/")
source_group("src/renderer/opengl" REGULAR_EXPRESSION "/src/renderer/opengl/")
source_group("src/renderer/vulkan" REGULAR_EXPRESSION "/src/renderer/vulkan/")
source_group("include" REGULAR_EXPRESSION "/include/")
source_group("include/renderer" REGULAR_EXPRESSION "/include/yttrium/renderer/")
add_library(yengine STATIC
	include/yttrium/renderer/2d.h
	include/yttrium/renderer/manager.h
	include/yttrium/renderer/material.h
	include/yttrium/renderer/mesh.h
	include/yttrium/renderer/modifiers.h
	include/yttrium/renderer/pass.h
	include/yttrium/renderer/program.h
	include/yttrium/renderer/report.h
	include/yttrium/renderer/texture.h
	include/yttrium/renderer/viewport.h
	include/yttrium/resource_loader.h
	src/model/formats/obj.cpp
	src/model/formats/obj.h
	src/model/mesh_data.cpp
	src/model/mesh_data.h
	src/renderer/2d.cpp
	src/renderer/2d.h
	src/renderer/backend.h
	src/renderer/builtin.cpp
	src/renderer/builtin.h
	src/renderer/material.cpp
	src/renderer/material.h
	src/renderer/modifiers.cpp
	src/renderer/pass.cpp
	src/renderer/pass.h
	src/renderer/renderer.cpp
	src/renderer/renderer.h
	src/renderer/texture.cpp
	src/renderer/texture.h
	src/renderer/viewport.cpp
	src/renderer/viewport.h
	src/resource_loader.cpp
	)
include_config(yengine ${PROJECT_BINARY_DIR}/config.h)
target_include_directories(yengine PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(yengine PRIVATE yapplication yaudio ycore primal Threads::Threads)
set_target_properties(yengine PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(YTTRIUM_RENDERER STREQUAL "opengl")
	target_sources(yengine PRIVATE
		src/renderer/opengl/api.h
		src/renderer/opengl/gl.cpp
		src/renderer/opengl/gl.h
		src/renderer/opengl/mesh.h
		src/renderer/opengl/program.cpp
		src/renderer/opengl/program.h
		src/renderer/opengl/renderer.cpp
		src/renderer/opengl/renderer.h
		src/renderer/opengl/texture.cpp
		src/renderer/opengl/texture.h
		src/renderer/opengl/version.h
		src/renderer/opengl/wrappers.cpp
		src/renderer/opengl/wrappers.h
		)
	add_yrc_sources(yengine
		src/renderer/opengl/2d_fs.glsl
		src/renderer/opengl/2d_vs.glsl
		)
	target_link_libraries(yengine PRIVATE OpenGL::GL)
	if(WIN32)
		primal_download("https://www.khronos.org/registry/OpenGL/api/GL/glext.h" NAME GL/glext.h)
		target_include_directories(yengine PRIVATE ${PRIMAL_DOWNLOAD_DIR})
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE src/renderer/opengl/gl.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	endif()
elseif(YTTRIUM_RENDERER STREQUAL "vulkan")
	y_sources(yengine "src/renderer/vulkan"
		src/renderer/vulkan/buffer.cpp
		src/renderer/vulkan/buffer.h
		src/renderer/vulkan/context.cpp
		src/renderer/vulkan/context.h
		src/renderer/vulkan/depth_buffer.cpp
		src/renderer/vulkan/depth_buffer.h
		src/renderer/vulkan/handles.cpp
		src/renderer/vulkan/handles.h
		src/renderer/vulkan/helpers.cpp
		src/renderer/vulkan/helpers.h
		src/renderer/vulkan/mesh.cpp
		src/renderer/vulkan/mesh.h
		src/renderer/vulkan/program.cpp
		src/renderer/vulkan/program.h
		src/renderer/vulkan/renderer.cpp
		src/renderer/vulkan/renderer.h
		src/renderer/vulkan/swapchain.cpp
		src/renderer/vulkan/swapchain.h
		src/renderer/vulkan/texture.cpp
		src/renderer/vulkan/texture.h
		src/renderer/vulkan/wrappers.cpp
		src/renderer/vulkan/wrappers.h
		)
	add_yglslc_sources(yengine
		FRAGMENT src/renderer/vulkan/2d_fs.glsl
		VERTEX src/renderer/vulkan/2d_vs.glsl
		)
	target_link_libraries(yengine PRIVATE Vulkan::Vulkan)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# VK_NULL_HANDLE is defined to be 0 and is used for both pointers and integers.
		target_compile_options(yengine PRIVATE -Wno-zero-as-null-pointer-constant)
	endif()
else()
	target_sources(yengine PRIVATE
		src/renderer/null/renderer.cpp
		src/renderer/null/renderer.h
		)
endif()
if(WIN32)
	target_compile_definitions(yengine PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	if(YTTRIUM_RENDERER STREQUAL "vulkan")
		target_compile_definitions(yengine PRIVATE VK_USE_PLATFORM_WIN32_KHR)
	endif()
else()
	if(YTTRIUM_RENDERER STREQUAL "vulkan")
		target_compile_definitions(yengine PRIVATE VK_USE_PLATFORM_XCB_KHR)
	endif()
endif()
primal_install(yengine EXPORT YttriumTargets)
install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
