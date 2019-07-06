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
	src/graphics.cpp
	src/graphics.h
	src/logic.cpp
	src/logic.h
	src/main.cpp
	)

add_executable(tetrium WIN32 ${_src})
source_group("src" FILES ${_src})
target_link_libraries(tetrium PRIVATE yaudio ycore yengine)
if(WIN32)
	set_property(TARGET tetrium APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
	target_link_libraries(tetrium PRIVATE optimized ywinmain)
	target_sources(tetrium PRIVATE tetrium.rc)
endif()

file(GLOB _localization_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/data/gui/*.ion)
add_custom_target(tetrium_localization
	COMMAND ytr data/i18n/en.ion data/gui.ion ${_localization_sources}
	COMMAND ytr data/i18n/ru.ion data/gui.ion ${_localization_sources}
	COMMENT "Updating localizations in ${CMAKE_CURRENT_SOURCE_DIR}/data/i18n/"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	BYPRODUCTS
		${CMAKE_CURRENT_SOURCE_DIR}/data/i18n/en.ion
		${CMAKE_CURRENT_SOURCE_DIR}/data/i18n/ru.ion
	)

add_ypack_target(tetrium_package
	INDEX data.index
	OUTPUT ${PROJECT_BINARY_DIR}/tetrium.ypq
	DEPENDS tetrium_localization)
add_dependencies(tetrium tetrium_package)

set_target_properties(tetrium tetrium_localization tetrium_package PROPERTIES FOLDER "examples")
