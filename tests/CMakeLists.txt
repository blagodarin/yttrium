find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests
	src/buffer_appender.cpp
	src/buffer.cpp
	src/color.cpp
	src/flags.cpp
	src/gui_loading.cpp
	src/image_formats.cpp
	src/image_formats.h
	src/image.cpp
	src/image.h
	src/ion_reader.cpp
	src/ion_writer.cpp
	src/iostream.cpp
	src/iostream.h
	src/key.cpp
	src/main.cpp
	src/margins.cpp
	src/matrix.cpp
	src/package.cpp
	src/point.cpp
	src/reader.cpp
	src/rect.cpp
	src/resource_loader.cpp
	src/source.cpp
	src/string_utils.cpp
	src/temporary_file.cpp
	src/test_utils.cpp
	src/test_utils.h
	src/texture_font.cpp
	src/tiny_string_map.cpp
	src/translation.cpp
	src/utils.cpp
	src/vector.cpp
	)
target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
target_compile_definitions(unit_tests PRIVATE _HAS_AUTO_PTR_ETC) # Workaround for Boost 1.64.
target_include_directories(unit_tests PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(unit_tests yttrium ${Boost_LIBRARIES})

add_custom_target(check COMMAND unit_tests --log_level=message WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set_target_properties(unit_tests check PROPERTIES FOLDER "tests")

if(Y_COTIRE)
	set_source_files_properties(src/main.cpp PROPERTIES COTIRE_EXCLUDED ON)
	cotire(unit_tests)
endif()
