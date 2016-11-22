add_library(yttrium SHARED
	audio/backend.cpp
	audio/formats/wav.cpp
	audio/manager.cpp
	audio/music.cpp
	audio/openal/backend.cpp
	audio/openal/format.cpp
	audio/openal/player.cpp
	audio/openal/sound.cpp
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
	renderer/gl/buffer.cpp
	renderer/gl/gl.cpp
	renderer/gl/gpu_program.cpp
	renderer/gl/renderer.cpp
	renderer/gl/texture.cpp
	renderer/gl/wrappers.cpp
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

if(TRUE)
	target_sources(yttrium PRIVATE
		image/formats/jpeg.cpp
		)
	target_link_libraries(yttrium jpeg)
endif()

if(TRUE)
	target_sources(yttrium PRIVATE
		audio/formats/ogg_vorbis.cpp
		)
	target_link_libraries(yttrium vorbisfile)
endif()

if(TRUE)
	target_sources(yttrium PRIVATE
		image/formats/png.cpp
		)
	target_link_libraries(yttrium png)
endif()

if(TRUE)
	target_sources(yttrium PRIVATE
		system/posix/dir.cpp
		system/posix/file.cpp
		system/posix/memory.cpp
		system/posix/stderr.cpp
		system/posix/temporary_file.cpp
		system/posix/time.cpp
		)
	target_link_libraries(yttrium openal)
endif()

if(TRUE)
	target_sources(yttrium PRIVATE
		system/x11/gl.cpp
		system/x11/window.cpp
		)
	target_link_libraries(yttrium GL X11 Xrandr)
endif()
