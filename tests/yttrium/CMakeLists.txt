find_package(Catch2)

add_executable(yttrium_tests
  iostream.cpp
  iostream.h
  main.cpp
  margins.cpp
  point.cpp
  rect.cpp
  )
target_link_libraries(yttrium_tests PRIVATE yttrium Catch2::Catch)
include_config(yttrium_tests ${PROJECT_BINARY_DIR}/config.h)

add_custom_target(catch COMMAND yttrium_tests --reporter compact WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(yttrium_tests catch PROPERTIES FOLDER "tests")
