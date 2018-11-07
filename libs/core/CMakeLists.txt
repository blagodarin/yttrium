set(_src main.cpp translation.cpp)
add_library(YCore ${_src})
source_group("src" FILES ${_src})
target_link_libraries(YCore PRIVATE Threads::Threads)
set_target_properties(YCore PROPERTIES FOLDER "libs")
if(BUILD_SHARED_LIBS)
	set_target_properties(YCore PROPERTIES DEFINE_SYMBOL Y_CORE_EXPORT)
	target_compile_definitions(YCore INTERFACE Y_CORE_IMPORT)
endif()
include_config(YCore ${PROJECT_BINARY_DIR}/config.h)

y_sources(YCore "include"
	../../include/yttrium/api.h
	../../include/yttrium/exceptions.h
	../../include/yttrium/translation.h
	)

########################################################################################################################
# audio
y_sources(YCore "include/audio"
	../../include/yttrium/audio/format.h
	../../include/yttrium/audio/reader.h
	../../include/yttrium/audio/utils.h
	)
y_sources(YCore "src/audio"
	audio/reader.cpp
	audio/reader.h
	audio/utils.cpp
	)
y_sources(YCore "src/audio/formats"
	audio/formats/wav_private.h
	audio/formats/wav.cpp
	audio/formats/wav.h
	)
if(Y_USE_OGG_VORBIS)
	if(WIN32)
		find_package(Y3_vorbisfile REQUIRED)
		target_link_libraries(YCore PRIVATE Y3::vorbisfile)
	else()
		find_library(OGG_LIBRARY ogg)
		find_library(VORBIS_LIBRARY vorbis)
		find_library(VORBISFILE_LIBRARY vorbisfile)
		target_link_libraries(YCore PRIVATE ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})
	endif()
	y_sources(YCore "src/audio/formats"
		audio/formats/ogg_vorbis.cpp
		audio/formats/ogg_vorbis.h
		)
endif()

########################################################################################################################
# image
y_sources(YCore "include"
	../../include/yttrium/image.h
	)
y_sources(YCore "src/image"
	image/image.cpp
	image/formats.cpp
	image/formats.h
	image/utils.cpp
	image/utils.h
	)
y_sources(YCore "src/image/formats"
	image/formats/dds.cpp
	image/formats/dds.h
	image/formats/tga.cpp
	image/formats/tga.h
	)
if(Y_USE_JPEG)
	if(WIN32)
		find_package(Y3_jpeg REQUIRED)
		target_link_libraries(YCore PRIVATE Y3::jpeg)
	else()
		find_package(JPEG REQUIRED)
		target_link_libraries(YCore PRIVATE JPEG::JPEG)
	endif()
	y_sources(YCore "src/image/formats"
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
		target_link_libraries(YCore PRIVATE Y3::png)
	else()
		find_package(PNG REQUIRED)
		target_link_libraries(YCore PRIVATE PNG::PNG)
	endif()
	y_sources(YCore "src/image/formats"
		image/formats/png.cpp
		)
endif()

########################################################################################################################
# ion
y_sources(YCore "include/ion"
	../../include/yttrium/ion/reader.h
	../../include/yttrium/ion/writer.h
	)
y_sources(YCore "src/ion"
	ion/reader.cpp
	ion/writer.cpp
	)

########################################################################################################################
# math
y_sources(YCore "include/math"
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
y_sources(YCore "include/memory"
	../../include/yttrium/memory/buffer_appender.h
	../../include/yttrium/memory/buffer.h
	../../include/yttrium/memory/pool.h
	)
y_sources(YCore "src/memory"
	memory/buffer_memory.cpp
	memory/buffer_memory.h
	memory/buffer.cpp
	memory/memory.h
	memory/pool.cpp
	memory/raw.h
	)
if(Y_ENABLE_BUFFER_MEMORY_TRACKING)
	y_sources(YCore "src/memory"
		memory/buffer_memory_tracker.cpp
		memory/buffer_memory_tracker.h
		)
endif()
if(UNIX)
	y_sources(YCore "src/memory/_posix"
		memory/_posix/memory.cpp
		)
elseif(WIN32)
	y_sources(YCore "src/memory/_windows"
		memory/_windows/memory.cpp
		)
endif()

########################################################################################################################
# script
y_sources(YCore "include/script"
	../../include/yttrium/script/args.h
	../../include/yttrium/script/code.h
	../../include/yttrium/script/context.h
	../../include/yttrium/script/value.h
	)
y_sources(YCore "src/script"
	script/args.cpp
	script/code.cpp
	script/context.cpp
	script/scanner.cpp
	script/scanner.h
	script/value.cpp
	)

########################################################################################################################
# storage
y_sources(YCore "include/storage"
	../../include/yttrium/storage/package.h
	../../include/yttrium/storage/reader.h
	../../include/yttrium/storage/source.h
	../../include/yttrium/storage/storage.h
	../../include/yttrium/storage/temporary_file.h
	../../include/yttrium/storage/writer.h
	)
y_sources(YCore "src/storage"
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
y_sources(YCore "src/storage/formats"
	storage/formats/ypq.cpp
	storage/formats/ypq.h
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE storage/writer.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
if(UNIX)
	y_sources(YCore "src/storage/_posix"
		storage/_posix/file.cpp
		storage/_posix/temporary_file.cpp
		storage/_posix/temporary_file.h
		)
	set_property(SOURCE storage/_posix/file.cpp APPEND PROPERTY COMPILE_DEFINITIONS _FILE_OFFSET_BITS=64)
elseif(WIN32)
	y_sources(YCore "src/storage/_windows"
		storage/_windows/file.cpp
		storage/_windows/temporary_file.cpp
		)
endif()

########################################################################################################################
# utils
y_sources(YCore "include/utils"
	../../include/yttrium/utils/flags.h
	../../include/yttrium/utils/numeric.h
	../../include/yttrium/utils/string.h
	../../include/yttrium/utils/tiny_string_map.h
	)
y_sources(YCore "src/utils"
	utils/algorithm.h
	utils/atomic_counters.h
	utils/fourcc.h
	utils/memory.h
	utils/string.cpp
	utils/string.h
	utils/tiny_string_map.cpp
	)

install(TARGETS YCore EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
