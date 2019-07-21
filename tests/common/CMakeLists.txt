#
# This file is part of the Yttrium toolkit.
# Copyright (C) 2019 Sergei Blagodarin.
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
	src/utils.cpp
	)

add_library(ytest STATIC ${_src})
source_group("src" FILES ${_src})

y_sources(ytest "include"
	include/utils.h
	)

include_config(ytest ${PROJECT_BINARY_DIR}/config.h)
target_compile_definitions(ytest PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
target_link_libraries(ytest PUBLIC Catch2::Catch2 PRIVATE ycore)
set_target_properties(ytest PROPERTIES FOLDER "libs")
