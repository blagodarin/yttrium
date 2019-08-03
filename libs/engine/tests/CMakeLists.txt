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
	src/gui_loading.cpp
	src/key.cpp
	src/resource_loader.cpp
	)

add_executable(test_engine ${_src})
source_group("src" FILES ${_src})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE src/gui_loading.cpp src/key.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-disabled-optimization)
endif()

include_config(test_engine ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(test_engine PRIVATE ycore yengine ytest)
set_target_properties(test_engine PROPERTIES FOLDER "tests")

add_test(NAME engine COMMAND test_engine --reporter compact)
add_custom_target(check_engine
	COMMAND test_engine --reporter compact
	COMMENT "Running engine tests"
	VERBATIM)
set_target_properties(check_engine PROPERTIES FOLDER ".phony")
