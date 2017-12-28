add_executable(ytr
	main.cpp
	)
target_link_libraries(ytr yttrium)
set_target_properties(ytr PROPERTIES FOLDER "tools")

if(Y_COTIRE)
	cotire(ytr)
endif()
