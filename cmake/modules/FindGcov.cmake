#
# Copyright 2018 Sergei Blagodarin
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

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message(FATAL_ERROR "Gcov is only supported for the GNU compiler!")
endif()

get_filename_component(_gcc_directory ${CMAKE_CXX_COMPILER} DIRECTORY)
get_filename_component(_gcc_name ${CMAKE_CXX_COMPILER} NAME)
string(REPLACE "g++" "gcov" _gcov_name ${_gcc_name})
find_program(GCOV_EXECUTABLE ${_gcov_name} PATHS ${_gcc_directory})
unset(_gcc_directory)
unset(_gcc_name)
unset(_gcov_name)

if(GCOV_EXECUTABLE)
	execute_process(COMMAND ${GCOV_EXECUTABLE} --version OUTPUT_VARIABLE _gcov_version_output ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "gcov \\(.+\\) [0-9]+\\.[0-9]+\\.[0-9]+" _gcov_version_line ${_gcov_version_output})
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+$" GCOV_VERSION_STRING ${_gcov_version_line})
	unset(_gcov_version_line)
	unset(_gcov_version_output)
endif()

mark_as_advanced(GCOV_EXECUTABLE)

find_package_handle_standard_args(Gcov
	REQUIRED_VARS GCOV_EXECUTABLE
	VERSION_VAR GCOV_VERSION_STRING)
