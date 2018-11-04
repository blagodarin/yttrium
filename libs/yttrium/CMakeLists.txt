add_library(yttrium SHARED globals.cpp)
source_group("src" FILES globals.cpp)
target_link_libraries(yttrium PRIVATE Threads::Threads)
set_target_properties(yttrium PROPERTIES DEFINE_SYMBOL YTTRIUM_EXPORT FOLDER "libs")
include_config(yttrium ${PROJECT_BINARY_DIR}/config.h)

y_sources(yttrium "include"
	../../include/yttrium/api.h
	../../include/yttrium/console.h
	../../include/yttrium/exceptions.h
	../../include/yttrium/resource_loader.h
	../../include/yttrium/translation.h
	)
y_sources(yttrium "src"
	console.cpp
	resource_loader.cpp
	translation.cpp
	)

########################################################################################################################
# application
y_sources(yttrium "include"
	../../include/yttrium/application.h
	../../include/yttrium/key.h
	../../include/yttrium/window.h
	)
y_sources(yttrium "src/application"
	application/application.cpp
	application/application.h
	application/key.cpp
	application/window.cpp
	application/window.h
	application/window_backend.h
	application/window_callbacks.h
	)
if(Y_RENDERER_OPENGL)
	y_sources(yttrium "src/application"
		application/opengl.h
		)
endif()
if(UNIX)
	y_sources(yttrium "src/application"
		application/openal.h
		)
	if(Y_RENDERER_OPENGL)
		y_sources(yttrium "src/application/_x11"
			application/_x11/glx.cpp
			application/_x11/glx.h
			application/_x11/window.cpp
			application/_x11/window.h
			)
		target_include_directories(yttrium SYSTEM PRIVATE ${X11_INCLUDE_DIR})
		target_link_libraries(yttrium PRIVATE ${X11_X11_LIB})
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set_property(SOURCE application/_x11/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-old-style-cast)
		endif()
	else()
		y_sources(yttrium "src/application/_xcb"
			application/_xcb/window.cpp
			application/_xcb/window.h
			)
		target_link_libraries(yttrium PRIVATE xcb xcb-image xcb-xkb xkbcommon xkbcommon-x11)
		if(Y_RENDERER_VULKAN)
			target_compile_definitions(yttrium PRIVATE VK_USE_PLATFORM_XCB_KHR)
		endif()
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			# https://bugs.freedesktop.org/show_bug.cgi?id=74080
			set_property(SOURCE application/_xcb/window.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-keyword-macro)
		endif()
	endif()
elseif(WIN32)
	y_sources(yttrium "src/application/_windows"
		application/_windows/api.h
		application/_windows/com.cpp
		application/_windows/com.h
		application/_windows/window.cpp
		application/_windows/window.h
		)
	if(Y_RENDERER_OPENGL)
		y_sources(yttrium "src/application/_windows"
			application/_windows/wgl.cpp
			application/_windows/wgl.h
			)
	endif()
endif()

########################################################################################################################
# audio
y_sources(yttrium "include/audio"
	../../include/yttrium/audio/format.h
	../../include/yttrium/audio/manager.h
	../../include/yttrium/audio/music_reader.h
	../../include/yttrium/audio/player.h
	../../include/yttrium/audio/reader.h
	../../include/yttrium/audio/sound.h
	../../include/yttrium/audio/utils.h
	)
y_sources(yttrium "src/audio"
	audio/backend.h
	audio/format.cpp
	audio/manager.cpp
	audio/manager.h
	audio/music_reader.cpp
	audio/music_reader.h
	audio/player.cpp
	audio/player.h
	audio/reader.cpp
	audio/reader.h
	audio/streamer.cpp
	audio/streamer.h
	audio/utils.cpp
	)
y_sources(yttrium "src/audio/formats"
	audio/formats/wav_private.h
	audio/formats/wav.cpp
	audio/formats/wav.h
	)
if(WIN32)
	y_sources(yttrium "src/audio/_wasapi"
		audio/_wasapi/backend.cpp
		audio/_wasapi/backend.h
		audio/_wasapi/player.cpp
		audio/_wasapi/player.h
		audio/_wasapi/sound.cpp
		audio/_wasapi/sound.h
		)
