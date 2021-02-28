// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/logger.h>
#include <yttrium/main.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include <yttrium/translation.h>

#include <iostream>

int ymain(int argc, char** argv)
{
	Yt::Logger logger;

	if (argc < 3)
	{
		std::cerr << "Usage: ytr TRANSLATION SOURCES...\n";
		return 1;
	}

	const auto translation_path = std::filesystem::u8path(argv[1]);

	auto translation_source = Yt::Source::from(translation_path);
	if (!translation_source)
	{
		std::cerr << "ERROR: Unable to open " << translation_path << " for reading\n";
		return 1;
	}

	const auto translation = Yt::Translation::load(*translation_source);
	if (!translation)
	{
		std::cerr << "ERROR: Failed to read " << translation_path << '\n';
		return 1;
	}

	translation_source.reset(); // To open the file for writing.

	for (int i = 2; i < argc; ++i)
	{
		const auto source_path = std::filesystem::u8path(argv[i]);

		const auto source = Yt::Source::from(source_path);
		if (!source)
		{
			std::cerr << "ERROR: Unable to open " << source_path << " for reading\n";
			return 1;
		}

		try
		{
			Yt::IonReader ion{ *source };
			for (auto token = ion.read(); token.type() != Yt::IonToken::Type::End; token = ion.read())
				if (token.translatable())
					translation->add(token.text());
		}
		catch (const Yt::IonError& e)
		{
			std::cerr << "ERROR: Failed to read " << source_path << ": " << e.what() << '\n';
			return 1;
		}
	}

	translation->remove_obsolete();

	if (translation->changed())
	{
		Yt::Writer output{ translation_path };
		if (!output)
		{
			std::cerr << "ERROR: Unable to open " << translation_path << " for writing\n";
			return 1;
		}
		translation->save(std::move(output));
	}
	return 0;
}
