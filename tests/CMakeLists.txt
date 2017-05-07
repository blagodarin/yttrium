find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests
	buffer.cpp
	buffer.h
	flags.cpp
	gui.cpp
	image_formats.cpp
	image_formats.h
	image.cpp
	image.h
	ion_reader.cpp
	ion_writer.cpp
	ion.cpp
	iostream.cpp
	iostream.h
	key_lookup.cpp
	main.cpp
	margins.cpp
	matrix.cpp
	package.cpp
	point.cpp
	reader.cpp
	rect.cpp
	resource_loader.cpp
	source.cpp
	string_utils.cpp
	string.cpp
	texture_font.cpp
	tiny_string_map.cpp
	utils.cpp
	vector.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(unit_tests PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(unit_tests yttrium ${Boost_LIBRARIES})

add_custom_target(check COMMAND unit_tests WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
