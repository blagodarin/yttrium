#
# This file is part of the Yttrium toolkit
# Copyright (C) 2019 Sergei Blagodarin
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
	../common/include/utils.h
	src/game.cpp
	src/game.h
	src/main.cpp
	src/model.cpp
	src/model.h
	)

add_executable(3d WIN32 ${_src})
source_group("src" FILES ${_src})
target_link_libraries(3d PRIVATE ycore yengine)
if(WIN32)
	set_property(TARGET 3d APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
	target_link_libraries(3d PRIVATE optimized ywinmain)
endif()

add_ypack_target(3d_package
	INDEX data.index
	OUTPUT ${PROJECT_BINARY_DIR}/3d.ypq)
add_dependencies(3d 3d_package)

set_target_properties(3d 3d_package PROPERTIES FOLDER "examples")
