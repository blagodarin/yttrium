cmake_minimum_required(VERSION 3.3)

string(REPLACE "," ";" PACKAGES "${BUILD}")
set(BUILD_DIR ${CMAKE_BINARY_DIR}/.build)
set(CACHE_DIR ${CMAKE_BINARY_DIR}/.cache)
set(PREFIX_DIR ${CMAKE_BINARY_DIR})
if(WIN32)
  set(GENERATOR "Visual Studio 15 2017 Win64")
  set(BUILD_OPTIONS -- /nologo /verbosity:minimal)
else()
  set(GENERATOR "Unix Makefiles")
  set(BUILD_OPTIONS)
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

if("Debug" IN_LIST CONFIGS)
  set(WITH_DEBUG ON)
endif()

if(NOT "Debug" STREQUAL "${CONFIGS}")
  set(WITH_RELEASE ON)
endif()

function(y3_cmake _dir)
  cmake_parse_arguments(_arg "" "TARGET" "CONFIG;OPTIONS" ${ARGN})
  message(STATUS "[Y3] Building ${_dir}")
  execute_process(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ${BUILD_DIR}/${_dir}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=${PREFIX_DIR}/lib
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/lib
      -DCMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO=${PREFIX_DIR}/lib
      -DCMAKE_INSTALL_PREFIX=${PREFIX_DIR}
      ${_arg_OPTIONS}
    WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
  if(_arg_TARGET)
    set(_target ${_arg_TARGET})
  else()
    set(_target "install")
  endif()
  foreach(_config ${_arg_CONFIG})
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR}/${_dir} --config ${_config} --target ${_target} ${BUILD_OPTIONS}
      WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
  endforeach()
endfunction()

function(y3_download _url)
  cmake_parse_arguments(_arg "" "NAME;SHA1" "" ${ARGN})
  if(_arg_NAME)
    set(_name ${_arg_NAME})
  else()
    string(REGEX REPLACE "^.*/([^/]+)$" "\\1" _name ${_url})
  endif()
  message(STATUS "[Y3] Downloading ${_name}")
  set(_path ${CACHE_DIR}/${_name})
  if(EXISTS ${_path})
    if(_arg_SHA1)
      file(SHA1 ${_path} _existing_sha1)
      if(${_arg_SHA1} STREQUAL ${_existing_sha1})
        return()
      endif()
    endif()
    file(REMOVE ${_path})
  endif()
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
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xz ${CACHE_DIR}/${_name} WORKING_DIRECTORY ${BUILD_DIR})
endfunction()

function(y3_git_clone _url)
  cmake_parse_arguments(_arg "" "DIR" "" ${ARGN})
  if(NOT Git_FOUND)
    find_package(Git REQUIRED)
  endif()
  file(MAKE_DIRECTORY ${BUILD_DIR})
  if(_arg_DIR AND EXISTS ${BUILD_DIR}/${_arg_DIR})
    message(STATUS "[Y3] Removing ${_arg_DIR}")
    file(REMOVE_RECURSE ${BUILD_DIR}/${_arg_DIR})
  endif()
  execute_process(COMMAND ${GIT_EXECUTABLE} clone ${_url} WORKING_DIRECTORY ${BUILD_DIR})
endfunction()

