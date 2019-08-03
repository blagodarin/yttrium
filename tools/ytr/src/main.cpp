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

#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/main.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include <yttrium/translation.h>

#include <iostream>

using namespace Yttrium;

int ymain(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cerr << "Usage: ytr TRANSLATION SOURCES...\n";
		return 1;
	}

	const auto translation_path = std::filesystem::u8path(argv[1]);

	auto translation_source = Source::from(translation_path);
	if (!translation_source)
	{
		std::cerr << "ERROR: Unable to open " << translation_path << " for reading\n";
		return 1;
	}

	const auto translation = Translation::load(*translation_source);
	if (!translation)
	{
		std::cerr << "ERROR: Failed to read " << translation_path << '\n';
		return 1;
	}

	translation_source.reset(); // To open the file for writing.

	for (int i = 2; i < argc; ++i)
	{
		const auto source_path = std::filesystem::u8path(argv[i]);

		const auto source = Source::from(source_path);
		if (!source)
		{
			std::cerr << "ERROR: Unable to open " << source_path << " for reading\n";
			return 1;
		}

		try
		{
			IonReader ion{ *source };
			for (auto token = ion.read(); token.type() != IonToken::Type::End; token = ion.read())
				if (token.translatable())
					translation->add(token.text());
		}
		catch (const IonError& e)
		{
			std::cerr << "ERROR: Failed to read " << source_path << ": " << e.what() << '\n';
			return 1;
		}
	}

	translation->remove_obsolete();

	Writer output{ translation_path };
	if (!output)
	{
		std::cerr << "ERROR: Unable to open " << translation_path << " for writing\n";
		return 1;
	}

	translation->save(std::move(output));
	return 0;
}
