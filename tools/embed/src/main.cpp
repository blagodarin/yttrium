// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <seir_main/u8main.hpp>

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
	int usage()
	{
		std::cerr
			<< "Usage:\n"
			<< "  yembed --string INPUT OUTPUT\n"
			<< "  yembed --uint8 INPUT OUTPUT\n";
		return 1;
	}

	enum class Format
	{
		string,
		uint8,
	};
}

int u8main(int argc, char** argv)
{
	if (argc != 4)
		return usage();

	Format format = Format::string;
	if (std::strcmp(argv[1], "--uint8") == 0)
		format = Format::uint8;
	else if (std::strcmp(argv[1], "--string") != 0)
		return usage();

	std::vector<char> buffer;
	{
		const auto inputPath = std::filesystem::u8path(argv[2]);
		std::ifstream input{ inputPath, std::ios::binary | std::ios::in };
		if (!input.is_open())
		{
			std::cerr << "ERROR: Unable to open " << inputPath << '\n';
			return 1;
		}
		const auto inputSize = input.seekg(0, std::ios::end).tellg();
		input.seekg(0, std::ios_base::beg);
		buffer.resize(static_cast<size_t>(inputSize));
		if (!input.read(buffer.data(), inputSize))
		{
			std::cerr << "ERROR: Unable to read " << inputPath << '\n';
			return 1;
		}
	}

	const auto outputPath = std::filesystem::u8path(argv[3]);
	std::ofstream output{ outputPath, std::ios::out | std::ios::trunc };
	if (!output.is_open())
	{
		std::cerr << "ERROR: Unable to open " << outputPath << '\n';
		return 1;
	}

	switch (format)
	{
	case Format::string: {
		output << std::hex << '"';
		bool newline = false; // To prevent "" at the end of newline-terminated files.
		for (const auto c : buffer)
		{
			if (newline)
			{
				output << "\"\n\"";
				newline = false;
			}
			switch (c)
			{
			case '\0': output << "\\0"; break;
			case '\a': output << "\\a"; break;
			case '\b': output << "\\b"; break;
			case '\t': output << "\\t"; break;
			case '\n':
				output << "\\n";
				newline = true;
				break;
			case '\v': output << "\\v"; break;
			case '\f': output << "\\f"; break;
			case '\r': output << "\\r"; break;
			case '"': output << "\\\""; break;
			case '\\': output << "\\\\"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20)
					output << "\\x" << ((c & 0xf0) >> 4) << (c & 0xf);
				else
					output << c;
			}
		}
		output << "\"\n";
		break;
	}
	case Format::uint8: {
		std::array<char, 5> item;
		std::string line;
		for (const auto c : buffer)
		{
			const auto itemLength = static_cast<size_t>(std::snprintf(item.data(), item.size(), "%u,", unsigned{ static_cast<unsigned char>(c) }));
			if (line.size() + itemLength >= 80)
			{
				line += '\n';
				output << line;
				line.clear();
			}
			line.append(item.data(), itemLength);
		}
		line += '\n';
		output << line;
		break;
	}
	}
	return 0;
}
