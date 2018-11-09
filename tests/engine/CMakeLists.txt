add_executable(yengine_tests
  gui_loading.cpp
  key.cpp
  resource_loader.cpp
  texture_font.cpp
  )
target_link_libraries(yengine_tests PRIVATE ycore yengine ytest)
set_target_properties(yengine_tests PROPERTIES FOLDER "tests")
include_config(yengine_tests ${PROJECT_BINARY_DIR}/config.h)
