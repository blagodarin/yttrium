#include <yttrium/resource_loader.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/storage.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_resource_loader_release_unused)
{
	using Yttrium::IonDocument;
	using Yttrium::ResourceLoader;
	using Yttrium::Storage;

	Storage storage{ Storage::UseFileSystem::Before };
	ResourceLoader resource_loader{ storage };
	const std::weak_ptr<const IonDocument> resource = resource_loader.load_ion("tests/ion/original.ion"); // We can't just load raw data, so ION it is.
	BOOST_REQUIRE_EQUAL(resource.use_count(), 1);
	resource_loader.release_unused();
	BOOST_CHECK_EQUAL(resource.use_count(), 0);
}
