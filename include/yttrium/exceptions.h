//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _include_yttrium_exceptions_h_
#define _include_yttrium_exceptions_h_

#include <yttrium/utils/string.h>

#include <stdexcept>

namespace Yttrium
{
	/// Thrown if the initialization of an object (e.g. Window) fails.
	/// This usually means the inability to use the corresponding object.
	class InitializationError : public std::runtime_error
	{
	public:
		template <typename... Args>
		explicit InitializationError(Args&&... args)
			: std::runtime_error{ make_string(std::forward<Args>(args)...) }
		{}
	};

	/// Thrown if the requested operation is not supported by the implementation.
	class NotSupportedError : public std::runtime_error
	{
	public:
		template <typename... Args>
		explicit NotSupportedError(Args&&... args)
			: std::runtime_error{ make_string(std::forward<Args>(args)...) }
		{}
	};

	/// Thrown if a resource can't be loaded from the supplied data.
	class DataError : public std::runtime_error
	{
	public:
		template <typename... Args>
		explicit DataError(Args&&... args)
			: std::runtime_error{ make_string(std::forward<Args>(args)...) }
		{}
	};

	///
	class GuiDataError : public DataError
	{
	public:
		template <typename... Args>
		explicit GuiDataError(Args&&... args)
			: DataError{ std::forward<Args>(args)... }
		{}
	};

	/// Thrown when required data is missing.
	class MissingDataError : public std::runtime_error
	{
	public:
		template <typename... Args>
		explicit MissingDataError(Args&&... args)
			: std::runtime_error{ make_string(std::forward<Args>(args)...) }
		{}
	};

	/// Thrown by ResourceLoader if it is unable to find the specified resource.
	class ResourceError : public MissingDataError
	{
	public:
		template <typename... Args>
		explicit ResourceError(Args&&... args)
			: MissingDataError{ std::forward<Args>(args)... }
		{}
	};
}

#endif
