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

find_program(CLANG_FORMAT_EXECUTABLE clang-format)

if(CLANG_FORMAT_EXECUTABLE)
	execute_process(COMMAND ${CLANG_FORMAT_EXECUTABLE} -version OUTPUT_VARIABLE _clang_format_version_output ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "LLVM [0-9]+\\.[0-9]+\\.[a-z0-9]+" _clang_format_version_line ${_clang_format_version_output})
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[a-z0-9]+" CLANG_FORMAT_VERSION_STRING ${_clang_format_version_line})
	unset(_clang_format_version_line)
	unset(_clang_format_version_output)
endif()

mark_as_advanced(CLANG_FORMAT_EXECUTABLE)

find_package_handle_standard_args(ClangFormat
	REQUIRED_VARS CLANG_FORMAT_EXECUTABLE
	VERSION_VAR CLANG_FORMAT_VERSION_STRING)
