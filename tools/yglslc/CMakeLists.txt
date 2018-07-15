add_executable(yglslc
	main.cpp
	)
target_include_directories(yglslc SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/include)
find_library(_lib_spirv "SPIRV" ${PROJECT_SOURCE_DIR}/3rdparty/lib)
find_library(_lib_glslang "glslang" ${PROJECT_SOURCE_DIR}/3rdparty/lib)
find_library(_lib_osdependent "OSDependent" ${PROJECT_SOURCE_DIR}/3rdparty/lib)
find_library(_lib_oglcompiler "OGLCompiler" ${PROJECT_SOURCE_DIR}/3rdparty/lib)
target_link_libraries(yglslc Threads::Threads ${_lib_spirv} ${_lib_glslang} ${_lib_osdependent} ${_lib_oglcompiler})
set_target_properties(yglslc PROPERTIES FOLDER "tools")

if(Y_SANITIZERS AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(yglslc PRIVATE -fno-sanitize=all)
	set_target_properties(yglslc PROPERTIES LINK_FLAGS -fno-sanitize=all)
endif()
