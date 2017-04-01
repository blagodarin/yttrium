#ifndef _tests_buffer_h_
#define _tests_buffer_h_

#include <yttrium/memory/buffer.h>

#include <random>

inline Yttrium::Buffer make_random_buffer(size_t size)
{
	std::default_random_engine engine;
	std::uniform_int_distribution<unsigned> distribution{0, std::numeric_limits<uint8_t>::max()}; // Visual C++ 2017 doesn't allow uint8_t distribution.
	Yttrium::Buffer buffer(size);
	for (auto& byte : buffer)
		byte = static_cast<uint8_t>(distribution(engine));
	return buffer;
}

#endif
