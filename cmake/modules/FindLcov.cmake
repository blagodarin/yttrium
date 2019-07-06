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

find_program(LCOV_EXECUTABLE lcov)
find_program(GENHTML_EXECUTABLE genhtml)

if(LCOV_EXECUTABLE)
	execute_process(COMMAND ${LCOV_EXECUTABLE} --version OUTPUT_VARIABLE _lcov_version ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "[0-9]+\\.[0-9]+.*$" LCOV_VERSION_STRING ${_lcov_version})
	unset(_lcov_version)
endif()

mark_as_advanced(LCOV_EXECUTABLE GENHTML_EXECUTABLE)

find_package_handle_standard_args(Lcov
	REQUIRED_VARS LCOV_EXECUTABLE GENHTML_EXECUTABLE
	VERSION_VAR LCOV_VERSION_STRING)
