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

find_program(CPPCHECK_EXECUTABLE cppcheck)

if(CPPCHECK_EXECUTABLE)
	execute_process(COMMAND ${CPPCHECK_EXECUTABLE} --version OUTPUT_VARIABLE _cppcheck_version ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "[0-9]+\\.[0-9]+.*$" CPPCHECK_VERSION_STRING ${_cppcheck_version})
	unset(_cppcheck_version)
endif()

mark_as_advanced(CPPCHECK_EXECUTABLE)

find_package_handle_standard_args(Cppcheck
	REQUIRED_VARS CPPCHECK_EXECUTABLE
	VERSION_VAR CPPCHECK_VERSION_STRING)
