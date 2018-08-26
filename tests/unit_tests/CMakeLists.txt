find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests
	gui_loading.cpp
	ion_reader.cpp
	ion_writer.cpp
	iostream.cpp
	iostream.h
	main.cpp
	matrix.cpp
	package.cpp
	reader.cpp
	test_utils.cpp
	test_utils.h
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(unit_tests PRIVATE yttrium Boost::unit_test_framework)
include_config(unit_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(check COMMAND unit_tests --log_level=message WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(unit_tests check PROPERTIES FOLDER "tests")
