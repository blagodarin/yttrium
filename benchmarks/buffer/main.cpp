#include <yttrium/memory/buffer.h>
#include "../../src/memory/buffer_memory.h"

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace Yttrium;

namespace
{
	constexpr auto Duration = 1000;

	const auto Granularity = Buffer::memory_granularity();
	const auto MaxSmallBlock = BufferMemory::MaxSmallBlockSize; // For brevity.

	class Measurement
	{
	public:
		struct Results
		{
			unsigned ops = 0;
			unsigned ms = 0;

			unsigned ns_per_op() const { return ms * 1000000.0 / ops; }
			unsigned ops_per_second() const { return ops * 1000.0 / ms; }
		};

		explicit Measurement(Results& results)
			: _results(results)
		{
		}

		bool next_iteration(unsigned ops)
		{
			_results.ops += ops;
			_results.ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
			return _results.ms < Duration;
		}

	private:
		Results& _results;
		const std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
	};

	double operator/(const Measurement::Results& lhs, const Measurement::Results& rhs)
	{
		return (static_cast<double>(lhs.ops) / lhs.ms) / (static_cast<double>(rhs.ops) / rhs.ms);
	}

	struct BenchmarkSetup
	{
		Measurement::Results malloc;
		Measurement::Results buffer;
	};

	std::ostream& operator<<(std::ostream& stream, const BenchmarkSetup& setup)
	{
		stream << "\n   - malloc: "
			<< setup.malloc.ms << " ms, "
			<< setup.malloc.ops_per_second() << " ops/s, "
			<< setup.malloc.ns_per_op() << " ns/op";
		stream << "\n   - buffer: " << setup.buffer.ms << " ms, "
			<< setup.buffer.ops_per_second() << " ops/s, "
			<< setup.buffer.ns_per_op() << " ns/op"
			<< " (" << setup.buffer / setup.malloc << " x)";
		return stream;
	}

	std::string human_readable_size(size_t size)
	{
		std::ostringstream stream;
		if (size < 1024)
			stream << size << " B";
		else if (size < 1024 * 1024)
			stream << size / 1024. << " KiB";
		else
			stream << size / 1024. / 1024. << " MiB";
		return stream.str();
	}

	std::array<uint8_t*, 1000> _malloc_allocations;
	std::array<Buffer, 1000> _buffer_allocations;

	struct AllocateSetup : BenchmarkSetup
	{
		const size_t size;
		const unsigned blocks;
		const unsigned cycles;

		AllocateSetup(size_t size_, unsigned blocks_, unsigned cycles_)
			: size(size_)
			, blocks(blocks_)
			, cycles(cycles_)
		{
			if (blocks > _malloc_allocations.size() || blocks > _buffer_allocations.size())
				throw std::logic_error("Insufficient allocation list size");
		}
	};

	std::ostream& operator<<(std::ostream& stream, const AllocateSetup& setup)
	{
		stream << "\n * " << human_readable_size(setup.size) << " x " << setup.blocks << ":";
		stream << static_cast<const BenchmarkSetup&>(setup);
		return stream;
	}

	AllocateSetup _allocate_setups[] =
	{
		// TODO: Small blocks.

		// Allocating big blocks using more and more memory for all allocations
		// leaving enough space for service data, if any, in both allocations and memory.
		// Number of cycles was experimentally chosen to produce reasonable cycle duration.

		{   63 * 1024,   7, 65536 }, // 0.5 MiB
		{   63 * 1024,  15, 65536 }, // 1 MiB
		{   63 * 1024,  23,  4096 }, // 1.5 MiB
		{   63 * 1024,  31,  2048 }, // 2 MiB
		{   63 * 1024,  63,  1024 }, // 4 MiB
		{   63 * 1024, 127,   256 }, // 8 MiB

		{  127 * 1024,  31,   512 }, // 4 MiB
		{  127 * 1024,  63,   256 }, // 8 MiB
		{  127 * 1024, 127,   128 }, // 16 MiB

		{  255 * 1024,  31,   256 }, // 8 MiB
		{  255 * 1024,  63,   128 }, // 16 MiB
		{  255 * 1024, 127,    64 }, // 32 MiB

		{  511 * 1024,  31,   128 }, // 16 MiB
		{  511 * 1024,  63,    64 }, // 32 MiB
		{  511 * 1024, 127,    32 }, // 64 MiB

		{ 1023 * 1024,  31,    64 }, // 32 MiB
		{ 1023 * 1024,  63,    32 }, // 64 MiB
		{ 1023 * 1024, 127,    16 }, // 128 MiB

		{ 2047 * 1024,  31,    32 }, // 64 MiB
		{ 2047 * 1024,  63,    16 }, // 128 MiB
		{ 2047 * 1024, 127,     8 }, // 256 MiB
	};

