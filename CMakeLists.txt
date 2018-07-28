cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

set(Boost_USE_STATIC_LIBS OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(utils)

option(Y_ALL "Build all components" OFF)
option(Y_EXAMPLES "Build examples (require tools)" OFF)
option(Y_TESTS "Build tests" OFF)
option(Y_TOOLS "Build tools" OFF)

if(Y_EXAMPLES AND NOT (Y_ALL OR Y_TOOLS))
	message(FATAL_ERROR "Y_EXAMPLES require Y_TOOLS")
endif()

set(Y_RENDERER "OpenGL" CACHE STRING "Renderer to use (OpenGL, Vulkan)")
if(Y_RENDERER STREQUAL "OpenGL")
	set(Y_RENDERER_OPENGL ON)
elseif(Y_RENDERER STREQUAL "Vulkan")
	set(Y_RENDERER_VULKAN ON)
else()
	set(Y_RENDERER_NULL ON)
endif()

find_package(Threads REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Werror -Weverything
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
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
	add_compile_options(-Werror -Wall -Wextra
		-Wconversion
		-Wdisabled-optimization
		#-Wdouble-promotion
		-Wduplicated-cond
		-Winline
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-include-dirs
		-Wnon-virtual-dtor
		-Wnull-dereference
		-Wold-style-cast
		#-Wpadded
		-Wpedantic
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		#-Wsuggest-attribute=const -fipa-pure-const
		-Wsuggest-final-methods
		-Wsuggest-final-types
		-Wsuggest-override
		-Wundef
		-Wuninitialized
		-Wunsafe-loop-optimizations
		#-Wuseless-cast
		#-Wzero-as-null-pointer-constant
		-Wno-ignored-qualifiers
		-Wno-old-style-cast # Triggered by X11 macros.
		-Wno-unknown-pragmas
		)

	option(Y_COVERAGE "Build with coverage analysis support" OFF)
	if(Y_COVERAGE)
		add_compile_options(--coverage)
		list(APPEND LINKER_FLAGS --coverage)
	endif()

	option(Y_SANITIZERS "Enable sanitizers" OFF)
	if(Y_SANITIZERS)
		add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize=nonnull-attribute -fno-sanitize=vptr)
		list(APPEND LINKER_FLAGS -fsanitize=address -fsanitize=undefined)
	endif()

	option(Y_TRAVIS "Enable Travis CI workarounds" OFF)
	if(Y_TRAVIS AND Y_SANITIZERS)
		list(APPEND LINKER_FLAGS -fuse-ld=gold)
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
print_list("Linker flags:" ${LINKER_FLAGS})

append_options(CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})
append_options(CMAKE_MODULE_LINKER_FLAGS ${LINKER_FLAGS})
append_options(CMAKE_SHARED_LINKER_FLAGS ${LINKER_FLAGS})

include_directories(${PROJECT_SOURCE_DIR}/include)

if(Y_RENDERER_VULKAN)
	add_subdirectory(tools/yglslc)
endif()

if(Y_ALL OR Y_TOOLS OR Y_RENDERER_OPENGL)
	add_subdirectory(tools/yrc)
endif()

add_subdirectory(src)

if(Y_ALL OR Y_TOOLS)
	add_subdirectory(tools/ypack)
	add_subdirectory(tools/ytr)
endif()

if(Y_ALL OR Y_EXAMPLES)
	if(WIN32)
		add_subdirectory(ywinmain)
	endif()
	add_subdirectory(examples/3d)
	add_subdirectory(examples/tetrium)
endif()

if(Y_ALL OR Y_TESTS)
	add_subdirectory(tests/unit_tests)
endif()

if(Y_ALL)
	add_subdirectory(benchmarks/buffer)
	add_subdirectory(benchmarks/tiny_string_map)
	add_subdirectory(tests/gen_test_data)

	add_custom_target(cppcheck
		COMMAND cppcheck --enable=all --quiet
			--inline-suppr --suppress=missingIncludeSystem --suppress=passedByValue --suppress=syntaxError
			-I "${PROJECT_SOURCE_DIR}/include"
			-i "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
			"--relative-paths=${PROJECT_SOURCE_DIR}"
			"--template=[{file}:{line}]: ({severity}/{id}) {message}"
			"${PROJECT_SOURCE_DIR}"
		VERBATIM)
endif()
