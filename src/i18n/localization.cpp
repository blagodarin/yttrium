#include <yttrium/i18n/localization.h>

#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>
#include <yttrium/proxy_allocator.h>
#include <yttrium/string.h>
#include "../base/instance_guard.h"
#include "translation.h"

namespace Yttrium
{
	class LocalizationImpl;

	using LocalizationGuard = InstanceGuard<LocalizationImpl>;

	class LocalizationImpl : public Localization
	{
	public:

		LocalizationImpl(const StaticString& file_name, Allocator* allocator)
			: _allocator("i18n"_s, allocator)
			, _translation(Translation::open(file_name, &_allocator))
			, _instance_guard(this, "Duplicate Localization construction")
		{
		}

	public:

		ProxyAllocator _allocator;
		Pointer<Translation> _translation;
		LocalizationGuard _instance_guard;
	};

	Pointer<Localization> Localization::create(const StaticString& file_name, Allocator* allocator)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();
		return make_pointer<LocalizationImpl>(*allocator, file_name, allocator);
	}

	String Localization::localize(const StaticString& source)
	{
		std::lock_guard<std::mutex> lock(LocalizationGuard::instance_mutex);
		return LocalizationGuard::instance
			? static_cast<TranslationImpl*>(LocalizationGuard::instance->_translation.get())->translate(source)
			: String(source);
	}
}
