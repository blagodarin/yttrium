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
		for (const auto& [extra_name, extra_data] : _sources)
			storage.attach_buffer(extra_name, ::make_buffer(extra_data));
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
	BOOST_CHECK_NO_THROW(fragment("class `test` { borders `1.0` `2.0` `3.0` `4.0` }"));

	BOOST_CHECK_THROW(fragment("class `test` { borders }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2.0` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2.0` `3.0` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2.0` `3.0` `4.0` `5.0` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `test` `2.0` `3.0` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `test` `3.0` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2.0` `test` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { borders `1.0` `2.0` `3.0` `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_color, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { color #12345678 }"));
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_text_color, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { text_color #12345678 }"));
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_style, GuiTest)
{
	BOOST_TEST_CONTEXT("checked")
	{
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { } checked { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { align `center` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { borders `1` `2` `3` `4` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { text_color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { checked { text_size `0.25` } }"));

		BOOST_CHECK_THROW(fragment("class `test` { checked }"), IonError);
		BOOST_CHECK_THROW(fragment("class `test` { checked { checked { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { checked { disabled { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { checked { hovered { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { checked { pressed { } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("disabled")
	{
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { } disabled { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { align `center` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { borders `1` `2` `3` `4` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { text_color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { disabled { text_size `0.25` } }"));

		BOOST_CHECK_THROW(fragment("class `test` { disabled }"), IonError);
		BOOST_CHECK_THROW(fragment("class `test` { disabled { checked { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { disabled { disabled { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { disabled { hovered { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { disabled { pressed { } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("hovered")
	{
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { } hovered { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { align `center` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { borders `1` `2` `3` `4` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { text_color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { hovered { text_size `0.25` } }"));

		BOOST_CHECK_THROW(fragment("class `test` { hovered }"), IonError);
		BOOST_CHECK_THROW(fragment("class `test` { hovered { checked { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { hovered { disabled { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { hovered { hovered { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { hovered { pressed { } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("pressed")
	{
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { } pressed { } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { align `center` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { borders `1` `2` `3` `4` } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { text_color #12345678 } }"));
		BOOST_CHECK_NO_THROW(fragment("class `test` { pressed { text_size `0.25` } }"));

		BOOST_CHECK_THROW(fragment("class `test` { pressed }"), IonError);
		BOOST_CHECK_THROW(fragment("class `test` { pressed { checked { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { pressed { disabled { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { pressed { hovered { } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("class `test` { pressed { pressed { } } }"), GuiDataError);
	}
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_text_size, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { text_size `0.25` }"));

	BOOST_CHECK_THROW(fragment("class `test` { text_size }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_size `0.25` `0.50` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { text_size `test` }"), GuiDataError);
}

BOOST_FIXTURE_TEST_CASE(test_gui_load_class_texture_rect, GuiTest)
{
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `2.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `4.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `` `2.0` `3.0` `4.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `` `3.0` `4.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `` `4.0` }"));
	BOOST_CHECK_NO_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `` }"));

	BOOST_CHECK_THROW(fragment("class `test` { texture_rect }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `4.0` `5.0` }"), IonError);
	BOOST_CHECK_THROW(fragment("class `test` { texture_rect `test` `2.0` `3.0` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { texture_rect `1.0` `test` `3.0` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `test` `4.0` }"), GuiDataError);
	BOOST_CHECK_THROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `test` }"), GuiDataError);
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
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { on_click { call `test` } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { on_enter { call `test` } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { on_update { call `test` } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button `test` { } } }"));
	}
	BOOST_TEST_CONTEXT("button.align")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `bottom` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `bottom` `left` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `bottom` `right` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `center` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `left` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `right` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `top` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `top` `left` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { align `top` `right` } } }"));
	}
	BOOST_TEST_CONTEXT("button.class")
	{
		BOOST_CHECK_NO_THROW(fragment("class `test` { } screen `test` { stretch { button [`test`] { } } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button [] { } } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button [`test`] { } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("button.position")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { position `1.0` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `4.0` } } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `4.0` `5.0` } } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position `test` `2.0` `3.0` `4.0` } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position `1.0` `test` `3.0` `4.0` } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `test` `4.0` } } }"), GuiDataError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `test` } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("button.state")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { state `checked` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { state `disabled` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { state `hovered` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { state `normal` } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { state `pressed` } } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { state } } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { state `test` } } }"), GuiDataError);
	}
	BOOST_TEST_CONTEXT("button.style")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { checked { } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { disabled { } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { hovered { } } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { pressed { } } } }"));
	}
	BOOST_TEST_CONTEXT("button.text")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { text \"test\" } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { button { text `test` } } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { text } } }"), IonError);
		BOOST_CHECK_THROW(fragment("screen `test` { stretch { button { text { } } } }"), IonError);
	}
	BOOST_TEST_CONTEXT("other")
	{
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { canvas { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { image { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { input { } } }"));
		BOOST_CHECK_NO_THROW(fragment("screen `test` { stretch { label { } } }"));

		BOOST_CHECK_THROW(fragment("screen `test` { stretch { test { } } }"), GuiDataError);
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
