add_library(YWinMain STATIC
	main.cpp
	)
set_target_properties(YWinMain PROPERTIES FOLDER "libs")
install(TARGETS YWinMain EXPORT YttriumTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
