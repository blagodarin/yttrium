// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/logger.h>
#include <yttrium/main.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	template <typename... Args>
	void check(bool condition, Args&&... args)
	{
		if (!condition)
			throw Yt::DataError{ std::forward<Args>(args)... };
	}

	std::vector<std::string> read_index(const std::filesystem::path& path)
	{
		auto source = Yt::Source::from(path);
		check(static_cast<bool>(source), "Bad index file");
		Yt::IonReader ion{ *source };
		ion.read().check_name("package");
		ion.read().check_list_begin();
		std::vector<std::string> paths;
		for (auto token = ion.read(); token.type() != Yt::IonToken::Type::ListEnd; token = ion.read())
			paths.emplace_back(std::string{ token.to_value() });
		ion.read().check_end();
		return paths;
	}

	int usage()
	{
		std::cerr
			<< "Usage:\n"
			<< "  ypack INDEX PACKAGE\n"
			<< "  ypack --touch INDEX\n";
		return 1;
	}
}

int ymain(int argc, char** argv)
{
	Yt::Logger logger;
	if (argc < 2)
		return usage();
	try
	{
		if (!std::strcmp(argv[1], "--touch"))
		{
			if (argc != 3)
				return usage();
			const auto index_path = std::filesystem::u8path(argv[2]);
			auto data_timestamp = std::filesystem::file_time_type::min();
			for (const auto& path : read_index(index_path))
				data_timestamp = std::max(data_timestamp, std::filesystem::last_write_time(std::filesystem::u8path(path)));
			if (data_timestamp > std::filesystem::last_write_time(index_path))
				std::filesystem::last_write_time(index_path, data_timestamp);
		}
		else
		{
			if (argc != 3)
				return usage();
			const auto package_path = std::filesystem::u8path(argv[2]);
			const auto paths = read_index(std::filesystem::u8path(argv[1]));
			if (auto package = Yt::PackageWriter::create(package_path, Yt::PackageType::Ypq))
			{
				for (const auto& path : paths)
					package->add(path);
				if (!package->commit())
				{
					std::cerr << "ERROR: Unable to write " << package_path << '\n';
					package.reset();
					std::filesystem::remove(package_path);
					return 1;
				}
			}
			else
			{
				std::cerr << "ERROR: Unable to open " << package_path << " for writing\n";
				return 1;
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	return 0;
}
