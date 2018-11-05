add_executable(ytr
	main.cpp
	)
target_link_libraries(ytr YCore)
set_target_properties(ytr PROPERTIES FOLDER "tools")
install(TARGETS ytr RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
