add_executable(3d WIN32
	../utils.h
	game.cpp
	game.h
	main.cpp
	model.cpp
	model.h
	)
target_link_libraries(3d yttrium)
if(WIN32)
	target_link_libraries(3d ywinmain)
endif()
