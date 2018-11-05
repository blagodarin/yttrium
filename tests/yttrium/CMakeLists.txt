add_executable(yttrium_tests
  buffer.cpp
  buffer_appender.cpp
  color.cpp
  flags.cpp
  gui_loading.cpp
  image.cpp
  image.h
  image_formats.cpp
  image_formats.h
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
  temporary_file.cpp
  test_utils.cpp
  test_utils.h
  texture_font.cpp
  tiny_string_map.cpp
  translation.cpp
  utils_numeric.cpp
  utils_string.cpp
  vector.cpp
  )
target_link_libraries(yttrium_tests PRIVATE YCore YEngine Catch2::Catch2)
include_config(yttrium_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(check COMMAND yttrium_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(yttrium_tests check PROPERTIES FOLDER "tests")
