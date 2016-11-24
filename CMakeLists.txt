cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)
find_package(JPEG)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

option(BUILD_RELEASE "Build release")
if(BUILD_RELEASE)
	add_definitions(-DNDEBUG)
else()
	add_definitions(-D_DEBUG)
endif()

option(BUILD_STATIC "Build static library")
if(BUILD_STATIC)
	add_definitions(-D__YTTRIUM_STATIC)
else()
	add_definitions(-D__YTTRIUM_SHARED)
endif()

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra")
	if(BUILD_RELEASE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)

add_subdirectory(src)

add_subdirectory(tests)

add_subdirectory(tools/generate-sounds)
add_subdirectory(tools/generate-test-images)
add_subdirectory(tools/ypack)
add_subdirectory(tools/ytr)

add_subdirectory(examples/3d)
add_subdirectory(examples/tetrium)
