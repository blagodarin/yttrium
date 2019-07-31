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

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
	int print_usage()
	{
		std::cerr
			<< "Usage:\n"
			<< "  yrc --string INPUT OUTPUT\n";
		return 1;
	}
}

int main(int argc, char** argv)
{
	if (argc != 4 || std::strcmp(argv[1], "--string")) //-V526
		return print_usage();

	std::vector<char> buffer;
	{
		std::ifstream input{ argv[2], std::ios::binary | std::ios::in };
		if (!input.is_open())
		{
			std::cerr << "ERROR: Unable to open \"" << argv[2] << "\"\n";
			return 1;
		}
		const auto input_size = input.seekg(0, std::ios::end).tellg();
		input.seekg(0, std::ios_base::beg);
		buffer.resize(static_cast<size_t>(input_size));
		if (!input.read(buffer.data(), input_size))
		{
			std::cerr << "ERROR: Unable to read \"" << argv[2] << "\"\n";
			return 1;
		}
	}

	std::ofstream output{ argv[3], std::ios::out | std::ios::trunc };
	if (!output.is_open())
	{
		std::cerr << "ERROR: Unable to open \"" << argv[3] << "\"\n";
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
