// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/exceptions.h>
#include <yttrium/gui/ion_gui.h>
#include <yttrium/ion/reader.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include <yttrium/test/utils.h>

#include <unordered_map>

#include <doctest.h>

TEST_CASE("gui.load")
{
	using Yt::GuiDataError;
	using Yt::IonError;
	using Yt::ResourceError;

	std::unordered_map<std::string, std::string> _sources;

	const auto add = [&_sources](std::string_view name, std::string_view data) {
		_sources.emplace(name, data);
	};

	const auto source = [&_sources](std::string_view data) {
		Yt::Storage storage{ Yt::Storage::UseFileSystem::Never };
		storage.attach_buffer("main.ion", ::make_buffer(data));
		for (const auto& [extra_name, extra_data] : _sources)
			storage.attach_buffer(extra_name, ::make_buffer(extra_data));
		Yt::ResourceLoader resource_loader{ storage };
		Yt::ScriptContext script_context;
		Yt::IonGui{ "main.ion", resource_loader, script_context };
	};

	const auto fragment = [&source](const std::string& data) {
		source("root screen `` { } " + data);
	};

	SUBCASE("")
	{
		CHECK_THROWS_AS(fragment("test"), GuiDataError);
	}

	SUBCASE("cursor")
	{
		CHECK_NOTHROW(fragment("cursor { default }"));
		CHECK_NOTHROW(fragment("cursor { custom }"));
		CHECK_NOTHROW(fragment("cursor { none }"));

		CHECK_THROWS_AS(fragment("cursor"), IonError);
		CHECK_THROWS_AS(fragment("cursor { }"), IonError);
		CHECK_THROWS_AS(fragment("cursor { test }"), GuiDataError);
	}

	SUBCASE("class")
	{
		CHECK_NOTHROW(fragment("class `test` { }"));
		CHECK_NOTHROW(fragment("class `test` { } class `another_test` { }"));

		CHECK_THROWS_AS(fragment("class `test`"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { } { }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { } class `test` { }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { test }"), GuiDataError);
	}

	SUBCASE("class.align")
	{
		CHECK_NOTHROW(fragment("class `test` { align `bottom` }"));
		CHECK_NOTHROW(fragment("class `test` { align `bottom` `left` }"));
		CHECK_NOTHROW(fragment("class `test` { align `bottom` `right` }"));
		CHECK_NOTHROW(fragment("class `test` { align `center` }"));
		CHECK_NOTHROW(fragment("class `test` { align `left` }"));
		CHECK_NOTHROW(fragment("class `test` { align `right` }"));
		CHECK_NOTHROW(fragment("class `test` { align `top` }"));
		CHECK_NOTHROW(fragment("class `test` { align `top` `left` }"));
		CHECK_NOTHROW(fragment("class `test` { align `top` `right` }"));

		CHECK_THROWS_AS(fragment("class `test` { align }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { align `bottom` `center` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `bottom` `test` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `center` `bottom` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `center` `left` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `center` `right` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `center` `test` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `center` `top` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `left` `bottom` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `left` `center` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `left` `test` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `left` `top` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `right` `bottom` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `right` `center` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `right` `test` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `right` `top` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `test` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `top` `center` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { align `top` `test` }"), GuiDataError);
	}

	SUBCASE("load.class.borders")
	{
		CHECK_NOTHROW(fragment("class `test` { borders `1.0` `2.0` `3.0` `4.0` }"));

		CHECK_THROWS_AS(fragment("class `test` { borders }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `2.0` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `2.0` `3.0` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `2.0` `3.0` `4.0` `5.0` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { borders `test` `2.0` `3.0` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `test` `3.0` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `2.0` `test` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { borders `1.0` `2.0` `3.0` `test` }"), GuiDataError);
	}

	SUBCASE("class.color")
	{
		CHECK_NOTHROW(fragment("class `test` { color #12345678 }"));
	}

	SUBCASE("class.text_color")
	{
		CHECK_NOTHROW(fragment("class `test` { text_color #12345678 }"));
	}

	SUBCASE("class.style")
	{
		SUBCASE("checked")
		{
			CHECK_NOTHROW(fragment("class `test` { checked { } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { } checked { } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { align `center` } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { borders `1` `2` `3` `4` } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { text_color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { checked { text_size `0.25` } }"));

			CHECK_THROWS_AS(fragment("class `test` { checked }"), IonError);
			CHECK_THROWS_AS(fragment("class `test` { checked { checked { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { checked { disabled { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { checked { hovered { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { checked { pressed { } } }"), GuiDataError);
		}

		SUBCASE("disabled")
		{
			CHECK_NOTHROW(fragment("class `test` { disabled { } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { } disabled { } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { align `center` } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { borders `1` `2` `3` `4` } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { text_color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { disabled { text_size `0.25` } }"));

			CHECK_THROWS_AS(fragment("class `test` { disabled }"), IonError);
			CHECK_THROWS_AS(fragment("class `test` { disabled { checked { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { disabled { disabled { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { disabled { hovered { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { disabled { pressed { } } }"), GuiDataError);
		}

		SUBCASE("hovered")
		{
			CHECK_NOTHROW(fragment("class `test` { hovered { } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { } hovered { } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { align `center` } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { borders `1` `2` `3` `4` } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { text_color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { hovered { text_size `0.25` } }"));

			CHECK_THROWS_AS(fragment("class `test` { hovered }"), IonError);
			CHECK_THROWS_AS(fragment("class `test` { hovered { checked { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { hovered { disabled { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { hovered { hovered { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { hovered { pressed { } } }"), GuiDataError);
		}

		SUBCASE("pressed")
		{
			CHECK_NOTHROW(fragment("class `test` { pressed { } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { } pressed { } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { align `center` } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { borders `1` `2` `3` `4` } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { text_color #12345678 } }"));
			CHECK_NOTHROW(fragment("class `test` { pressed { text_size `0.25` } }"));

			CHECK_THROWS_AS(fragment("class `test` { pressed }"), IonError);
			CHECK_THROWS_AS(fragment("class `test` { pressed { checked { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { pressed { disabled { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { pressed { hovered { } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("class `test` { pressed { pressed { } } }"), GuiDataError);
		}
	}

	SUBCASE("class.text_size")
	{
		CHECK_NOTHROW(fragment("class `test` { text_size `0.25` }"));

		CHECK_THROWS_AS(fragment("class `test` { text_size }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { text_size `0.25` `0.50` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { text_size `test` }"), GuiDataError);
	}

	SUBCASE("class.texture_rect")
	{
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `2.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `4.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `` `2.0` `3.0` `4.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `` `3.0` `4.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `2.0` `` `4.0` }"));
		CHECK_NOTHROW(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `` }"));

		CHECK_THROWS_AS(fragment("class `test` { texture_rect }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `4.0` `5.0` }"), IonError);
		CHECK_THROWS_AS(fragment("class `test` { texture_rect `test` `2.0` `3.0` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { texture_rect `1.0` `test` `3.0` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { texture_rect `1.0` `2.0` `test` `4.0` }"), GuiDataError);
		CHECK_THROWS_AS(fragment("class `test` { texture_rect `1.0` `2.0` `3.0` `test` }"), GuiDataError);
	}

	SUBCASE("include")
	{
		CHECK_THROWS_AS(fragment("include"), IonError);
		CHECK_THROWS_AS(fragment("include `test.ion`"), ResourceError);

		add("test.ion", "");
		CHECK_NOTHROW(fragment("include `test.ion`"));
		CHECK_NOTHROW(fragment("include `test.ion` include `test.ion`"));
		CHECK_THROWS_AS(fragment("include `test.ion` `test.ion`"), IonError);
	}

	SUBCASE("on_*")
	{
		CHECK_NOTHROW(fragment("on_key `` { call `test` }"));
		CHECK_THROWS_AS(fragment("on_key `` { call }"), IonError);
		CHECK_THROWS_AS(fragment("on_key `` { call `test` `test` }"), IonError);

		CHECK_NOTHROW(fragment("on_key `` { enter `test` }"));
		CHECK_THROWS_AS(fragment("on_key `` { enter }"), IonError);
		CHECK_THROWS_AS(fragment("on_key `` { enter `test` `test` }"), IonError);

		CHECK_NOTHROW(fragment("on_key `` { quit }"));
		CHECK_THROWS_AS(fragment("on_key `` { quit `test` }"), IonError);

		CHECK_NOTHROW(fragment("on_key `` { return }"));
		CHECK_THROWS_AS(fragment("on_key `` { return `test` }"), IonError);

		CHECK_NOTHROW(fragment("on_key `` { return_to `test` }"));
		CHECK_THROWS_AS(fragment("on_key `` { return_to }"), IonError);
		CHECK_THROWS_AS(fragment("on_key `` { return_to `test` `test` }"), IonError);

		CHECK_THROWS_AS(fragment("on_key `` { test }"), GuiDataError);

		CHECK_NOTHROW(fragment("on_key `` { call `test` enter `test` quit return return_to `test` }"));
	}

	SUBCASE("on_key")
	{
		CHECK_NOTHROW(fragment("on_key `` { }"));
		CHECK_NOTHROW(fragment("on_key `` { } { }"));
		CHECK_NOTHROW(fragment("on_key `` { } { call `test` }"));
		CHECK_NOTHROW(fragment("on_key `` { call `test` }"));
		CHECK_NOTHROW(fragment("on_key `` { call `test` } { }"));
		CHECK_NOTHROW(fragment("on_key `` { call `test` } { call `test` }"));

		CHECK_THROWS_AS(fragment("on_key"), IonError);
		CHECK_THROWS_AS(fragment("on_key { }"), IonError);
		CHECK_THROWS_AS(fragment("on_key ``"), IonError);
		CHECK_THROWS_AS(fragment("on_key `` { } { } { }"), IonError);
	}

	SUBCASE("screen")
	{
		CHECK_NOTHROW(source("root screen `` { }"));
		CHECK_NOTHROW(source("root screen `main` { }"));
		CHECK_NOTHROW(source("root screen `main` { } screen `test` { }"));

		CHECK_THROWS_AS(source(""), GuiDataError);
		CHECK_THROWS_AS(source("root screen { }"), IonError);
		CHECK_THROWS_AS(source("root screen `main` { } screen { }"), IonError);
		CHECK_THROWS_AS(source("root screen `main` { } screen { }"), IonError);
		CHECK_THROWS_AS(source("root screen `main` { } screen `` { }"), GuiDataError);
		CHECK_THROWS_AS(source("root screen `main` { } screen `main` { }"), GuiDataError);
		CHECK_THROWS_AS(source("root screen `main` { } root screen `test` { }"), GuiDataError);

		CHECK_NOTHROW(fragment(""));
		CHECK_NOTHROW(fragment("screen `test` { }"));
		CHECK_NOTHROW(fragment("screen `test` { transparent }"));

		CHECK_THROWS_AS(fragment("screen { }"), IonError);
		CHECK_THROWS_AS(fragment("screen `` { }"), GuiDataError);
		CHECK_THROWS_AS(fragment("screen `test`"), IonError);
		CHECK_THROWS_AS(fragment("screen `test` { } screen `test` { }"), GuiDataError);
		CHECK_THROWS_AS(fragment("screen `test` { test }"), GuiDataError);
	}

	SUBCASE("screen.cursor")
	{
		CHECK_NOTHROW(fragment("screen `test` { cursor { default } }"));
		CHECK_NOTHROW(fragment("screen `test` { cursor { custom } }"));
		CHECK_NOTHROW(fragment("screen `test` { cursor { none } }"));

		CHECK_THROWS_AS(fragment("screen `test` { cursor }"), IonError);
		CHECK_THROWS_AS(fragment("screen `test` { cursor { } }"), IonError);
		CHECK_THROWS_AS(fragment("screen `test` { cursor { test } }"), GuiDataError);
	}

	SUBCASE("screen.on")
	{
		SUBCASE("enter")
		{
			CHECK_NOTHROW(fragment("screen `test` { on_enter { call `test` } }"));

			CHECK_THROWS_AS(fragment("screen `test` { on_enter }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_enter { call `test` } { call `test` } }"), IonError);
		}
		SUBCASE("event")
		{
			CHECK_NOTHROW(fragment("screen `test` { on_event `test` { call `test` } }"));

			CHECK_THROWS_AS(fragment("screen `test` { on_event }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_event { call `test` } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_event `test` }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_event `test` { call `test` } { call `test` } }"), IonError);
		}
		SUBCASE("key")
		{
			CHECK_NOTHROW(fragment("screen `test` { on_key `test` { call `test` } }"));
			CHECK_NOTHROW(fragment("screen `test` { on_key `test` { call `test` } { call `test` } }"));

			CHECK_THROWS_AS(fragment("screen `test` { on_key }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_key { call `test` } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_key `test` }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_key `test` { call `test` } { call `test` } { call `test` } }"), IonError);
		}
		SUBCASE("return")
		{
			CHECK_NOTHROW(fragment("screen `test` { on_return { call `test` } }"));

			CHECK_THROWS_AS(fragment("screen `test` { on_return }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { on_return { call `test` } { call `test` } }"), IonError);
		}
	}

	SUBCASE("screen.layout")
	{
		SUBCASE("center")
		{
			CHECK_NOTHROW(fragment("screen `test` { center `4.0` `3.0` { } }"));

			CHECK_THROWS_AS(fragment("screen `test` { center }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { center { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { center `4.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { center `4.0` `3.0` }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { center `4.0` `3.0` `2.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { center `test` `3.0` { } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { center `4.0` `test` { } }"), GuiDataError);
		}

		SUBCASE("left")
		{
			CHECK_NOTHROW(fragment("screen `test` { left `4.0` { } }"));

			CHECK_THROWS_AS(fragment("screen `test` { left }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { left { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { left `4.0` `3.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { left `test` { } }"), GuiDataError);
		}

		SUBCASE("right")
		{
			CHECK_NOTHROW(fragment("screen `test` { right `4.0` { } }"));

			CHECK_THROWS_AS(fragment("screen `test` { right }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { right { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { right `4.0` `3.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { right `test` { } }"), GuiDataError);
		}

		SUBCASE("stretch")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch `4.0` `3.0` { } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch `4.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch `4.0` `3.0` `2.0` { } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch `test` `3.0` { } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch `4.0` `test` { } }"), GuiDataError);
		}
	}

	SUBCASE("screen.layout_widget")
	{
		SUBCASE("button")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { on_click { call `test` } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { on_enter { call `test` } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { on_update { call `test` } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button `test` { } } }"));
		}

		SUBCASE("button.align")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `bottom` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `bottom` `left` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `bottom` `right` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `center` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `left` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `right` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `top` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `top` `left` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { align `top` `right` } } }"));
		}

		SUBCASE("button.class")
		{
			CHECK_NOTHROW(fragment("class `test` { } screen `test` { stretch { button [`test`] { } } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch { button [] { } } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button [`test`] { } } }"), GuiDataError);
		}

		SUBCASE("button.position")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { position `1.0` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `4.0` } } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `4.0` `5.0` } } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position `test` `2.0` `3.0` `4.0` } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position `1.0` `test` `3.0` `4.0` } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position `1.0` `2.0` `test` `4.0` } } }"), GuiDataError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { position `1.0` `2.0` `3.0` `test` } } }"), GuiDataError);
		}

		SUBCASE("button.state")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { state `checked` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { state `disabled` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { state `hovered` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { state `normal` } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { state `pressed` } } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { state } } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { state `test` } } }"), GuiDataError);
		}

		SUBCASE("button.style")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { checked { } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { disabled { } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { hovered { } } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { pressed { } } } }"));
		}

		SUBCASE("button.text")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { text \"test\" } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { button { text `test` } } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { text } } }"), IonError);
			CHECK_THROWS_AS(fragment("screen `test` { stretch { button { text { } } } }"), IonError);
		}

		SUBCASE("other")
		{
			CHECK_NOTHROW(fragment("screen `test` { stretch { canvas { } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { image { } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { input { } } }"));
			CHECK_NOTHROW(fragment("screen `test` { stretch { label { } } }"));

			CHECK_THROWS_AS(fragment("screen `test` { stretch { test { } } }"), GuiDataError);
		}
	}

	SUBCASE("translation")
	{
		CHECK_THROWS_AS(fragment("translation"), IonError);
		CHECK_THROWS_AS(fragment("translation `translation.ion`"), ResourceError);

		add("translation.ion", "");
		CHECK_NOTHROW(fragment("translation `translation.ion`"));
		CHECK_NOTHROW(fragment("translation `translation.ion` translation `translation.ion`"));
		CHECK_THROWS_AS(fragment("translation `translation.ion` `translation.ion`"), IonError);

		add("garbage.ion", "garbage");
		CHECK_THROWS_AS(fragment("translation `garbage.ion`"), IonError);
	}
}
