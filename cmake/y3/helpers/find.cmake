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

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

function(y3_find_package _package)
  set(_one_value_args
    HEADER
    HEADER_PREFIX
    LIBRARY
    LIBRARY_DEBUG
    VERSION_HEADER
    VERSION_REGEX
    VERSION_REGEX_MAJOR
    VERSION_REGEX_MINOR
    VERSION_REGEX_PATCH
    VERSION_SUFFIX
    )

  cmake_parse_arguments(_arg "C" "${_one_value_args}" "" ${ARGN})

  if(_arg_C)
    set(_language C)
  else()
    set(_language NONE)
  endif()

  if(_arg_LIBRARY_DEBUG)
    set(_library_debug ${_arg_LIBRARY_DEBUG})
  else()
    set(_library_debug ${_arg_LIBRARY})
  endif()

  if(_arg_HEADER_PREFIX)
    find_path(${_package}_INCLUDE_DIR ${_arg_HEADER} PATH_SUFFIXES ${_arg_HEADER_PREFIX})
  else()
    find_path(${_package}_INCLUDE_DIR ${_arg_HEADER})
  endif()
  mark_as_advanced(${_package}_INCLUDE_DIR)

  if(_arg_VERSION_HEADER AND ${_package}_INCLUDE_DIR)
    set(_version_header "${${_package}_INCLUDE_DIR}/${_arg_VERSION_HEADER}")
    if(EXISTS "${_version_header}")
      if(_arg_VERSION_REGEX)
        file(STRINGS "${_version_header}" _version_string REGEX "${_arg_VERSION_REGEX}")
        string(REGEX REPLACE "${_arg_VERSION_REGEX}" "\\1" ${_package}_VERSION_STRING "${_version_string}")
      elseif(_arg_VERSION_REGEX_MAJOR AND _arg_VERSION_REGEX_MINOR AND _arg_VERSION_REGEX_PATCH)
        file(STRINGS "${_version_header}" _major_string REGEX "${_arg_VERSION_REGEX_MAJOR}")
        file(STRINGS "${_version_header}" _minor_string REGEX "${_arg_VERSION_REGEX_MINOR}")
        file(STRINGS "${_version_header}" _patch_string REGEX "${_arg_VERSION_REGEX_PATCH}")
        string(REGEX REPLACE "${_arg_VERSION_REGEX_MAJOR}" "\\1" _major "${_major_string}")
        string(REGEX REPLACE "${_arg_VERSION_REGEX_MINOR}" "\\1" _minor "${_minor_string}")
        string(REGEX REPLACE "${_arg_VERSION_REGEX_PATCH}" "\\1" _patch "${_patch_string}")
        set(${_package}_VERSION_STRING "${_major}.${_minor}.${_patch}")
      endif()
      if(${_package}_VERSION_STRING AND _arg_VERSION_SUFFIX)
        set(${_package}_VERSION_STRING "${${_package}_VERSION_STRING}-${_arg_VERSION_SUFFIX}")
      endif()
    endif()
  endif()

  find_library(${_package}_LIBRARY_RELEASE NAMES ${_arg_LIBRARY} PATH_SUFFIXES Release)
  find_library(${_package}_LIBRARY_DEBUG NAMES ${_library_debug} PATH_SUFFIXES Debug)
  select_library_configurations(${_package})
  unset(${_package}_FOUND)
  mark_as_advanced(${_package}_LIBRARY)

  find_package_handle_standard_args(${_package}
    REQUIRED_VARS ${_package}_LIBRARY ${_package}_INCLUDE_DIR
    VERSION_VAR ${_package}_VERSION_STRING)

  if(${_package}_INCLUDE_DIR AND ${_package}_LIBRARY)
    add_library(Y3::${_package} UNKNOWN IMPORTED)
    set_target_properties(Y3::${_package} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${${_package}_INCLUDE_DIR}")
    if(${_package}_LIBRARY_RELEASE)
      set_property(TARGET Y3::${_package} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS Release)
      set_target_properties(Y3::${_package} PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE ${_language}
        IMPORTED_LOCATION_RELEASE "${${_package}_LIBRARY_RELEASE}")
    endif()
    if(${_package}_LIBRARY_DEBUG)
      set_property(TARGET Y3::${_package} APPEND PROPERTY
        IMPORTED_CONFIGURATIONS Debug)
      set_target_properties(Y3::${_package} PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG ${_language}
        IMPORTED_LOCATION_DEBUG "${${_package}_LIBRARY_DEBUG}")
    endif()
  endif()
endfunction()
