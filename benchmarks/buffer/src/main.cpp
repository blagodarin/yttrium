//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/memory/buffer.h>

#include <array>
#include <cstdlib>

#include <catch2/catch.hpp>

namespace
{
	const auto Granularity = Yt::Buffer::memory_granularity();
	constexpr size_t MaxBlocks = 1024; // 64 MiB.

	template <size_t BlockSize, size_t BlockCount>
	void allocate(std::array<Yt::Buffer, BlockCount>& buffers)
	{
		for (auto& buffer : buffers)
			buffer.reset(BlockSize);
		for (auto i = buffers.size(); i > 0;)
			buffers[--i] = {};
	}

	template <size_t BlockSize, size_t BlockCount>
	void allocate(std::array<uint8_t*, BlockCount>& pointers)
	{
		for (auto& pointer : pointers)
			pointer = static_cast<uint8_t*>(std::malloc(BlockSize));
		for (auto i = pointers.size(); i > 0;)
			std::free(pointers[--i]);
	}

	template <size_t BlockSize, size_t BlockCount>
	void allocate_and_touch(std::array<Yt::Buffer, BlockCount>& buffers)
	{
		for (auto& buffer : buffers)
		{
			buffer.reset(BlockSize);
			for (size_t i = 0; i < BlockSize; i += Granularity)
				buffer[i] = 1;
		}
		for (auto i = buffers.size(); i > 0;)
			buffers[--i] = {};
	}

	template <size_t BlockSize, size_t BlockCount>
	void allocate_and_touch(std::array<uint8_t*, BlockCount>& pointers)
	{
		for (auto& pointer : pointers)
		{
			pointer = static_cast<uint8_t*>(std::malloc(BlockSize));
			for (size_t i = 0; i < BlockSize; i += Granularity)
				pointer[i] = 1;
		}
		for (auto i = pointers.size(); i > 0;)
			std::free(pointers[--i]);
	}

	template <size_t BlockSize, size_t BlockCount>
	void allocate_and_fill(std::array<Yt::Buffer, BlockCount>& buffers)
	{
		for (auto& buffer : buffers)
		{
			buffer.reset(BlockSize);
			std::memset(buffer.data(), 1, BlockSize);
		}
		for (auto i = buffers.size(); i > 0;)
			buffers[--i] = {};
	}

	template <size_t BlockSize, size_t BlockCount>
	void allocate_and_fill(std::array<uint8_t*, BlockCount>& pointers)
	{
		for (auto& pointer : pointers)
		{
			pointer = static_cast<uint8_t*>(std::malloc(BlockSize));
			std::memset(pointer, 1, BlockSize);
		}
		for (auto i = pointers.size(); i > 0;)
			std::free(pointers[--i]);
	}
}

TEST_CASE("Allocate")
{
	BENCHMARK_ADVANCED("buffer (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks> buffers;
		meter.measure([&buffers] { ::allocate<63 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks> pointers;
		meter.measure([&pointers] { ::allocate<63 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 2> buffers;
		meter.measure([&buffers] { ::allocate<127 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 2> pointers;
		meter.measure([&pointers] { ::allocate<127 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 4> buffers;
		meter.measure([&buffers] { ::allocate<255 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 4> pointers;
		meter.measure([&pointers] { ::allocate<255 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 8> buffers;
		meter.measure([&buffers] { ::allocate<511 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 8> pointers;
		meter.measure([&pointers] { ::allocate<511 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 16> buffers;
		meter.measure([&buffers] { ::allocate<1023 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 16> pointers;
		meter.measure([&pointers] { ::allocate<1023 * 1024>(pointers); });
	};
}

TEST_CASE("Allocate and touch")
{
	BENCHMARK_ADVANCED("buffer (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks> buffers;
		meter.measure([&buffers] { ::allocate_and_touch<63 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks> pointers;
		meter.measure([&pointers] { ::allocate_and_touch<63 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 2> buffers;
		meter.measure([&buffers] { ::allocate_and_touch<127 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 2> pointers;
		meter.measure([&pointers] { ::allocate_and_touch<127 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 4> buffers;
		meter.measure([&buffers] { ::allocate_and_touch<255 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 4> pointers;
		meter.measure([&pointers] { ::allocate_and_touch<255 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 8> buffers;
		meter.measure([&buffers] { ::allocate_and_touch<511 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 8> pointers;
		meter.measure([&pointers] { ::allocate_and_touch<511 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 16> buffers;
		meter.measure([&buffers] { ::allocate_and_touch<1023 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 16> pointers;
		meter.measure([&pointers] { ::allocate_and_touch<1023 * 1024>(pointers); });
	};
}

TEST_CASE("Allocate and fill")
{
	BENCHMARK_ADVANCED("buffer (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks> buffers;
		meter.measure([&buffers] { ::allocate_and_fill<63 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (63 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks> pointers;
		meter.measure([&pointers] { ::allocate_and_fill<63 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 2> buffers;
		meter.measure([&buffers] { ::allocate_and_fill<127 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (127 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 2> pointers;
		meter.measure([&pointers] { ::allocate_and_fill<127 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 4> buffers;
		meter.measure([&buffers] { ::allocate_and_fill<255 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (255 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 4> pointers;
		meter.measure([&pointers] { ::allocate_and_fill<255 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 8> buffers;
		meter.measure([&buffers] { ::allocate_and_fill<511 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (511 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 8> pointers;
		meter.measure([&pointers] { ::allocate_and_fill<511 * 1024>(pointers); });
	};

	BENCHMARK_ADVANCED("buffer (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<Yt::Buffer, MaxBlocks / 16> buffers;
		meter.measure([&buffers] { ::allocate_and_fill<1023 * 1024>(buffers); });
	};

	BENCHMARK_ADVANCED("malloc (1023 KiB)")
	(Catch::Benchmark::Chronometer meter)
	{
		std::array<uint8_t*, MaxBlocks / 16> pointers;
		meter.measure([&pointers] { ::allocate_and_fill<1023 * 1024>(pointers); });
	};
}
