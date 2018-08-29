cmake_minimum_required(VERSION 3.0)
project(bootstrap_replace CXX)
add_executable(bootstrap_replace main.cpp)
install(TARGETS bootstrap_replace RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
