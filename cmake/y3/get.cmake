#
# Copyright 2019 Sergei Blagodarin
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

cmake_minimum_required(VERSION 3.3)

string(REPLACE "," ";" PACKAGES "${BUILD}")
set(BUILD_DIR ${CMAKE_BINARY_DIR}/.build)
if(CACHE)
  set(CACHE_DIR ${CACHE})
else()
  set(CACHE_DIR ${CMAKE_BINARY_DIR}/.cache)
endif()
set(PREFIX_DIR ${CMAKE_BINARY_DIR})
if(WIN32)
  set(DEFAULT_GENERATOR "Visual Studio 15 2017 Win64")
  set(BUILD_OPTIONS -- /nologo /verbosity:minimal)
else()
  set(DEFAULT_GENERATOR "Ninja")
  set(BUILD_OPTIONS)
endif()
if(NOT GENERATOR)
  set(GENERATOR "${DEFAULT_GENERATOR}")
endif()

if(CONFIG)
  if(NOT CONFIG STREQUAL "Debug")
    if(WIN32)
      set(CONFIG "RelWithDebInfo")
    else()
      set(CONFIG "Release")
    endif()
  endif()
  set(CONFIGS ${CONFIG})
else()
  if(WIN32)
    set(CONFIG "RelWithDebInfo")
    set(CONFIGS ${CONFIG} "Debug")
  else()
    set(CONFIG "Release")
    set(CONFIGS ${CONFIG})
  endif()
