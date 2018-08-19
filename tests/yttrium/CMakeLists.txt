find_package(Catch2 REQUIRED)

add_executable(yttrium_tests
	main.cpp
	point.cpp
	)
include_config(yttrium_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(check2 COMMAND yttrium_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(yttrium_tests check2 PROPERTIES FOLDER "tests")
