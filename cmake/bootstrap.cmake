cmake_minimum_required(VERSION 3.3)

string(REPLACE "," ";" PACKAGES "${BUILD}")
set(BUILD_DIR ${CMAKE_BINARY_DIR}/.build)
set(CACHE_DIR ${CMAKE_BINARY_DIR}/.cache)
set(PREFIX_DIR ${CMAKE_BINARY_DIR})
if(WIN32)
  set(GENERATOR "Visual Studio 15 2017 Win64")
  set(CONFIGS RelWithDebInfo Debug)
  set(BUILD_OPTIONS -- /nologo /verbosity:minimal)
else()
  set(GENERATOR "Unix Makefiles")
  set(CONFIGS Release)
  set(BUILD_OPTIONS)
endif()

function(y_cmake _dir)
  cmake_parse_arguments(_arg "" "TARGET" "CONFIG;OPTIONS" ${ARGN})
  execute_process(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ${BUILD_DIR}/${_dir} -DCMAKE_INSTALL_PREFIX=${PREFIX_DIR} ${_arg_OPTIONS}
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

function(y_download _url)
  cmake_parse_arguments(_arg "" "SHA1" "" ${ARGN})
  string(REGEX REPLACE "^.*/([^/]+)$" "\\1" _name ${_url})
  message(STATUS "Downloading: ${_name}")
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

function(y_extract _name)
  cmake_parse_arguments(_arg "" "DIR" "" ${ARGN})
  file(MAKE_DIRECTORY ${BUILD_DIR})
  if(_arg_DIR AND EXISTS ${BUILD_DIR}/${_arg_DIR})
    message(STATUS "Removing: ${_arg_DIR}")
    file(REMOVE_RECURSE ${BUILD_DIR}/${_arg_DIR})
  endif()
  message(STATUS "Extracting: ${_name}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xz ${CACHE_DIR}/${_name} WORKING_DIRECTORY ${BUILD_DIR})
endfunction()

function(y_git_clone _url)
  cmake_parse_arguments(_arg "" "DIR" "" ${ARGN})
  if(NOT Git_FOUND)
    find_package(Git REQUIRED)
  endif()
  file(MAKE_DIRECTORY ${BUILD_DIR})
  if(_arg_DIR AND EXISTS ${BUILD_DIR}/${_arg_DIR})
    message(STATUS "Removing: ${_arg_DIR}")
    file(REMOVE_RECURSE ${BUILD_DIR}/${_arg_DIR})
  endif()
  execute_process(COMMAND ${GIT_EXECUTABLE} clone ${_url} WORKING_DIRECTORY ${BUILD_DIR})
endfunction()

macro(_y_add_package _package)
  if(_y_packages)
    set(_y_packages "${_y_packages};${_package}" PARENT_SCOPE)
  else()
    set(_y_packages "${_package}" PARENT_SCOPE)
  endif()
endmacro()

function(y_package _package)
  cmake_parse_arguments(_arg "" "" "REQUIRES" ${ARGN})
  if("all" IN_LIST PACKAGES)
    _y_add_package(${_package})
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
    _y_add_package(${_package})
  endif()
endfunction()

y_package(libvorbis REQUIRES libogg)
y_package(libjpeg REQUIRES nasm)
y_package(libpng REQUIRES nasm zlib)
y_package(catch2)
y_package(libogg)
y_package(nasm)
y_package(openal)
y_package(opengl)
y_package(zlib)

if("catch2" IN_LIST _y_packages)
  set(_version "2.3.0")
  set(_package "Catch2-${_version}")
  y_download("https://github.com/catchorg/Catch2/archive/v${_version}.tar.gz" SHA1 "e913061207ca04dcd3d29e49a226f8caa26304fa")
  y_extract("v${_version}.tar.gz" DIR ${_package})
  y_cmake(${_package}
    CONFIG Release
    OPTIONS -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF -DCATCH_INSTALL_HELPERS=OFF -DPKGCONFIG_INSTALL_DIR=${CMAKE_BINARY_DIR}/.trash)
endif()

if("libogg" IN_LIST _y_packages)
  set(_package "ogg")
  y_git_clone("https://git.xiph.org/ogg.git" DIR ${_package})
  y_cmake(${_package}
    TARGET "ogg"
    CONFIG ${CONFIGS})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/ogg/config_types.h
    ${BUILD_DIR}/${_package}/include/ogg/ogg.h
    ${BUILD_DIR}/${_package}/include/ogg/os_types.h
    DESTINATION ${PREFIX_DIR}/include/ogg)
  if(WIN32)
    file(RENAME ${BUILD_DIR}/${_package}/Debug/ogg.lib ${BUILD_DIR}/${_package}/Debug/oggd.lib)
    file(RENAME ${BUILD_DIR}/${_package}/Debug/ogg.pdb ${BUILD_DIR}/${_package}/Debug/oggd.pdb)
    file(INSTALL
      ${BUILD_DIR}/${_package}/RelWithDebInfo/ogg.lib
      ${BUILD_DIR}/${_package}/ogg.dir/RelWithDebInfo/ogg.pdb
      DESTINATION ${PREFIX_DIR}/lib)
    file(INSTALL
      ${BUILD_DIR}/${_package}/Debug/oggd.lib
      ${BUILD_DIR}/${_package}/Debug/oggd.pdb
      DESTINATION ${PREFIX_DIR}/lib)
  endif()
endif()

if("nasm" IN_LIST _y_packages)
  set(_version "2.13.03")
  set(_package "nasm-${_version}")
  y_download("https://www.nasm.us/pub/nasm/releasebuilds/${_version}/win64/${_package}-win64.zip" SHA1 "149a814fa53980976a7fc081231f59cfbcd02543")
  y_extract("${_package}-win64.zip" DIR ${_package})
  file(INSTALL
    ${BUILD_DIR}/${_package}/nasm.exe
    DESTINATION ${PREFIX_DIR}/bin)
endif()

if("openal" IN_LIST _y_packages)
  set(_version "1.18.2")
  set(_package "openal-soft-${_version}-bin")
  y_download("http://openal-soft.org/openal-binaries/${_package}.zip" SHA1 "0d2edd1b77cbb998f12e064d0eefea3508446776")
  y_extract("${_package}.zip" DIR ${_package})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include
    ${BUILD_DIR}/${_package}/libs
    DESTINATION ${PREFIX_DIR})
  if(WIN32)
    y_download("https://openal.org/downloads/oalinst.zip" SHA1 "45e08368c6755c58902b7746ff3e51ad2df8a8b8")
    y_extract("oalinst.zip")
    execute_process(COMMAND ${BUILD_DIR}/oalinst.exe /s)
  endif()
