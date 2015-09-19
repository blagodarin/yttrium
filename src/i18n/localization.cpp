#include <yttrium/i18n/localization.h>

#include <yttrium/i18n/translation.h>
#include <yttrium/memory_manager.h>
#include <yttrium/pointer.h>
#include <yttrium/proxy_allocator.h>
#include "../base/instance_guard.h"

namespace Yttrium
{
	class LocalizationImpl;

	using LocalizationGuard = InstanceGuard<LocalizationImpl>;

	class LocalizationImpl : public Localization
	{
	public:

		LocalizationImpl(const StaticString& file_name, Allocator* allocator)
			: _allocator("i18n", allocator)
			, _translation(file_name, &_allocator)
			, _instance_guard(this, "Duplicate Localization construction")
		{
		}

	private:

		ProxyAllocator _allocator;
		Translation _translation;
		LocalizationGuard _instance_guard;
	};

	Pointer<Localization> Localization::create(const StaticString& file_name, Allocator* allocator)
	{
		if (!allocator)
			allocator = MemoryManager::default_allocator();
		return make_pointer<LocalizationImpl>(*allocator, file_name, allocator);
	}
}
