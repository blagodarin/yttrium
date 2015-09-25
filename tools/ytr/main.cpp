#include <yttrium/i18n/translation.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>
#include <yttrium/string.h>

#include <iostream>

using namespace Yttrium;

void update_translation(Translation& translation, const IonObject& source);

bool update_tr(Translation& translation, const IonObject& tr_object)
{
	if (tr_object.size() != 1)
		return false;

	const auto& tr_node = *tr_object.begin();
	if (tr_node.name() != "tr" || tr_node.size() != 1)
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
		std::cerr << "Usage: ytr TRANSLATION SOURCES..." << std::endl;
		return 1;
	}

	const auto& translation = Translation::open(argv[1]);

	for (int i = 2; i < argc; ++i)
	{
		IonDocument document;
		if (!document.load(argv[i]))
		{
			std::cerr << "ERROR: Failed to load source file \"" << argv[i] << "\"" << std::endl;
			return 1;
		}
		update_translation(*translation, document.root());
	}

	translation->remove_obsolete();

	if (!translation->save(argv[1]))
	{
		std::cerr << "ERROR: Failed to save translation file \"" << argv[1] << "\"" << std::endl;
		return 1;
	}

	return 0;
}
