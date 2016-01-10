#include "buffer_memory_tracking.h"

#if Y_ENABLE_BUFFER_MEMORY_TRACKING

#define PRINT_TRACKING Y_IS_DEBUG

#if PRINT_TRACKING
	#include <iostream>
	#include <sstream>
#endif

namespace Yttrium
{
	BufferMemoryTracker _buffer_memory_tracker;

	BufferMemoryTracker::~BufferMemoryTracker()
	{
	#if PRINT_TRACKING
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
	#if Y_IS_DEBUG
		std::cerr << "\n(DEBUG)  * max_allocations : " << _allocations.maximum_value();
		std::cerr << "\n(DEBUG)  * total_system_allocations : " << _total_system_allocations;
		std::cerr << "\n(DEBUG)  * total_allocations : " << _total_allocations;
		std::cerr << "\n(DEBUG)  * total_reallocations : " << _total_reallocations;
		std::cerr << "\n(DEBUG)  * remaining_free_blocks : " << human_readable_size(_total_allocated.current_value());
		for (const auto& block_count : buffer_memory_free_block_count())
			std::cerr << "\n(DEBUG)     - " << human_readable_size(block_count.first) << " : " << block_count.second;
	#endif
		std::cerr << std::endl;
		const auto total_usable_memory = _total_capacity.current_value();
		if (total_usable_memory > 0)
		{
			std::cerr << "(ERROR) Buffer memory leaked: " << human_readable_size(total_usable_memory);
		#if Y_IS_DEBUG
			std::cerr << " (in " << _allocations.current_value() << " allocations)";
		#endif
			std::cerr << std::endl;
		}
	#endif
	}
}

#endif
