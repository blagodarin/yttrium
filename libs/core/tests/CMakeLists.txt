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
	src/buffer.cpp
	src/buffer_appender.cpp
	src/color.cpp
	src/flags.cpp
	src/image.cpp
	src/image.h
	src/image_formats.cpp
	src/image_formats.h
	src/ion_reader.cpp
	src/ion_writer.cpp
	src/iostream.cpp
	src/iostream.h
	src/logger.cpp
	src/margins.cpp
	src/matrix.cpp
	src/package.cpp
	src/point.cpp
	src/reader.cpp
	src/rect.cpp
	src/script.cpp
	src/source.cpp
	src/temporary_file.cpp
	src/test_utils.cpp
	src/translation.cpp
	src/utils_numeric.cpp
	src/utils_string.cpp
	src/vector.cpp
	)

add_executable(test_core ${_src})
source_group("src" FILES ${_src})
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set_property(SOURCE tiny_string_map.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-self-assign-overloaded)
endif()

include_config(test_core ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_core PRIVATE ycore ytest)
set_target_properties(test_core PROPERTIES FOLDER "tests" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(test_core PRIVATE
		/wd5204 # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
		)
	target_link_options(test_core PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
add_test(NAME core COMMAND test_core WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
