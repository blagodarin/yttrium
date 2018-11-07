add_executable(gen_test_data
	main.cpp
	)
target_link_libraries(gen_test_data ycore)
set_target_properties(gen_test_data PROPERTIES FOLDER "tests")
