// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/exceptions.h>
#include <yttrium/base/logger.h>
#include <yttrium/ion/reader.h>
#include <yttrium/main.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include <yttrium/storage/yp_writer.h>

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
			const auto packagePath = std::filesystem::u8path(argv[2]);
			Yt::Writer fileWriter{ packagePath };
			if (!fileWriter)
			{
				std::cerr << "ERROR: Unable to open " << packagePath << " for writing\n";
				return 1;
			}
			auto packageWriter = std::make_unique<Yt::YpWriter>(std::move(fileWriter));
			for (const auto& path : read_index(std::filesystem::u8path(argv[1])))
				packageWriter->add(path);
			if (!packageWriter->commit())
			{
				std::cerr << "ERROR: Unable to write " << packagePath << '\n';
				packageWriter.reset();
				std::filesystem::remove(packagePath);
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
