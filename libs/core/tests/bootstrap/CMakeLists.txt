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

add_executable(bootstrap_core_tests ${_src})
source_group("src" FILES ${_src})

include_config(bootstrap_core_tests ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(bootstrap_core_tests PRIVATE ycore)
set_target_properties(bootstrap_core_tests PROPERTIES FOLDER "tests" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(bootstrap_core_tests PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()

add_custom_target(bootstrap
	COMMAND bootstrap_core_tests
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../data
	COMMENT "Bootstrapping core tests"
	VERBATIM)
set_target_properties(bootstrap PROPERTIES FOLDER "tests")
