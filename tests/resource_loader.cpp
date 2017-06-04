#include <yttrium/memory/buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/storage.h>
#include "test_utils.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(resource_loader_release_unused)
{
	using Yttrium::ResourceLoader;
	using Yttrium::Storage;
	using Yttrium::Translation;

	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("translation", ::make_buffer("tr \"foo\" \"bar\""));

	ResourceLoader resource_loader{ storage };
	const std::weak_ptr<const Translation> translation = resource_loader.load_translation("translation");
	BOOST_CHECK(!translation.expired());
	resource_loader.release_unused();
	BOOST_CHECK(translation.expired());
}
