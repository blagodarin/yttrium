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

include(${CMAKE_CURRENT_LIST_DIR}/../helpers/find.cmake)

y3_find_package(freetype C
	HEADER "ft2build.h"
	HEADER_PREFIX "freetype2"
	LIBRARY "freetype"
	VERSION_HEADER "freetype/freetype.h"
	VERSION_REGEX_MAJOR "^#define[ \t]+FREETYPE_MAJOR[ \t]+(.+)"
	VERSION_REGEX_MINOR "^#define[ \t]+FREETYPE_MINOR[ \t]+(.+)"
	VERSION_REGEX_PATCH "^#define[ \t]+FREETYPE_PATCH[ \t]+(.+)")
