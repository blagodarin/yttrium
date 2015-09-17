#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/memory_manager.h>
#include <yttrium/string.h>

#include <cstdio>
#include <map>

using namespace Yttrium;

std::map<String, String> load_translations(const char* file_name)
{
	IonDocument document;
	if (!document.load(file_name))
		return {};
	std::map<String, String> translations;
	try
	{
		const auto& root = document.root();
		for (const auto& node : root)
		{
			if (node.name() != S("tr") || node.size() != 2)
				throw std::runtime_error("");
			const StaticString* source = nullptr;
			const StaticString* translation = nullptr;
			if (!node.first()->get(&source) || !node.last()->get(&translation))
				throw std::runtime_error("");
			translations.emplace(*source, *translation);
		}
	}
	catch (const std::runtime_error&)
	{
		printf("ERROR: Corrupted translation file \"%s\"\n", file_name);
		return {};
	}
	return translations;
}

void update_translations(std::map<String, String>& translations, const IonObject& source);

bool update_tr(std::map<String, String>& translations, const IonObject& tr_object)
{
	if (tr_object.size() != 1)
		return false;

	const auto& tr_node = *tr_object.begin();
	if (tr_node.name() != S("tr") || tr_node.size() != 1)
		return false;

	const auto& tr_value = *tr_node.first();
	if (tr_value.type() != IonValue::Type::String)
		return false;

	translations[tr_value.string()];
	return true;
}

void update_translations(std::map<String, String>& translations, const IonValue& source)
{
	switch (source.type())
	{
	case IonValue::Type::List:
		for (const auto& value : source.list())
			update_translations(translations, value);
		break;
	case IonValue::Type::Object:
		if (!update_tr(translations, *source.object()))
			update_translations(translations, *source.object());
		break;
	default:
		break;
	}
}

void update_translations(std::map<String, String>& translations, const IonObject& source)
{
	for (const auto& node : source)
		for (const auto& value : node)
			update_translations(translations, value);
}

int main(int argc, char** argv)
{
	MemoryManager memory_manager;

	if (argc < 3)
	{
		printf("Usage: ytr TRANSLATION SOURCES...\n");
		return 1;
	}

	auto&& translations = load_translations(argv[1]);

	for (int i = 2; i < argc; ++i)
	{
		IonDocument document;
		if (!document.load(argv[i]))
		{
			printf("ERROR: Failed to load source file \"%s\"\n", argv[i]);
			return 1;
		}
		update_translations(translations, document.root());
	}

	IonDocument document;
	for (const auto& translation : translations)
	{
		auto& node = *document.root().append(S("tr"));
		node.append(translation.first);
		node.append(translation.second);
	}
	if (!document.save(argv[1]))
	{
		printf("ERROR: Failed to save translation file \"%s\"\n", argv[1]);
		return 1;
	}

	return 0;
}
