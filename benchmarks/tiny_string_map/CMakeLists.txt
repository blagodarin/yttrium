add_executable(benchmark_tiny_string_map
	main.cpp
	)
target_link_libraries(benchmark_tiny_string_map yttrium)
set_target_properties(benchmark_tiny_string_map PROPERTIES FOLDER "benchmarks")

if(Y_COTIRE)
	cotire(benchmark_tiny_string_map)
endif()
