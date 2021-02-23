# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

primal_set_folder(examples)
source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")

add_executable(tetrium WIN32
	src/graphics.cpp
	src/graphics.h
	src/logic.cpp
	src/logic.h
	src/main.cpp
	)
target_link_libraries(tetrium PRIVATE yaudio ycore yengine ymain)
set_target_properties(tetrium PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(tetrium PRIVATE "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,LIBCMT,LIBCMTD>")
endif()
if(WIN32)
	set_property(TARGET tetrium APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
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

if(INSTALL_EXAMPLES)
	primal_install(tetrium)
	install(FILES ${PROJECT_BINARY_DIR}/tetrium.ypq DESTINATION ${CMAKE_INSTALL_DATADIR})
endif()
