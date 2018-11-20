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

#include <yttrium/ion/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/translation.h>

#include <iostream>

using namespace Yttrium;

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cerr << "Usage: ytr TRANSLATION SOURCES...\n";
		return 1;
	}

	const auto translation = [](std::string_view path) {
		const auto source = Source::from(path);
		return source ? Translation::load(*source) : nullptr;
	}(argv[1]);

	if (!translation)
	{
		std::cerr << "ERROR: Unable to read \"" << argv[1] << "\"\n";
		return 1;
	}

	for (int i = 2; i < argc; ++i)
	{
		const auto source = Source::from(argv[i]);
		if (!source)
		{
			std::cerr << "ERROR: Unable to read \"" << argv[i] << "\"\n";
			return 1;
		}
		try
		{
			IonReader ion{ *source };
			for (auto token = ion.read(); token.type() != IonReader::Token::Type::End; token = ion.read())
				if (token.translatable())
					translation->add(token.text());
		}
		catch (const IonError& e)
		{
			std::cerr << "ERROR(" << argv[i] << "): " << e.what() << "\n";
			return 1;
		}
	}

	translation->remove_obsolete();

	if (!translation->save(argv[1]))
	{
		std::cerr << "ERROR: Unable to write \"" << argv[1] << "\"\n";
		return 1;
	}
}
