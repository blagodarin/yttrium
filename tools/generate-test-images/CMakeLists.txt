add_executable(generate-test-images
	main.cpp
	)
target_link_libraries(generate-test-images yttrium)
set_target_properties(generate-test-images PROPERTIES FOLDER "tools")
