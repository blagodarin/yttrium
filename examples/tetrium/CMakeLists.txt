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
add_executable(tetrium
	cursor.cpp
	game.cpp
	graphics.cpp
	logic.cpp
	main.cpp
	statistics.cpp
	)
target_link_libraries(tetrium yttrium)
add_dependencies(tetrium tetrium_package)
