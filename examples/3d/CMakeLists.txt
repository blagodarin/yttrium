add_ypack_target(3d_package
	OUTPUT ${PROJECT_BINARY_DIR}/3d.ypq
	INDEX data/index.txt)

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
add_dependencies(3d 3d_package)

set_target_properties(3d 3d_package PROPERTIES FOLDER "examples")
