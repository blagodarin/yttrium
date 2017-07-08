include(TestBigEndian)

option(NO_JPEG "Exclude JPEG support")
option(NO_OGG_VORBIS "Exclude Ogg Vorbis support")
option(NO_PNG "Exclude PNG support")

set(Y_RENDERER "OpenGL" CACHE STRING "Renderer to use (OpenGL, Vulkan)")

test_big_endian(Y_IS_BIG_ENDIAN)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(Y_IS_LINUX ON)
else()
	set(Y_IS_LINUX OFF)
endif()
set(Y_IS_POSIX ${UNIX})
set(Y_IS_WINDOWS ${WIN32})
set(Y_NO_JPEG ${NO_JPEG})
set(Y_NO_OGG_VORBIS ${NO_OGG_VORBIS})
set(Y_NO_PNG ${NO_PNG})
configure_file(config.h.in config.h)

add_library(yttrium
	../include/yttrium/api.h
	../include/yttrium/console.h
	../include/yttrium/exceptions.h
	../include/yttrium/flags.h
	../include/yttrium/resource_loader.h
	../include/yttrium/std/optional.h
	../include/yttrium/std/string_view.h
	../include/yttrium/std/variant.h
	../include/yttrium/string.h
	../include/yttrium/tiny_string_map.h
	../include/yttrium/translation.h
	config.h
	console.cpp
	globals.cpp
	resource_loader.cpp
	string_utils.cpp
	string.cpp
	tiny_string_map.cpp
	translation.cpp
	)
target_link_libraries(yttrium Threads::Threads)
set_target_properties(yttrium PROPERTIES DEFINE_SYMBOL YTTRIUM_EXPORT)
if(WIN32)
	target_sources(yttrium PRIVATE
		system/windows/windows.h
		)
endif()

########################################################################################################################
# audio
target_sources(yttrium PRIVATE
	../include/yttrium/audio/format.h
	../include/yttrium/audio/manager.h
	../include/yttrium/audio/music.h
	../include/yttrium/audio/player.h
	../include/yttrium/audio/reader.h
	../include/yttrium/audio/sound.h
	audio/formats/wav_private.h
	audio/formats/wav.cpp
	audio/formats/wav.h
	audio/backend.h
	audio/manager.cpp
	audio/manager.h
	audio/music.cpp
	audio/music.h
	audio/player.cpp
	audio/player.h
	audio/reader.cpp
	audio/reader.h
	audio/streamer.cpp
	audio/streamer.h
	)
if(ON)
	find_package(OpenAL REQUIRED)
	target_sources(yttrium PRIVATE
		audio/openal/backend.cpp
		audio/openal/backend.h
		audio/openal/format.cpp
		audio/openal/format.h
		audio/openal/player.cpp
		audio/openal/player.h
		audio/openal/sound.cpp
		audio/openal/sound.h
		system/openal.h
		)
	target_include_directories(yttrium PRIVATE ${OPENAL_INCLUDE_DIR})
	target_link_libraries(yttrium ${OPENAL_LIBRARY})
endif()
if(NOT NO_OGG_VORBIS)
	find_library(OGG_LIBRARY ogg)
	find_library(VORBIS_LIBRARY vorbis)
	find_library(VORBISFILE_LIBRARY vorbisfile)
	target_link_libraries(yttrium ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})
	target_sources(yttrium PRIVATE
		audio/formats/ogg_vorbis.cpp
		audio/formats/ogg_vorbis.h
		)
endif()

