#include <yttrium/memory/buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string.h>

#include <chrono>
#include <iostream>

using namespace Yttrium;

auto benchmark_v1(Reader& reader, int iterations)
{
	std::cout << "Benchmarking version 1...\n";
	String string;
	string.reserve(1024);
	const auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iterations; ++i)
	{
		reader.seek(0);
		while (reader.read_line(string))
			;
	}
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

auto benchmark_v2(Reader& reader, int iterations)
{
	std::cout << "Benchmarking version 2...\n";
	std::string string;
	string.reserve(1024);
	const auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iterations; ++i)
	{
		reader.seek(0);
		while (reader.read_line(string))
			;
	}
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int main(int, char**)
{
	Reader file_reader("LICENSE");
	if (!file_reader)
		return 1;
	Reader buffer_reader(file_reader.to_buffer());
	constexpr int iterations = 100000;
	const auto duration_v1 = benchmark_v1(buffer_reader, iterations);
	const auto duration_v2 = benchmark_v2(buffer_reader, iterations);
	std::cout << "Results:\n";
	std::cout << "\tVersion 1: " << duration_v1 << " ms\n";
	std::cout << "\tVersion 2: " << duration_v2 << " ms\n";
}