function(y3_git_apply _dir _patch)
  if(NOT Git_FOUND)
    find_package(Git REQUIRED)
  endif()
  message(STATUS "[Y3] Patching ${_name}")
  execute_process(COMMAND ${GIT_EXECUTABLE} apply ${_patch} WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
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
y3_package(png REQUIRES zlib)
y3_package(catch2)
y3_package(glslang)
y3_package(ogg)
y3_package(nasm)
y3_package(openal)
y3_package(opengl)
y3_package(vulkan)
y3_package(zlib)

if("catch2" IN_LIST _y3_packages)
  set(_version "2.3.0")
  set(_package "Catch2-${_version}")
  y3_download("https://github.com/catchorg/Catch2/archive/v${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "e913061207ca04dcd3d29e49a226f8caa26304fa")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    CONFIG ${CONFIG}
    OPTIONS -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF -DCATCH_INSTALL_HELPERS=OFF -DPKGCONFIG_INSTALL_DIR=${CMAKE_BINARY_DIR}/.trash)
endif()

if("glslang" IN_LIST _y3_packages)
  set(_version "7.8.2853")
  set(_package "glslang-${_version}")
  y3_download("https://github.com/KhronosGroup/glslang/archive/${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "9b6d3734abb351e8218e31c4b08c08805f2c22fc")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    CONFIG ${CONFIGS}
    OPTIONS -DENABLE_AMD_EXTENSIONS=OFF -DENABLE_GLSLANG_BINARIES=OFF -DENABLE_HLSL=OFF -DENABLE_NV_EXTENSIONS=OFF -DENABLE_SPVREMAPPER=OFF -DENABLE_OPT=OFF)
endif()

if("ogg" IN_LIST _y3_packages)
  set(_package "ogg")
  y3_git_clone("https://git.xiph.org/ogg.git" DIR ${_package})
  y3_cmake(${_package}
    TARGET "ogg"
    CONFIG ${CONFIGS}
    OPTIONS -DCMAKE_DEBUG_POSTFIX=d)
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/ogg/config_types.h
    ${BUILD_DIR}/${_package}/include/ogg/ogg.h
    ${BUILD_DIR}/${_package}/include/ogg/os_types.h
    DESTINATION ${PREFIX_DIR}/include/ogg)
  if(WIN32 AND WITH_DEBUG)
    file(RENAME ${BUILD_DIR}/${_package}/ogg.dir/Debug/ogg.pdb ${PREFIX_DIR}/lib/oggd.pdb)
  endif()
endif()

if("nasm" IN_LIST _y3_packages)
  set(_version "2.13.03")
  set(_package "nasm-${_version}")
  y3_download("https://www.nasm.us/pub/nasm/releasebuilds/${_version}/win64/${_package}-win64.zip"
    NAME "${_package}.zip"
    SHA1 "149a814fa53980976a7fc081231f59cfbcd02543")
  y3_extract("${_package}.zip" DIR ${_package})
  set(NASM_EXECUTABLE ${BUILD_DIR}/${_package}/nasm.exe)
endif()

if("openal" IN_LIST _y3_packages)
  set(_version "1.18.2")
  set(_package "openal-soft-${_version}-bin")
  y3_download("http://openal-soft.org/openal-binaries/${_package}.zip" SHA1 "0d2edd1b77cbb998f12e064d0eefea3508446776")
  y3_extract("${_package}.zip" DIR ${_package})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include
    ${BUILD_DIR}/${_package}/libs
    DESTINATION ${PREFIX_DIR})
  if(WIN32)
    y3_download("https://openal.org/downloads/oalinst.zip" SHA1 "45e08368c6755c58902b7746ff3e51ad2df8a8b8")
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
  set(_version "1.1.82.0")
  set(_package "Vulkan-Headers-sdk-${_version}")
  y3_download("https://github.com/KhronosGroup/Vulkan-Headers/archive/sdk-${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "2b6814b0a854710b8ee470f1b4f1dbac94edc1b7")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    CONFIG ${CONFIG}
    OPTIONS -DCMAKE_INSTALL_DATADIR=${CMAKE_BINARY_DIR}/.trash)
  set(_package "Vulkan-Loader-sdk-${_version}")
  y3_download("https://github.com/KhronosGroup/Vulkan-Loader/archive/sdk-${_version}.tar.gz"
    NAME "${_package}.tar.gz"
    SHA1 "f8c25d4821ab04a145b7896a295e3a71da891fcb")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  # TODO: Build Vulkan loader.
endif()

if("zlib" IN_LIST _y3_packages)
  set(_version "1.2.11")
  set(_package "zlib-${_version}")
  y3_download("https://zlib.net/${_package}.tar.xz"
    SHA1 "e1cb0d5c92da8e9a8c2635dfa249c341dfd00322")
  y3_extract("${_package}.tar.xz" DIR ${_package})
  y3_cmake(${_package}
    TARGET "zlibstatic"
    CONFIG ${CONFIGS}
    OPTIONS -DSKIP_INSTALL_FILES=ON)
  file(INSTALL
    ${BUILD_DIR}/${_package}/zconf.h
    ${BUILD_DIR}/${_package}/zlib.h
    DESTINATION ${PREFIX_DIR}/include)
  if(WIN32 AND WITH_DEBUG)
    file(RENAME ${BUILD_DIR}/${_package}/zlibstatic.dir/Debug/zlibstatic.pdb ${PREFIX_DIR}/lib/zlibstaticd.pdb)
  endif()
