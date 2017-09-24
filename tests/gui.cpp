#include <yttrium/exceptions.h>
#include <yttrium/gui/gui.h>
#include <yttrium/ion/reader.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include "test_utils.h"

#include <unordered_map>

#include <boost/test/unit_test.hpp>

using Yttrium::Gui;
using Yttrium::GuiDataError;
using Yttrium::IonError;
using Yttrium::ResourceError;
using Yttrium::ResourceLoader;
using Yttrium::ScriptContext;
using Yttrium::Storage;

class GuiTest
{
public:
	void add(std::string_view name, std::string_view data)
	{
		_sources.emplace(name, data);
	}

	void source(std::string_view data) const
	{
		Storage storage{Storage::UseFileSystem::Never};
		storage.attach_buffer("main.ion", ::make_buffer(data));
		for (const auto& source : _sources)
			storage.attach_buffer(source.first, ::make_buffer(source.second));
		ResourceLoader resource_loader{storage};
		ScriptContext script_context;
		Gui{resource_loader, script_context, "main.ion"};
	}

	void fragment(const std::string& data) const
	{
		source("root screen `` { } " + data);
	}

private:
	std::unordered_map<std::string, std::string> _sources;
};

BOOST_FIXTURE_TEST_CASE(test_gui_load, GuiTest)
{
	BOOST_CHECK_NO_THROW(source(R"(
		class "class"
		{
		}
		root screen "root"
		{
			center "10" "10"
			{
				button ["class"]
				{
					position "4" "4" "2" "2"
					text `Push`
					on_click { enter "test" }
				}
			}
			left "10"
			{
				label
				{
					position "1" "1" "5" "1"
					text `Hello world!`
				}
			}
			right "10"
			{
				canvas "right"
				{
					position "1" "1" "2" "8"
				}
			}
			stretch
			{
				image
				{
				}
			}
		}
	)"));

	BOOST_CHECK_THROW(fragment("test"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_cursor, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("cursor { default }"));
	BOOST_CHECK_NO_THROW(fragment("cursor { custom }"));
	BOOST_CHECK_NO_THROW(fragment("cursor { none }"));

	BOOST_CHECK_THROW(fragment("cursor"), IonError);
	BOOST_CHECK_THROW(fragment("cursor { }"), IonError);
	BOOST_CHECK_THROW(fragment("cursor { test }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { } class `another_test` { }"));

	BOOST_CHECK_THROW(fragment("class `test`"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { } { }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { } class `test` { }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { test }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_align, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `bottom` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `bottom` `left` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `bottom` `right` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `center` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `left` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `right` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `top` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `top` `left` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { align `top` `right` }"));

	BOOST_CHECK_THROW(fragment("class `test` { align }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { align `bottom` `center` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `bottom` `test` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `center` `bottom` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `center` `left` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `center` `right` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `center` `test` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `center` `top` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `left` `bottom` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `left` `center` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `left` `test` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `left` `top` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `right` `bottom` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `right` `center` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `right` `test` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `right` `top` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `test` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `top` `center` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { align `top` `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_borders, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { borders `1` `2` `3` `4` }"));

	BOOST_CHECK_THROW(fragment("class `test` { borders }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2` `3` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2` `3` `4` `5` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2` `3` `4` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2.0` `3` `4` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2` `3.0` `4` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1` `2` `3` `4.0` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_color, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { color `0.25` `0.50` `0.75` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { color `0.25` `0.50` `0.75` `1.00` }"));

	BOOST_CHECK_THROW(fragment("class `test` { color }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` `0.50` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` `0.50` `0.75` `1.00` `1.25` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { color `test` `0.50` `0.75` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` `test` `0.75` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` `0.50` `test` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { color `0.25` `0.50` `0.75` `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_text_color, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { text_color `0.25` `0.50` `0.75` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { text_color `0.25` `0.50` `0.75` `1.00` }"));

	BOOST_CHECK_THROW(fragment("class `test` { text_color }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` `0.50` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` `0.50` `0.75` `1.00` `1.25` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `test` `0.50` `0.75` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` `test` `0.75` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` `0.50` `test` `1.00` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { text_color `0.25` `0.50` `0.75` `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_text_size, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { text_size `0.25` }"));

	BOOST_CHECK_THROW(fragment("class `test` { text_size }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_size `0.25` `0.50` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_size `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_include, GuiTest)
{
	BOOST_CHECK_THROW(fragment("include"), IonError);
	BOOST_CHECK_THROW(fragment("include `test.ion`"), ResourceError);

	add("test.ion", "");
	BOOST_CHECK_NO_THROW(fragment("include `test.ion`"));
	BOOST_CHECK_NO_THROW(fragment("include `test.ion` include `test.ion`"));
	BOOST_CHECK_THROW(fragment("include `test.ion` `test.ion`"), IonError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_on, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("on_key `` { call `test` }"));
	BOOST_CHECK_THROW(fragment("on_key `` { call }"), IonError);
	BOOST_CHECK_THROW(fragment("on_key `` { call `test` `test` }"), IonError);

	BOOST_CHECK_NO_THROW(fragment("on_key `` { enter `test` }"));
	BOOST_CHECK_THROW(fragment("on_key `` { enter }"), IonError);
	BOOST_CHECK_THROW(fragment("on_key `` { enter `test` `test` }"), IonError);

	BOOST_CHECK_NO_THROW(fragment("on_key `` { quit }"));
	BOOST_CHECK_THROW(fragment("on_key `` { quit `test` }"), IonError);

	BOOST_CHECK_NO_THROW(fragment("on_key `` { return }"));
	BOOST_CHECK_THROW(fragment("on_key `` { return `test` }"), IonError);

	BOOST_CHECK_NO_THROW(fragment("on_key `` { return_to `test` }"));
	BOOST_CHECK_THROW(fragment("on_key `` { return_to }"), IonError);
	BOOST_CHECK_THROW(fragment("on_key `` { return_to `test` `test` }"), IonError);

	BOOST_CHECK_THROW(fragment("on_key `` { test }"), GuiDataError);

	BOOST_CHECK_NO_THROW(fragment("on_key `` { call `test` enter `test` quit return return_to `test` }"));
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_on_key, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("on_key `` { }"));
	BOOST_CHECK_NO_THROW(fragment("on_key `` { } { }"));
	BOOST_CHECK_NO_THROW(fragment("on_key `` { } { call `test` }"));
	BOOST_CHECK_NO_THROW(fragment("on_key `` { call `test` }"));
	BOOST_CHECK_NO_THROW(fragment("on_key `` { call `test` } { }"));
	BOOST_CHECK_NO_THROW(fragment("on_key `` { call `test` } { call `test` }"));

	BOOST_CHECK_THROW(fragment("on_key"), IonError);
	BOOST_CHECK_THROW(fragment("on_key { }"), IonError);
	BOOST_CHECK_THROW(fragment("on_key ``"), IonError);
	BOOST_CHECK_THROW(fragment("on_key `` { } { } { }"), IonError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_screen, GuiTest)
{
	BOOST_CHECK_NO_THROW(source("root screen `` { }"));
	BOOST_CHECK_NO_THROW(source("root screen `main` { }"));
	BOOST_CHECK_NO_THROW(source("root screen `main` { } screen `test` { }"));

	BOOST_CHECK_THROW(source(""), GuiDataError);
	BOOST_CHECK_THROW(source("root screen { }"), IonError);
	BOOST_CHECK_THROW(source("root screen `main` { } screen { }"), IonError);
	BOOST_CHECK_THROW(source("root screen `main` { } screen { }"), IonError);
	BOOST_CHECK_THROW(source("root screen `main` { } screen `` { }"), GuiDataError);
	BOOST_CHECK_THROW(source("root screen `main` { } screen `main` { }"), GuiDataError);
	BOOST_CHECK_THROW(source("root screen `main` { } root screen `test` { }"), GuiDataError);

	BOOST_CHECK_NO_THROW(fragment(""));
	BOOST_CHECK_NO_THROW(fragment("screen `test` { }"));
	BOOST_CHECK_NO_THROW(fragment("transparent screen `test` { }"));

	BOOST_CHECK_THROW(fragment("screen { }"), IonError);
	BOOST_CHECK_THROW(fragment("screen `` { }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("screen `test`"), IonError);
	BOOST_CHECK_THROW(fragment("screen `test` { } screen `test` { }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("screen `test` { test }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_screen_cursor, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("screen `test` { cursor { default } }"));
	BOOST_CHECK_NO_THROW(fragment("screen `test` { cursor { custom } }"));
	BOOST_CHECK_NO_THROW(fragment("screen `test` { cursor { none } }"));

	BOOST_CHECK_THROW(fragment("screen `test` { cursor }"), IonError);
	BOOST_CHECK_THROW(fragment("screen `test` { cursor { } }"), IonError);
	BOOST_CHECK_THROW(fragment("screen `test` { cursor { test } }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_screen_handlers, GuiTest)
{
	BOOST_TEST_CONTEXT("on_enter")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { on_enter { call `test` } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { on_enter }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_enter { call `test` } { call `test` } }"), IonError);
	}
	BOOST_TEST_CONTEXT("on_event")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { on_event `test` { call `test` } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { on_event }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_event { call `test` } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_event `test` }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_event `test` { call `test` } { call `test` } }"), IonError);
	}
	BOOST_TEST_CONTEXT("on_key")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { on_key `test` { call `test` } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { on_key `test` { call `test` } { call `test` } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { on_key }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_key { call `test` } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_key `test` }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_key `test` { call `test` } { call `test` } { call `test` } }"), IonError);
	}
	BOOST_TEST_CONTEXT("on_return")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { on_return { call `test` } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { on_return }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { on_return { call `test` } { call `test` } }"), IonError);
	}
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_screen_layout, GuiTest)
{
	BOOST_TEST_CONTEXT("center")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { center `4.0` `3.0` { } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { center }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { center { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { center `4.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { center `4.0` `3.0` }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { center `4.0` `3.0` `2.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { center `test` `3.0` { } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { center `4.0` `test` { } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("left")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { left `4.0` { } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { left }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { left { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { left `4.0` `3.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { left `test` { } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("right")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { right `4.0` { } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { right }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { right { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { right `4.0` `3.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { right `test` { } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("stretch")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch `4.0` `3.0` { } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch `4.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch `4.0` `3.0` `2.0` { } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch `test` `3.0` { } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch `4.0` `test` { } }"), GuiDataError);
	}
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_screen_layout_widget, GuiTest)
{
	BOOST_TEST_CONTEXT("button")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button `test` { } } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { button [`test`] { } } }"));
	}
	BOOST_TEST_CONTEXT("canvas")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { canvas { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { canvas `test` { } } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { canvas [`test`] { } } }"));
	}
	BOOST_TEST_CONTEXT("image")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { image { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { image `test` { } } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { image [`test`] { } } }"));
	}
	BOOST_TEST_CONTEXT("input")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { input { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { input `test` { } } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { input [`test`] { } } }"));
	}
	BOOST_TEST_CONTEXT("label")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { label { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { label `test` { } } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { label [`test`] { } } }"));
	}
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_translation, GuiTest)
{
	BOOST_CHECK_THROW(fragment("translation"), IonError);
	BOOST_CHECK_THROW(fragment("translation `translation.ion`"), ResourceError);

	add("translation.ion", "");
	BOOST_CHECK_NO_THROW(fragment("translation `translation.ion`"));
	BOOST_CHECK_THROW(fragment("translation `translation.ion` translation `translation.ion`"), GuiDataError);
	BOOST_CHECK_THROW(fragment("translation `translation.ion` `translation.ion`"), IonError);

	add("garbage.ion", "garbage");
	BOOST_CHECK_THROW(fragment("translation `garbage.ion`"), IonError);
}
