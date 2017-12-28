add_executable(benchmark_buffer
	main.cpp
	)
target_link_libraries(benchmark_buffer yttrium)
set_target_properties(benchmark_buffer PROPERTIES FOLDER "benchmarks")

if(Y_COTIRE)
	cotire(benchmark_buffer)
endif()
