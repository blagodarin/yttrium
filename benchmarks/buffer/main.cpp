#include <yttrium/memory/buffer.h>

#include <chrono>
#include <cstring>
#include <iostream>

using namespace Yttrium;

namespace
{
	const unsigned ops_per_iteration = 1000;

	void common_allocate_deallocate(unsigned size, unsigned duration)
	{
		const auto start = std::chrono::steady_clock::now();
		unsigned milliseconds = 0;
		unsigned ops = 0;
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
				::free(::malloc(size));
			ops += ops_per_iteration;
			milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		} while (milliseconds < duration);
		std::cout << " * common: " << milliseconds << " ms, " << static_cast<unsigned>(ops * 1000.0 / milliseconds) << " ops/s" << std::endl;
	}

	void buffer_allocate_deallocate(unsigned size, unsigned duration)
	{
		const auto start = std::chrono::steady_clock::now();
		unsigned milliseconds = 0;
		unsigned ops = 0;
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
				Buffer buffer(size);
			ops += ops_per_iteration;
			milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		} while (milliseconds < duration);
		std::cout << " * buffer: " << milliseconds << " ms, " << static_cast<unsigned>(ops * 1000.0 / milliseconds) << " ops/s" << std::endl;
	}

	void common_allocate_fill_deallocate(unsigned size, unsigned duration)
	{
		const auto start = std::chrono::steady_clock::now();
		unsigned milliseconds = 0;
		unsigned ops = 0;
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				const auto buffer = ::malloc(size);
				::memset(buffer, -1, size);
				::free(buffer);
			}
			ops += ops_per_iteration;
			milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		} while (milliseconds < duration);
		std::cout << " * common: " << milliseconds << " ms, " << static_cast<unsigned>(ops * 1000.0 / milliseconds) << " ops/s" << std::endl;
	}

	void buffer_allocate_fill_deallocate(unsigned size, unsigned duration)
	{
		const auto start = std::chrono::steady_clock::now();
		unsigned milliseconds = 0;
		unsigned ops = 0;
		do
		{
			for (unsigned i = 0; i < ops_per_iteration; ++i)
			{
				Buffer buffer(size);
				::memset(buffer.data(), -1, size);
			}
			ops += ops_per_iteration;
			milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		} while (milliseconds < duration);
		std::cout << " * buffer: " << milliseconds << " ms, " << static_cast<unsigned>(ops * 1000.0 / milliseconds) << " ops/s" << std::endl;
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

	const auto run_duration = 1000;

	for (unsigned i = 16; i <= 24; ++i)
	{
		const auto size = size_t{1} << i;
		std::cout << "Allocate-deallocate, " << human_readable_size(size) << ":" << std::endl;
		common_allocate_deallocate(size, run_duration);
		buffer_allocate_deallocate(size, run_duration);
	}

	std::cout << std::endl;

	for (unsigned i = 16; i <= 24; ++i)
	{
		const auto size = size_t{1} << i;
		std::cout << "Allocate-fill-deallocate, " << human_readable_size(size) << ":" << std::endl;
		common_allocate_fill_deallocate(size, run_duration);
		buffer_allocate_fill_deallocate(size, run_duration);
	}

	return 0;
}
