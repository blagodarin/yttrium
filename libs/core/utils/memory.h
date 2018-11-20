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

#ifndef _libs_yttrium_utils_memory_h_
#define _libs_yttrium_utils_memory_h_

#include <memory>

namespace Yttrium
{
	template <auto Deleter>
	struct UniquePtrDeleter
	{
		template <typename T>
		void operator()(T* p) noexcept
		{
			if (p)
				Deleter(p);
		}
	};

	template <typename T, auto Deleter>
	using UniquePtr = std::unique_ptr<T, UniquePtrDeleter<Deleter>>;
}

#endif
