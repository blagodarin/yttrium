// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/main.h>

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
			<< "  yrc --string INPUT OUTPUT\n";
		return 1;
	}
}

int ymain(int argc, char** argv)
{
	if (argc != 4 || std::strcmp(argv[1], "--string") != 0)
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
	return 0;
}
