# Localization.
file(GLOB LOCALIZATION_SOURCES RELATIVE "${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/examples/tetrium/data/gui/*.ion")
add_custom_target(tetrium_localization
	ytr examples/tetrium/data/i18n/en.ion ${LOCALIZATION_SOURCES}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
	BYPRODUCTS "${PROJECT_SOURCE_DIR}/examples/tetrium/data/i18n/en.ion"
	)

# Package.
add_ypack_target(tetrium_package
	OUTPUT ${PROJECT_BINARY_DIR}/tetrium.ypq
	INDEX data/index.txt
	DEPENDS tetrium_localization)

# Executable.
add_executable(tetrium WIN32
	../utils.h
	src/graphics.cpp
	src/graphics.h
	src/logic.cpp
	src/logic.h
	src/main.cpp
	src/statistics.cpp
	src/statistics.h
	)
target_link_libraries(tetrium YCore YEngine)
if(WIN32)
	set_property(TARGET tetrium APPEND_STRING PROPERTY LINK_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
	target_link_libraries(tetrium optimized YWinMain)
endif()
add_dependencies(tetrium tetrium_package)

set_target_properties(tetrium tetrium_localization tetrium_package PROPERTIES FOLDER "examples")
