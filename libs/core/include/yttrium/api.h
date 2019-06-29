//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#	define Y_EXPORT __declspec(dllexport)
#	define Y_IMPORT __declspec(dllimport)
#	define Y_PRIVATE
#elif defined(__GNUC__)
#	define Y_EXPORT __attribute__((visibility("default")))
#	define Y_IMPORT
#	define Y_PRIVATE __attribute__((visibility("hidden")))
#else
#	error
#endif

#if defined(Y_CORE_EXPORT)
#	define Y_CORE_API Y_EXPORT
#elif defined(Y_CORE_IMPORT)
#	define Y_CORE_API Y_IMPORT
#else
#	define Y_CORE_API
#endif

#if defined(Y_ENGINE_EXPORT)
#	define Y_ENGINE_API Y_EXPORT
#elif defined(Y_ENGINE_IMPORT)
#	define Y_ENGINE_API Y_IMPORT
#else
#	define Y_ENGINE_API
#endif
