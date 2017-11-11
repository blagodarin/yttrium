# Localization.
file(GLOB LOCALIZATION_SOURCES RELATIVE "${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/examples/tetrium/data/gui/*.ion")
add_custom_target(tetrium_localization
	ytr examples/tetrium/data/i18n/en.ion ${LOCALIZATION_SOURCES}
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
	BYPRODUCTS "${PROJECT_SOURCE_DIR}/examples/tetrium/data/i18n/en.ion"
	)

# Package.
add_custom_target(tetrium_package
	ypack examples/tetrium/data/index.txt tetrium.ypq
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
	BYPRODUCTS "${PROJECT_SOURCE_DIR}/tetrium.ypq"
	)
add_dependencies(tetrium_package tetrium_localization)

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
target_link_libraries(tetrium yttrium)
if(WIN32)
	target_link_libraries(tetrium ywinmain)
endif()
add_dependencies(tetrium tetrium_package)

set_target_properties(tetrium tetrium_localization tetrium_package PROPERTIES FOLDER "examples")
