add_executable(tetrium
	blocks.cpp
	cursor.cpp
	game.cpp
	main.cpp
	tetrium.cpp
	)
target_link_libraries(tetrium yttrium)

# TODO: Build localization.

add_custom_target(tetrium_ypq
	ypack examples/tetrium/data/index.txt tetrium.ypq
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
add_dependencies(tetrium tetrium_ypq)
