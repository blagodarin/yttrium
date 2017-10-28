add_executable(yglslc
	main.cpp
	)
target_link_libraries(yglslc Threads::Threads SPIRV glslang OSDependent OGLCompiler)
set_target_properties(yglslc PROPERTIES FOLDER "tools")
