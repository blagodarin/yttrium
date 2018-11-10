add_executable(ycore_tests
  buffer.cpp
  buffer_appender.cpp
  color.cpp
  flags.cpp
  image.cpp
  image.h
  image_formats.cpp
  image_formats.h
  ion_reader.cpp
  ion_writer.cpp
  iostream.cpp
  iostream.h
  margins.cpp
  matrix.cpp
  package.cpp
  point.cpp
  reader.cpp
  rect.cpp
  source.cpp
  temporary_file.cpp
  test_utils.cpp
  tiny_string_map.cpp
  translation.cpp
  utils_numeric.cpp
  utils_string.cpp
  vector.cpp
  )
target_link_libraries(ycore_tests PRIVATE ycore ytest)
set_target_properties(ycore_tests PROPERTIES FOLDER "tests")
include_config(ycore_tests ${PROJECT_BINARY_DIR}/config.h)
add_test(NAME ycore COMMAND ycore_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