endif()

if("opengl" IN_LIST _y_packages)
  y_download("https://khronos.org/registry/OpenGL/api/GL/glext.h")
  y_download("https://khronos.org/registry/OpenGL/api/GL/wglext.h")
  y_download("https://khronos.org/registry/EGL/api/KHR/khrplatform.h")
  file(INSTALL
    ${CACHE_DIR}/glext.h
    ${CACHE_DIR}/wglext.h
    DESTINATION ${PREFIX_DIR}/include/GL)
  file(INSTALL
    ${CACHE_DIR}/khrplatform.h
    DESTINATION ${PREFIX_DIR}/include/KHR)
endif()

if("zlib" IN_LIST _y_packages)
  set(_version "1.2.11")
  set(_package "zlib-${_version}")
  y_download("https://zlib.net/${_package}.tar.xz" SHA1 "e1cb0d5c92da8e9a8c2635dfa249c341dfd00322")
  y_extract("${_package}.tar.xz" DIR ${_package})
  y_cmake(${_package}
    CONFIG ${CONFIGS}
    OPTIONS -DSKIP_INSTALL_FILES=ON)
endif()

if("libjpeg" IN_LIST _y_packages)
  set(_version "1.5.3")
  set(_package "libjpeg-turbo-${_version}")
  y_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz" SHA1 "87ebf4cab2bb27fcb8e7ccb18ec4eb680e1f2c2d")
  y_extract("${_package}.tar.gz" DIR ${_package})
  y_cmake(${_package}
    TARGET "jpeg-static"
    CONFIG ${CONFIGS}
    OPTIONS -DWITH_CRT_DLL=ON -DWITH_TURBOJPEG=OFF)
  file(INSTALL
    ${BUILD_DIR}/${_package}/jconfig.h
    ${BUILD_DIR}/${_package}/jerror.h
    ${BUILD_DIR}/${_package}/jmorecfg.h
    ${BUILD_DIR}/${_package}/jpeglib.h
    DESTINATION ${PREFIX_DIR}/include)
  if(WIN32)
    file(RENAME ${BUILD_DIR}/${_package}/Debug/jpeg-static.lib ${BUILD_DIR}/${_package}/Debug/jpeg-staticd.lib)
    file(RENAME ${BUILD_DIR}/${_package}/Debug/jpeg-static.pdb ${BUILD_DIR}/${_package}/Debug/jpeg-staticd.pdb)
    file(INSTALL
      ${BUILD_DIR}/${_package}/RelWithDebInfo/jpeg-static.lib
      ${BUILD_DIR}/${_package}/jpeg-static.dir/RelWithDebInfo/jpeg-static.pdb
      DESTINATION ${PREFIX_DIR}/lib)
    file(INSTALL
      ${BUILD_DIR}/${_package}/Debug/jpeg-staticd.lib
      ${BUILD_DIR}/${_package}/Debug/jpeg-staticd.pdb
      DESTINATION ${PREFIX_DIR}/lib)
  endif()
