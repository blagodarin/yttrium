set(_src console.cpp resource_loader.cpp)
add_library(YEngine ${_src})
source_group("src" FILES ${_src})
target_link_libraries(YEngine PRIVATE YCore Threads::Threads)
set_target_properties(YEngine PROPERTIES FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_target_properties(YEngine PROPERTIES DEFINE_SYMBOL Y_ENGINE_EXPORT)
	target_compile_definitions(YEngine INTERFACE Y_ENGINE_IMPORT)
endif()
include_config(YEngine ${PROJECT_BINARY_DIR}/config.h)

y_sources(YEngine "include"
	../../include/yttrium/console.h
	../../include/yttrium/resource_loader.h
	)

########################################################################################################################
# application
y_sources(YEngine "include"
	../../include/yttrium/application.h
	../../include/yttrium/key.h
	../../include/yttrium/window.h
	)
y_sources(YEngine "src/application"
	application/application.cpp
	application/application.h
	application/key.cpp
	application/window.cpp
	application/window.h
	application/window_backend.h
	application/window_callbacks.h
	)
if(Y_RENDERER_OPENGL)
	y_sources(YEngine "src/application"
		application/opengl.h
		)
endif()
if(UNIX)
	y_sources(YEngine "src/application"
		application/openal.h
		)
	if(Y_RENDERER_OPENGL)
		y_sources(YEngine "src/application/_x11"
			application/_x11/application.cpp
			application/_x11/application.h
			application/_x11/glx.cpp
			application/_x11/glx.h
			application/_x11/window.cpp
			application/_x11/window.h
			)
		target_include_directories(YEngine SYSTEM PRIVATE ${X11_INCLUDE_DIR})
		target_link_libraries(YEngine PRIVATE ${X11_X11_LIB})
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			# Xlib macros (DefaultScreen, RootWindow) produce old-style-cast warnings.
			set_property(SOURCE application/_x11/application.cpp application/_x11/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-old-style-cast)
		endif()
	else()
		y_sources(YEngine "src/application/_xcb"
			application/_xcb/application.cpp
			application/_xcb/application.h
			application/_xcb/window.cpp
			application/_xcb/window.h
			)
		target_link_libraries(YEngine PRIVATE xcb xcb-image xcb-xkb xkbcommon xkbcommon-x11)
		if(Y_RENDERER_VULKAN)
			target_compile_definitions(YEngine PRIVATE VK_USE_PLATFORM_XCB_KHR)
		endif()
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			# https://bugs.freedesktop.org/show_bug.cgi?id=74080
			set_property(SOURCE application/_xcb/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-keyword-macro)
		endif()
	endif()
elseif(WIN32)
	y_sources(YEngine "src/application/_windows"
		application/_windows/api.h
		application/_windows/com.cpp
		application/_windows/com.h
		application/_windows/window.cpp
		application/_windows/window.h
		)
	if(Y_RENDERER_OPENGL)
		y_sources(YEngine "src/application/_windows"
			application/_windows/wgl.cpp
			application/_windows/wgl.h
			)
	endif()
endif()

########################################################################################################################
# audio
y_sources(YEngine "include/audio"
	../../include/yttrium/audio/manager.h
	../../include/yttrium/audio/music_reader.h
	../../include/yttrium/audio/player.h
	../../include/yttrium/audio/sound.h
	)
y_sources(YEngine "src/audio"
	audio/backend.h
	audio/manager.cpp
	audio/manager.h
	audio/music_reader.cpp
	audio/music_reader.h
	audio/player.cpp
	audio/player.h
	audio/streamer.cpp
	audio/streamer.h
	)
if(WIN32)
	y_sources(YEngine "src/audio/_wasapi"
		audio/_wasapi/backend.cpp
		audio/_wasapi/backend.h
		audio/_wasapi/player.cpp
		audio/_wasapi/player.h
		audio/_wasapi/sound.cpp
		audio/_wasapi/sound.h
		)
else()
	find_package(OpenAL REQUIRED)
	y_sources(YEngine "src/audio/_openal"
		audio/_openal/backend.cpp
		audio/_openal/backend.h
		audio/_openal/format.cpp
		audio/_openal/format.h
		audio/_openal/player.cpp
		audio/_openal/player.h
		audio/_openal/sound.cpp
		audio/_openal/sound.h
		)
	target_include_directories(YEngine SYSTEM PRIVATE ${OPENAL_INCLUDE_DIR})
	target_link_libraries(YEngine PRIVATE ${OPENAL_LIBRARY})
endif()

########################################################################################################################
# gui
y_sources(YEngine "include/gui"
	../../include/yttrium/gui/gui.h
	../../include/yttrium/gui/text_capture.h
	../../include/yttrium/gui/texture_font.h
	)
y_sources(YEngine "src/gui"
	gui/actions.cpp
	gui/actions.h
	gui/cursor.h
	gui/gui.cpp
	gui/gui.h
	gui/key_lookup.cpp
	gui/key_lookup.h
	gui/layout.cpp
	gui/layout.h
	gui/screen.cpp
	gui/screen.h
	gui/texture_font.cpp
	gui/texture_font.h
	gui/widget_data.cpp
	gui/widget_data.h
	gui/widget.cpp
	gui/widget.h
	)
y_sources(YEngine "src/gui/ion"
	gui/ion/loader.cpp
	gui/ion/loader.h
	)
y_sources(YEngine "src/gui/logic"
	gui/logic/line_editor.cpp
	gui/logic/line_editor.h
	)
y_sources(YEngine "src/gui/widgets"
	gui/widgets/button.cpp
	gui/widgets/button.h
	gui/widgets/canvas.cpp
	gui/widgets/canvas.h
	gui/widgets/image.cpp
	gui/widgets/image.h
	gui/widgets/input.cpp
	gui/widgets/input.h
	gui/widgets/label.cpp
	gui/widgets/label.h
	)

########################################################################################################################
# renderer
y_sources(YEngine "include/renderer"
	../../include/yttrium/renderer/manager.h
	../../include/yttrium/renderer/material.h
	../../include/yttrium/renderer/mesh.h
	../../include/yttrium/renderer/modifiers.h
	../../include/yttrium/renderer/pass.h
	../../include/yttrium/renderer/program.h
	../../include/yttrium/renderer/texture.h
	../../include/yttrium/renderer/textured_rect.h
	)
y_sources(YEngine "src/renderer"
	renderer/backend.h
	renderer/debug_renderer.cpp
	renderer/debug_renderer.h
	renderer/material.cpp
	renderer/material.h
	renderer/mesh_data.cpp
	renderer/mesh_data.h
	renderer/modifiers.cpp
	renderer/pass.cpp
	renderer/pass.h
	renderer/renderer.cpp
	renderer/renderer.h
	renderer/texture.cpp
	renderer/texture.h
	)
y_sources(YEngine "src/renderer/builtin"
	renderer/builtin/builtin.h
	renderer/builtin/builtin.cpp
	renderer/builtin/debug_texture.cpp
	renderer/builtin/debug_texture.h
	)
y_sources(YEngine "src/renderer/formats"
	renderer/formats/obj.cpp
	renderer/formats/obj.h
	)
if(Y_RENDERER_OPENGL)
	y_sources(YEngine "src/renderer/_opengl"
		renderer/_opengl/api.h
		renderer/_opengl/gl.cpp
		renderer/_opengl/gl.h
		renderer/_opengl/mesh.h
		renderer/_opengl/program.cpp
		renderer/_opengl/program.h
		renderer/_opengl/renderer.cpp
		renderer/_opengl/renderer.h
		renderer/_opengl/texture.cpp
		renderer/_opengl/texture.h
		renderer/_opengl/version.h
		renderer/_opengl/wrappers.cpp
		renderer/_opengl/wrappers.h
		)
	add_yrc_sources(YEngine
		renderer/_opengl/2d_fs.glsl
		renderer/_opengl/2d_vs.glsl
		)
	source_group("src\\renderer\\_opengl" FILES
		renderer/_opengl/2d_fs.glsl
		renderer/_opengl/2d_vs.glsl
		)
	target_link_libraries(YEngine PRIVATE OpenGL::GL)
	if(WIN32)
		find_path(GLEXT_INCLUDE_DIR GL/glext.h)
		find_path(WGLEXT_INCLUDE_DIR GL/wglext.h)
		if(NOT GLEXT_INCLUDE_DIR)
			message(SEND_ERROR "Unable to find <GL/glext.h>")
		endif()
		if(NOT WGLEXT_INCLUDE_DIR)
			message(SEND_ERROR "Unable to find <GL/wglext.h>")
		endif()
		target_include_directories(YEngine SYSTEM PRIVATE ${GLEXT_INCLUDE_DIR} ${WGLEXT_INCLUDE_DIR})
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE renderer/_opengl/gl.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	endif()
elseif(Y_RENDERER_VULKAN)
	y_sources(YEngine "src/renderer/_vulkan"
		renderer/_vulkan/buffer.cpp
		renderer/_vulkan/buffer.h
		renderer/_vulkan/context.cpp
		renderer/_vulkan/context.h
		renderer/_vulkan/depth_buffer.cpp
		renderer/_vulkan/depth_buffer.h
		renderer/_vulkan/handles.cpp
		renderer/_vulkan/handles.h
		renderer/_vulkan/helpers.cpp
		renderer/_vulkan/helpers.h
		renderer/_vulkan/mesh.cpp
		renderer/_vulkan/mesh.h
		renderer/_vulkan/program.cpp
		renderer/_vulkan/program.h
		renderer/_vulkan/renderer.cpp
		renderer/_vulkan/renderer.h
		renderer/_vulkan/swapchain.cpp
		renderer/_vulkan/swapchain.h
		renderer/_vulkan/texture.cpp
		renderer/_vulkan/texture.h
		renderer/_vulkan/wrappers.cpp
		renderer/_vulkan/wrappers.h
		)
	add_yglslc_sources(YEngine
		FRAGMENT renderer/_vulkan/2d_fs.glsl
		VERTEX renderer/_vulkan/2d_vs.glsl
		)
	source_group("src\\renderer\\_vulkan" FILES
		renderer/_vulkan/2d_fs.glsl
		renderer/_vulkan/2d_vs.glsl
		)
	target_link_libraries(YEngine PRIVATE Vulkan::Vulkan)
else()
	y_sources(YEngine "src/renderer/_null"
		renderer/_null/renderer.cpp
		renderer/_null/renderer.h
		)
endif()

install(TARGETS YEngine EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
