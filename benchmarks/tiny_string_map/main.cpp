//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/utils/tiny_string_map.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

using namespace Yttrium;

template <typename T>
void run_tiny(const T& map, const std::string& name)
{
	{
		std::cerr << "Tiny '" << name << "': 100'000'000 hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 25'000'000; ++i)
		{
			std::ignore = map.find("vwxyz01");
			std::ignore = map.find("abcdefg");
			std::ignore = map.find("opqrstu");
			std::ignore = map.find("hijklmn");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Tiny '" << name << "': 100'000'000 clear misses\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 25'000'000; ++i)
		{
			std::ignore = map.find("ABCDEFG");
			std::ignore = map.find("HIJKLMN");
			std::ignore = map.find("OPQRSTU");
			std::ignore = map.find("VWXYZ01");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Tiny '" << name << "': 100'000'000 almost-hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 25'000'000; ++i)
		{
			std::ignore = map.find("vwxyz02");
			std::ignore = map.find("abcdefG");
			std::ignore = map.find("opqrstU");
			std::ignore = map.find("hijklmN");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
}

template <typename T>
void run_small(const T& map, const std::string& name)
{
	{
		std::cerr << "Small '" << name << "': 100'000'000 hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 12'500'000; ++i)
		{
			std::ignore = map.find("VWXYZ01");
			std::ignore = map.find("abcdefg");
			std::ignore = map.find("ABCDEFG");
			std::ignore = map.find("vwxyz01");
			std::ignore = map.find("OPQRSTU");
			std::ignore = map.find("hijklmn");
			std::ignore = map.find("HIJKLMN");
			std::ignore = map.find("opqrstu");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Small '" << name << "': 100'000'000 clear misses\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 12'500'000; ++i)
		{
			std::ignore = map.find("1234567");
			std::ignore = map.find("qwertyu");
			std::ignore = map.find("sdfghjk");
			std::ignore = map.find("zxcvbnm");
			std::ignore = map.find("mnbvcxz");
			std::ignore = map.find("kjhgfds");
			std::ignore = map.find("uytrewq");
			std::ignore = map.find("7654321");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Small '" << name << "': 100'000'000 almost-hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 12'500'000; ++i)
		{
			std::ignore = map.find("VWXYZ02");
			std::ignore = map.find("abcdefG");
			std::ignore = map.find("ABCDEFg");
			std::ignore = map.find("vwxyz02");
			std::ignore = map.find("OPQRSTu");
			std::ignore = map.find("hijklmN");
			std::ignore = map.find("HIJKLMn");
			std::ignore = map.find("opqrstU");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
}

int main(int, char**)
{
	{
		std::map<std::string_view, std::string_view> map{
			{ "abcdefg", "ABCDEFG" },
			{ "hijklmn", "HIJKLMN" },
			{ "opqrstu", "OPQRSTU" },
			{ "vwxyz01", "VWXYZ01" },
		};
		run_tiny(map, "std::map");
	}
	{
		TinyStringMap map;
		map.insert_or_assign("abcdefg", "ABCDEFG");
		map.insert_or_assign("hijklmn", "HIJKLMN");
		map.insert_or_assign("opqrstu", "OPQRSTU");
		map.insert_or_assign("vwxyz01", "VWXYZ01");
		run_tiny(map, "TinyStringMap");
	}
	{
		std::map<std::string_view, std::string_view> map{
			{ "abcdefg", "ABCDEFG" },
			{ "hijklmn", "HIJKLMN" },
			{ "opqrstu", "OPQRSTU" },
			{ "vwxyz01", "VWXYZ01" },
			{ "ABCDEFG", "abcdefg" },
			{ "HIJKLMN", "hijklmn" },
			{ "OPQRSTU", "opqrstu" },
			{ "VWXYZ01", "vwxyz01" },
		};
		run_small(map, "std::map");
	}
	{
		TinyStringMap map;
		map.insert_or_assign("abcdefg", "ABCDEFG");
		map.insert_or_assign("hijklmn", "HIJKLMN");
		map.insert_or_assign("opqrstu", "OPQRSTU");
		map.insert_or_assign("vwxyz01", "VWXYZ01");
		map.insert_or_assign("ABCDEFG", "abcdefg");
		map.insert_or_assign("HIJKLMN", "hijklmn");
		map.insert_or_assign("OPQRSTU", "opqrstu");
		map.insert_or_assign("VWXYZ01", "vwxyz01");
		run_small(map, "TinyStringMap");
	}
}
