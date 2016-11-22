cmake_minimum_required(VERSION 3.7)

project(yttrium CXX)

find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	if(TRUE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
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
