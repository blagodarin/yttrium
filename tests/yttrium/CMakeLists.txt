find_package(Catch2)

add_executable(yttrium_tests
  buffer_appender.cpp
  color.cpp
  flags.cpp
  iostream.cpp
  iostream.h
  main.cpp
  margins.cpp
  point.cpp
  rect.cpp
  resource_loader.cpp
  source.cpp
  temporary_file.cpp
  test_utils.cpp
  test_utils.h
  texture_font.cpp
  tiny_string_map.cpp
  translation.cpp
  utils.cpp
  vector.cpp
  )
target_link_libraries(yttrium_tests PRIVATE yttrium Catch2::Catch)
include_config(yttrium_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(catch COMMAND yttrium_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(yttrium_tests catch PROPERTIES FOLDER "tests")
