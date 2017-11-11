cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

option(Y_EXAMPLES "Enable examples (also enables tools)" OFF)
option(Y_EXTRA "Enable extra stuff" OFF)
option(Y_TESTS "Enable tests" OFF)
option(Y_TOOLS "Enable tools" OFF)

set(Y_RENDERER "OpenGL" CACHE STRING "Renderer to use (OpenGL, Vulkan)")

if(Y_EXAMPLES AND NOT Y_TOOLS)
	message(FATAL_ERROR "Y_EXAMPLES require Y_TOOLS")
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(
		-Weverything -Werror
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-conversion
		-Wno-double-promotion
		-Wno-exit-time-destructors
		-Wno-float-equal
		-Wno-global-constructors
		-Wno-gnu-statement-expression
		-Wno-padded
		-Wno-switch-enum
		-Wno-weak-vtables
		)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-g -Wall -Werror -Wextra -Wnon-virtual-dtor -Wold-style-cast -Wpedantic -Wshadow -Wsign-conversion -Wno-unknown-pragmas)

	option(Y_SANITIZERS "Enable sanitizers" OFF)
	if(Y_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute -fno-sanitize=vptr)
		set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
	endif()

	option(Y_TRAVIS "Enable Travis CI workarounds" OFF)
	if(Y_TRAVIS)
		add_compile_options(-Wno-old-style-cast -Wno-sign-compare -Wno-sign-conversion)
		if(Y_SANITIZERS)
			set(LINKER_FLAGS "${LINKER_FLAGS} -fuse-ld=gold")
		endif()
	endif()

	option(Y_COVERAGE "Build with coverage analysis support" OFF)
	if(Y_COVERAGE)
		add_compile_options(--coverage -O0)
		set(LINKER_FLAGS "${LINKER_FLAGS} --coverage")
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/EHsc /std:c++latest /W4 /wd4251)
	add_definitions(/D_USE_MATH_DEFINES)
endif()

message(STATUS "Build flags:")
get_property(CURRENT_COMPILE_OPTIONS DIRECTORY PROPERTY COMPILE_OPTIONS)
string(REPLACE ";" " " CURRENT_COMPILE_OPTIONS " ${CURRENT_COMPILE_OPTIONS}")
message(STATUS "  COMPILER:${CURRENT_COMPILE_OPTIONS}")
message(STATUS "  LINKER:${LINKER_FLAGS}")

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

if(Y_RENDERER STREQUAL "Vulkan")
	add_subdirectory(tools/yglslc)
endif()

if(Y_TOOLS OR Y_RENDERER STREQUAL "OpenGL")
	add_subdirectory(tools/yrc)
endif()

add_subdirectory(src)

if(Y_TOOLS)
	add_subdirectory(tools/ypack)
	add_subdirectory(tools/ytr)
endif()

if(Y_EXAMPLES)
	if(WIN32)
		add_subdirectory(ywinmain)
	endif()
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(Y_EXTRA)
	add_subdirectory(tools/generate-sounds)
	add_subdirectory(tools/generate-test-images)

	add_subdirectory(benchmarks/buffer)
	add_subdirectory(benchmarks/tiny_string_map)

	add_custom_target(cppcheck
		COMMAND cppcheck --enable=all --quiet
			--inline-suppr --suppress=missingIncludeSystem --suppress=passedByValue --suppress=syntaxError
			-I "${PROJECT_SOURCE_DIR}/include"
			-i "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
			-i "${PROJECT_SOURCE_DIR}/3rdparty"
			"--relative-paths=${PROJECT_SOURCE_DIR}"
			"--template=[{file}:{line}]: ({severity}/{id}) {message}"
			"${PROJECT_SOURCE_DIR}"
		VERBATIM)
endif()

if(Y_TESTS)
	add_subdirectory(tests)
endif()
