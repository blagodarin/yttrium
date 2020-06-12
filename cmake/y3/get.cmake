#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2020 Sergei Blagodarin.
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

cmake_minimum_required(VERSION 3.12)

string(REPLACE "," ";" PACKAGES "${BUILD}")
set(BUILD_DIR ${CMAKE_BINARY_DIR}/.build)
if(CACHE)
	set(CACHE_DIR ${CACHE})
else()
	set(CACHE_DIR ${CMAKE_BINARY_DIR}/.cache)
endif()
set(PREFIX_DIR ${CMAKE_BINARY_DIR})

string(REPLACE "," ";" _y3_configs "${CONFIG}")
if(_y3_configs STREQUAL "")
	message(FATAL_ERROR "y3: CONFIG is required")
endif()
list(GET _y3_configs 0 _y3_config)

function(y3_run)
	cmake_parse_arguments(_arg "" "WORKING_DIRECTORY" "COMMAND" ${ARGN})
	execute_process(COMMAND ${_arg_COMMAND}
		WORKING_DIRECTORY ${_arg_WORKING_DIRECTORY}
		RESULT_VARIABLE _result)
	if(NOT _result EQUAL 0)
		message(FATAL_ERROR "Fatal error")
	endif()
endfunction()

function(y3_cmake _dir)
	cmake_parse_arguments(_arg "HEADER_ONLY;SIMPLE" "TARGET" "CL;OPTIONS" ${ARGN})
	set(_source_dir ${BUILD_DIR}/${_dir})
	if(_arg_HEADER_ONLY)
		set(_configs ${_y3_config})
	else()
		set(_configs ${_y3_configs})
	endif()
	if(_arg_TARGET)
		set(_target ${_arg_TARGET})
	else()
		set(_target "install")
	endif()
	if(_arg_CL)
		string(REPLACE ";" " " _cl "${_arg_CL}")
		set(_env_cl "CL=$ENV{CL} ${_cl}")
	endif()
	foreach(_config ${_configs})
		set(_options -DCMAKE_BUILD_TYPE=${_config} -DCMAKE_INSTALL_PREFIX=${PREFIX_DIR})
		if(NOT _arg_SIMPLE)
			if(WIN32)
				set(_output_suffix "/${_config}")
			else()
				unset(_output_suffix)
			endif()
			list(APPEND _options
				-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PREFIX_DIR}/lib${_output_suffix}
				-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PREFIX_DIR}/lib${_output_suffix}
				-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${PREFIX_DIR}/bin)
			if(WIN32)
				list(APPEND _options -DCMAKE_COMPILE_PDB_OUTPUT_DIRECTORY=${PREFIX_DIR}/lib${_output_suffix})
			endif()
		endif()
		if(PIC)
			list(APPEND _options -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
		endif()
		set(_build_dir ${_source_dir}-${_config})
		if(EXISTS ${_build_dir})
			message(STATUS "[Y3] Removing ${_dir}-${_config}")
			file(REMOVE_RECURSE ${_build_dir})
		endif()
		message(STATUS "[Y3] Building ${_dir} (${_config})")
		file(MAKE_DIRECTORY ${_build_dir})
		y3_run(COMMAND ${CMAKE_COMMAND} -E env RCFLAGS=/nologo ${CMAKE_COMMAND} -G Ninja ${_source_dir} ${_options} ${_arg_OPTIONS}
			WORKING_DIRECTORY ${_build_dir})
		y3_run(COMMAND ${CMAKE_COMMAND} -E env ${_env_cl} ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target}
			WORKING_DIRECTORY ${_build_dir})
	endforeach()
endfunction()

function(y3_download _url)
	cmake_parse_arguments(_arg "" "NAME;SHA1" "" ${ARGN})
	if(_arg_NAME)
		set(_name ${_arg_NAME})
	else()
		string(REGEX REPLACE "^.*/([^/]+)$" "\\1" _name ${_url})
	endif()
	set(_path ${CACHE_DIR}/${_name})
	if(EXISTS ${_path})
		if(_arg_SHA1)
			file(SHA1 ${_path} _existing_sha1)
			if(${_arg_SHA1} STREQUAL ${_existing_sha1})
				message(STATUS "[Y3] Found ${_name}")
				return()
			endif()
		endif()
		file(REMOVE ${_path})
	endif()
	message(STATUS "[Y3] Downloading ${_name}")
	if(_arg_SHA1)
		file(DOWNLOAD ${_url} ${_path} TLS_VERIFY ON EXPECTED_HASH SHA1=${_arg_SHA1})
	else()
		file(DOWNLOAD ${_url} ${_path} TLS_VERIFY ON)
	endif()