endif()

if("libpng" IN_LIST _y_packages)
  set(_version "1.6.35")
  set(_package "libpng-${_version}")
  y_download("https://downloads.sourceforge.net/project/libpng/libpng16/${_version}/${_package}.tar.xz" SHA1 "0df1561aa1da610e892239348970d574b14deed0")
  y_extract("${_package}.tar.xz" DIR ${_package})
  y_cmake(${_package}
    CONFIG ${CONFIGS}
    OPTIONS -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DSKIP_INSTALL_EXECUTABLES=ON -DSKIP_INSTALL_EXPORT=ON -DSKIP_INSTALL_FILES=ON -DSKIP_INSTALL_PROGRAMS=ON)
endif()

if("libvorbis" IN_LIST _y_packages)
  set(_package "vorbis")
  y_git_clone("https://git.xiph.org/vorbis.git" DIR ${_package})
  y_cmake(${_package}
    TARGET "vorbisfile"
    CONFIG ${CONFIGS}
    OPTIONS -DOGG_ROOT=${PREFIX_DIR})
  file(INSTALL
    ${BUILD_DIR}/${_package}/include/vorbis/codec.h
    ${BUILD_DIR}/${_package}/include/vorbis/vorbisfile.h
    DESTINATION ${PREFIX_DIR}/include/vorbis)
  if(WIN32)
    file(RENAME ${BUILD_DIR}/${_package}/lib/Debug/vorbis.lib ${BUILD_DIR}/${_package}/lib/Debug/vorbisd.lib)
    file(RENAME ${BUILD_DIR}/${_package}/lib/Debug/vorbis.pdb ${BUILD_DIR}/${_package}/lib/Debug/vorbisd.pdb)
    file(RENAME ${BUILD_DIR}/${_package}/lib/Debug/vorbisfile.lib ${BUILD_DIR}/${_package}/lib/Debug/vorbisfiled.lib)
    file(RENAME ${BUILD_DIR}/${_package}/lib/Debug/vorbisfile.pdb ${BUILD_DIR}/${_package}/lib/Debug/vorbisfiled.pdb)
    file(INSTALL
      ${BUILD_DIR}/${_package}/lib/RelWithDebInfo/vorbis.lib
      ${BUILD_DIR}/${_package}/lib/vorbis.dir/RelWithDebInfo/vorbis.pdb
      ${BUILD_DIR}/${_package}/lib/RelWithDebInfo/vorbisfile.lib
      ${BUILD_DIR}/${_package}/lib/vorbisfile.dir/RelWithDebInfo/vorbisfile.pdb
      DESTINATION ${PREFIX_DIR}/lib)
    file(INSTALL
      ${BUILD_DIR}/${_package}/lib/Debug/vorbisd.lib
      ${BUILD_DIR}/${_package}/lib/Debug/vorbisd.pdb
      ${BUILD_DIR}/${_package}/lib/Debug/vorbisfiled.lib
      ${BUILD_DIR}/${_package}/lib/Debug/vorbisfiled.pdb
      DESTINATION ${PREFIX_DIR}/lib)
  endif()
endif()