else()
	find_package(OpenAL REQUIRED)
	y_sources(yttrium "src/audio/_openal"
		audio/_openal/backend.cpp
		audio/_openal/backend.h
		audio/_openal/format.cpp
		audio/_openal/format.h
		audio/_openal/player.cpp
		audio/_openal/player.h
		audio/_openal/sound.cpp
		audio/_openal/sound.h
		)
	target_include_directories(yttrium SYSTEM PRIVATE ${OPENAL_INCLUDE_DIR})
	target_link_libraries(yttrium PRIVATE ${OPENAL_LIBRARY})
endif()
if(Y_USE_OGG_VORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(yttrium PRIVATE Y3::vorbisfile)
	else()
		find_library(OGG_LIBRARY ogg)
		find_library(VORBIS_LIBRARY vorbis)
		find_library(VORBISFILE_LIBRARY vorbisfile)
		target_link_libraries(yttrium PRIVATE ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})
	endif()
	y_sources(yttrium "src/audio/formats"
		audio/formats/ogg_vorbis.cpp
		audio/formats/ogg_vorbis.h
		)
endif()

########################################################################################################################
# gui
y_sources(yttrium "include/gui"
	../../include/yttrium/gui/gui.h
	../../include/yttrium/gui/text_capture.h
	../../include/yttrium/gui/texture_font.h
	)
