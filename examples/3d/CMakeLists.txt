add_executable(3d WIN32
	../utils.h
	src/game.cpp
	src/game.h
	src/main.cpp
	src/model.cpp
	src/model.h
	)
target_link_libraries(3d yttrium)
if(WIN32)
	target_link_libraries(3d ywinmain)
endif()
set_target_properties(3d PROPERTIES FOLDER "examples")
