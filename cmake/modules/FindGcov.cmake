if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message(FATAL_ERROR "Gcov is only supported for the GNU compiler!")
endif()

get_filename_component(_gcc_directory ${CMAKE_CXX_COMPILER} DIRECTORY)
get_filename_component(_gcc_name ${CMAKE_CXX_COMPILER} NAME)
string(REPLACE "g++" "gcov" _gcov_name ${_gcc_name})
find_program(GCOV_EXECUTABLE ${_gcov_name} PATHS ${_gcc_directory})
unset(_gcc_directory)
unset(_gcc_name)
unset(_gcov_name)

if(GCOV_EXECUTABLE)
	execute_process(COMMAND ${GCOV_EXECUTABLE} --version OUTPUT_VARIABLE _gcov_version_output ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX MATCH "gcov \\(.+\\) [0-9]+\\.[0-9]+\\.[0-9]+" _gcov_version_line ${_gcov_version_output})
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+$" GCOV_VERSION_STRING ${_gcov_version_line})
	unset(_gcov_version_line)
	unset(_gcov_version_output)
endif()

mark_as_advanced(GCOV_EXECUTABLE)

find_package_handle_standard_args(Gcov
	REQUIRED_VARS GCOV_EXECUTABLE
	VERSION_VAR GCOV_VERSION_STRING)
