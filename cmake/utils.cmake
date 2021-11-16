# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

function(yttrium_embed _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	cmake_parse_arguments(_arg "" "GROUP" "FILES" ${ARGN})
	if(NOT _arg_GROUP)
		set(_arg_GROUP "Yttrium Files")
	endif()
	get_target_property(_binary_dir ${_target} BINARY_DIR)
	get_target_property(_source_dir ${_target} SOURCE_DIR)
	foreach(_file ${_arg_FILES})
		get_filename_component(_source_name ${_file} NAME)
		set(_output ${_source_name}.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND yembed --string ${_source_dir}/${_file} ${_output}
			MAIN_DEPENDENCY ${_file}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group(${_arg_GROUP} FILES ${_binary_dir}/${_output})
	endforeach()
	target_include_directories(${_target} PRIVATE ${_binary_dir})
endfunction()

function(yttrium_embed_spirv _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	cmake_parse_arguments(_arg "" "GROUP" "FRAGMENT;VERTEX" ${ARGN})
	if(NOT _arg_GROUP)
		set(_arg_GROUP "Yttrium Files")
	endif()
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
		source_group(${_arg_GROUP} FILES ${_binary_dir}/${_output})
	endforeach()
	foreach(_source ${_arg_VERTEX})
		get_filename_component(_source_name ${_source} NAME)
		set(_output ${_source_name}.spirv.inc)
		add_custom_command(OUTPUT ${_binary_dir}/${_output}
			COMMAND Vulkan::glslc -fshader-stage=vertex -mfmt=num -O -o ${_output} ${_source_dir}/${_source}
			MAIN_DEPENDENCY ${_source}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_binary_dir}/${_output})
		source_group(${_arg_GROUP} FILES ${_binary_dir}/${_output})
	endforeach()
	target_include_directories(${_target} PRIVATE ${_binary_dir})
endfunction()

function(yttrium_target_package _target)
	if(NOT TARGET ${_target})
		message(SEND_ERROR "'${_target}' is not a target")
		return()
	endif()
	cmake_parse_arguments(_arg "EMBED" "GROUP;INDEX;OUTPUT" "DEPENDS" ${ARGN})
	if(NOT _arg_INDEX)
		message(SEND_ERROR "Missing INDEX")
		return()
	endif()
	if(NOT _arg_OUTPUT)
		message(SEND_ERROR "Missing OUTPUT")
		return()
	endif()
	if(NOT _arg_GROUP)
		set(_arg_GROUP "Yttrium Files")
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
	if(_arg_EMBED)
		add_custom_command(OUTPUT ${_absolute_output}.inc
			COMMAND ypack ${_relative_index} ${_absolute_output}
			COMMAND yembed --uint8 ${_absolute_output} ${_absolute_output}.inc
			MAIN_DEPENDENCY ${_absolute_index} DEPENDS ypack yembed
			COMMENT "Generating ${_absolute_output}.inc"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_absolute_index})
		source_group(${_arg_GROUP} FILES ${_absolute_index})
		target_include_directories(${_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	else()
		add_custom_command(OUTPUT ${_absolute_output}
			COMMAND ypack ${_relative_index} ${_absolute_output}
			MAIN_DEPENDENCY ${_absolute_index} DEPENDS ypack
			COMMENT "Generating ${_relative_output}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM)
		target_sources(${_target} PRIVATE ${_absolute_index} ${_absolute_output})
		source_group(${_arg_GROUP} FILES ${_absolute_index} ${_absolute_output})
	endif()
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
