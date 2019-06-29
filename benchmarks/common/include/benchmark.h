//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

template <int Iterations>
class Benchmark
{
public:
	template <typename Function>
	void add(std::string_view name, Function&& function)
	{
		auto& measurement = _measurements.emplace_back(name);
		measurement._times.reserve(Iterations);
		for (int i = 0; i < Iterations; ++i)
		{
			const auto start_time = Clock::now();
			if (!std::forward<Function>(function)())
				throw std::exception{};
			measurement._times.emplace_back(Clock::now() - start_time);
		}
		std::sort(measurement._times.begin(), measurement._times.end());
	}

	void print() const
	{
		const auto skip = Iterations / 8;
		const auto count = Iterations - 2 * skip;

		struct Result
		{
			const double _average_ns;
			const double _median_ns;
			constexpr Result(double average_ns, double median_ns) noexcept
				: _average_ns{ average_ns }, _median_ns{ median_ns } {}
		};

		std::vector<Result> results;
		results.reserve(_measurements.size());
		for (const auto& measurement : _measurements)
		{
			const auto begin = measurement._times.cbegin() + skip;
			const auto end = begin + count;
			const auto average_ns = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::accumulate(begin, end, Duration::zero())).count()) / count;
			const auto median_ns = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(*std::next(begin, count / 2) + *std::next(begin, (count + 1) / 2)).count()) * 0.5;
			results.emplace_back(average_ns, median_ns);
		}

		std::cerr << "Iterations: " << Iterations << "\n";
		for (std::size_t i = 0; i < results.size(); ++i)
		{
			using namespace std::literals::string_view_literals;
			auto average = results[i]._average_ns;
			const auto average_rate = 1'000'000'000.0 / average;
			auto median = results[i]._median_ns;
			const auto median_rate = 1'000'000'000.0 / median;
			auto units = "ns"sv;
			if (average >= 10'000'000'000.0)
			{
				average /= 1'000'000'000.0;
				median /= 1'000'000'000.0;
				units = "s"sv;
			}
			else if (average >= 10'000'000.0)
			{
				average /= 1'000'000.0;
				median /= 1'000'000.0;
				units = "ms"sv;
			}
			else if (average >= 10'000.0)
			{
				average /= 1'000.0;
				median /= 1'000.0;
				units = "us"sv;
			}
			std::cerr
				<< '\t' << _measurements[i]._name << " - avg. " << std::lround(average) << ' ' << units << "/op (" << std::lround(average_rate) << " ops/s)"
				<< ", med. " << std::lround(median) << ' ' << units << "/op (" << std::lround(median_rate) << " ops/s)";
			if (i > 0)
			{
				const auto baseline = results[0]._average_ns;
				const auto current = results[i]._average_ns;
				if (current > baseline)
					std::cerr << " - " << std::fixed << current / baseline << " times slower"; // TODO: Percentage.
				else
					std::cerr << " - " << std::fixed << baseline / current << " times faster";
			}
			std::cerr << '\n';
		}
	}

private:
	using Clock = std::chrono::high_resolution_clock;
	using Duration = Clock::duration;
	struct Measurement
	{
		std::string _name;
		std::vector<Duration> _times;
		explicit Measurement(std::string_view name)
			: _name{ name } {}
	};
	std::vector<Measurement> _measurements;
};
