add_executable(ypack
	main.cpp
	)
target_link_libraries(ypack yttrium ${Boost_LIBRARIES})
