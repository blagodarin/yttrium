cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(WITH_EXAMPLES "Enable examples (also enables tools)" OFF)
option(WITH_EXTRA "Enable extra stuff" OFF)
option(WITH_TESTS "Enable tests" OFF)
option(WITH_TOOLS "Enable tools" OFF)

if(WITH_EXAMPLES AND NOT WITH_TOOLS)
	message(FATAL_ERROR "WITH_EXAMPLES require WITH_TOOLS")
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wall -Werror -Wextra -Wno-unknown-pragmas -Wnon-virtual-dtor -Wshadow) # -Wpedantic breaks terse static asserts (GCC 6.3.0).

	option(FIX_TRAVIS "Fix Travis CI issues" OFF)
	if(NOT FIX_TRAVIS)
		add_compile_options(-Wold-style-cast -Wsign-conversion)
	else()
		add_compile_options(-Wno-sign-compare)
	endif()

	option(COVERAGE "Build with coverage analysis support" OFF)
	if(COVERAGE)
		add_compile_options(--coverage -O0)
		set(LINKER_FLAGS "${LINKER_FLAGS} --coverage")
	endif()

	# Boost.Variant is required until GCC 7.
	find_package(Boost REQUIRED)
	include_directories("${Boost_INCLUDE_DIR}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/EHsc /std:c++latest /W4 /wd4251)
	add_definitions(/D_USE_MATH_DEFINES)
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/3rdparty/include")
link_directories("${PROJECT_SOURCE_DIR}/3rdparty/lib")

include_directories("${PROJECT_SOURCE_DIR}/include")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

add_subdirectory(src)

if(WITH_TOOLS)
	add_subdirectory(tools/ypack)
	add_subdirectory(tools/ytr)
	set_target_properties(
		ypack
		ytr
		PROPERTIES FOLDER "tools")
endif()

if(WITH_EXAMPLES)
	if(WIN32)
		add_subdirectory(ywinmain)
	endif()
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
	set_target_properties(
		3d
		tetrium tetrium_localization tetrium_package
		PROPERTIES FOLDER "examples")
endif()

if(WITH_EXTRA)
	add_subdirectory(tools/generate-sounds)
	add_subdirectory(tools/generate-test-images)
	set_target_properties(
		generate-sounds
		generate-test-images
		PROPERTIES FOLDER "tools")

	add_subdirectory(benchmarks/buffer)
	add_subdirectory(benchmarks/tiny_string_map)
	set_target_properties(
		benchmark_buffer
		benchmark_tiny_string_map
		PROPERTIES FOLDER "benchmarks")

	add_custom_target(cppcheck
		COMMAND cppcheck --enable=all --quiet
			--inline-suppr --suppress=missingIncludeSystem --suppress=passedByValue
			-I "${PROJECT_SOURCE_DIR}/include"
			-i "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
			-i "${PROJECT_SOURCE_DIR}/3rdparty"
			"--relative-paths=${PROJECT_SOURCE_DIR}"
			"--template=[{file}:{line}]: ({severity}/{id}) {message}"
			"${PROJECT_SOURCE_DIR}"
		VERBATIM)
endif()

if(WITH_TESTS)
	add_subdirectory(tests)
	add_custom_target(check COMMAND unit_tests --log_level=message WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
	set_target_properties(
		unit_tests check
		PROPERTIES FOLDER "tests")
endif()
