add_executable(ypack
	main.cpp
	)
target_link_libraries(ypack yttrium)
set_target_properties(ypack PROPERTIES FOLDER "tools")
install(TARGETS ypack RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