y_sources(yttrium "src/gui"
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
y_sources(yttrium "src/gui/ion"
	gui/ion/loader.cpp
	gui/ion/loader.h
	)
y_sources(yttrium "src/gui/logic"
	gui/logic/line_editor.cpp
	gui/logic/line_editor.h
	)
y_sources(yttrium "src/gui/widgets"
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
# image
y_sources(yttrium "include"
	../../include/yttrium/image.h
	)
y_sources(yttrium "src/image"
	image/image.cpp
	image/formats.cpp
	image/formats.h
	image/utils.cpp
	image/utils.h
	)
y_sources(yttrium "src/image/formats"
	image/formats/dds.cpp
	image/formats/dds.h
	image/formats/tga.cpp
	image/formats/tga.h
	)
if(Y_USE_JPEG)
	if(WIN32)
		find_package(Y3_jpeg REQUIRED)
		target_link_libraries(yttrium PRIVATE Y3::jpeg)
	else()
		find_package(JPEG REQUIRED)
		target_link_libraries(yttrium PRIVATE JPEG::JPEG)
	endif()
	y_sources(yttrium "src/image/formats"
		image/formats/jpeg.cpp
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set_property(SOURCE image/formats/jpeg.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
	endif()
endif()
if(Y_USE_PNG)
	if(WIN32)
		find_package(Y3_png REQUIRED)
		target_link_libraries(yttrium PRIVATE Y3::png)
	else()
		find_package(PNG REQUIRED)
		target_link_libraries(yttrium PRIVATE PNG::PNG)
	endif()
	y_sources(yttrium "src/image/formats"
		image/formats/png.cpp
		)
endif()

########################################################################################################################
# ion
y_sources(yttrium "include/ion"
	../../include/yttrium/ion/reader.h
	../../include/yttrium/ion/writer.h
	)
y_sources(yttrium "src/ion"
	ion/reader.cpp
	ion/writer.cpp
	)

########################################################################################################################
# math
y_sources(yttrium "include/math"
	../../include/yttrium/math/color.h
	../../include/yttrium/math/euler.h
	../../include/yttrium/math/line.h
	../../include/yttrium/math/margins.h
	../../include/yttrium/math/matrix.h
	../../include/yttrium/math/plane.h
	../../include/yttrium/math/point.h
	../../include/yttrium/math/quad.h
	../../include/yttrium/math/rect.h
	../../include/yttrium/math/size.h
	../../include/yttrium/math/vector.h
	)

########################################################################################################################
# memory
y_sources(yttrium "include/memory"
	../../include/yttrium/memory/buffer_appender.h
	../../include/yttrium/memory/buffer.h
	../../include/yttrium/memory/pool.h
	)
y_sources(yttrium "src/memory"
	memory/buffer_memory.cpp
	memory/buffer_memory.h
	memory/buffer.cpp
	memory/memory.h
	memory/pool.cpp
	memory/raw.h
	)
if(Y_ENABLE_BUFFER_MEMORY_TRACKING)
	y_sources(yttrium "src/memory"
		memory/buffer_memory_tracker.cpp
		memory/buffer_memory_tracker.h
		)
endif()
if(UNIX)
	y_sources(yttrium "src/memory/_posix"
		memory/_posix/memory.cpp
		)
elseif(WIN32)
	y_sources(yttrium "src/memory/_windows"
		memory/_windows/memory.cpp
		)
endif()

########################################################################################################################
# renderer
y_sources(yttrium "include/renderer"
	../../include/yttrium/renderer/manager.h
	../../include/yttrium/renderer/material.h
	../../include/yttrium/renderer/mesh.h
	../../include/yttrium/renderer/modifiers.h
	../../include/yttrium/renderer/pass.h
	../../include/yttrium/renderer/program.h
	../../include/yttrium/renderer/texture.h
	../../include/yttrium/renderer/textured_rect.h
	)
y_sources(yttrium "src/renderer"
	renderer/backend.cpp
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
y_sources(yttrium "src/renderer/builtin"
	renderer/builtin/builtin.h
	renderer/builtin/builtin.cpp
	renderer/builtin/debug_texture.cpp
	renderer/builtin/debug_texture.h
	)
y_sources(yttrium "src/renderer/formats"
	renderer/formats/obj.cpp
	renderer/formats/obj.h
	)
if(Y_RENDERER_OPENGL)
	y_sources(yttrium "src/renderer/_opengl"
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
	add_yrc_sources(yttrium
		renderer/_opengl/2d_fs.glsl
		renderer/_opengl/2d_vs.glsl
		)
	source_group("src\\renderer\\_opengl" FILES
		renderer/_opengl/2d_fs.glsl
		renderer/_opengl/2d_vs.glsl
		)
	target_link_libraries(yttrium PRIVATE OpenGL::GL)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(SOURCE renderer/_opengl/gl.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
	endif()
elseif(Y_RENDERER_VULKAN)
	y_sources(yttrium "src/renderer/_vulkan"
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
	add_yglslc_sources(yttrium
		FRAGMENT renderer/_vulkan/2d_fs.glsl
		VERTEX renderer/_vulkan/2d_vs.glsl
		)
	source_group("src\\renderer\\_vulkan" FILES
		renderer/_vulkan/2d_fs.glsl
		renderer/_vulkan/2d_vs.glsl
		)
	target_link_libraries(yttrium PRIVATE Vulkan::Vulkan)
else()
	y_sources(yttrium "src/renderer/_null"
		renderer/_null/renderer.cpp
		renderer/_null/renderer.h
		)
endif()

########################################################################################################################
# script
y_sources(yttrium "include/script"
	../../include/yttrium/script/args.h
	../../include/yttrium/script/code.h
	../../include/yttrium/script/context.h
	../../include/yttrium/script/value.h
	)
y_sources(yttrium "src/script"
	script/args.cpp
	script/code.cpp
	script/context.cpp
	script/scanner.cpp
	script/scanner.h
	script/value.cpp
	)

########################################################################################################################
# storage
y_sources(yttrium "include/storage"
	../../include/yttrium/storage/package.h
	../../include/yttrium/storage/reader.h
	../../include/yttrium/storage/source.h
	../../include/yttrium/storage/storage.h
	../../include/yttrium/storage/temporary_file.h
	../../include/yttrium/storage/writer.h
	)
y_sources(yttrium "src/storage"
	storage/file.h
	storage/package.cpp
	storage/package.h
	storage/reader.cpp
	storage/source.cpp
	storage/source.h
	storage/storage.cpp
	storage/writer.cpp
	storage/writer.h
	)
y_sources(yttrium "src/storage/formats"
	storage/formats/ypq.cpp
	storage/formats/ypq.h
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE storage/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(UNIX)
	y_sources(yttrium "src/storage/_posix"
		storage/_posix/file.cpp
		storage/_posix/temporary_file.cpp
		storage/_posix/temporary_file.h
		)
	set_property(SOURCE storage/_posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
elseif(WIN32)
	y_sources(yttrium "src/storage/_windows"
		storage/_windows/file.cpp
		storage/_windows/temporary_file.cpp
		)
endif()

########################################################################################################################
# utils
y_sources(yttrium "include/utils"
	../../include/yttrium/utils/flags.h
	../../include/yttrium/utils/numeric.h
	../../include/yttrium/utils/string.h
	../../include/yttrium/utils/tiny_string_map.h
	)
y_sources(yttrium "src/utils"
	utils/algorithm.h
	utils/atomic_counters.h
	utils/fourcc.h
	utils/memory.h
	utils/string.cpp
	utils/string.h
	utils/tiny_string_map.cpp
	)

install(DIRECTORY ../../include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS yttrium
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
