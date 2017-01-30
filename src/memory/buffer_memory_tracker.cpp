#include "buffer_memory_tracker.h"

#if Y_ENABLE_BUFFER_MEMORY_TRACKING

#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
	#include <iostream>
	#include <sstream>
#endif

namespace Yttrium
{
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
	void BufferMemoryTracker::print_state(const std::map<size_t, size_t>& free_block_count)
	{
		const auto human_readable_size = [](size_t size)
		{
			std::ostringstream stream;
			if (size < 1024)
				stream << size << " B";
			else if (size < 1024 * 1024)
				stream << size / 1024. << " KiB";
			else
				stream << size / 1024. / 1024. << " MiB";
			return stream.str();
		};

		const auto max_allocated = _total_allocated.maximum_value();
		if (!max_allocated)
			return;
		std::cerr << "(DEBUG) Buffer memory statistics:";
		std::cerr << "\n(DEBUG)  * max_allocated : " << human_readable_size(max_allocated);
		std::cerr << "\n(DEBUG)  * max_total_capacity : " << human_readable_size(_total_capacity.maximum_value());
	#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
		std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations.maximum_value();
		std::cerr << "\n(DEBUG)  * total_system_allocations : " << _total_system_allocations;
		std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
		std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
		std::cerr << "\n(DEBUG)  * remaining_free_blocks : " << human_readable_size(_total_allocated.current_value());
		for (const auto& block_count : free_block_count)
			std::cerr << "\n(DEBUG)     - " << human_readable_size(block_count.first) << " : " << block_count.second;
	#endif
		std::cerr << "\n";
		const auto total_usable_memory = _total_capacity.current_value();
		if (total_usable_memory > 0)
		{
			std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(total_usable_memory);
		#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
			std::cerr << " (in " << _allocations.current_value() << " allocations)";
		#endif
			std::cerr << "\n";
		}
	}
#endif
}

#endif
