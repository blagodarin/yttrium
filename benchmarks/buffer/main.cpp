#include <yttrium/memory/buffer.h>

#include <chrono>
#include <cstring>
#include <iostream>

using namespace Yttrium;

namespace
{
	const unsigned ops_per_iteration = 1000;
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

		bool add(unsigned ops)
		{
			_ops += ops;
			_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
			return _milliseconds < step_duration;
		}

	private:
		const std::string _name;
		unsigned _milliseconds = 0;
		unsigned _ops = 0;
		const std::chrono::steady_clock::time_point _start = std::chrono::steady_clock::now();
	};

	void common_allocate_deallocate(unsigned size)
	{
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
				::free(::malloc(size));
		} while (measurement.add(ops_per_iteration));
	}

	void buffer_allocate_deallocate(unsigned size)
	{
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
				Buffer buffer(size);
		} while (measurement.add(ops_per_iteration));
	}

	void common_allocate_touch_deallocate(unsigned size)
	{
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				const auto buffer = static_cast<uint8_t*>(::malloc(size));
				for (size_t i = 0; i < size; i += granularity)
					buffer[i] = -1;
				::free(buffer);
			}
		} while (measurement.add(ops_per_iteration));
	}

	void buffer_allocate_touch_deallocate(unsigned size)
	{
		const auto granularity = Buffer::memory_granularity();
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				Buffer buffer(size);
				for (size_t i = 0; i < size; i += granularity)
					buffer[i] = -1;
			}
		} while (measurement.add(ops_per_iteration));
	}

	void common_allocate_fill_deallocate(unsigned size)
	{
		Measurement measurement("common");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				const auto buffer = ::malloc(size);
				::memset(buffer, -1, size);
				::free(buffer);
			}
		} while (measurement.add(ops_per_iteration));
	}

	void buffer_allocate_fill_deallocate(unsigned size)
	{
		Measurement measurement("buffer");
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				Buffer buffer(size);
				::memset(buffer.data(), -1, size);
			}
		} while (measurement.add(ops_per_iteration));
	}
}

int main(int, char**)
{
	const auto human_readable_size = [](unsigned bytes) -> std::string
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