endfunction()

function(y3_extract _name)
	cmake_parse_arguments(_arg "" "DIR;TO" "" ${ARGN})
	if(_arg_TO)
		set(_target_dir ${CMAKE_BINARY_DIR}/${_arg_TO})
	else()
		set(_target_dir ${BUILD_DIR})
	endif()
	file(MAKE_DIRECTORY ${_target_dir})
	if(_arg_DIR)
		set(_output_dir ${_target_dir}/${_arg_DIR})
		if(EXISTS ${_output_dir})
			message(STATUS "[Y3] Removing ${_arg_DIR}")
			file(REMOVE_RECURSE ${_output_dir})
		endif()
	endif()
	message(STATUS "[Y3] Extracting ${_name}")
	y3_run(COMMAND ${CMAKE_COMMAND} -E tar xz ${CACHE_DIR}/${_name}
		WORKING_DIRECTORY ${_target_dir})
endfunction()

function(y3_git_clone _url)
	cmake_parse_arguments(_arg "" "DIR;COMMIT" "" ${ARGN})
	if(NOT _arg_DIR)
		message(FATAL_ERROR "y3_git_clone: DIR is required")
	endif()
	if(NOT Git_FOUND)
		find_package(Git REQUIRED)
	endif()
	file(MAKE_DIRECTORY ${BUILD_DIR})
	set(_dir ${BUILD_DIR}/${_arg_DIR})
	if(EXISTS ${_dir})
		message(STATUS "[Y3] Removing ${_arg_DIR}")
		file(REMOVE_RECURSE ${_dir})
	endif()
	unset(_clone_options)
	if(_arg_COMMIT)
		list(APPEND _clone_options --no-checkout)
	else()
		list(APPEND _clone_options --depth 1)
	endif()
	y3_run(COMMAND ${GIT_EXECUTABLE} clone ${_clone_options} ${_url} ${_arg_DIR}
		WORKING_DIRECTORY ${BUILD_DIR})
	if(_arg_COMMIT)
		y3_run(COMMAND ${GIT_EXECUTABLE} checkout ${_arg_COMMIT}
			WORKING_DIRECTORY ${_dir})
	endif()
endfunction()

