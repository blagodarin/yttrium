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

	const auto translation_source = Source::from(argv[1]);
	if (!translation_source)
	{
		std::cerr << "ERROR: Unable to open \"" << argv[1] << "\"\n";
		return 1;
	}

	const auto translation = Translation::load(*translation_source);

	for (int i = 2; i < argc; ++i)
	{
		const auto source = Source::from(argv[i]);
		if (!source)
		{
			std::cerr << "ERROR: Unable to open source \"" << argv[i] << "\"\n";
			return 1;
		}
		try
		{
			IonReader ion{*source};
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
	translation->save(argv[1]);
}
