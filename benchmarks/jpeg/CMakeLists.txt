#
# Copyright 2019 Sergei Blagodarin
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

add_executable(benchmark_jpeg
	main.cpp
	)
target_link_libraries(benchmark_jpeg PRIVATE ycore)
if(WIN32)
	find_package(Y3_jpeg REQUIRED)
	target_link_libraries(benchmark_jpeg PRIVATE Y3::jpeg)
else()
	find_package(JPEG REQUIRED)
	mark_as_advanced(jconfig_dir)
	target_link_libraries(benchmark_jpeg PRIVATE JPEG::JPEG)
endif()
set_target_properties(benchmark_jpeg PROPERTIES FOLDER "benchmarks")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-useless-cast)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-tautological-type-limit-compare)
endif()
