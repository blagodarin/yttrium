cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(WITH_EXAMPLES "Enable examples (also enables tools)" OFF)
option(WITH_EXTRA "Enable extra stuff" OFF)
option(WITH_TESTS "Enable tests" OFF)
option(WITH_TOOLS "Enable tools" OFF)
option(Y_SANITIZERS "Enable sanitizers" OFF)

set(Y_RENDERER "OpenGL" CACHE STRING "Renderer to use (OpenGL, Vulkan)")

if(WITH_EXAMPLES AND NOT WITH_TOOLS)
	message(FATAL_ERROR "WITH_EXAMPLES require WITH_TOOLS")
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
	add_compile_options(-g -Wall -Werror -Wextra -Wnon-virtual-dtor -Wpedantic -Wshadow -Wno-unknown-pragmas)

	if(Y_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute -fno-sanitize=vptr)
		set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
	endif()

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

if(Y_RENDERER STREQUAL "Vulkan")
	add_subdirectory(tools/yglslc)
endif()

if(WITH_TOOLS OR Y_RENDERER STREQUAL "OpenGL")
	add_subdirectory(tools/yrc)
endif()

add_subdirectory(src)

if(WITH_TOOLS)
	add_subdirectory(tools/ypack)
	add_subdirectory(tools/ytr)
endif()

if(WITH_EXAMPLES)
	if(WIN32)
		add_subdirectory(ywinmain)
	endif()
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(WITH_EXTRA)
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

if(WITH_TESTS)
	add_subdirectory(tests)
endif()
