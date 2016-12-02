cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories(include)

add_subdirectory(src)

add_subdirectory(tests)

add_subdirectory(tools/generate-sounds)
add_subdirectory(tools/generate-test-images)
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

add_subdirectory(benchmarks/buffer)
add_subdirectory(benchmarks/readline)

add_subdirectory(examples/3d)
add_subdirectory(examples/tetrium)
