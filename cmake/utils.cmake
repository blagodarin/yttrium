# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

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
