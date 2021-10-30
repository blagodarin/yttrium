// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/event.h>

#include <doctest/doctest.h>

TEST_CASE("key_event")
{
	using Yt::Key;
	using Yt::KeyEvent;

	CHECK(KeyEvent(Key::A, false, false)._key == Key::A);
	CHECK(KeyEvent(Key::B, false, false)._key == Key::B);
	CHECK(!KeyEvent(Key::A, false, false)._pressed);
	CHECK(KeyEvent(Key::A, true, false)._pressed);
	CHECK(!KeyEvent(Key::A, false, false)._autorepeat);
	CHECK(KeyEvent(Key::A, false, true)._autorepeat);
}
