add_executable(3d WIN32
	game.cpp
	main.cpp
	model.cpp
	)
target_link_libraries(3d yttrium)
if(WIN32)
	target_link_libraries(3d ywinmain)
endif()
