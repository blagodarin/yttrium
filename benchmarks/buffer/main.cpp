#include <yttrium/memory/buffer.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>

using namespace Yttrium;

namespace
{
	const unsigned ops_per_cycle = 10;
	const unsigned cycles_per_iteration = 100;
	const auto step_duration = 10000;

	class Measurement
	{
	public:
		Measurement(const std::string& name)
			: _name(name)
		{
		}

		~Measurement()
		{
			std::cout << "   - " << _name << ": "
				<< _milliseconds << " ms, "
				<< static_cast<unsigned>(_ops * 1000.0 / _milliseconds) << " ops/s, "
				<< static_cast<unsigned>(_milliseconds * 1000000.0 / _ops) << " ns/op"
				<< std::endl;
		}

		bool next_iteration()
		{
			_ops += ops_per_cycle * cycles_per_iteration;
			_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
			return _milliseconds < step_duration;
		}

	private:
		const std::string _name;
		unsigned _milliseconds = 0;
		unsigned _ops = 0;
		const std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
	};

	void common_allocate_deallocate(size_t size)
	{
		std::vector<uint8_t*> buffers(ops_per_cycle);
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
					buffer = static_cast<uint8_t*>(::malloc(size));
				for (auto& buffer : buffers)
					::free(buffer);
			}
		} while (measurement.next_iteration());
	}

	void buffer_allocate_deallocate(size_t size)
	{
		std::vector<Buffer> buffers(ops_per_cycle);
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
					buffer = Buffer(size);
				for (auto& buffer : buffers)
					buffer = {};
			}
		} while (measurement.next_iteration());
	}

	void common_allocate_touch_deallocate(size_t size)
	{
		std::vector<uint8_t*> buffers(ops_per_cycle);
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
				{
					buffer = static_cast<uint8_t*>(::malloc(size));
					for (size_t i = 0; i < size; i += granularity)
						buffer[i] = -1;
				}
				for (auto& buffer : buffers)
					::free(buffer);
			}
		} while (measurement.next_iteration());
	}

	void buffer_allocate_touch_deallocate(size_t size)
	{
		std::vector<Buffer> buffers(ops_per_cycle);
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
				{
					buffer = Buffer(size);
					for (size_t i = 0; i < size; i += granularity)
						buffer[i] = -1;
				}
				for (auto& buffer : buffers)
					buffer = {};
			}
		} while (measurement.next_iteration());
	}

	void common_allocate_fill_deallocate(size_t size)
	{
		std::vector<uint8_t*> buffers(ops_per_cycle);
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
				{
					buffer = static_cast<uint8_t*>(::malloc(size));
					::memset(buffer, -1, size);
				}
				for (auto& buffer : buffers)
					::free(buffer);
			}
		} while (measurement.next_iteration());
	}

	void buffer_allocate_fill_deallocate(size_t size)
	{
		std::vector<Buffer> buffers(ops_per_cycle);
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < cycles_per_iteration; ++i)
			{
				for (auto& buffer : buffers)
				{
					buffer = Buffer(size);
					::memset(buffer.data(), -1, size);
				}
				for (auto& buffer : buffers)
					buffer = {};
			}
		} while (measurement.next_iteration());
	}
}

int main(int, char**)
{
	const auto human_readable_size = [](size_t bytes)
	{
		if (bytes < 1024)
			return std::to_string(bytes) + " B";
		bytes /= 1024;
		if (bytes < 1024)
			return std::to_string(bytes) + " KiB";
		bytes /= 1024;
		return std::to_string(bytes) + " MiB";
	};

	std::cout << "Allocate-deallocate:" << std::endl;

	for (unsigned i = 16; i <= 24; ++i)
	{
		const auto size = size_t{1} << i;
		std::cout << " * " << human_readable_size(size) << ":" << std::endl;
		common_allocate_deallocate(size);
		buffer_allocate_deallocate(size);
	}

	std::cout << "\nAllocate-touch-deallocate:" << std::endl;

	for (unsigned i = 16; i <= 24; ++i)
	{
		const auto size = size_t{1} << i;
		std::cout << " * " << human_readable_size(size) << ":" << std::endl;
		common_allocate_touch_deallocate(size);
		buffer_allocate_touch_deallocate(size);
	}

	std::cout << "\nAllocate-fill-deallocate:" << std::endl;

	for (unsigned i = 16; i <= 24; ++i)
	{
		const auto size = size_t{1} << i;
		std::cout << " * " << human_readable_size(size) << ":" << std::endl;
		common_allocate_fill_deallocate(size);
		buffer_allocate_fill_deallocate(size);
	}

	return 0;
}
