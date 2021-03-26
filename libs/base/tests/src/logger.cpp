// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/logger.h>
#include "../../src/ring_log.h"

#include <mutex>
#include <thread>

#include <doctest.h>

TEST_CASE("logger.flush")
{
	std::vector<std::string> messages;
	std::mutex mutex;

	Yt::Logger logger{ [&](std::string_view message) {
		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
		std::scoped_lock lock{ mutex };
		messages.emplace_back(message);
	} };
	{
		std::scoped_lock lock{ mutex };
		CHECK(messages.empty());
	}

	Yt::Logger::write("Hello...");
	Yt::Logger::write("...world!");
	{
		std::scoped_lock lock{ mutex };
		CHECK(messages.empty());
	}

	Yt::Logger::flush();
	Yt::Logger::write("Hello?");
	{
		std::scoped_lock lock{ mutex };
		REQUIRE(messages.size() == 2);
		CHECK(messages[0] == "Hello...");
		CHECK(messages[1] == "...world!");
	}

	Yt::Logger::flush();
	{
		std::scoped_lock lock{ mutex };
		REQUIRE(messages.size() == 3);
		CHECK(messages[2] == "Hello?");
	}
}

TEST_CASE("logger.flush_on_destruction")
{
	std::vector<std::string> messages;
	std::mutex mutex;

	auto logger = std::make_unique<Yt::Logger>([&](std::string_view message) {
		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
		std::scoped_lock lock{ mutex };
		messages.emplace_back(message);
	});
	{
		std::scoped_lock lock{ mutex };
		CHECK(messages.empty());
	}

	Yt::Logger::write("Hello...");
	Yt::Logger::write("...world!");
	{
		std::scoped_lock lock{ mutex };
		CHECK(messages.empty());
	}

	logger.reset();
	{
		std::scoped_lock lock{ mutex };
		REQUIRE(messages.size() == 2);
		CHECK(messages[0] == "Hello...");
		CHECK(messages[1] == "...world!");
	}

	Yt::Logger::write("Hello?");
	Yt::Logger::flush();
	{
		std::scoped_lock lock{ mutex };
		CHECK(messages.size() == 2);
	}
}

TEST_CASE("logger.ring_log")
{
	constexpr size_t buffer_size = Yt::RingLog::BufferSize;
	constexpr size_t string_size = 251; // A prime number.
	static_assert(string_size <= Yt::RingLog::MaxStringSize);
	constexpr size_t max_strings = buffer_size / string_size;

	Yt::RingLog log;
	CHECK(log.empty());

	std::string string;
	CHECK(!log.pop(string));

	char next = 'A';
	for (size_t i = 0; i < max_strings + 1; ++i)
	{
		log.push(std::string(string_size, next));
		CHECK(!log.empty());
		next = next == 'Z' ? 'A' : static_cast<char>(next + 1);
	}

	next = 'B';
	for (size_t i = 0; i < max_strings; ++i)
	{
		CHECK(!log.empty());
		CHECK(log.pop(string));
		CHECK(string == std::string(string_size, next));
		next = next == 'Z' ? 'A' : static_cast<char>(next + 1);
	}

	CHECK(log.empty());
	CHECK(!log.pop(string));
}
