find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests
	buffer.cpp
	gui_loading.cpp
	image_formats.cpp
	image_formats.h
	image.cpp
	image.h
	ion_reader.cpp
	ion_writer.cpp
	iostream.cpp
	iostream.h
	key.cpp
	main.cpp
	matrix.cpp
	package.cpp
	reader.cpp
	resource_loader.cpp
	source.cpp
	string_utils.cpp
	test_utils.cpp
	test_utils.h
	tiny_string_map.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(unit_tests PRIVATE yttrium Boost::unit_test_framework)
include_config(unit_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(check COMMAND unit_tests --log_level=message WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(unit_tests check PROPERTIES FOLDER "tests")
