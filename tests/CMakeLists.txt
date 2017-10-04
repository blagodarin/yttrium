find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests
	buffer_appender.cpp
	buffer.cpp
	flags.cpp
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
	margins.cpp
	matrix.cpp
	package.cpp
	point.cpp
	reader.cpp
	rect.cpp
	resource_loader.cpp
	source.cpp
	string_utils.cpp
	test_utils.cpp
	test_utils.h
	texture_font.cpp
	tiny_string_map.cpp
	translation.cpp
	utils.cpp
	vector.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_compile_definitions(unit_tests PRIVATE _HAS_AUTO_PTR_ETC) # Workaround for Boost 1.64.
target_include_directories(unit_tests PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(unit_tests yttrium ${Boost_LIBRARIES})
