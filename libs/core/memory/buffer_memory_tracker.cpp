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

#include "buffer_memory_tracker.h"

#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING

#	include <iostream>
#	include <sstream>

namespace
{
	const auto human_readable_size(size_t size)
	{
		std::ostringstream stream;
		if (size < 1024)
			stream << size << " B";
		else if (size < 1024 * 1024)
			stream << static_cast<double>(size) / 1024. << " KiB";
		else
			stream << static_cast<double>(size) / 1024. / 1024. << " MiB";
		return stream.str();
	}
}

namespace Yttrium
{
	void BufferMemoryTracker::print_state(const std::map<size_t, size_t>& free_block_count)
	{
		if (const auto max_allocated = _total_allocated.maximum_value(); max_allocated > 0)
		{
			std::cerr << "(DEBUG) Buffer memory statistics:";
			std::cerr << "\n(DEBUG)  * max_allocated : " << ::human_readable_size(max_allocated);
			std::cerr << "\n(DEBUG)  * max_total_capacity : " << ::human_readable_size(_total_capacity.maximum_value());
			std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations.maximum_value();
			std::cerr << "\n(DEBUG)  * total_system_allocations : " << _total_system_allocations;
			std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
			std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
			std::cerr << "\n(DEBUG)  * remaining_free_blocks : " << ::human_readable_size(_total_allocated.current_value());
			for (const auto& [block_bytes, block_count] : free_block_count)
				std::cerr << "\n(DEBUG)     - " << ::human_readable_size(block_bytes) << " x " << block_count;
			std::cerr << "\n";
			if (const auto total_usable_memory = _total_capacity.current_value(); total_usable_memory > 0)
			{
				std::cerr << "(ERROR) Buffer memory leaked: " << ::human_readable_size(total_usable_memory);
				std::cerr << " (in " << _allocations.current_value() << " allocations)";
				std::cerr << "\n";
			}
		}
	}
}

#endif