endif()

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
  cmake_parse_arguments(_arg "BUILD_TO_PREFIX;HEADER_ONLY;OUT_OF_SOURCE" "TARGET" "CL;OPTIONS" ${ARGN})
  set(_source_dir ${BUILD_DIR}/${_dir})
  if(_arg_OUT_OF_SOURCE)
    set(_build_dir ${_source_dir}-build)
    if(EXISTS ${_build_dir})
      message(STATUS "[Y3] Removing ${_dir}-build")
      file(REMOVE_RECURSE ${_build_dir})
    endif()
    file(MAKE_DIRECTORY ${_build_dir})
  else()
    set(_build_dir ${_source_dir})
  endif()
  message(STATUS "[Y3] Building ${_dir}")
  unset(_options)
  if(_arg_HEADER_ONLY)
    set(_configs ${CONFIG})
  else()
    set(_configs ${CONFIGS})
    if(_arg_BUILD_TO_PREFIX)
      if(WIN32)
        list(APPEND _options
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=${PREFIX_DIR}/lib/Debug
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/lib/Release
          -DCMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG=${PREFIX_DIR}/lib/Debug
          -DCMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/lib/Release
          -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG=${PREFIX_DIR}/lib/Debug
          -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/lib/Release
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG=${PREFIX_DIR}/bin/Debug
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/bin/Release)
      else()
        list(APPEND _options
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PREFIX_DIR}/lib
          -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PREFIX_DIR}/lib
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${PREFIX_DIR}/bin)
      endif()
    endif()
  endif()
  y3_run(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ${_source_dir}
      -DCMAKE_INSTALL_PREFIX=${PREFIX_DIR}
      ${_options}
      ${_arg_OPTIONS}
    WORKING_DIRECTORY ${_build_dir})
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
    y3_run(COMMAND ${CMAKE_COMMAND} -E env ${_env_cl} ${CMAKE_COMMAND} --build ${_build_dir} --config ${_config} --target ${_target} ${BUILD_OPTIONS}
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
  cmake_parse_arguments(_arg "" "DIR" "" ${ARGN})
  file(MAKE_DIRECTORY ${BUILD_DIR})
  if(_arg_DIR AND EXISTS ${BUILD_DIR}/${_arg_DIR})
    message(STATUS "[Y3] Removing ${_arg_DIR}")
    file(REMOVE_RECURSE ${BUILD_DIR}/${_arg_DIR})
  endif()
  message(STATUS "[Y3] Extracting ${_name}")
  y3_run(COMMAND ${CMAKE_COMMAND} -E tar xz ${CACHE_DIR}/${_name}
    WORKING_DIRECTORY ${BUILD_DIR})
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
  if(NOT _arg_COMMIT)
    list(APPEND _clone_options --depth 1)
  endif()
  y3_run(COMMAND ${GIT_EXECUTABLE} clone ${_clone_options} ${_url}
    WORKING_DIRECTORY ${BUILD_DIR})
  if(_arg_COMMIT)
    y3_run(COMMAND ${GIT_EXECUTABLE} reset --hard ${_arg_COMMIT}
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

if("catch2" IN_LIST _y3_packages)
  set(_version "2.7.1")
  set(_package "Catch2-${_version}")
  y3_download("https://github.com/catchorg/Catch2/archive/v${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "45b3f4ad38f3a5cace6edabd42099de740185237")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package} HEADER_ONLY
    OPTIONS -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF -DCATCH_INSTALL_HELPERS=OFF -DPKGCONFIG_INSTALL_DIR=${CMAKE_BINARY_DIR}/.trash)
endif()

if("cppcheck" IN_LIST _y3_packages)
  set(_version "1.88")
  set(_package "cppcheck-${_version}")
  y3_download("https://github.com/danmar/cppcheck/archive/${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "5d6b957bf4d40bf87585214019f5e50f2179fe37")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    OPTIONS -DWARNINGS_ANSI_ISO=OFF)
endif()

if("freetype" IN_LIST _y3_packages)
  set(_version "2.10.0")
  set(_package "freetype-${_version}")
  y3_download("https://downloads.sourceforge.net/project/freetype/freetype2/${_version}/${_package}.tar.bz2"
    SHA1 "f6abf03e0e3189a0de883981c57d3861b5d314f5")
  y3_extract("${_package}.tar.bz2" DIR ${_package})
  y3_cmake(${_package} BUILD_TO_PREFIX OUT_OF_SOURCE
    OPTIONS
      -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=ON
      -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON
      -DCMAKE_DISABLE_FIND_PACKAGE_PNG=ON
      -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON
      -DDISABLE_FORCE_DEBUG_POSTFIX=ON
      -DSKIP_INSTALL_LIBRARIES=ON)
endif()

if("glslang" IN_LIST _y3_packages)
  set(_version "7.11.3188")
  set(_package "glslang-${_version}")
  y3_download("https://github.com/KhronosGroup/glslang/archive/${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "296afb72c4607efeacab560e7c21236a14e93676")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    OPTIONS -DENABLE_AMD_EXTENSIONS=OFF -DENABLE_GLSLANG_BINARIES=OFF -DENABLE_HLSL=OFF -DENABLE_NV_EXTENSIONS=OFF -DENABLE_SPVREMAPPER=OFF -DENABLE_OPT=OFF)
endif()

if("ogg" IN_LIST _y3_packages)
  set(_package "ogg")
  y3_git_clone("git://git.xiph.org/ogg.git" DIR ${_package})
  y3_cmake(${_package} TARGET "ogg" BUILD_TO_PREFIX)
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/ogg/config_types.h
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

if("openal" IN_LIST _y3_packages)
  set(_version "1.19.1")
  set(_package "openal-soft-${_version}-bin")
  y3_download("http://openal-soft.org/openal-binaries/${_package}.zip"
    SHA1 "db2acacbbb6b2c3dc41a20691888e79759658e2a")
  y3_extract("${_package}.zip" DIR ${_package})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include
    ${BUILD_DIR}/${_package}/libs
    DESTINATION ${PREFIX_DIR})
  if(WIN32)
    y3_download("https://openal.org/downloads/oalinst.zip"
      SHA1 "45e08368c6755c58902b7746ff3e51ad2df8a8b8")
    y3_extract("oalinst.zip")
    execute_process(COMMAND ${BUILD_DIR}/oalinst.exe /s)
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
  set(_version "1.1.106.0")
  set(_package "Vulkan-Headers-sdk-${_version}")
  y3_download("https://github.com/KhronosGroup/Vulkan-Headers/archive/sdk-${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "805cfb84af68e21f78c991d32202015adcf09587")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package} HEADER_ONLY)
  set(_package "Vulkan-Loader-sdk-${_version}")
  y3_download("https://github.com/KhronosGroup/Vulkan-Loader/archive/sdk-${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "0950817354cd3dd182934732caf170fe6c2e1c21")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  if(WIN32)
    y3_cmake(${_package} TARGET "vulkan"
      OPTIONS -DVULKAN_HEADERS_INSTALL_DIR=${PREFIX_DIR})
    if("Debug" IN_LIST CONFIGS)
      file(INSTALL
        ${BUILD_DIR}/${_package}/loader/Debug/vulkan-1.dll
        ${BUILD_DIR}/${_package}/loader/Debug/vulkan-1.pdb
        DESTINATION ${PREFIX_DIR}/bin/Debug)
      file(INSTALL
        ${BUILD_DIR}/${_package}/loader/Debug/vulkan-1.lib
        DESTINATION ${PREFIX_DIR}/lib/Debug)
    endif()
    if("RelWithDebInfo" IN_LIST CONFIGS)
      file(INSTALL
        ${BUILD_DIR}/${_package}/loader/RelWithDebInfo/vulkan-1.dll
        ${BUILD_DIR}/${_package}/loader/RelWithDebInfo/vulkan-1.pdb
        DESTINATION ${PREFIX_DIR}/bin/Release)
      file(INSTALL
        ${BUILD_DIR}/${_package}/loader/RelWithDebInfo/vulkan-1.lib
        DESTINATION ${PREFIX_DIR}/lib/Release)
    endif()
  else()
    y3_cmake(${_package}
      OPTIONS -DBUILD_WSI_WAYLAND_SUPPORT=OFF -DBUILD_WSI_XLIB_SUPPORT=OFF -DVULKAN_HEADERS_INSTALL_DIR=${PREFIX_DIR})
  endif()
  file(REMOVE_RECURSE ${PREFIX_DIR}/share)
endif()

if("jpeg" IN_LIST _y3_packages)
  set(_version "2.0.2")
  set(_package "libjpeg-turbo-${_version}")
  y3_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz"
    SHA1 "1cff52d50b81755d0bdcf9055eb22157f39a1695")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  set(_options -DENABLE_SHARED=OFF -DREQUIRE_SIMD=ON -DWITH_ARITH_DEC=OFF -DWITH_ARITH_ENC=OFF -DWITH_TURBOJPEG=OFF)
  if(WIN32)
    list(APPEND _options -DCMAKE_ASM_NASM_COMPILER=${NASM_EXECUTABLE} -DWITH_CRT_DLL=ON)
  endif()
  y3_cmake(${_package} TARGET "jpeg-static" BUILD_TO_PREFIX
    OPTIONS ${_options})
  file(INSTALL
    ${BUILD_DIR}/${_package}/jconfig.h
    ${BUILD_DIR}/${_package}/jerror.h
    ${BUILD_DIR}/${_package}/jmorecfg.h
    ${BUILD_DIR}/${_package}/jpeglib.h
    DESTINATION ${PREFIX_DIR}/include)
endif()

if("vorbis" IN_LIST _y3_packages)
  set(_package "vorbis")
  y3_git_clone("git://git.xiph.org/vorbis.git" DIR ${_package})
  y3_git_apply(${_package} ${CMAKE_CURRENT_LIST_DIR}/patches/vorbis.patch)
  y3_cmake(${_package} TARGET "vorbisfile" BUILD_TO_PREFIX
    OPTIONS -DOGG_ROOT=${PREFIX_DIR}
    CL -wd4244 -wd4267 -wd4305 -wd4996)
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/vorbis/codec.h
    ${BUILD_DIR}/${_package}/include/vorbis/vorbisfile.h
    DESTINATION ${PREFIX_DIR}/include/vorbis)
endif()
