// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/exceptions.h>
#include <yttrium/base/logger.h>
#include <yttrium/base/string.h>
#include <yttrium/ion/reader.h>
#include <yttrium/storage/compressor.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include <yttrium/storage/yp_writer.h>

#include <seir_main/u8main.hpp>

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

	struct Index
	{
		struct FileGroup
		{
			int32_t _compressionLevel = 0;
			std::vector<std::string> _files;
		};
		Yt::YpWriter::Compression _compression = Yt::YpWriter::Compression::None;
		std::vector<FileGroup> _groups;
	};

	Index readIndex(const std::filesystem::path& path)
	{
		auto source = Yt::Source::from(path);
		check(static_cast<bool>(source), "Bad index file");
		Index result;
		Yt::IonReader ion{ *source };
		auto token = ion.read();
		if (token.is_name("compressor"))
		{
			check(token.next(ion).to_value() == "zlib", "Bad compression algorithm");
			result._compression = Yt::YpWriter::Compression::Zlib;
			token.next(ion);
		}
		while (token.type() != Yt::IonToken::Type::End)
		{
			token.check_name("files");
			token.next(ion);
			auto& group = result._groups.emplace_back();
			if (token.type() == Yt::IonToken::Type::ObjectBegin)
			{
				token.next(ion).check_name("compression");
				check(Yt::from_chars(token.next(ion).to_value(), group._compressionLevel), "Bad compression level");
				token.next(ion).check_object_end();
				token.next(ion);
			}
			token.check_list_begin();
			token.next(ion);
			while (token.type() != Yt::IonToken::Type::ListEnd)
			{
				group._files.emplace_back(std::string{ token.to_value() });
				token.next(ion);
			}
			token.next(ion);
		}
		return result;
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

int u8main(int argc, char** argv)
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
			const auto indexPath = std::filesystem::u8path(argv[2]);
			auto dataTimestamp = std::filesystem::file_time_type::min();
			const auto index = readIndex(indexPath);
			for (const auto& group : index._groups)
				for (const auto& file : group._files)
					dataTimestamp = std::max(dataTimestamp, std::filesystem::last_write_time(std::filesystem::u8path(file)));
			if (dataTimestamp > std::filesystem::last_write_time(indexPath))
				std::filesystem::last_write_time(indexPath, dataTimestamp);
		}
		else
		{
			if (argc != 3)
				return usage();
			const auto index = readIndex(std::filesystem::u8path(argv[1]));
			const auto packagePath = std::filesystem::u8path(argv[2]);
			Yt::Writer fileWriter{ packagePath };
			if (!fileWriter)
			{
				std::cerr << "ERROR: Unable to open " << packagePath << " for writing\n";
				return 1;
			}
			auto packageWriter = std::make_unique<Yt::YpWriter>(std::move(fileWriter), index._compression);
			for (const auto& group : index._groups)
				for (const auto& file : group._files)
					packageWriter->add(file, group._compressionLevel);
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
