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

cmake_minimum_required(VERSION 3.7)

project(yttrium VERSION 0.0.1 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/y3/modules ${PROJECT_SOURCE_DIR}/cmake/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/utils.cmake)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_EXTRAS "Build all extra stuff" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_CLANG_FORMAT "Enable funning ClangFormat ('format' target)" OFF)
option(ENABLE_CLANG_TIDY "Enable running ClangTidy ('tidy' target)" OFF)
option(ENABLE_CPPCHECK "Enable running Cppcheck ('cppcheck' target)" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

option(Y_USE_JPEG "Include JPEG support (requires libjpeg)" ON)
option(Y_USE_OGG_VORBIS "Include Ogg Vorbis support (requires libvorbisfile)" ON)

set(Y_RENDERER "opengl" CACHE STRING "Renderer to use (opengl, vulkan)")
if(Y_RENDERER STREQUAL "opengl")
	find_package(OpenGL REQUIRED)
	find_package(X11 REQUIRED)
	set(Y_RENDERER_OPENGL ON)
elseif(Y_RENDERER STREQUAL "vulkan")
	find_package(Vulkan REQUIRED)
	# TODO: Use find_package for XCB and libxkbcommon.
	set(Y_RENDERER_VULKAN ON)
else()
	set(Y_RENDERER_NULL ON)
endif()

find_package(Threads REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Werror -Weverything
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-covered-switch-default
		-Wno-exit-time-destructors
		-Wno-float-equal
		-Wno-global-constructors
		-Wno-gnu-statement-expression
		-Wno-header-hygiene # TODO: Remove "using namespace" from examples.
		-Wno-nested-anon-types
		-Wno-padded
		-Wno-return-std-move-in-c++11
		-Wno-switch-enum
		-Wno-weak-vtables
		)

	if(ENABLE_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute)
		if(BUILD_SHARED_LIBS)
			add_compile_options(-fno-sanitize=vptr)
		endif()
		list(APPEND DYNAMIC_LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Werror -Wall -Wextra
		-Wconversion
		-Wdisabled-optimization
		-Wdouble-promotion
		-Wduplicated-cond
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-include-dirs
		-Wnon-virtual-dtor
		-Wnull-dereference
		-Wold-style-cast
		-Wpedantic
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		-Wsuggest-attribute=noreturn
		-Wsuggest-override
		-Wundef
		-Wuninitialized
		-Wunsafe-loop-optimizations
		-Wuseless-cast
		-Wzero-as-null-pointer-constant
		)

	if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(
			-Winline # Issued for header-only code.
			)
	endif()

	option(Y_COVERAGE "Build with coverage analysis support" OFF)
	if(Y_COVERAGE)
		add_compile_options(--coverage)
		list(APPEND DYNAMIC_LINKER_FLAGS --coverage)

		option(Y_COVERAGE_LCOV "Enable coverage analysis using Lcov" OFF)
		if(Y_COVERAGE_LCOV)
			find_package(Gcov REQUIRED)
			find_package(Lcov REQUIRED)
			add_custom_target(lcov
				COMMAND ${LCOV_EXECUTABLE} --capture --no-external --quiet
					--base-directory ${PROJECT_SOURCE_DIR}
					--directory ${PROJECT_BINARY_DIR}
					--gcov-tool ${GCOV_EXECUTABLE}
					--output-file ${PROJECT_BINARY_DIR}/coverage.info
				COMMAND ${LCOV_EXECUTABLE} --summary ${PROJECT_BINARY_DIR}/coverage.info
				COMMAND ${GENHTML_EXECUTABLE} ${PROJECT_BINARY_DIR}/coverage.info --quiet
					--output-directory ${PROJECT_BINARY_DIR}/coverage
					--rc genhtml_hi_limit=67
					--rc genhtml_med_limit=34
				VERBATIM)
			add_custom_target(lcov_clean
				COMMAND ${LCOV_EXECUTABLE} --zerocounters --quiet --directory ${PROJECT_BINARY_DIR}
				VERBATIM)
		endif()
	endif()

	if(ENABLE_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute)
		if(BUILD_SHARED_LIBS)
			add_compile_options(-fno-sanitize=vptr)
		endif()
		list(APPEND DYNAMIC_LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/EHsc /std:c++latest /W4
		/wd4251 # class '...' needs to have dll-interface to be used by clients of class '...'
		)
	add_definitions(/D_USE_MATH_DEFINES)
endif()

get_property(DIRECTORY_COMPILE_OPTIONS DIRECTORY PROPERTY COMPILE_OPTIONS)

print_list("Compiler flags:" ${DIRECTORY_COMPILE_OPTIONS})
print_list("Static linker flags:" ${STATIC_LINKER_FLAGS})
print_list("Dynamic linker flags:" ${DYNAMIC_LINKER_FLAGS})

append_options(CMAKE_EXE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_MODULE_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_SHARED_LINKER_FLAGS ${DYNAMIC_LINKER_FLAGS})
append_options(CMAKE_STATIC_LINKER_FLAGS ${STATIC_LINKER_FLAGS})

set(Y_ENABLE_BUFFER_MEMORY_DEBUGGING OFF)
set(Y_ENABLE_BUFFER_MEMORY_TRACKING ON) # Disable for profiling purposes only.
configure_file(cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(libs/core)

if(Y_RENDERER_VULKAN)
	add_subdirectory(tools/yglslc)
endif()
add_subdirectory(tools/yrc)
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

add_subdirectory(libs/engine)

if(WIN32)
	add_subdirectory(libs/winmain)
endif()

if(BUILD_TESTS)
	enable_testing()
	find_package(Catch2 2.3.0 REQUIRED)
	mark_as_advanced(Catch2_DIR)
	add_subdirectory(libs/test)
	add_subdirectory(tests/core)
	add_subdirectory(tests/engine)
	add_custom_target(check
		COMMAND ycore_tests --reporter compact
		COMMAND yengine_tests --reporter compact
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		VERBATIM)
	set_target_properties(check PROPERTIES FOLDER "tests")
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(BUILD_EXTRAS)
	add_subdirectory(benchmarks/buffer)
	add_subdirectory(tests/gen_test_data)
endif()

if(ENABLE_CLANG_FORMAT)
	find_package(ClangFormat REQUIRED)
	file(GLOB_RECURSE _sources LIST_DIRECTORIES FALSE RELATIVE ${PROJECT_SOURCE_DIR} *.h *.cpp)
	add_custom_target(format
		COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${_sources}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		VERBATIM)
	set_target_properties(format PROPERTIES FOLDER ".support")
endif()

if(ENABLE_CLANG_TIDY)
	find_package(ClangTidy REQUIRED)
	set(_cpps)
	foreach(_target ycore yengine)
		get_target_property(_source_dir ${_target} SOURCE_DIR)
		get_target_property(_sources ${_target} SOURCES)
		foreach(_source ${_sources})
			if(_source MATCHES "^.*\.cpp$" AND NOT _source STREQUAL "utils/string.cpp") # The skipped file segfaults clang-tidy.
				list(APPEND _cpps ${_source_dir}/${_source})
			endif()
		endforeach()
	endforeach()
	add_custom_target(tidy
		COMMAND ${CLANG_TIDY_EXECUTABLE} -header-filter=.* -p=${PROJECT_BINARY_DIR} -quiet ${_cpps}
		VERBATIM)
	set_target_properties(tidy PROPERTIES FOLDER ".support")
endif()

if(ENABLE_CPPCHECK)
	find_package(Cppcheck REQUIRED)
	add_custom_target(cppcheck
		COMMAND ${CPPCHECK_EXECUTABLE} --enable=all --quiet
			-I ${PROJECT_SOURCE_DIR}/include
			-i ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}
			--include=${PROJECT_BINARY_DIR}/config.h
			--relative-paths=${PROJECT_SOURCE_DIR}
			--suppressions-list=${PROJECT_SOURCE_DIR}/.cppcheck-suppress
			"--template=[{file}:{line}]: ({severity}/{id}) {message}"
			${PROJECT_SOURCE_DIR}
		VERBATIM)
	set_target_properties(cppcheck PROPERTIES FOLDER ".support")
endif()

if(WIN32)
	set(_cmake_config_dir ${CMAKE_INSTALL_PREFIX}/cmake)
else()
	set(_cmake_config_dir ${CMAKE_INSTALL_LIBDIR}/cmake/Yttrium)
endif()

configure_package_config_file(cmake/YttriumConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake
	INSTALL_DESTINATION ${_cmake_config_dir})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake
	COMPATIBILITY SameMinorVersion)

install(DIRECTORY include/yttrium DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/YttriumConfigVersion.cmake DESTINATION ${_cmake_config_dir})
install(EXPORT YttriumTargets NAMESPACE Yttrium:: DESTINATION ${_cmake_config_dir})