########################################################################################################################
# gui
target_sources(yttrium PRIVATE
	../include/yttrium/gui/gui.h
	../include/yttrium/gui/text_capture.h
	../include/yttrium/gui/texture_font.h
	gui/ion/loader.cpp
	gui/ion/loader.h
	gui/logic/line_editor.cpp
	gui/logic/line_editor.h
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

########################################################################################################################
# image
target_sources(yttrium PRIVATE
	../include/yttrium/image.h
	image/formats/dds.cpp
	image/formats/dds.h
	image/formats/tga.cpp
	image/formats/tga.h
	image/formats.cpp
	image/formats.h
	image/image.cpp
	image/utils.cpp
	image/utils.h
	)
if(NOT NO_JPEG)
	find_package(JPEG REQUIRED)
	target_sources(yttrium PRIVATE
		image/formats/jpeg.cpp
		)
	target_include_directories(yttrium PRIVATE ${JPEG_INCLUDE_DIR})
	target_link_libraries(yttrium ${JPEG_LIBRARIES})
endif()
if(NOT NO_PNG)
	find_package(PNG REQUIRED)
	target_sources(yttrium PRIVATE
		image/formats/png.cpp
		)
	target_compile_definitions(yttrium PRIVATE ${PNG_DEFINITIONS})
	target_include_directories(yttrium PRIVATE ${PNG_INCLUDE_DIRS})
	target_link_libraries(yttrium ${PNG_LIBRARIES})
endif()

########################################################################################################################
# ion
target_sources(yttrium PRIVATE
	../include/yttrium/ion/reader.h
	../include/yttrium/ion/writer.h
	ion/reader.cpp
	ion/writer.cpp
	)

########################################################################################################################
# math
target_sources(yttrium PRIVATE
	../include/yttrium/math/color.h
	../include/yttrium/math/euler.h
	../include/yttrium/math/line.h
	../include/yttrium/math/margins.h
	../include/yttrium/math/matrix.h
	../include/yttrium/math/plane.h
	../include/yttrium/math/point.h
	../include/yttrium/math/quad.h
	../include/yttrium/math/rect.h
	../include/yttrium/math/size.h
	../include/yttrium/math/vector.h
	)

########################################################################################################################
# memory
target_sources(yttrium PRIVATE
	../include/yttrium/memory/buffer_appender.h
	../include/yttrium/memory/buffer.h
	../include/yttrium/memory/pool.h
	memory/buffer_memory.cpp
	memory/buffer_memory.h
	memory/buffer_memory_tracker.cpp
	memory/buffer_memory_tracker.h
	memory/buffer.cpp
	memory/pool.cpp
	memory/raw.h
	system/memory.h
	)
if(UNIX)
	target_sources(yttrium PRIVATE
		system/posix/memory.cpp
		)
elseif(WIN32)
	target_sources(yttrium PRIVATE
		system/windows/memory.cpp
		)
endif()

########################################################################################################################
# renderer
target_sources(yttrium PRIVATE
	../include/yttrium/renderer/gpu_program.h
	../include/yttrium/renderer/index_buffer.h
	../include/yttrium/renderer/material.h
	../include/yttrium/renderer/mesh.h
	../include/yttrium/renderer/modifiers.h
	../include/yttrium/renderer/renderer.h
	../include/yttrium/renderer/texture.h
	../include/yttrium/renderer/textured_rect.h
	../include/yttrium/renderer/vertex_buffer.h
	renderer/formats/obj.cpp
	renderer/formats/obj.h
	renderer/debug_renderer.cpp
	renderer/debug_renderer.h
	renderer/debug_texture.cpp
	renderer/debug_texture.h
	renderer/material.cpp
	renderer/material.h
	renderer/mesh_data.cpp
	renderer/mesh_data.h
	renderer/modifiers.cpp
	renderer/renderer.cpp
	renderer/renderer.h
	renderer/texture.cpp
	renderer/texture.h
	)
if(Y_RENDERER STREQUAL "OpenGL")
	find_package(OpenGL REQUIRED)
	target_sources(yttrium PRIVATE
		renderer/gl/api.h
		renderer/gl/buffer.cpp
		renderer/gl/buffer.h
		renderer/gl/gl.cpp
		renderer/gl/gl.h
		renderer/gl/gpu_program.cpp
		renderer/gl/gpu_program.h
		renderer/gl/mesh.h
		renderer/gl/renderer.cpp
		renderer/gl/renderer.h
		renderer/gl/texture.cpp
		renderer/gl/texture.h
		renderer/gl/version.h
		renderer/gl/wrappers.cpp
		renderer/gl/wrappers.h
		system/gl.h
		)
	target_compile_definitions(yttrium PRIVATE Y_RENDERER_OPENGL)
	target_include_directories(yttrium PRIVATE ${OPENGL_INCLUDE_DIR})
	target_link_libraries(yttrium ${OPENGL_gl_LIBRARY})
endif()
if(Y_RENDERER STREQUAL "Vulkan")
	find_package(Vulkan REQUIRED)
	target_sources(yttrium PRIVATE
		renderer/vulkan/buffer.h
		renderer/vulkan/gpu_program.h
		renderer/vulkan/mesh.h
		renderer/vulkan/renderer.cpp
		renderer/vulkan/renderer.h
		renderer/vulkan/texture.h
		)
	target_compile_definitions(yttrium PRIVATE Y_RENDERER_VULKAN)
	target_include_directories(yttrium PRIVATE ${Vulkan_INCLUDE_DIR})
	target_link_libraries(yttrium Vulkan::Vulkan)
endif()

########################################################################################################################
# script
target_sources(yttrium PRIVATE
	../include/yttrium/script/args.h
	../include/yttrium/script/code.h
	../include/yttrium/script/context.h
	../include/yttrium/script/value.h
	script/args.cpp
	script/code.cpp
	script/context.cpp
	script/scanner.cpp
	script/scanner.h
	script/value.cpp
	)

########################################################################################################################
# storage
target_sources(yttrium PRIVATE
	../include/yttrium/storage/package.h
	../include/yttrium/storage/reader.h
	../include/yttrium/storage/source.h
	../include/yttrium/storage/storage.h
	../include/yttrium/storage/temporary_file.h
	../include/yttrium/storage/writer.h
	storage/formats/ypq.cpp
	storage/formats/ypq.h
	storage/package.cpp
	storage/package.h
	storage/reader.cpp
	storage/source.cpp
	storage/source.h
	storage/storage.cpp
	storage/writer.cpp
	storage/writer.h
	system/file.h
	)
if(UNIX)
	target_sources(yttrium PRIVATE
		system/posix/file.cpp
		system/posix/temporary_file.cpp
		)
elseif(WIN32)
	target_sources(yttrium PRIVATE
		system/windows/file.cpp
		system/windows/temporary_file.cpp
		)
endif()

########################################################################################################################
# utils
target_sources(yttrium PRIVATE
	../include/yttrium/utils.h
	utils/atomic_counters.h
	utils/fourcc.h
	utils/unique_ptr.h
	)

########################################################################################################################
# window
target_sources(yttrium PRIVATE
	../include/yttrium/key.h
	../include/yttrium/window.h
	system/window.h
	window/backend.h
	window/key.cpp
	window/window.cpp
	)
if(UNIX)
	find_package(X11 REQUIRED)
	target_sources(yttrium PRIVATE
		system/x11/window.cpp
		system/x11/window.h
		)
	if(Y_RENDERER STREQUAL "OpenGL")
		target_sources(yttrium PRIVATE
			system/x11/glx.cpp
			system/x11/glx.h
			)
	endif()
	target_include_directories(yttrium PRIVATE ${X11_INCLUDE_DIR})
	target_link_libraries(yttrium ${X11_X11_LIB})
elseif(WIN32)
	target_sources(yttrium PRIVATE
		system/gdi/window.cpp
		system/gdi/window.h
		)
	if(Y_RENDERER STREQUAL "OpenGL")
		target_sources(yttrium PRIVATE
			system/gdi/wgl.cpp
			system/gdi/wgl.h
			)
	endif()
endif()
