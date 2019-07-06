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

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	message(FATAL_ERROR "ClangTidy is only supported for the Clang compiler!")
endif()

get_filename_component(_clang_directory ${CMAKE_CXX_COMPILER} DIRECTORY)
get_filename_component(_clang_name ${CMAKE_CXX_COMPILER} NAME)
string(REPLACE "clang++" "clang-tidy" _clang_tidy_name ${_clang_name})
find_program(CLANG_TIDY_EXECUTABLE ${_clang_tidy_name} PATHS ${_clang_directory})
unset(_clang_directory)
unset(_clang_name)
unset(_clang_tidy_name)

if(CLANG_TIDY_EXECUTABLE)
	execute_process(COMMAND ${CLANG_TIDY_EXECUTABLE} -version OUTPUT_VARIABLE _clang_tidy_version_output ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "LLVM version .+" _clang_tidy_version_line ${_clang_tidy_version_output})
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[a-z0-9]+" CLANG_TIDY_VERSION_STRING ${_clang_tidy_version_line})
	unset(_clang_tidy_version_line)
	unset(_clang_tidy_version_output)
endif()

mark_as_advanced(CLANG_TIDY_EXECUTABLE)

find_package_handle_standard_args(ClangTidy
	REQUIRED_VARS CLANG_TIDY_EXECUTABLE
	VERSION_VAR CLANG_TIDY_VERSION_STRING)
