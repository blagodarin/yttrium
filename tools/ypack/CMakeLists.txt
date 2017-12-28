add_executable(ypack
	main.cpp
	)
target_link_libraries(ypack yttrium)
set_target_properties(ypack PROPERTIES FOLDER "tools")

if(Y_COTIRE)
	cotire(ypack)
endif()
