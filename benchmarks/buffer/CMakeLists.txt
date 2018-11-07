add_executable(benchmark_buffer
	main.cpp
	)
target_link_libraries(benchmark_buffer ycore)
set_target_properties(benchmark_buffer PROPERTIES FOLDER "benchmarks")
