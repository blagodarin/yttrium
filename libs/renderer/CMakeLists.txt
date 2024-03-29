# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
source_group("src/backend/null" REGULAR_EXPRESSION "/src/backend/null/")
source_group("src/backend/opengl" REGULAR_EXPRESSION "/src/backend/opengl/")
source_group("src/backend/vulkan" REGULAR_EXPRESSION "/src/backend/vulkan/")
source_group("src/model" REGULAR_EXPRESSION "/src/model/")
source_group("src/model/formats" REGULAR_EXPRESSION "/src/model/formats/")
source_group("include" REGULAR_EXPRESSION "/include/yttrium/renderer/")
add_library(Y_renderer STATIC
	include/yttrium/renderer/2d.h
	include/yttrium/renderer/manager.h
	include/yttrium/renderer/material.h
	include/yttrium/renderer/mesh.h
	include/yttrium/renderer/metrics.h
	include/yttrium/renderer/modifiers.h
	include/yttrium/renderer/pass.h
	include/yttrium/renderer/program.h
	include/yttrium/renderer/texture.h
	include/yttrium/renderer/viewport.h
	src/2d.cpp
	src/2d.h
	src/backend/backend.h
	src/builtin.cpp
	src/builtin.h
	src/material.cpp
	src/material.h
	src/model/formats/obj.cpp
	src/model/formats/obj.h
	src/model/mesh_data.cpp
	src/model/mesh_data.h
	src/modifiers.cpp
	src/pass.cpp
	src/pass.h
	src/renderer.cpp
	src/renderer.h
	src/texture.cpp
	src/texture.h
	src/viewport.cpp
	src/viewport.h
	)
target_include_directories(Y_renderer PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Y_renderer PUBLIC Seir::graphics PRIVATE Y_application Seir::base Seir::data Seir::image Seir::math fmt::fmt Threads::Threads)
if(YTTRIUM_RENDERER_OPENGL)
	target_sources(Y_renderer PRIVATE
		src/backend/opengl/api.h
		src/backend/opengl/gl.cpp
		src/backend/opengl/gl.h
		src/backend/opengl/mesh.h
		src/backend/opengl/program.cpp
		src/backend/opengl/program.h
		src/backend/opengl/renderer.cpp
		src/backend/opengl/renderer.h
		src/backend/opengl/texture.cpp
		src/backend/opengl/texture.h
		src/backend/opengl/version.h
		src/backend/opengl/wrappers.cpp
		src/backend/opengl/wrappers.h
		)
	yttrium_embed(Y_renderer
		GROUP "embed"
		FILES src/backend/opengl/2d_fs.glsl src/backend/opengl/2d_vs.glsl
		)
	target_link_libraries(Y_renderer PRIVATE OpenGL::GL)
	if(WIN32)
		seir_download("https://www.khronos.org/registry/OpenGL/api/GL/glext.h" NAME GL/glext.h)
		target_include_directories(Y_renderer SYSTEM PRIVATE ${SEIR_DOWNLOAD_DIR})
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE src/backend/opengl/gl.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	endif()
elseif(YTTRIUM_RENDERER_VULKAN)
	target_sources(Y_renderer PRIVATE
		src/backend/vulkan/buffer.cpp
		src/backend/vulkan/buffer.h
		src/backend/vulkan/context.cpp
		src/backend/vulkan/context.h
		src/backend/vulkan/depth_buffer.cpp
		src/backend/vulkan/depth_buffer.h
		src/backend/vulkan/handles.cpp
		src/backend/vulkan/handles.h
		src/backend/vulkan/helpers.cpp
		src/backend/vulkan/helpers.h
		src/backend/vulkan/mesh.cpp
		src/backend/vulkan/mesh.h
		src/backend/vulkan/program.cpp
		src/backend/vulkan/program.h
		src/backend/vulkan/renderer.cpp
		src/backend/vulkan/renderer.h
		src/backend/vulkan/swapchain.cpp
		src/backend/vulkan/swapchain.h
		src/backend/vulkan/texture.cpp
		src/backend/vulkan/texture.h
		src/backend/vulkan/wrappers.cpp
		src/backend/vulkan/wrappers.h
		)
	yttrium_embed_spirv(Y_renderer
		GROUP "embed"
		FRAGMENT src/backend/vulkan/2d_fs.glsl
		VERTEX src/backend/vulkan/2d_vs.glsl
		)
	target_link_libraries(Y_renderer PRIVATE Vulkan::Vulkan)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# VK_NULL_HANDLE is defined to be 0 and is used for both pointers and integers.
		target_compile_options(Y_renderer PRIVATE -Wno-zero-as-null-pointer-constant)
	endif()
else()
	target_sources(Y_renderer PRIVATE
		src/backend/null/renderer.cpp
		src/backend/null/renderer.h
		)
endif()
if(WIN32)
	target_compile_definitions(Y_renderer PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	if(YTTRIUM_RENDERER_VULKAN)
		target_compile_definitions(Y_renderer PRIVATE VK_USE_PLATFORM_WIN32_KHR)
	endif()
else()
	if(YTTRIUM_RENDERER_VULKAN)
		target_compile_definitions(Y_renderer PRIVATE VK_USE_PLATFORM_XCB_KHR)
	endif()
endif()
seir_target(Y_renderer FOLDER libs STATIC_RUNTIME ON)
seir_install(Y_renderer EXPORT YttriumTargets INCLUDE_DIRECTORY include/yttrium)
