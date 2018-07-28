add_executable(yglslc
	main.cpp
	)
find_library(_lib_glslang "glslang")
find_library(_lib_hlsl "HLSL")
find_library(_lib_oglcompiler "OGLCompiler")
find_library(_lib_osdependent "OSDependent")
find_library(_lib_spirv "SPIRV")
find_path(_include_glslang "SPIRV/GlslangToSpv.h")
target_include_directories(yglslc SYSTEM PRIVATE ${_include_glslang})
target_link_libraries(yglslc Threads::Threads ${_lib_glslang} ${_lib_hlsl} ${_lib_oglcompiler} ${_lib_osdependent} ${_lib_spirv})
set_target_properties(yglslc PROPERTIES FOLDER "tools")
