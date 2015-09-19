#include <yttrium/i18n/translation.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/memory_manager.h>
#include <yttrium/string.h>

#include <cstdio>
#include <map>

using namespace Yttrium;

void update_translation(Translation& translation, const IonObject& source);

bool update_tr(Translation& translation, const IonObject& tr_object)
{
	if (tr_object.size() != 1)
		return false;

	const auto& tr_node = *tr_object.begin();
	if (tr_node.name() != S("tr") || tr_node.size() != 1)
		return false;

	const auto& tr_value = *tr_node.first();
	if (tr_value.type() != IonValue::Type::String)
		return false;

	translation.add(tr_value.string());
	return true;
}

void update_translation(Translation& translation, const IonValue& source)
{
	switch (source.type())
	{
	case IonValue::Type::List:
		for (const auto& value : source.list())
			update_translation(translation, value);
		break;
	case IonValue::Type::Object:
		if (!update_tr(translation, *source.object()))
			update_translation(translation, *source.object());
		break;
	default:
		break;
	}
}

void update_translation(Translation& translation, const IonObject& source)
{
	for (const auto& node : source)
		for (const auto& value : node)
			update_translation(translation, value);
}

int main(int argc, char** argv)
{
	MemoryManager memory_manager;

	if (argc < 3)
	{
		printf("Usage: ytr TRANSLATION SOURCES...\n");
		return 1;
	}

	Translation translation(argv[1]);

	for (int i = 2; i < argc; ++i)
	{
		IonDocument document;
		if (!document.load(argv[i]))
		{
			printf("ERROR: Failed to load source file \"%s\"\n", argv[i]);
			return 1;
		}
		update_translation(translation, document.root());
	}

	if (!translation.save(argv[1]))
	{
		printf("ERROR: Failed to save translation file \"%s\"\n", argv[1]);
		return 1;
	}

	return 0;
}
