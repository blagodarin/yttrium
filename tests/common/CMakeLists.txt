add_library(ytest STATIC
  main.cpp
  utils.cpp
  )
target_link_libraries(ytest PUBLIC Catch2::Catch2 PRIVATE ycore)
set_target_properties(ytest PROPERTIES FOLDER "tests")
