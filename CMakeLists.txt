#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2019 Sergei Blagodarin.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.19)

project(Yttrium VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
include(GNUInstallDirs)

set(AULOS_STATIC_RUNTIME ON)
FetchContent_Declare(Primal GIT_REPOSITORY https://github.com/blagodarin/primal.git GIT_TAG e51e653659b9b4276d483e436967c56c27df6006)
FetchContent_Declare(Aulos GIT_REPOSITORY https://github.com/blagodarin/aulos.git GIT_TAG 14d2933316def00a2d405fa6ce06842cfef42854)
FetchContent_MakeAvailable(Primal Aulos)

primal_set_defaults()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/y3/modules ${PROJECT_SOURCE_DIR}/cmake/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include(cmake/utils.cmake)

function(_y_cmake _source_dir _build_dir _install_dir)
	cmake_parse_arguments(_arg "" "TARGET" "OPTIONS" ${ARGN})
	file(REMOVE_RECURSE ${_build_dir})
	file(MAKE_DIRECTORY ${_build_dir})
	file(REMOVE_RECURSE ${_install_dir})
	set(_cmake_args ${_source_dir} -G ${CMAKE_GENERATOR})
	if(CMAKE_GENERATOR_PLATFORM)
		list(APPEND _cmake_args -A ${CMAKE_GENERATOR_PLATFORM})
	endif()
	if(CMAKE_GENERATOR_TOOLSET)
		list(APPEND _cmake_args -T ${CMAKE_GENERATOR_TOOLSET})
	endif()
	get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	if(NOT _is_multi_config AND CMAKE_BUILD_TYPE)
		list(APPEND _cmake_args -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
	endif()
	list(APPEND _cmake_args -DCMAKE_INSTALL_PREFIX=${_install_dir} -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DCMAKE_POLICY_DEFAULT_CMP0091=NEW)
	if(_arg_TARGET)
		set(_target ${_arg_TARGET})
		if(_is_multi_config)
			list(APPEND _cmake_args -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=${_install_dir}/lib -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE=${_install_dir}/lib)
		else()
			list(APPEND _cmake_args -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_install_dir}/lib)
		endif()
	else()
		set(_target "install")
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} -E env RCFLAGS=/nologo ${CMAKE_COMMAND} ${_cmake_args} ${_arg_OPTIONS} WORKING_DIRECTORY ${_build_dir} COMMAND_ERROR_IS_FATAL ANY)
	if(_is_multi_config)
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} --config Debug COMMAND_ERROR_IS_FATAL ANY)
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} --config Release COMMAND_ERROR_IS_FATAL ANY)
	else()
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} COMMAND_ERROR_IS_FATAL ANY)
	endif()
endfunction()

function(y_provide_nasm _output)
	set(_version "2.15.05")
	set(_package "nasm-${_version}")
	if(WIN32)
		primal_download("https://www.nasm.us/pub/nasm/releasebuilds/${_version}/win64/${_package}-win64.zip"
			SHA1 "f3d25401783109ec999508af4dc967facf64971a"
			NAME "${_package}.zip"
			EXTRACT_DIR "${_package}"
			RESULT _ok)
		set(${_output} ${CMAKE_BINARY_DIR}/${_package}/nasm.exe PARENT_SCOPE)
	else()
		message(FATAL_ERROR "'nasm' is only available on Windows")
	endif()
endfunction()

function(y_provide_jpeg _output)
	y_provide_nasm(NASM_EXECUTABLE)
	set(_version "2.0.6")
	set(_package "libjpeg-turbo-${_version}")
	primal_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz"
		SHA1 "5406c7676d7df89fb4da791ad5af51202910fb25"
		EXTRACT_DIR "${_package}"
		PATCH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/y3/patches/jpeg.patch"
		RESULT _ok)
	set(_install_dir ${CMAKE_BINARY_DIR}/${_package}-install)
	if(_ok)
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${CMAKE_BINARY_DIR}/${_package}-build)
		_y_cmake(${_source_dir} ${_build_dir} ${_install_dir} TARGET jpeg-static OPTIONS
			-DCMAKE_ASM_NASM_COMPILER=${NASM_EXECUTABLE}
			-DCMAKE_DEBUG_POSTFIX=d
			-DENABLE_SHARED=OFF
			-DREQUIRE_SIMD=ON
			-DWITH_ARITH_DEC=OFF
			-DWITH_ARITH_ENC=OFF
			-DWITH_CRT_DLL=ON # Doesn't work, set to ON to prevent manual flag manipulation.
			-DWITH_TURBOJPEG=OFF
			)
		file(INSTALL
			${_build_dir}/jconfig.h
			${_source_dir}/jerror.h
			${_source_dir}/jmorecfg.h
			${_source_dir}/jpeglib.h
			DESTINATION ${_install_dir}/include)
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()

