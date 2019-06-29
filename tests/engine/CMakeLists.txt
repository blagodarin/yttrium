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

set(_src
	src/gui_loading.cpp
	src/key.cpp
	src/resource_loader.cpp
	)

add_executable(yengine_tests ${_src})
source_group("src" FILES ${_src})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_property(SOURCE src/gui_loading.cpp src/key.cpp APPEND PROPERTY COMPILE_OPTIONS -Wno-disabled-optimization)
endif()

include_config(yengine_tests ${PROJECT_BINARY_DIR}/config.h)
target_link_libraries(yengine_tests PRIVATE ycore yengine ytest)
set_target_properties(yengine_tests PROPERTIES FOLDER "tests")

add_test(NAME yengine COMMAND yengine_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
