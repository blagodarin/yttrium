#include "utils.h"

#include <yttrium/memory/buffer.h>

#include <cstring>
#include <random>

Yttrium::Buffer make_buffer(std::string_view text)
{
	return {text.size(), text.data()};
}

Yttrium::Buffer make_random_buffer(size_t size)
{
	std::default_random_engine engine;
	std::uniform_int_distribution<unsigned> distribution{0, std::numeric_limits<uint8_t>::max()}; // Visual C++ 2017 doesn't allow uint8_t distribution.
	Yttrium::Buffer buffer(size);
	for (auto& byte : buffer)
		byte = static_cast<uint8_t>(distribution(engine));
	return buffer;
}