endif()

if("jpeg" IN_LIST _y3_packages)
  set(_version "1.5.3")
  set(_package "libjpeg-turbo-${_version}")
  y3_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz"
    SHA1 "87ebf4cab2bb27fcb8e7ccb18ec4eb680e1f2c2d")
  y3_extract("${_package}.tar.gz" DIR ${_package})
  y3_cmake(${_package}
    TARGET "jpeg-static"
    CONFIG ${CONFIGS}
    OPTIONS -DCMAKE_DEBUG_POSTFIX=d -DNASM=${NASM_EXECUTABLE} -DWITH_CRT_DLL=ON -DWITH_TURBOJPEG=OFF)
  file(INSTALL
    ${BUILD_DIR}/${_package}/jconfig.h
    ${BUILD_DIR}/${_package}/jerror.h
    ${BUILD_DIR}/${_package}/jmorecfg.h
    ${BUILD_DIR}/${_package}/jpeglib.h
    DESTINATION ${PREFIX_DIR}/include)
  if(WIN32 AND WITH_DEBUG)
    file(RENAME ${BUILD_DIR}/${_package}/jpeg-static.dir/Debug/jpeg-static.pdb ${PREFIX_DIR}/lib/jpeg-staticd.pdb)
  endif()
endif()

if("png" IN_LIST _y3_packages)
  set(_version "1.6.35")
  set(_package "libpng-${_version}")
  y3_download("https://downloads.sourceforge.net/project/libpng/libpng16/${_version}/${_package}.tar.xz"
    SHA1 "0df1561aa1da610e892239348970d574b14deed0")
  y3_extract("${_package}.tar.xz" DIR ${_package})
  y3_git_apply(${_package} ${CMAKE_CURRENT_LIST_DIR}/patches/libpng.patch)
  y3_cmake(${_package}
    TARGET "png_static"
    CONFIG ${CONFIGS}
    OPTIONS -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DSKIP_INSTALL_ALL=ON -DZLIB_INCLUDE_DIR=${PREFIX_DIR}/include)
  file(INSTALL
    ${BUILD_DIR}/${_package}/png.h
    ${BUILD_DIR}/${_package}/pngconf.h
    ${BUILD_DIR}/${_package}/pnglibconf.h
    DESTINATION ${PREFIX_DIR}/include)
  if(WIN32)
    if(WITH_RELEASE)
      file(RENAME ${PREFIX_DIR}/lib/png_static.pdb ${PREFIX_DIR}/lib/libpng16_static.pdb)
    endif()
    if(WITH_DEBUG)
      file(RENAME ${BUILD_DIR}/${_package}/png_static.dir/Debug/png_static.pdb ${PREFIX_DIR}/lib/libpng16_staticd.pdb)
    endif()
  endif()
endif()

if("vorbis" IN_LIST _y3_packages)
  set(_package "vorbis")
  y3_git_clone("https://git.xiph.org/vorbis.git" DIR ${_package})
  y3_git_apply(${_package} ${CMAKE_CURRENT_LIST_DIR}/patches/libvorbis.patch)
  y3_cmake(${_package}
    TARGET "vorbisfile"
    CONFIG ${CONFIGS}
    OPTIONS -DCMAKE_DEBUG_POSTFIX=d -DOGG_ROOT=${PREFIX_DIR})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/vorbis/codec.h
    ${BUILD_DIR}/${_package}/include/vorbis/vorbisfile.h
    DESTINATION ${PREFIX_DIR}/include/vorbis)
  if(WIN32 AND WITH_DEBUG)
    file(RENAME ${BUILD_DIR}/${_package}/lib/vorbis.dir/Debug/vorbis.pdb ${PREFIX_DIR}/lib/vorbisd.pdb)
    file(RENAME ${BUILD_DIR}/${_package}/lib/vorbisfile.dir/Debug/vorbisfile.pdb ${PREFIX_DIR}/lib/vorbisfiled.pdb)
  endif()
endif()
