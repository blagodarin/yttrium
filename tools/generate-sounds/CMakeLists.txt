add_executable(generate-sounds
	main.cpp
	)
target_link_libraries(generate-sounds yttrium)
set_target_properties(generate-sounds PROPERTIES FOLDER "tools")
