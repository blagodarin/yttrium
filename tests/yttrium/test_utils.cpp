#include "test_utils.h"

#include <yttrium/memory/buffer.h>

#include <random>

#include <catch/catch.hpp>

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

TEST_CASE("test_utils.make_buffer")
{
	const std::string string = "test";
	const auto buffer = ::make_buffer(string);
	CHECK(buffer.size() == string.size());
	CHECK(!std::memcmp(buffer.data(), string.data(), string.size()));
}
