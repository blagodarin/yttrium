cmake_minimum_required(VERSION 3.7)

project(yttrium CXX)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/bootstrap/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/utils.cmake)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

option(Y_BUILD_ALL "Build everything" OFF)
option(Y_BUILD_EXAMPLES "Build examples" OFF)
option(Y_BUILD_TESTS "Build tests" OFF)

option(Y_USE_JPEG "Include JPEG support" ON)
option(Y_USE_OGG_VORBIS "Include Ogg Vorbis support" ON)
option(Y_USE_PNG "Include PNG support" ON)

set(Y_RENDERER "OpenGL" CACHE STRING "Renderer to use (OpenGL, Vulkan)")
if(Y_RENDERER STREQUAL "OpenGL")
	find_package(OpenGL REQUIRED)
	find_package(X11 REQUIRED)
	set(Y_RENDERER_OPENGL ON)
elseif(Y_RENDERER STREQUAL "Vulkan")
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
		-Wno-padded
		-Wno-switch-enum
		-Wno-weak-vtables
		)
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
		)

	if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(
			-Winline # Issued for header-only code.
			)
	endif()

	if(NOT Y_RENDERER_VULKAN)
		add_compile_options(
			-Wzero-as-null-pointer-constant # Issued for every usage of VK_NULL_HANDLE.
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

	option(Y_SANITIZERS "Enable sanitizers" OFF)
	if(Y_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute -fno-sanitize=vptr)
		list(APPEND DYNAMIC_LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
	endif()

	option(Y_TRAVIS "Enable Travis CI workarounds" OFF)
	if(Y_TRAVIS AND Y_SANITIZERS)
		list(APPEND DYNAMIC_LINKER_FLAGS -fuse-ld=gold)
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
configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Tools that don't depend on Yttrium library.
if(Y_RENDERER_VULKAN)
	add_subdirectory(tools/yglslc)
endif()
add_subdirectory(tools/yrc)

add_subdirectory(libs/yttrium)

# Tools that depend on Yttrium library.
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

if(Y_BUILD_EXAMPLES OR Y_BUILD_ALL)
	if(WIN32)
		add_subdirectory(libs/ywinmain)
	endif()
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(Y_BUILD_TESTS OR Y_BUILD_ALL)
	find_package(Catch2 2.3.0 REQUIRED)
	add_subdirectory(tests/yttrium)
endif()

if(Y_BUILD_ALL)
	add_subdirectory(benchmarks/buffer)
	add_subdirectory(benchmarks/tiny_string_map)
	add_subdirectory(tests/gen_test_data)

	find_program(CPPCHECK_EXECUTABLE cppcheck)
	if(CPPCHECK_EXECUTABLE)
		add_custom_target(cppcheck
			COMMAND ${CPPCHECK_EXECUTABLE} --enable=all --quiet
				--inline-suppr --suppress=missingIncludeSystem --suppress=passedByValue --suppress=syntaxError --suppress=unusedFunction --suppress=unusedPrivateFunction
				-I ${PROJECT_SOURCE_DIR}/include
				-i ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}
				--include=${PROJECT_BINARY_DIR}/config.h
				--relative-paths=${PROJECT_SOURCE_DIR}
				"--template=[{file}:{line}]: ({severity}/{id}) {message}"
				${PROJECT_SOURCE_DIR}
			VERBATIM)
	endif()
endif()
