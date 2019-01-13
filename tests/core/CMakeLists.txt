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

add_executable(ycore_tests
	buffer.cpp
	buffer_appender.cpp
	color.cpp
	flags.cpp
	image.cpp
	image.h
	image_formats.cpp
	image_formats.h
	ion_reader.cpp
	ion_writer.cpp
	iostream.cpp
	iostream.h
	margins.cpp
	matrix.cpp
	package.cpp
	point.cpp
	reader.cpp
	rect.cpp
	script.cpp
	source.cpp
	temporary_file.cpp
	test_utils.cpp
	translation.cpp
	utils_numeric.cpp
	utils_string.cpp
	vector.cpp
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE tiny_string_map.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-self-assign-overloaded)
endif()
target_link_libraries(ycore_tests PRIVATE ycore ytest)
set_target_properties(ycore_tests PROPERTIES FOLDER "tests")
include_config(ycore_tests ${PROJECT_BINARY_DIR}/config.h)
add_test(NAME ycore COMMAND ycore_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
