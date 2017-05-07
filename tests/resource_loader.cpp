#include <yttrium/memory/buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/storage.h>

#include <boost/test/unit_test.hpp>

namespace
{
	Yttrium::Buffer make_buffer(const std::string& string)
	{
		return { string.size(), string.data() };
	}
}

BOOST_AUTO_TEST_CASE(test_resource_loader_release_unused)
{
	using Yttrium::IonDocument;
	using Yttrium::ResourceLoader;
	using Yttrium::Storage;
	using Yttrium::Translation;

	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("document", ::make_buffer("a { b c }"));
	storage.attach_buffer("translation", ::make_buffer("tr \"foo\" \"bar\""));

	ResourceLoader resource_loader{ storage };
	const std::weak_ptr<const IonDocument> document = resource_loader.load_ion("document");
	BOOST_CHECK(!document.expired());
	const std::weak_ptr<const Translation> translation = resource_loader.load_translation("translation");
	BOOST_CHECK(!translation.expired());
	resource_loader.release_unused();
	BOOST_CHECK(document.expired());
	BOOST_CHECK(translation.expired());
}
