cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(COVERAGE "Build with coverage analysis support" OFF)
option(WITH_EXAMPLES "Enable examples" OFF)
option(WITH_EXTRA "Enable extra stuff" OFF)
option(WITH_TESTS "Enable tests" OFF)

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unknown-pragmas -Wnon-virtual-dtor -Wpedantic -Wshadow -Wsign-conversion") # -Wold-style-cast removed because of Boost issues on Travis.
	if (COVERAGE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
	endif()
	find_package(Boost REQUIRED) # For Boost.Variant which is needed until GCC 7.
	include_directories("${Boost_INCLUDE_DIR}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-D_USE_MATH_DEFINES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /std:c++latest /W4 /wd4251")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories("${PROJECT_SOURCE_DIR}/include")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

add_subdirectory(src)

add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)
set_target_properties(
	ypack
	ytr
	PROPERTIES FOLDER "tools")

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
			"--relative-paths=${PROJECT_SOURCE_DIR}"
			"${PROJECT_SOURCE_DIR}"
		VERBATIM)
endif()

if(WITH_TESTS)
	add_subdirectory(tests)
	set_target_properties(
		check unit_tests
		PROPERTIES FOLDER "tests")
endif()

if(COVERAGE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_custom_target(uncover
			COMMAND lcov --zerocounters --quiet --directory "${PROJECT_BINARY_DIR}"
			VERBATIM)
		add_custom_target(cover
			COMMAND lcov --capture --no-external --quiet --directory "${PROJECT_BINARY_DIR}" --output-file "${PROJECT_BINARY_DIR}/coverage.info"
			COMMAND genhtml --quiet "${PROJECT_BINARY_DIR}/coverage.info" --output-directory "${PROJECT_BINARY_DIR}/coverage"
			VERBATIM)
	endif()
endif()
