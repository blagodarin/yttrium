#
# This file is part of the Yttrium toolkit
# Copyright (C) 2019 Sergei Blagodarin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(_src
	src/main.cpp
	)

add_executable(yglslc ${_src})
source_group("src" FILES ${_src})

find_library(_lib_glslang "glslang")
find_library(_lib_oglcompiler "OGLCompiler")
find_library(_lib_osdependent "OSDependent")
find_library(_lib_spirv "SPIRV")
find_path(_include_glslang "SPIRV/GlslangToSpv.h")
mark_as_advanced(_include_glslang _lib_glslang _lib_oglcompiler _lib_osdependent _lib_spirv)
target_include_directories(yglslc SYSTEM PRIVATE ${_include_glslang})
target_link_libraries(yglslc PRIVATE ${_lib_glslang} ${_lib_oglcompiler} ${_lib_osdependent} ${_lib_spirv} Threads::Threads)
set_target_properties(yglslc PROPERTIES FOLDER "tools")

install(TARGETS yglslc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
