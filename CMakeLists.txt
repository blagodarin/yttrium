# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)

project(Yttrium VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
include(GNUInstallDirs)

if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(CMAKE_FOLDER "Yttrium")
	set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

set(AULOS_AULOSPLAY ON)
set(AULOS_STATIC_RUNTIME ON)
FetchContent_Declare(Primal GIT_REPOSITORY https://github.com/blagodarin/primal.git GIT_TAG d01b475e5b263bd7e53f7b691f21f32b22bc03b3)
FetchContent_Declare(Aulos GIT_REPOSITORY https://github.com/blagodarin/aulos.git GIT_TAG ac2ce48b5cd6f8a5aa6acc1e022d7b97866377fb)
FetchContent_MakeAvailable(Primal Aulos)

primal_set_defaults()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include(cmake/utils.cmake)

option(YTTRIUM_WITH_JPEG "Enable JPEG support (requires libjpeg)" ON)
option(YTTRIUM_WITH_OGGVORBIS "Enable Ogg Vorbis support (requires libvorbisfile)" ON)
if(NOT YTTRIUM_SUBPROJECT)
	option(YTTRIUM_CLANG_TIDY "Enable running ClangTidy ('tidy' target)" OFF)
	option(YTTRIUM_SANITIZERS "Enable sanitizers" OFF)
	option(YTTRIUM_TESTS "Build tests" OFF)
	mark_as_advanced(YTTRIUM_CLANG_TIDY YTTRIUM_SANITIZERS)
endif()

primal_provide_fmt(fmt_ROOT STATIC_RUNTIME ON)
find_package(fmt REQUIRED)
if(WIN32)
	primal_provide_freetype(Freetype_ROOT STATIC_RUNTIME ON)
else()
	find_package(PkgConfig REQUIRED)
endif()
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)
if(YTTRIUM_WITH_JPEG)
	if(WIN32)
		primal_provide_jpeg(JPEG_ROOT STATIC_RUNTIME ON)
	endif()
	find_package(JPEG REQUIRED)
endif()
if(YTTRIUM_WITH_OGGVORBIS)
	if(WIN32)
		primal_provide_ogg(Ogg_ROOT STATIC_RUNTIME ON)
		primal_provide_vorbis(Vorbis_ROOT STATIC_RUNTIME ON)
		find_package(Vorbis REQUIRED)
	else()
		pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
		add_library(Vorbis::vorbisfile ALIAS PkgConfig::vorbisfile)
	endif()
endif()

get_target_architecture(_target_arch)
if(_target_arch MATCHES "^amd64$")
	set(Y_ARCH_AMD64 ON)
endif()
if(_target_arch MATCHES "^amd64|x86$")
	set(Y_ARCH_X86 ON)
endif()
message(STATUS "Target architecture: ${_target_arch}")

set(YTTRIUM_RENDERER "opengl" CACHE STRING "Renderer backend")
set_property(CACHE YTTRIUM_RENDERER PROPERTY STRINGS "opengl" "vulkan" "none")
if(YTTRIUM_RENDERER STREQUAL "opengl")
	find_package(OpenGL REQUIRED)
	set(Y_RENDERER_OPENGL ON)
elseif(YTTRIUM_RENDERER STREQUAL "vulkan")
	find_package(Vulkan REQUIRED)
	if(NOT TARGET Vulkan::glslc)
		message(SEND_ERROR "Could not find glslc, please install Vulkan SDK")
	endif()
	set(Y_RENDERER_VULKAN ON)
else()
	set(YTTRIUM_RENDERER "none")
	set(Y_RENDERER_NULL ON)
endif()
message(STATUS "Renderer: ${YTTRIUM_RENDERER}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(Y_ARCH_X86 AND NOT Y_ARCH_AMD64)
		add_compile_options(-msse2)
	endif()

	if(YTTRIUM_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute)
		list(APPEND DYNAMIC_LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
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
	if(Y_ARCH_X86 AND NOT Y_ARCH_AMD64)
		add_compile_options(-msse2)
	endif()

	option(YTTRIUM_COVERAGE "Build with coverage analysis support" OFF)
	if(YTTRIUM_COVERAGE)
		add_compile_options(--coverage)
		list(APPEND DYNAMIC_LINKER_FLAGS --coverage)
	endif()

	option(YTTRIUM_PROFILING "Build with profiling support" OFF)
	if(YTTRIUM_PROFILING)
		add_compile_options(-pg)
		list(APPEND DYNAMIC_LINKER_FLAGS -pg)
	endif()

	mark_as_advanced(YTTRIUM_COVERAGE YTTRIUM_PROFILING)

	if(YTTRIUM_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute)
		if(BUILD_SHARED_LIBS)
			add_compile_options(-fno-sanitize=vptr)
		endif()
		list(APPEND DYNAMIC_LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
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
	if(Y_ARCH_X86 AND NOT Y_ARCH_AMD64)
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

configure_file(cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)

add_subdirectory(libs/core)
add_subdirectory(libs/application)
add_subdirectory(libs/audio)
add_subdirectory(libs/gui)
add_subdirectory(libs/main)

add_subdirectory(tools/yrc)
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

add_subdirectory(libs/engine)

if(YTTRIUM_TESTS)
	enable_testing()
	add_subdirectory(libs/test)
	add_subdirectory(libs/application/tests)
	add_subdirectory(libs/audio/tests)
	add_subdirectory(libs/core/tests)
	add_subdirectory(libs/core/tests/bootstrap)
	add_subdirectory(libs/engine/tests)
endif()

if(YTTRIUM_CLANG_TIDY)
	find_package(ClangTidy 7.0.0 REQUIRED)
	set(_cpps)
	foreach(_target ycore yengine)
		get_target_property(_source_dir ${_target} SOURCE_DIR)
		get_target_property(_sources ${_target} SOURCES)
		foreach(_source ${_sources})
			if(_source MATCHES "^.*\.cpp$" AND NOT _source STREQUAL "utils/string.cpp") # The skipped file segfaults clang-tidy.
				list(APPEND _cpps ${_source_dir}/${_source})
			endif()
		endforeach()
	endforeach()
	add_custom_target(tidy
		COMMAND ${CLANG_TIDY_EXECUTABLE} -header-filter=.* -p=${PROJECT_BINARY_DIR} -quiet ${_cpps}
		COMMENT "Running ClangTidy"
		VERBATIM)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/YttriumConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake COMPATIBILITY SameMinorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium)
install(EXPORT YttriumTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium NAMESPACE Yttrium::)
