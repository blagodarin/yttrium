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

function(add_yglslc_sources _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	set(_multi_value_args FRAGMENT VERTEX)
	cmake_parse_arguments(_arg "" "" "${_multi_value_args}" ${ARGN})
	get_target_property(_binary_dir ${_target} BINARY_DIR)
	get_target_property(_source_dir ${_target} SOURCE_DIR)
	foreach(_source ${_arg_FRAGMENT})
		get_filename_component(_source_name ${_source} NAME)
		set(_output ${_source_name}.spirv.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND yglslc --fragment ${_source_dir}/${_source} ${_output}
			MAIN_DEPENDENCY ${_source} DEPENDS yglslc
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group("yglslc" FILES ${_binary_dir}/${_output})
	endforeach()
	foreach(_source ${_arg_VERTEX})
		get_filename_component(_source_name ${_source} NAME)
		set(_output ${_source_name}.spirv.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND yglslc --vertex ${_source_dir}/${_source} ${_output}
			MAIN_DEPENDENCY ${_source} DEPENDS yglslc
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group("yglslc" FILES ${_binary_dir}/${_output})
	endforeach()
	target_include_directories(${_target} PRIVATE ${_binary_dir})
endfunction()

function(add_ypack_target _target)
	set(_one_value_args OUTPUT INDEX)
	set(_multi_value_args DEPENDS)
	cmake_parse_arguments(_arg "" "${_one_value_args}" "${_multi_value_args}" ${ARGN})
	file(RELATIVE_PATH _output ${CMAKE_CURRENT_BINARY_DIR} ${_arg_OUTPUT})
	add_custom_target(${_target}
		ypack ${CMAKE_CURRENT_SOURCE_DIR}/${_arg_INDEX} ${_arg_OUTPUT}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_arg_INDEX}
		BYPRODUCTS ${_arg_OUTPUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating ${_output}"
		VERBATIM)
	if(_arg_DEPENDS)
		add_dependencies(${_target} ${_arg_DEPENDS})
	endif()
endfunction()

function(add_yrc_sources _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	get_target_property(_binary_dir ${_target} BINARY_DIR)
	get_target_property(_source_dir ${_target} SOURCE_DIR)
	foreach(_source ${ARGN})
		get_filename_component(_source_name ${_source} NAME)
		set(_output ${_source_name}.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND yrc --string ${_source_dir}/${_source} ${_output}
			MAIN_DEPENDENCY ${_source} DEPENDS yrc
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group("yrc" FILES ${_binary_dir}/${_output})
	endforeach()
	target_include_directories(${_target} PRIVATE ${_binary_dir})
endfunction()

function(append_options _variable)
	string(REPLACE ";" " " _value "${ARGN}")
	if(NOT "${${_variable}}" STREQUAL "")
		set(_separator " ")
	else()
		set(_separator "")
	endif()
	set(${_variable} "${${_variable}}${_separator}${_value}" PARENT_SCOPE)
endfunction()

function(get_target_architecture _variable)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -E -P ${PROJECT_SOURCE_DIR}/cmake/arch.cpp OUTPUT_VARIABLE _architecture)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} /EP /nologo ${PROJECT_SOURCE_DIR}/cmake/arch.cpp OUTPUT_VARIABLE _architecture)
		# https://developercommunity.visualstudio.com/content/problem/325122/c1356-unable-to-find-mspdbcoredll.html
		if(NOT _architecture AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
			set(_architecture "amd64")
		endif()
	endif()
	if(NOT _architecture)
		message(SEND_ERROR "Can't detect target architecture")
	endif()
	string(STRIP "${_architecture}" _architecture)
	set(${_variable} "${_architecture}" PARENT_SCOPE)
endfunction()

function(include_config _target _path)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${_target} PRIVATE /FI${_path})
	else()
		target_compile_options(${_target} PRIVATE -imacros ${_path})
	endif()
endfunction()

function(print_list)
	cmake_parse_arguments(_arg "" "PREFIX" "VALUES" ${ARGN})
	if(NOT "${_arg_VALUES}" STREQUAL "")
		string(REPLACE ";" " " text "${_arg_VALUES}")
	else()
		set(text "(none)")
	endif()
	message(STATUS "${_arg_PREFIX}${text}")
endfunction()

function(_concatenate _result _first _second)
	if("${${_first}}" STREQUAL "")
		set(${_result} "${${_first}}${${_second}}" PARENT_SCOPE)
	else()
		set(${_result} "${${_first}} ${${_second}}" PARENT_SCOPE)
	endif()
endfunction()

function(_print_config_flags _config)
	message(STATUS "  ${_config}:")
	string(TOUPPER ${_config} _CONFIG)
	_concatenate(_flags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_${_CONFIG})
	print_list(PREFIX "    CXX:                 " VALUES ${_flags})
	_concatenate(_flags CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_${_CONFIG})
	print_list(PREFIX "    EXE_LINKER:          " VALUES ${_flags})
	_concatenate(_flags CMAKE_MODULE_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS_${_CONFIG})
	print_list(PREFIX "    MODULE_LINKER:       " VALUES ${_flags})
	_concatenate(_flags CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_${_CONFIG})
	print_list(PREFIX "    SHARED_LINKER:       " VALUES ${_flags})
	_concatenate(_flags CMAKE_STATIC_LINKER_FLAGS CMAKE_STATIC_LINKER_FLAGS_${_CONFIG})
	print_list(PREFIX "    STATIC_LINKER:       " VALUES ${_flags})
endfunction()

function(print_current_flags _static_linking _dynamic_linking)
	message(STATUS "Build flags:")
	_print_config_flags("Debug")
	_print_config_flags("MinSizeRel")
	_print_config_flags("Release")
	_print_config_flags("RelWithDebInfo")
	message(STATUS "  (project):")
	get_property(_compile_definitions DIRECTORY PROPERTY COMPILE_DEFINITIONS)
	print_list(PREFIX "    COMPILE_DEFINITIONS: " VALUES ${_compile_definitions})
	get_property(_compile_options DIRECTORY PROPERTY COMPILE_OPTIONS)
	print_list(PREFIX "    COMPILE_OPTIONS:     " VALUES ${_compile_options})
	print_list(PREFIX "    STATIC_LINKER:       " VALUES ${${_static_linking}})
	print_list(PREFIX "    (other)_LINKER:      " VALUES ${${_dynamic_linking}})
endfunction()

function(y_sources _target _group)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	target_sources(${_target} PRIVATE ${ARGN})
	string(REPLACE "/" "\\" _group "${_group}")
	source_group("${_group}" FILES ${ARGN})
endfunction()
