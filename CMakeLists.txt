# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(Yttrium VERSION 0.0.1 LANGUAGES CXX)

include(CMakeDependentOption)
include(FetchContent)
include(GNUInstallDirs)

if(PROJECT_IS_TOP_LEVEL)
	set(CMAKE_FOLDER ".subprojects")
else()
	set(CMAKE_FOLDER "Yttrium")
endif()

option(YTTRIUM_AUDIO_AULOS "Enable Aulos audio support" OFF)
option(YTTRIUM_AUDIO_OGGVORBIS "Enable Ogg Vorbis audio support (requires libvorbisfile)" OFF)
option(YTTRIUM_AUDIO_WAV "Enable WAV audio support" OFF)
option(YTTRIUM_IMAGE_BMP "Enable BMP image support" OFF)
option(YTTRIUM_IMAGE_DDS "Enable DDS image support" OFF)
option(YTTRIUM_IMAGE_ICO "Enable ICO image support" OFF)
option(YTTRIUM_IMAGE_JPEG "Enable JPEG image support (requires libjpeg)" OFF)
option(YTTRIUM_IMAGE_PNG "Enable PNG image support (write only)" OFF)
option(YTTRIUM_IMAGE_TGA "Enable TGA image support" OFF)
cmake_dependent_option(YTTRIUM_COMPRESSION_ZLIB "Enable zlib compression support" OFF "NOT YTTRIUM_IMAGE_PNG" ON)
if(PROJECT_IS_TOP_LEVEL)
	option(YTTRIUM_TESTS "Build tests" OFF)
endif()

