add_executable(unit_tests
	buffer.cpp
	dir.cpp
	gui.cpp
	image.cpp
	ion.cpp
	key_lookup.cpp
	main.cpp
	margins.cpp
	memory.cpp
	package.cpp
	point.cpp
	reader.cpp
	rect.cpp
	static_string.cpp
	string.cpp
	string_format.cpp
	texture_font.cpp
	tiny_string_map.cpp
	unique_ptr.cpp
	utils.cpp
	vector.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(unit_tests PRIVATE "${PROJECT_SOURCE_DIR}")
target_link_libraries(unit_tests yttrium ${Boost_LIBRARIES})

add_custom_target(check COMMAND unit_tests WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
