#include <yttrium/memory/buffer.h>

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace Yttrium;

namespace
{
	constexpr auto Duration = 10000;

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

		Measurement(Results& results)
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

	std::array<uint8_t*, 1000> _malloc_allocations;
	std::array<Buffer, 1000> _buffer_allocations;

	struct BenchmarkSetup
	{
		const unsigned size;
		const unsigned blocks;
		const unsigned cycles;
		Measurement::Results malloc;
		Measurement::Results buffer;

		BenchmarkSetup(unsigned size, unsigned blocks, unsigned cycles)
			: size(size)
			, blocks(blocks)
			, cycles(cycles)
		{
			if (blocks > _malloc_allocations.size() || blocks > _buffer_allocations.size())
				throw std::logic_error("Insufficient allocation list size");
		}
	};

	BenchmarkSetup _benchmark_setups[] =
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

	void malloc_allocate_deallocate(BenchmarkSetup& setup)
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

	void buffer_allocate_deallocate(BenchmarkSetup& setup)
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

	void malloc_allocate_touch_deallocate(BenchmarkSetup& setup)
	{
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement(setup.malloc);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					const auto allocation = static_cast<uint8_t*>(::malloc(setup.size));
					for (size_t i = 0; i < setup.size; i += granularity)
						allocation[i] = -1;
					_malloc_allocations[j++] = allocation;
				}
				do
				{
					::free(_malloc_allocations[--j]);
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void buffer_allocate_touch_deallocate(BenchmarkSetup& setup)
	{
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement(setup.buffer);
		do
		{
			for (unsigned i = 0; i < setup.cycles; ++i)
			{
				unsigned j = 0;
				while (j < setup.blocks)
				{
					Buffer buffer(setup.size);
					for (size_t i = 0; i < setup.size; i += granularity)
						buffer[i] = -1;
					_buffer_allocations[j++] = std::move(buffer);
				}
				do
				{
					_buffer_allocations[--j] = {};
				} while (j > 0);
			}
		} while (measurement.next_iteration(setup.cycles * setup.blocks));
	}

	void malloc_allocate_fill_deallocate(BenchmarkSetup& setup)
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

	void buffer_allocate_fill_deallocate(BenchmarkSetup& setup)
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
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage:\n"
			"\t" << argv[0] << " 1  -- run \"Allocate-deallocate\" benchmark;\n"
			"\t" << argv[0] << " 2  -- run \"Allocate-touch-deallocate\" benchmark;\n"
			"\t" << argv[0] << " 3  -- run \"Allocate-fill-deallocate\" benchmark;\n"
			"\t" << argv[0] << "    -- print this usage." << std::endl;
		return 0;
	}

	const auto print_results = [](const std::string& name)
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

		std::cout << name << ":";
		for (const auto& setup : _benchmark_setups)
		{
			std::cout
				<< "\n * " << human_readable_size(setup.size) << " x " << setup.blocks << ":"
				<< "\n   - malloc: "
					<< setup.malloc.ms << " ms, "
					<< setup.malloc.ops_per_second() << " ops/s, "
					<< setup.malloc.ns_per_op() << " ns/op"
				<< "\n   - buffer: "
					<< setup.buffer.ms << " ms, "
					<< setup.buffer.ops_per_second() << " ops/s, "
					<< setup.buffer.ns_per_op() << " ns/op"
					<< " (" << static_cast<double>(setup.buffer.ops_per_second()) / setup.malloc.ops_per_second() << " x)";
		}
		std::cout << std::endl;
	};

	switch (::atoi(argv[1]))
	{
	case 1:
		for (auto& setup : _benchmark_setups)
		{
			malloc_allocate_deallocate(setup);
			buffer_allocate_deallocate(setup);
		}
		print_results("Allocate-deallocate");
		break;
	case 2:
		for (auto& setup : _benchmark_setups)
		{
			malloc_allocate_touch_deallocate(setup);
			buffer_allocate_touch_deallocate(setup);
		}
		print_results("Allocate-touch-deallocate");
		break;
	case 3:
		for (auto& setup : _benchmark_setups)
		{
			malloc_allocate_fill_deallocate(setup);
			buffer_allocate_fill_deallocate(setup);
		}
		print_results("Allocate-fill-deallocate");
		break;
	}

	return 0;
}