function(y3_git_apply _dir _patch)
	if(NOT Git_FOUND)
		find_package(Git REQUIRED)
	endif()
	message(STATUS "[Y3] Patching ${_dir}")
	y3_run(COMMAND ${GIT_EXECUTABLE} apply ${_patch}
		WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
endfunction()

function(y3_bootstrap)
	if(WIN32)
		set(_version "1.9.0")
		set(_package "ninja-${_version}")
		y3_download("https://github.com/ninja-build/ninja/releases/download/v${_version}/ninja-win.zip"
			NAME "${_package}.zip"
			SHA1 "c68f192e85a12927443bbf535d27b4aa830e7b32")
		y3_extract("${_package}.zip" TO "bin")
		set(ENV{PATH} "${PREFIX_DIR}/bin;$ENV{PATH}")
		find_program(NINJA_EXECUTABLE ninja)
		if(NINJA_EXECUTABLE)
			message(STATUS "[Y3] Found Ninja: ${NINJA_EXECUTABLE}")
		endif()
	endif()
endfunction()

macro(_y3_add_package _package)
	if(_y3_packages)
		set(_y3_packages "${_y3_packages};${_package}" PARENT_SCOPE)
	else()
		set(_y3_packages "${_package}" PARENT_SCOPE)
	endif()
endmacro()

function(y3_package _package)
	cmake_parse_arguments(_arg "" "" "REQUIRES" ${ARGN})
	if("all" IN_LIST PACKAGES)
		_y3_add_package(${_package})
	elseif(${_package} IN_LIST PACKAGES)
		if(NOT ONLY)
			foreach(_dependency ${_arg_REQUIRES})
				if(NOT ${_dependency} IN_LIST _y_packages)
					list(APPEND _dependencies ${_dependency})
				endif()
			endforeach()
			if(_dependencies)
				set(PACKAGES "${PACKAGES};${_dependencies}" PARENT_SCOPE)
			endif()
		endif()
		_y3_add_package(${_package})
	endif()
endfunction()

y3_package(vorbis REQUIRES ogg)
y3_package(jpeg REQUIRES nasm)
y3_package(aulos)
y3_package(catch2)
y3_package(cppcheck)
y3_package(freetype)
y3_package(glslang)
y3_package(ogg)
y3_package(lcov)
y3_package(nasm)
y3_package(openal)
y3_package(opengl)
y3_package(vulkan)

y3_bootstrap()

if("aulos" IN_LIST _y3_packages)
	set(_version "0.0.4")
	set(_package "aulos-${_version}")
	y3_git_clone("https://github.com/blagodarin/aulos.git" DIR ${_package} TAG "f328318f0a97906927f5c0dc1225081f8c91a1b4") # 0.0.4 with fixed CMake scripts.
	y3_cmake(${_package} SIMPLE)
endif()

if("catch2" IN_LIST _y3_packages)
	set(_version "2.12.2")
	set(_package "Catch2-${_version}")
	y3_download("https://github.com/catchorg/Catch2/archive/v${_version}.tar.gz"
		NAME "${_package}.tar.gz"
		SHA1 "3244a6cd2d63498c3dd1f46c6326a6fac5ab110a")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_cmake(${_package} HEADER_ONLY
		OPTIONS -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF -DCATCH_INSTALL_HELPERS=OFF -DPKGCONFIG_INSTALL_DIR=${CMAKE_BINARY_DIR}/.trash)
endif()

if("cppcheck" IN_LIST _y3_packages)
	set(_version "1.90")
	set(_package "cppcheck-${_version}")
	y3_download("https://github.com/danmar/cppcheck/archive/${_version}.tar.gz"
		NAME "${_package}.tar.gz"
		SHA1 "852d69a60b6caf023a3420943ebdc29e99fd0349")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_cmake(${_package}
		OPTIONS -DWARNINGS_ANSI_ISO=OFF)
endif()

if("freetype" IN_LIST _y3_packages)
	set(_version "2.10.2")
	set(_package "freetype-${_version}")
	y3_download("https://downloads.sourceforge.net/project/freetype/freetype2/${_version}/${_package}.tar.gz"
		SHA1 "2c53944cd7eaefb9cb207672d8a4368c31aa97c4")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_cmake(${_package}
		OPTIONS
			-DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_BZip2=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_PNG=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON
			-DDISABLE_FORCE_DEBUG_POSTFIX=ON
			-DSKIP_INSTALL_LIBRARIES=ON
		CL -wd4018 -wd4244 -wd4267 -wd4312)
endif()

if("glslang" IN_LIST _y3_packages)
	set(_version "8.13.3743")
	set(_package "glslang-${_version}")
	y3_download("https://github.com/KhronosGroup/glslang/archive/${_version}.tar.gz"
		NAME "${_package}.tar.gz"
		SHA1 "1ff733e45b0dfea91278d263e409e37044a88cce")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_cmake(${_package}
		OPTIONS -DBUILD_EXTERNAL=OFF -DENABLE_CTEST=OFF -DENABLE_GLSLANG_BINARIES=OFF -DENABLE_HLSL=OFF -DENABLE_RTTI=ON -DENABLE_SPVREMAPPER=OFF -DENABLE_OPT=OFF)
endif()

if("ogg" IN_LIST _y3_packages)
	set(_version "1.3.4")
	set(_package "libogg-${_version}")
	y3_download("http://downloads.xiph.org/releases/ogg/${_package}.tar.xz"
		SHA1 "f07499a35566aa62affb5ca989f62eed5b8092c3")
	y3_extract("${_package}.tar.xz" DIR ${_package})
	y3_cmake(${_package} TARGET "ogg")
	file(INSTALL
		${BUILD_DIR}/${_package}-${_y3_config}/include/ogg/config_types.h
		${BUILD_DIR}/${_package}/include/ogg/ogg.h
		${BUILD_DIR}/${_package}/include/ogg/os_types.h
		DESTINATION ${PREFIX_DIR}/include/ogg)
endif()

if("lcov" IN_LIST _y3_packages)
	set(_version "1.14")
	set(_package "lcov-${_version}")
	y3_download("https://downloads.sourceforge.net/project/ltp/Coverage%20Analysis/LCOV-${_version}/${_package}.tar.gz"
		SHA1 "5570beba61685b7eadf9351972e164dabaf24f9d")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_run(COMMAND make install PREFIX=${PREFIX_DIR} CFG_DIR=${CMAKE_BINARY_DIR}/.trash MAN_DIR=${CMAKE_BINARY_DIR}/.trash
		WORKING_DIRECTORY ${BUILD_DIR}/${_package})
endif()

if("nasm" IN_LIST _y3_packages)
	set(_version "2.14.02")
	set(_package "nasm-${_version}")
	if(WIN32)
		y3_download("https://www.nasm.us/pub/nasm/releasebuilds/${_version}/win64/${_package}-win64.zip"
			NAME "${_package}.zip"
			SHA1 "d027f30446a96fa842335713c773e81ca8cff793")
		y3_extract("${_package}.zip" DIR ${_package})
		set(NASM_EXECUTABLE ${BUILD_DIR}/${_package}/nasm.exe)
	endif()
endif()

if("opengl" IN_LIST _y3_packages)
	y3_download("https://khronos.org/registry/OpenGL/api/GL/glext.h")
	y3_download("https://khronos.org/registry/OpenGL/api/GL/wglext.h")
	y3_download("https://khronos.org/registry/EGL/api/KHR/khrplatform.h")
	file(INSTALL
		${CACHE_DIR}/glext.h
		${CACHE_DIR}/wglext.h
		DESTINATION ${PREFIX_DIR}/include/GL)
	file(INSTALL
		${CACHE_DIR}/khrplatform.h
		DESTINATION ${PREFIX_DIR}/include/KHR)
endif()

if("vulkan" IN_LIST _y3_packages)
	set(_version "1.2.135.0")
	set(_package "Vulkan-Headers-sdk-${_version}")
	y3_download("https://github.com/KhronosGroup/Vulkan-Headers/archive/sdk-${_version}.tar.gz"
		NAME "${_package}.tar.gz"
		SHA1 "a40b7c099295151c3e8aa6e689631cbd33ded393")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	y3_cmake(${_package} HEADER_ONLY)
	set(_package "Vulkan-Loader-sdk-${_version}")
	y3_download("https://github.com/KhronosGroup/Vulkan-Loader/archive/sdk-${_version}.tar.gz"
		NAME "${_package}.tar.gz"
		SHA1 "d3d274958883f9314b79263017e05df4f0aec4ed")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	if(WIN32)
		y3_cmake(${_package} TARGET "vulkan"
			OPTIONS -DVULKAN_HEADERS_INSTALL_DIR=${PREFIX_DIR})
		foreach(_config ${_y3_configs})
			file(INSTALL
				${BUILD_DIR}/${_package}/loader/${_config}/vulkan-1.dll
				${BUILD_DIR}/${_package}/loader/${_config}/vulkan-1.pdb
				DESTINATION ${PREFIX_DIR}/bin/${_config})
			file(INSTALL
				${BUILD_DIR}/${_package}/loader/${_config}/vulkan-1.lib
				DESTINATION ${PREFIX_DIR}/lib/${_config})
		endforeach()
	else()
		y3_cmake(${_package}
			OPTIONS -DBUILD_WSI_WAYLAND_SUPPORT=OFF -DBUILD_WSI_XLIB_SUPPORT=OFF -DVULKAN_HEADERS_INSTALL_DIR=${PREFIX_DIR})
	endif()
	file(REMOVE_RECURSE ${PREFIX_DIR}/share)
endif()

if("jpeg" IN_LIST _y3_packages)
	set(_version "2.0.4")
	set(_package "libjpeg-turbo-${_version}")
	y3_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz"
		SHA1 "163d8f96d0999526a117de0388624241b54dcd67")
	y3_extract("${_package}.tar.gz" DIR ${_package})
	set(_options -DENABLE_SHARED=OFF -DREQUIRE_SIMD=ON -DWITH_ARITH_DEC=OFF -DWITH_ARITH_ENC=OFF -DWITH_TURBOJPEG=OFF)
	if(WIN32)
		list(APPEND _options -DCMAKE_ASM_NASM_COMPILER=${NASM_EXECUTABLE} -DWITH_CRT_DLL=ON)
	endif()
	y3_cmake(${_package} TARGET "jpeg-static"
		OPTIONS ${_options})
	file(INSTALL
		${BUILD_DIR}/${_package}-${_y3_config}/jconfig.h
		${BUILD_DIR}/${_package}/jerror.h
		${BUILD_DIR}/${_package}/jmorecfg.h
		${BUILD_DIR}/${_package}/jpeglib.h
		DESTINATION ${PREFIX_DIR}/include)
endif()

if("vorbis" IN_LIST _y3_packages)
	set(_package "vorbis")
	y3_git_clone("git://git.xiph.org/vorbis.git" DIR ${_package})
	y3_git_apply(${_package} ${CMAKE_CURRENT_LIST_DIR}/patches/vorbis.patch)
	y3_cmake(${_package} TARGET "vorbisfile"
		OPTIONS -DOGG_ROOT=${PREFIX_DIR}
		CL -wd4244 -wd4267 -wd4305 -wd4996)
	file(INSTALL
		${BUILD_DIR}/${_package}/include/vorbis/codec.h
		${BUILD_DIR}/${_package}/include/vorbis/vorbisfile.h
		DESTINATION ${PREFIX_DIR}/include/vorbis)
endif()
