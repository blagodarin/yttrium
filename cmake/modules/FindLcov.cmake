find_program(LCOV_EXECUTABLE lcov)
find_program(GENHTML_EXECUTABLE genhtml)

if(LCOV_EXECUTABLE)
	execute_process(COMMAND ${LCOV_EXECUTABLE} --version OUTPUT_VARIABLE _lcov_version ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "[0-9]+\\.[0-9]+.*$" LCOV_VERSION_STRING ${_lcov_version})
	unset(_lcov_version)
endif()

mark_as_advanced(LCOV_EXECUTABLE GENHTML_EXECUTABLE)

find_package_handle_standard_args(Lcov
	REQUIRED_VARS LCOV_EXECUTABLE GENHTML_EXECUTABLE
	VERSION_VAR LCOV_VERSION_STRING)
