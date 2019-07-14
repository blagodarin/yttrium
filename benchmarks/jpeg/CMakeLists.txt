#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2019 Sergei Blagodarin.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(_src
	../common/include/benchmark.h
	src/main.cpp
	)

add_executable(benchmark_jpeg ${_src})
source_group("src" FILES ${_src})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE src/main.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE src/main.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set_property(SOURCE src/main.cpp APPEND PROPERTY COMPILE_OPTIONS
		/wd4324 # '`anonymous-namespace'::JpegErrorHandler': structure was padded due to alignment specifier
		/wd4611 # interaction between '_setjmp' and C++ object destruction is non-portable
		)
endif()

target_link_libraries(benchmark_jpeg PRIVATE ycore)
set_target_properties(benchmark_jpeg PROPERTIES FOLDER "benchmarks")
if(WIN32)
	find_package(Y3_jpeg REQUIRED)
	target_link_libraries(benchmark_jpeg PRIVATE Y3::jpeg)
else()
	find_package(JPEG REQUIRED)
	mark_as_advanced(jconfig_dir)
	target_link_libraries(benchmark_jpeg PRIVATE JPEG::JPEG)
endif()
