include(TestBigEndian)

option(NO_JPEG "Exclude JPEG support")
option(NO_OGG_VORBIS "Exclude Ogg Vorbis support")
option(NO_PNG "Exclude PNG support")

test_big_endian(Y_IS_BIG_ENDIAN)
set(Y_IS_LINUX $<${CMAKE_SYSTEM_NAME} STREQUAL Linux>)
set(Y_IS_POSIX ${UNIX})
set(Y_IS_WINDOWS ${WIN32})
set(Y_NO_JPEG ${NO_JPEG})
set(Y_NO_OGG_VORBIS ${NO_OGG_VORBIS})
set(Y_NO_PNG ${NO_PNG})
configure_file(config.h.in config.h)

add_library(yttrium
	audio/formats/wav.cpp
	audio/manager.cpp
	audio/music.cpp
	audio/player.cpp
	audio/playlist.cpp
	audio/reader.cpp
	audio/streamer.cpp
	base/log.cpp
	base/static_string.cpp
	base/string.cpp
	base/string_format.cpp
	base/thread_buffer.cpp
	base/time.cpp
	base/tiny_string_map.cpp
	globals.cpp
	gui/gui.cpp
	gui/ion/classes.cpp
	gui/ion/loader.cpp
	gui/ion/property_loader.cpp
	gui/layer.cpp
	gui/layout.cpp
	gui/logic/line_editor.cpp
	gui/properties.cpp
	gui/texture_font.cpp
	gui/widgets/button.cpp
	gui/widgets/canvas.cpp
	gui/widgets/image.cpp
	gui/widgets/input.cpp
	gui/widgets/label.cpp
	gui/widgets/widget.cpp
	image/formats.cpp
	image/formats/dds.cpp
	image/formats/tga.cpp
	image/image.cpp
	image/utils.cpp
	ion/document.cpp
	ion/list.cpp
	ion/node.cpp
	ion/object.cpp
	ion/parser.cpp
	ion/utils.cpp
	ion/value.cpp
	math/matrix.cpp
	math/vector.cpp
	memory/buffer_memory.cpp
	memory/buffer_memory_tracker.cpp
	memory/buffer.cpp
	memory/heap_allocator.cpp
	memory/named_allocator.cpp
	memory/pool.cpp
	renderer/debug_renderer.cpp
	renderer/debug_texture.cpp
	renderer/formats/obj.cpp
	renderer/gl/buffer.cpp
	renderer/gl/gl.cpp
	renderer/gl/gpu_program.cpp
	renderer/gl/renderer.cpp
	renderer/gl/texture.cpp
	renderer/gl/wrappers.cpp
	renderer/material.cpp
	renderer/mesh_data.cpp
	renderer/modifiers.cpp
	renderer/renderer.cpp
	renderer/texture.cpp
	resources/resource_loader.cpp
	resources/resource_ptr.cpp
	resources/translation.cpp
	script/args.cpp
	script/code.cpp
	script/context.cpp
	script/scanner.cpp
	script/value.cpp
	storage/formats/ypq.cpp
	storage/package.cpp
	storage/reader.cpp
	storage/storage.cpp
	storage/writer.cpp
	window/bindings.cpp
	window/console.cpp
	window/key_lookup.cpp
	window/key.cpp
	window/window.cpp
	)
target_include_directories(yttrium PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(yttrium ${OPENGL_gl_LIBRARY} Threads::Threads)
set_target_properties(yttrium PROPERTIES DEFINE_SYMBOL YTTRIUM_EXPORT)

# OpenAL port.
find_package(OpenAL REQUIRED)
target_sources(yttrium PRIVATE
	audio/openal/backend.cpp
	audio/openal/format.cpp
	audio/openal/player.cpp
	audio/openal/sound.cpp
	)
target_include_directories(yttrium PRIVATE ${OPENAL_INCLUDE_DIR})
target_link_libraries(yttrium ${OPENAL_LIBRARY})

# JPEG support.
if(NOT NO_JPEG)
	find_package(JPEG REQUIRED)
	target_sources(yttrium PRIVATE
		image/formats/jpeg.cpp
		)
	target_include_directories(yttrium PRIVATE ${JPEG_INCLUDE_DIR})
	target_link_libraries(yttrium ${JPEG_LIBRARIES})
endif()

# Ogg Vorbis support.
if(NOT NO_OGG_VORBIS)
	# TODO: Use find_library or some third-party package.
	target_link_libraries(yttrium vorbisfile)
	target_sources(yttrium PRIVATE
		audio/formats/ogg_vorbis.cpp
		)
endif()

# PNG support.
if(NOT NO_PNG)
	find_package(PNG REQUIRED)
	target_sources(yttrium PRIVATE
		image/formats/png.cpp
		)
	target_compile_definitions(yttrium PRIVATE ${PNG_DEFINITIONS})
	target_include_directories(yttrium PRIVATE ${PNG_INCLUDE_DIRS})
	target_link_libraries(yttrium ${PNG_LIBRARIES})
endif()

# Operating system.
if(UNIX)
	target_sources(yttrium PRIVATE
		system/posix/dir.cpp
		system/posix/file.cpp
		system/posix/memory.cpp
		system/posix/stderr.cpp
		system/posix/temporary_file.cpp
		system/posix/time.cpp
		)
elseif(WIN32)
	# TODO: Add Windows support.
endif()

# Window system.
if(UNIX)
	find_package(X11 REQUIRED)
	target_sources(yttrium PRIVATE
		system/x11/gl.cpp
		system/x11/window.cpp
		)
	target_include_directories(yttrium PRIVATE ${X11_INCLUDE_DIR})
	target_link_libraries(yttrium ${X11_X11_LIB} ${X11_Xrandr_LIB})
elseif(WIN32)
	# TODO: Add GDI support.
endif()
