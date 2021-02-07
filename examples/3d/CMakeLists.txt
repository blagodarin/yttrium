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
	src/game.cpp
	src/game.h
	src/main.cpp
	src/model.cpp
	src/model.h
	)

add_executable(3d WIN32 ${_src})
source_group("src" FILES ${_src})
target_link_libraries(3d PRIVATE ycore yengine ymain)
set_target_properties(3d PROPERTIES FOLDER "examples" MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(3d PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
if(WIN32)
	set_property(TARGET 3d APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
endif()

add_ypack_target(3d_package
	INDEX data.index
	OUTPUT ${PROJECT_BINARY_DIR}/3d.ypq)
add_dependencies(3d 3d_package)
set_target_properties(3d_package PROPERTIES FOLDER "examples")

if(INSTALL_EXAMPLES)
	install(TARGETS 3d RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(FILES ${PROJECT_BINARY_DIR}/3d.ypq DESTINATION ${CMAKE_INSTALL_DATADIR})
endif()
