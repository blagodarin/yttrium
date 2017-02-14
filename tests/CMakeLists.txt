add_executable(unit_tests
	buffer.cpp
	gui.cpp
	image.cpp
	image.h
	image_formats.cpp
	image_formats.h
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
	string_utils.cpp
	texture_font.cpp
	tiny_string_map.cpp
	utils.cpp
	vector.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(unit_tests yttrium ${Boost_LIBRARIES})

add_custom_target(check COMMAND unit_tests WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
