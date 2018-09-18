add_executable(yrc
	main.cpp
	)
set_target_properties(yrc PROPERTIES FOLDER "tools")
install(TARGETS yrc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