	void malloc_allocate(AllocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
					_malloc_allocations[j++] = static_cast<uint8_t*>(::malloc(setup.size));
				do
				{
					::free(_malloc_allocations[--j]);
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void buffer_allocate(AllocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
					_buffer_allocations[j++] = Buffer(setup.size);
				do
				{
					_buffer_allocations[--j] = {};
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void malloc_allocate_touch(AllocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					const auto allocation = static_cast<uint8_t*>(::malloc(setup.size));
					for (std::size_t k = 0; k < setup.size; k += Granularity)
						allocation[k] = 1;
					_malloc_allocations[j++] = allocation;
				}
				do
				{
					::free(_malloc_allocations[--j]);
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void buffer_allocate_touch(AllocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					Buffer buffer(setup.size);
					for (std::size_t k = 0; k < setup.size; k += Granularity)
						buffer[k] = 1;
					_buffer_allocations[j++] = std::move(buffer);
				}
				do
				{
					_buffer_allocations[--j] = {};
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void malloc_allocate_fill(AllocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					const auto allocation = static_cast<uint8_t*>(::malloc(setup.size));
					::memset(allocation, -1, setup.size);
					_malloc_allocations[j++] = allocation;
				}
				do
				{
					::free(_malloc_allocations[--j]);
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void buffer_allocate_fill(AllocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					Buffer buffer(setup.size);
					::memset(buffer.data(), -1, setup.size);
					_buffer_allocations[j++] = std::move(buffer);
				}
				do
				{
					_buffer_allocations[--j] = {};
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	struct ReallocateSetup : BenchmarkSetup
	{
		const size_t initial_size;
		const size_t final_size;
		const unsigned cycles;

		ReallocateSetup(size_t initial_size_, size_t final_size_, unsigned cycles_)
			: initial_size(initial_size_), final_size(final_size_), cycles(cycles_) {}
	};

	std::ostream& operator<<(std::ostream& stream, const ReallocateSetup& setup)
	{
		stream << "\n * " << human_readable_size(setup.initial_size) << " - " << human_readable_size(setup.final_size) << ":";
		stream << static_cast<const BenchmarkSetup&>(setup);
		return stream;
	}

	ReallocateSetup _grow_setups[] =
	{
		{                 1,       Granularity, 100000 },
		{       Granularity,     MaxSmallBlock,   1000 },
		{     MaxSmallBlock, 2 * MaxSmallBlock,   1000 },
		{ 2 * MaxSmallBlock, 256 * 1024 * 1024,    100 },
		{                 1, 256 * 1024 * 1024,    100 },
	};

	void malloc_grow(ReallocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				void* allocation = nullptr;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
					allocation = ::realloc(allocation, size);
				::free(allocation);
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	void buffer_grow(ReallocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				Buffer buffer;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
					buffer.resize(size);
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	ReallocateSetup _grow_touch_setups[] =
	{
		{                 1,       Granularity, 100000 },
		{       Granularity,     MaxSmallBlock,   1000 },
		{     MaxSmallBlock, 2 * MaxSmallBlock,   1000 },
		{ 2 * MaxSmallBlock, 256 * 1024 * 1024,      1 },
		{                 1, 256 * 1024 * 1024,      1 },
	};

	void malloc_grow_touch(ReallocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				void* allocation = nullptr;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
				{
					allocation = ::realloc(allocation, size);
					for (auto j = size / 2; j < size; j += Granularity)
						static_cast<uint8_t*>(allocation)[j] = 1;
				}
				::free(allocation);
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	void buffer_grow_touch(ReallocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				Buffer buffer;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
				{
					buffer.resize(size);
					for (auto j = size / 2; j < size; j += Granularity)
						buffer[j] = 1;
				}
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	ReallocateSetup _grow_fill_setups[] =
	{
		{                 1,       Granularity, 100000 },
		{       Granularity,     MaxSmallBlock,   1000 },
		{     MaxSmallBlock, 2 * MaxSmallBlock,   1000 },
		{ 2 * MaxSmallBlock, 256 * 1024 * 1024,      1 },
		{                 1, 256 * 1024 * 1024,      1 },
	};

	void malloc_grow_fill(ReallocateSetup& setup)
	{
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				void* allocation = nullptr;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
				{
					allocation = ::realloc(allocation, size);
					::memset(static_cast<uint8_t*>(allocation) + size / 2, -1, size - size / 2);
				}
				::free(allocation);
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	void buffer_grow_fill(ReallocateSetup& setup)
	{
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				Buffer buffer;
				for (auto size = setup.initial_size; size <= setup.final_size; size *= 2)
				{
					buffer.resize(size);
					::memset(buffer.begin() + size / 2, -1, size - size / 2);
				}
			}
		} while (measurement.next_iteration(setup.cycles));
	}

	template <typename Setups, typename Setup>
	void run_benchmark(const char* name, Setups& setups, void (*malloc_function)(Setup&), void (*buffer_function)(Setup&))
	{
		for (auto& setup : setups)
			malloc_function(setup);
		for (auto& setup : setups)
			buffer_function(setup);
		std::cout << name << ":";
		for (const auto& setup : setups)
			std::cout << setup;
		std::cout << "\n";
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage:"
			"\n\t" << argv[0] << " 1  -- run \"Allocate\" benchmark;"
			"\n\t" << argv[0] << " 2  -- run \"Allocate & touch\" benchmark;"
			"\n\t" << argv[0] << " 3  -- run \"Allocate & fill\" benchmark;"
			"\n\t" << argv[0] << " 4  -- run \"Grow\" benchmark;"
			"\n\t" << argv[0] << " 5  -- run \"Grow & touch\" benchmark;"
			"\n\t" << argv[0] << " 6  -- run \"Grow & fill\" benchmark;"
			"\n\t" << argv[0] << "    -- print this usage."
			"\n";
		return 0;
	}

	switch (::atoi(argv[1]))
	{
	case 1: run_benchmark("Allocate",         _allocate_setups,   malloc_allocate,       buffer_allocate);       break;
	case 2: run_benchmark("Allocate & touch", _allocate_setups,   malloc_allocate_touch, buffer_allocate_touch); break;
	case 3: run_benchmark("Allocate & fill",  _allocate_setups,   malloc_allocate_fill,  buffer_allocate_fill);  break;
	case 4: run_benchmark("Grow",             _grow_setups,       malloc_grow,           buffer_grow);           break;
	case 5: run_benchmark("Grow & touch",     _grow_touch_setups, malloc_grow_touch,     buffer_grow_touch);     break;
	case 6: run_benchmark("Grow & fill",      _grow_fill_setups,  malloc_grow_fill,      buffer_grow_fill);      break;
	}
}
