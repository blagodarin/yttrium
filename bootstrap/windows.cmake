cmake_minimum_required(VERSION 3.3)

string(REPLACE "," ";" PACKAGES "${BUILD}")
set(BUILD_DIR ${CMAKE_BINARY_DIR}/.build)
set(CACHE_DIR ${CMAKE_BINARY_DIR}/.cache)
set(PREFIX_DIR ${CMAKE_BINARY_DIR})
set(GENERATOR "Visual Studio 15 2017 Win64")

function(y_cmake _dir)
  cmake_parse_arguments(_arg "" "" "OPTIONS" ${ARGN})
  execute_process(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ${BUILD_DIR}/${_dir} -DCMAKE_INSTALL_PREFIX=${PREFIX_DIR} ${_arg_OPTIONS}
    WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
  execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR}/${_dir} --config Release --target INSTALL -- /nologo /verbosity:minimal
    WORKING_DIRECTORY ${BUILD_DIR}/${_dir})
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
    file(REMOVE_RECURSE ${_arg_DIR})
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
    file(REMOVE_RECURSE ${_arg_DIR})
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
    foreach(_dependency ${_arg_REQUIRES})
      if(NOT ${_dependency} IN_LIST _y_packages)
        list(APPEND _dependencies ${_dependency})
      endif()
    endforeach()
    if(_dependencies)
      set(PACKAGES "${PACKAGES};${_dependencies}" PARENT_SCOPE)
    endif()
    _y_add_package(${_package})
  endif()
endfunction()

y_package(libvorbis REQUIRES libogg)
y_package(libjpeg REQUIRES nasm)
y_package(libpng REQUIRES nasm zlib)
y_package(libogg)
y_package(nasm)
y_package(openal)
y_package(opengl)
y_package(zlib)

if("libogg" IN_LIST _y_packages)
  y_git_clone("https://git.xiph.org/ogg.git" DIR "ogg")
  y_cmake("ogg")
endif()

if("nasm" IN_LIST _y_packages)
  set(_version "2.13.03")
  y_download("https://www.nasm.us/pub/nasm/releasebuilds/${_version}/win64/nasm-${_version}-win64.zip" SHA1 "149a814fa53980976a7fc081231f59cfbcd02543")
  y_extract("nasm-${_version}-win64.zip" DIR "nasm-${_version}")
  file(INSTALL ${BUILD_DIR}/nasm-${_version}/nasm.exe DESTINATION ${PREFIX_DIR}/bin)
endif()

if("openal" IN_LIST _y_packages)
  set(_version "1.18.2")
  y_download("http://openal-soft.org/openal-binaries/openal-soft-${_version}-bin.zip" SHA1 "0d2edd1b77cbb998f12e064d0eefea3508446776")
  y_download("https://openal.org/downloads/oalinst.zip" SHA1 "45e08368c6755c58902b7746ff3e51ad2df8a8b8")
  y_extract("openal-soft-${_version}-bin.zip")
  file(INSTALL ${BUILD_DIR}/openal-soft-${_version}-bin/include DESTINATION ${PREFIX_DIR})
  file(INSTALL ${BUILD_DIR}/openal-soft-${_version}-bin/libs DESTINATION ${PREFIX_DIR})
  y_extract("oalinst.zip")
  execute_process(COMMAND ${BUILD_DIR}/oalinst.exe /s)
endif()

if("opengl" IN_LIST _y_packages)
  y_download("https://khronos.org/registry/OpenGL/api/GL/glext.h")
  y_download("https://khronos.org/registry/OpenGL/api/GL/wglext.h")
  y_download("https://khronos.org/registry/EGL/api/KHR/khrplatform.h")
  file(INSTALL ${CACHE_DIR}/glext.h DESTINATION ${PREFIX_DIR}/include/GL)
  file(INSTALL ${CACHE_DIR}/wglext.h DESTINATION ${PREFIX_DIR}/include/GL)
  file(INSTALL ${CACHE_DIR}/khrplatform.h DESTINATION ${PREFIX_DIR}/include/KHR)
endif()

if("zlib" IN_LIST _y_packages)
  set(_version "1.2.11")
  y_download("https://zlib.net/zlib-${_version}.tar.xz" SHA1 "e1cb0d5c92da8e9a8c2635dfa249c341dfd00322")
  y_extract("zlib-${_version}.tar.xz" DIR "zlib-${_version}")
  y_cmake("zlib-${_version}" OPTIONS -DSKIP_INSTALL_FILES=ON)
endif()

if("libjpeg" IN_LIST _y_packages)
  set(_version "1.5.3")
  y_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/libjpeg-turbo-${_version}.tar.gz" SHA1 "87ebf4cab2bb27fcb8e7ccb18ec4eb680e1f2c2d")
  y_extract("libjpeg-turbo-${_version}.tar.gz" DIR "libjpeg-turbo-${_version}")
  y_cmake("libjpeg-turbo-${_version}" OPTIONS -DWITH_TURBOJPEG=OFF)
endif()

if("libpng" IN_LIST _y_packages)
  set(_version "1.6.35")
  y_download("https://downloads.sourceforge.net/project/libpng/libpng16/${_version}/libpng-${_version}.tar.xz" SHA1 "0df1561aa1da610e892239348970d574b14deed0")
  y_extract("libpng-${_version}.tar.xz" DIR "libpng-${_version}")
  y_cmake("libpng-${_version}" OPTIONS -DPNG_TESTS=OFF -DSKIP_INSTALL_EXECUTABLES=ON -DSKIP_INSTALL_EXPORT=ON -DSKIP_INSTALL_FILES=ON -DSKIP_INSTALL_PROGRAMS=ON)
endif()

if("libvorbis" IN_LIST _y_packages)
  y_git_clone("https://git.xiph.org/vorbis.git" DIR "vorbis")
  y_cmake("vorbis" OPTIONS -DOGG_ROOT=${PREFIX_DIR})
endif()
