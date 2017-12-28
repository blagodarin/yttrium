add_executable(yrc
	main.cpp
	)
target_link_libraries(yrc)
set_target_properties(yrc PROPERTIES FOLDER "tools")

if(Y_COTIRE)
	cotire(yrc)
endif()
