add_library(ywinmain STATIC
	main.cpp
	)
set_target_properties(ywinmain PROPERTIES FOLDER "libs")
install(TARGETS ywinmain EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