function(y_provide_freetype _output)
	set(_version "2.10.4")
	set(_package "freetype-${_version}")
	primal_download("https://downloads.sourceforge.net/project/freetype/freetype2/${_version}/${_package}.tar.gz"
		SHA1 "040d6a4be23708132c85ef9df837eb3f8a04c4ab"
		EXTRACT_DIR "${_package}"
		PATCH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/y3/patches/freetype.patch"
		RESULT _ok)
	set(_install_dir ${CMAKE_BINARY_DIR}/${_package}-install)
	if(_ok)
		set(_build_dir ${CMAKE_BINARY_DIR}/${_package}-build)
		_y_cmake(${CMAKE_BINARY_DIR}/${_package} ${CMAKE_BINARY_DIR}/${_package}-build ${_install_dir} OPTIONS
			-DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_BZip2=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_PNG=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON
			)
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_CLANG_TIDY "Enable running ClangTidy ('tidy' target)" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(INSTALL_EXAMPLES "Install examples" OFF)

option(YTTRIUM_WITH_JPEG "Enable JPEG support (requires libjpeg)" ON)
option(YTTRIUM_WITH_OGGVORBIS "Enable Ogg Vorbis support (requires libvorbisfile)" ON)

if(WIN32)
	if(YTTRIUM_WITH_JPEG)
		y_provide_jpeg(JPEG_ROOT)
	endif()
	y_provide_freetype(Freetype_ROOT)
else()
	find_package(PkgConfig REQUIRED)
endif()
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)
if(YTTRIUM_WITH_JPEG)
	find_package(JPEG REQUIRED)
endif()

get_target_architecture(_target_arch)
set(TARGET_ARCH "${_target_arch}" CACHE STRING "Target architecture")
set_property(CACHE TARGET_ARCH PROPERTY STRINGS "amd64" "x86")
if(TARGET_ARCH MATCHES "^amd64$")
	set(Y_ARCH_AMD64 ON)
endif()
if(TARGET_ARCH MATCHES "^amd64|x86$")
	set(Y_ARCH_X86 ON)
endif()
message(STATUS "Target architecture: ${TARGET_ARCH}")

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

	if(ENABLE_SANITIZERS)
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

	link_libraries(stdc++fs) # Required for GCC 8 standard library which is used by Clang.
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(Y_ARCH_X86 AND NOT Y_ARCH_AMD64)
		add_compile_options(-msse2)
	endif()

	option(ENABLE_COVERAGE "Build with coverage analysis support" OFF)
	if(ENABLE_COVERAGE)
		add_compile_options(--coverage)
		list(APPEND DYNAMIC_LINKER_FLAGS --coverage)
	endif()

	option(ENABLE_PROFILING "Build with profiling support" OFF)
	if(ENABLE_PROFILING)
		add_compile_options(-pg)
		list(APPEND DYNAMIC_LINKER_FLAGS -pg)
	endif()

	if(ENABLE_SANITIZERS)
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
	add_compile_options(/std:c++latest)
	add_compile_definitions(_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING _USE_MATH_DEFINES)
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

print_current_flags(RC_FLAGS STATIC_LINKER_FLAGS DYNAMIC_LINKER_FLAGS)

append_options(CMAKE_EXE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_MODULE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_RC_FLAGS ${RC_FLAGS})
append_options(CMAKE_SHARED_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_STATIC_LINKER_FLAGS ${STATIC_LINKER_FLAGS})

configure_file(cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)

add_subdirectory(libs/core)
add_subdirectory(libs/audio)
add_subdirectory(libs/main)

add_subdirectory(tools/yrc)
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

add_subdirectory(libs/engine)

if(BUILD_TESTS)
	enable_testing()
	primal_download("https://raw.githubusercontent.com/onqtam/doctest/2.4.4/doctest/doctest.h" SHA1 "7c25b870f3e0cde6d6c8596c60f3592fe557989a")
	add_subdirectory(libs/test)
	add_subdirectory(libs/audio/tests)
	add_subdirectory(libs/core/tests)
	add_subdirectory(libs/core/tests/bootstrap)
	add_subdirectory(libs/engine/tests)
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(ENABLE_CLANG_TIDY)
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
