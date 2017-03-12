#include <yttrium/ion/reader.h>
#include <yttrium/storage/reader.h>
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

	const auto translation = Translation::open(Reader{argv[1]});
	if (!translation)
	{
		std::cerr << "ERROR: Unable to open translation \"" << argv[1] << "\"\n";
		return 1;
	}

	for (int i = 2; i < argc; ++i)
	{
		const Reader reader{argv[i]};
		if (!reader)
		{
			std::cerr << "ERROR: Unable to open source \"" << argv[i] << "\"\n";
			return 1;
		}
		try
		{
			IonReader ion{reader};
			enum { None, Open, Tr, Source, Close } stage = None;
			StaticString source;
			for (auto token = ion.read(); token.type() != IonReader::Token::Type::End; token = ion.read())
			{
				if (token.type() == IonReader::Token::Type::ObjectBegin)
					stage = Open;
				else if (stage == Open && token.type() == IonReader::Token::Type::Name && token.text() == "tr")
					stage = Tr;
				else if (stage == Tr && token.type() == IonReader::Token::Type::Value)
				{
					stage = Source;
					source = token.text();
				}
				else if (stage == Source && token.type() == IonReader::Token::Type::ObjectEnd)
					translation->add(source);
				else
					stage = None;
			}
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
