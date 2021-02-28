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
			COMMAND Vulkan::glslc -fshader-stage=fragment -mfmt=num -O -o ${_output} ${_source_dir}/${_source}
			MAIN_DEPENDENCY ${_source}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group("yglslc" FILES ${_binary_dir}/${_output})
	endforeach()
	foreach(_source ${_arg_VERTEX})
		get_filename_component(_source_name ${_source} NAME)
		set(_output ${_source_name}.spirv.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND Vulkan::glslc -fshader-stage=vertex -mfmt=num -O -o ${_output} ${_source_dir}/${_source}
			MAIN_DEPENDENCY ${_source}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group("yglslc" FILES ${_binary_dir}/${_output})
	endforeach()
	target_include_directories(${_target} PRIVATE ${_binary_dir})
endfunction()

function(yttrium_target_package _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	cmake_parse_arguments(_arg "" "INDEX;OUTPUT" "DEPENDS" ${ARGN})
	if(NOT _arg_INDEX)
		message(FATAL_ERROR "Missing INDEX")
		return()
	endif()
	if(NOT _arg_OUTPUT)
		message(FATAL_ERROR "Missing OUTPUT")
		return()
	endif()
	file(REAL_PATH ${_arg_INDEX} _absolute_index BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	file(RELATIVE_PATH _relative_index ${CMAKE_CURRENT_SOURCE_DIR} ${_absolute_index})
	file(REAL_PATH ${_arg_OUTPUT} _absolute_output BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	file(RELATIVE_PATH _relative_output ${CMAKE_CURRENT_BINARY_DIR} ${_absolute_output})
	string(MAKE_C_IDENTIFIER ${_relative_output} _output_identifier)
	add_custom_target(ypack_${_output_identifier}
		COMMAND ypack --touch ${_relative_index}
		COMMENT "Checking ${_relative_output} dependencies"
		DEPENDS ${_arg_DEPENDS}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM)
	add_dependencies(${_target} ypack_${_output_identifier})
	set_target_properties(ypack_${_output_identifier} PROPERTIES FOLDER "Yttrium")
	add_custom_command(OUTPUT ${_absolute_output}
		COMMAND ypack ${_relative_index} ${_absolute_output}
		MAIN_DEPENDENCY ${_absolute_index}
		COMMENT "Generating ${_relative_output}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM)
	target_sources(${_target} PRIVATE ${_absolute_index} ${_absolute_output})
	source_group("Yttrium Files" FILES ${_absolute_index} ${_absolute_output})
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

function(_print_list)
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
	_print_list(PREFIX "    CXX:                  " VALUES ${_flags})
	if(WIN32)
		_concatenate(_flags CMAKE_RC_FLAGS CMAKE_RC_FLAGS_${_CONFIG})
		_print_list(PREFIX "    RC:                   " VALUES ${_flags})
	endif()
	_concatenate(_flags CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_${_CONFIG})
	_print_list(PREFIX "    EXE_LINKER:           " VALUES ${_flags})
	_concatenate(_flags CMAKE_MODULE_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS_${_CONFIG})
	_print_list(PREFIX "    MODULE_LINKER:        " VALUES ${_flags})
	_concatenate(_flags CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_${_CONFIG})
	_print_list(PREFIX "    SHARED_LINKER:        " VALUES ${_flags})
	_concatenate(_flags CMAKE_STATIC_LINKER_FLAGS CMAKE_STATIC_LINKER_FLAGS_${_CONFIG})
	_print_list(PREFIX "    STATIC_LINKER:        " VALUES ${_flags})
endfunction()

function(print_current_flags _rc_flags _static_flags _dynamic_flags)
	message(STATUS "Build flags:")
	_print_config_flags("Debug")
	_print_config_flags("MinSizeRel")
	_print_config_flags("Release")
	_print_config_flags("RelWithDebInfo")
	message(STATUS "  (project):")
	get_property(_compile_options DIRECTORY PROPERTY COMPILE_OPTIONS)
	_print_list(PREFIX "    COMPILE_OPTIONS:      " VALUES ${_compile_options})
	get_property(_compile_definitions DIRECTORY PROPERTY COMPILE_DEFINITIONS)
	_print_list(PREFIX "    COMPILE_DEFINITIONS:  " VALUES ${_compile_definitions})
	if(WIN32)
		_print_list(PREFIX "    RC:                   " VALUES ${${_rc_flags}})
	endif()
	_print_list(PREFIX "    STATIC_LINKER:        " VALUES ${${_static_flags}})
	_print_list(PREFIX "    (dynamic)_LINKER:     " VALUES ${${_dynamic_flags}})
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
