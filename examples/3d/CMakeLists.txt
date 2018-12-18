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

add_ypack_target(3d_package
	OUTPUT ${PROJECT_BINARY_DIR}/3d.ypq
	INDEX data/index.txt)

add_executable(3d WIN32
	../utils.h
	game.cpp
	game.h
	main.cpp
	model.cpp
	model.h
	)
target_link_libraries(3d ycore yengine)
if(WIN32)
	set_property(TARGET 3d APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
	target_link_libraries(3d optimized ywinmain)
endif()
add_dependencies(3d 3d_package)

set_target_properties(3d 3d_package PROPERTIES FOLDER "examples")
