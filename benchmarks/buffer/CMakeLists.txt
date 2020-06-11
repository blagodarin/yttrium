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
	src/main.cpp
	)

add_executable(benchmark_buffer ${_src})
source_group("src" FILES ${_src})

target_compile_definitions(benchmark_buffer PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
target_link_libraries(benchmark_buffer PRIVATE ycore ytest)
set_target_properties(benchmark_buffer PROPERTIES FOLDER "benchmarks")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(benchmark_buffer PRIVATE
		/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
		)
endif()

add_custom_target(b_buffer
	COMMAND benchmark_buffer
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMENT "Running Buffer benchmark"
	VERBATIM)
set_target_properties(b_buffer PROPERTIES FOLDER ".phony")