set(SEIR_AUDIO ON)
set(SEIR_STATIC_RUNTIME ON)
FetchContent_Declare(Seir GIT_REPOSITORY https://github.com/blagodarin/seir.git GIT_TAG c4626ae4759a1c4d5a1b3e83b0d6a38f5b97b871)
FetchContent_MakeAvailable(Seir)

seir_defaults()

include(cmake/utils.cmake)

find_package(Threads REQUIRED)
if(NOT WIN32)
	find_package(PkgConfig REQUIRED)
endif()

# fmt
seir_provide_fmt(fmt_ROOT STATIC_RUNTIME ON)
find_package(fmt REQUIRED)

# Freetype
if(WIN32)
	seir_provide_freetype(Freetype_ROOT STATIC_RUNTIME ON)
endif()
find_package(Freetype REQUIRED)

# JPEG
if(YTTRIUM_IMAGE_JPEG)
	if(WIN32)
		seir_provide_jpeg(JPEG_ROOT STATIC_RUNTIME ON)
	endif()
	find_package(JPEG REQUIRED)
endif()

# Aulos
if(YTTRIUM_AUDIO_AULOS)
	set(AULOS_STATIC_RUNTIME ON)
	FetchContent_Declare(Aulos GIT_REPOSITORY https://github.com/blagodarin/aulos.git GIT_TAG ef05badf4f8152e606a2fd8e315af5bddd486c97)
	FetchContent_MakeAvailable(Aulos)
endif()

# Vorbis
if(YTTRIUM_AUDIO_OGGVORBIS)
	if(WIN32)
		seir_provide_ogg(Ogg_ROOT STATIC_RUNTIME ON)
		seir_provide_vorbis(Vorbis_ROOT STATIC_RUNTIME ON)
		find_package(Vorbis REQUIRED)
	else()
		pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
		add_library(Vorbis::vorbisfile ALIAS PkgConfig::vorbisfile)
	endif()
endif()

# ZLIB
if(YTTRIUM_COMPRESSION_ZLIB)
	if(WIN32)
		seir_provide_zlib(ZLIB_ROOT STATIC_RUNTIME ON)
	endif()
	find_package(ZLIB REQUIRED)
endif()

set(YTTRIUM_RENDERER "opengl" CACHE STRING "Renderer backend")
set_property(CACHE YTTRIUM_RENDERER PROPERTY STRINGS "opengl" "vulkan" "none")
if(YTTRIUM_RENDERER STREQUAL "opengl")
	find_package(OpenGL REQUIRED)
	set(YTTRIUM_RENDERER_OPENGL ON)
elseif(YTTRIUM_RENDERER STREQUAL "vulkan")
	find_package(Vulkan REQUIRED)
	if(NOT TARGET Vulkan::glslc)
		message(SEND_ERROR "Could not find glslc, please install Vulkan SDK")
	endif()
	set(YTTRIUM_RENDERER_VULKAN ON)
endif()
message(STATUS "Renderer: ${YTTRIUM_RENDERER}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(SEIR_ARCHITECTURE STREQUAL "x86")
		add_compile_options(-msse2)
	endif()

	# Warnings.
	add_compile_options(-Werror -Weverything
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-covered-switch-default # default label in switch which covers all enumeration values
		-Wno-ctad-maybe-unsupported
		-Wno-double-promotion
		-Wno-exit-time-destructors
		-Wno-float-equal
		-Wno-global-constructors
		-Wno-nested-anon-types
		-Wno-padded
		-Wno-return-std-move-in-c++11
		-Wno-switch-enum # enumeration value '___' not explicitly handled in switch
		-Wno-weak-vtables # '___' has no out-of-line virtual method definitions; its vtable will be emitted in every translation unit
		-Wno-zero-as-null-pointer-constant # False positives.
		)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(SEIR_ARCHITECTURE STREQUAL "x86")
		add_compile_options(-msse2)
	endif()

	# Warnings.
	add_compile_options(-Werror -Wall -Wextra
		-Wconversion
		-Wdisabled-optimization
		-Wdouble-promotion
		-Wduplicated-cond
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-include-dirs
		-Wnon-virtual-dtor
		-Wnull-dereference
		-Wold-style-cast
		-Wpedantic
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		-Wsuggest-attribute=noreturn
		-Wsuggest-override
		-Wundef
		-Wuninitialized
		-Wunsafe-loop-optimizations
		#-Wuseless-cast # Issued for casting uint64_t to size_t required on platforms with 32-bit size_t (e. g. Win32).
		#-Wzero-as-null-pointer-constant # False positives.
		)
	if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(
			-Winline # Issued for header-only code.
			)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	if(SEIR_ARCHITECTURE STREQUAL "x86")
		add_compile_options(/arch:SSE2)
	endif()
	add_compile_definitions(_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING)
	list(APPEND RC_FLAGS /nologo)

	# Warnings.
	add_compile_options(/WX /Wall
		/wd4061 # enumerator '___' in switch of enum '___' is not explicitly handled by a case label
		/wd4191 # 'reinterpret_cast': unsafe conversion from '___' to '___'
		/wd4251 # class '___' needs to have dll-interface to be used by clients of class '___'
		/wd4355 # 'this': used in base member initializer list
		/wd4365 # conversion from '___' to '___', signed/unsigned mismatch
		/wd4388 # signed/unsigned mismatch
		/wd4464 # relative include path contains '..'
		/wd4514 # unreferenced inline function has been removed
		/wd4571 # Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
		/wd4582 # constructor is not implicitly called
		/wd4583 # destructor is not implicitly called
		/wd4623 # default constructor was implicitly defined as deleted
		/wd4625 # copy constructor was implicitly defined as deleted
		/wd4626 # assignment operator was implicitly defined as deleted
		/wd4668 # '___' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		/wd4710 # function not inlined
		/wd4711 # function '___' selected for automatic inline expansion
		/wd4738 # storing 32-bit float result in memory, possible loss of performance
		/wd4774 # format string expected in argument ___ is not a string literal
		/wd4800 # Implicit conversion from '___' to bool. Possible information loss
		/wd4820 # '___' bytes padding added after data member '___'
		/wd4866 # compiler may not enforce left-to-right evaluation order for call to '___'
		/wd4868 # compiler may not enforce left-to-right evaluation order in braced initializer list
		/wd5026 # move constructor was implicitly defined as deleted
		/wd5027 # move assignment operator was implicitly defined as deleted
		/wd5039 # pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
		/wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
		/wd5220 # a non-static data member with a volatile qualified type no longer implies
		)
endif()

append_options(CMAKE_EXE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_MODULE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_RC_FLAGS ${RC_FLAGS})
append_options(CMAKE_SHARED_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_STATIC_LINKER_FLAGS ${STATIC_LINKER_FLAGS})

add_subdirectory(tools/embed)

add_subdirectory(libs/base)
add_subdirectory(libs/storage)
add_subdirectory(libs/geometry)
add_subdirectory(libs/i18n)
add_subdirectory(libs/image)
add_subdirectory(libs/ion)
add_subdirectory(libs/application)
add_subdirectory(libs/audio)
add_subdirectory(libs/renderer)
add_subdirectory(libs/gui)

add_subdirectory(tools/pack)
add_subdirectory(tools/ytr)

if(YTTRIUM_TESTS)
	seir_provide_doctest(doctest_ROOT STATIC_RUNTIME ON)
	find_package(doctest REQUIRED)
	enable_testing()
	add_subdirectory(libs/application/tests)
	add_subdirectory(libs/base/tests)
	add_subdirectory(libs/geometry/tests)
	add_subdirectory(libs/i18n/tests)
	add_subdirectory(libs/image/tests)
	add_subdirectory(libs/image/tests/bootstrap)
	add_subdirectory(libs/ion/tests)
	add_subdirectory(libs/storage/tests)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/YttriumConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake COMPATIBILITY SameMinorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium)
install(EXPORT YttriumTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium NAMESPACE Yttrium::)
