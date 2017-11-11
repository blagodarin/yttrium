#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include "test_utils.h"

#include <boost/test/unit_test.hpp>

using Yttrium::DataError;
using Yttrium::ResourceError;
using Yttrium::ResourceLoader;
using Yttrium::Storage;
using Yttrium::Translation;

const std::string_view _translation_data = "tr \"foo\" \"bar\"";

BOOST_AUTO_TEST_CASE(resource_loader_load)
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("one", ::make_buffer(_translation_data));
	storage.attach_buffer("two", ::make_buffer(_translation_data));
	storage.attach_buffer("bad", ::make_buffer("\"bad\""));

	ResourceLoader resource_loader{ storage };
	const auto one = resource_loader.load_translation("one");
	BOOST_CHECK(one);
	BOOST_CHECK_EQUAL(one, resource_loader.load_translation("one"));
	BOOST_CHECK_NE(one, resource_loader.load_translation("two"));
	BOOST_CHECK_EQUAL(resource_loader.load_translation("two"), resource_loader.load_translation("two"));
	BOOST_CHECK_THROW(resource_loader.load_translation("three"), ResourceError);
	BOOST_CHECK_THROW(resource_loader.load_translation("bad"), DataError);

	BOOST_CHECK(!resource_loader.load_material("material"));
	BOOST_CHECK(!resource_loader.load_mesh("mesh"));
	BOOST_CHECK(!resource_loader.load_sound("sound"));
	BOOST_CHECK(!resource_loader.load_texture_2d("texture_2d"));
}

BOOST_AUTO_TEST_CASE(resource_loader_open)
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("one", ::make_buffer(_translation_data));

	ResourceLoader resource_loader{ storage };
	BOOST_CHECK(resource_loader.open("one"));
	BOOST_CHECK_THROW(resource_loader.open("two"), ResourceError);
}

BOOST_AUTO_TEST_CASE(resource_loader_release_unused)
{
	Storage storage{ Storage::UseFileSystem::Never };
	storage.attach_buffer("one", ::make_buffer(_translation_data));
	storage.attach_buffer("two", ::make_buffer(_translation_data));

	ResourceLoader resource_loader{ storage };
	const auto one = resource_loader.load_translation("one");
	const std::weak_ptr<const Translation> two = resource_loader.load_translation("two");
	BOOST_CHECK(!two.expired());
	resource_loader.release_unused();
	BOOST_CHECK(two.expired());
}
