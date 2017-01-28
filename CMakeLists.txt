cmake_minimum_required(VERSION 3.7)

project(yttrium C CXX)

message(STATUS "CMAKE_SYSTEM_NAME=\"${CMAKE_SYSTEM_NAME}\"")
message(STATUS "CMAKE_CXX_COMPILER_ID=\"${CMAKE_CXX_COMPILER_ID}\"")

option(BOOST_ALL_DYN_LINK "Define BOOST_ALL_DYN_LINK" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Boost REQUIRED COMPONENTS date_time program_options unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
if(BOOST_ALL_DYN_LINK)
	add_definitions(-DBOOST_ALL_DYN_LINK)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_definitions(-D_USE_MATH_DEFINES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4251 /wd4267 /wd4800")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include_directories(${Boost_INCLUDE_DIR} include)

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
