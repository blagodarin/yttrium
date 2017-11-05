add_executable(yglslc
	main.cpp
	)
target_link_libraries(yglslc Threads::Threads SPIRV glslang OSDependent OGLCompiler)
set_target_properties(yglslc PROPERTIES FOLDER "tools")

if(Y_SANITIZERS AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(yglslc PRIVATE -fno-sanitize=all)
	set_target_properties(yglslc PROPERTIES LINK_FLAGS -fno-sanitize=all)
endif()
