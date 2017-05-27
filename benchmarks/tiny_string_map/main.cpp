#include <yttrium/tiny_string_map.h>

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
			map.find("vwxyz01");
			map.find("abcdefg");
			map.find("opqrstu");
			map.find("hijklmn");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Tiny '" << name << "': 100'000'000 clear misses\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 25'000'000; ++i)
		{
			map.find("ABCDEFG");
			map.find("HIJKLMN");
			map.find("OPQRSTU");
			map.find("VWXYZ01");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Tiny '" << name << "': 100'000'000 almost-hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 25'000'000; ++i)
		{
			map.find("vwxyz02");
			map.find("abcdefG");
			map.find("opqrstU");
			map.find("hijklmN");
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
			map.find("VWXYZ01");
			map.find("abcdefg");
			map.find("ABCDEFG");
			map.find("vwxyz01");
			map.find("OPQRSTU");
			map.find("hijklmn");
			map.find("HIJKLMN");
			map.find("opqrstu");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Small '" << name << "': 100'000'000 clear misses\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 12'500'000; ++i)
		{
			map.find("1234567");
			map.find("qwertyu");
			map.find("sdfghjk");
			map.find("zxcvbnm");
			map.find("mnbvcxz");
			map.find("kjhgfds");
			map.find("uytrewq");
			map.find("7654321");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
	{
		std::cerr << "Small '" << name << "': 100'000'000 almost-hits\n";
		const auto t0 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 12'500'000; ++i)
		{
			map.find("VWXYZ02");
			map.find("abcdefG");
			map.find("ABCDEFg");
			map.find("vwxyz02");
			map.find("OPQRSTu");
			map.find("hijklmN");
			map.find("HIJKLMn");
			map.find("opqrstU");
		}
		const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0).count();
		std::cerr << "\tDuration: " << dt << " ms\n\n";
	}
}

int main(int, char**)
{
	{
		std::map<std::string_view, std::string_view> map
		{
			{"abcdefg", "ABCDEFG"},
			{"hijklmn", "HIJKLMN"},
			{"opqrstu", "OPQRSTU"},
			{"vwxyz01", "VWXYZ01"},
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
		std::map<std::string_view, std::string_view> map
		{
			{"abcdefg", "ABCDEFG"},
			{"hijklmn", "HIJKLMN"},
			{"opqrstu", "OPQRSTU"},
			{"vwxyz01", "VWXYZ01"},
			{"ABCDEFG", "abcdefg"},
			{"HIJKLMN", "hijklmn"},
			{"OPQRSTU", "opqrstu"},
			{"VWXYZ01", "vwxyz01"},
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
