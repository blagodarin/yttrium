//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "../../../libs/engine/src/gui/key_lookup.h"

#include <catch2/catch.hpp>

TEST_CASE("key_event")
{
	using Yttrium::Key;
	using Yttrium::KeyEvent;

	CHECK(KeyEvent(Key::A, false, false)._key == Key::A);
	CHECK(KeyEvent(Key::B, false, false)._key == Key::B);
	CHECK(!KeyEvent(Key::A, false, false)._pressed);
	CHECK(KeyEvent(Key::A, true, false)._pressed);
	CHECK(!KeyEvent(Key::A, false, false)._autorepeat);
	CHECK(KeyEvent(Key::A, false, true)._autorepeat);
}

TEST_CASE("key_lookup")
{
	using Yttrium::Key;
	using Yttrium::lookup_key;

	CHECK(lookup_key({}) == Key::Null);
	CHECK(lookup_key("no_such_key") == Key::Null);

	// Special keys (0x01 - 0x0F).

	CHECK(lookup_key("lshift") == Key::LShift);
	CHECK(lookup_key("rshift") == Key::RShift);
	CHECK(lookup_key("lctrl") == Key::LControl);
	CHECK(lookup_key("rctrl") == Key::RControl);
	CHECK(lookup_key("lalt") == Key::LAlt);
	CHECK(lookup_key("ralt") == Key::RAlt);
	CHECK(lookup_key("up") == Key::Up);
	CHECK(lookup_key("down") == Key::Down);
	CHECK(lookup_key("left") == Key::Left);
	CHECK(lookup_key("right") == Key::Right);
	CHECK(lookup_key("print") == Key::Print);
	CHECK(lookup_key("pause") == Key::Pause);
	CHECK(lookup_key("esc") == Key::Escape);

	// Text-producing keys (0x10 - 0x3F).

	CHECK(lookup_key("0") == Key::_0);
	CHECK(lookup_key("1") == Key::_1);
	CHECK(lookup_key("2") == Key::_2);
	CHECK(lookup_key("3") == Key::_3);
	CHECK(lookup_key("4") == Key::_4);
	CHECK(lookup_key("5") == Key::_5);
	CHECK(lookup_key("6") == Key::_6);
	CHECK(lookup_key("7") == Key::_7);
	CHECK(lookup_key("8") == Key::_8);
	CHECK(lookup_key("9") == Key::_9);
	CHECK(lookup_key("a") == Key::A);
	CHECK(lookup_key("b") == Key::B);
	CHECK(lookup_key("c") == Key::C);
	CHECK(lookup_key("d") == Key::D);
	CHECK(lookup_key("e") == Key::E);
	CHECK(lookup_key("f") == Key::F);
	CHECK(lookup_key("g") == Key::G);
	CHECK(lookup_key("h") == Key::H);
	CHECK(lookup_key("i") == Key::I);
	CHECK(lookup_key("j") == Key::J);
	CHECK(lookup_key("k") == Key::K);
	CHECK(lookup_key("l") == Key::L);
	CHECK(lookup_key("m") == Key::M);
	CHECK(lookup_key("n") == Key::N);
	CHECK(lookup_key("o") == Key::O);
	CHECK(lookup_key("p") == Key::P);
	CHECK(lookup_key("q") == Key::Q);
	CHECK(lookup_key("r") == Key::R);
	CHECK(lookup_key("s") == Key::S);
	CHECK(lookup_key("t") == Key::T);
	CHECK(lookup_key("u") == Key::U);
	CHECK(lookup_key("v") == Key::V);
	CHECK(lookup_key("w") == Key::W);
	CHECK(lookup_key("x") == Key::X);
	CHECK(lookup_key("y") == Key::Y);
	CHECK(lookup_key("z") == Key::Z);
	CHECK(lookup_key(",") == Key::Comma);
	CHECK(lookup_key(".") == Key::Period);
	CHECK(lookup_key("semicolon") == Key::Semicolon);
	CHECK(lookup_key("'") == Key::Apostrophe);
	CHECK(lookup_key("/") == Key::Slash);
	CHECK(lookup_key("\\") == Key::Backslash);
	CHECK(lookup_key("[") == Key::LBracket);
	CHECK(lookup_key("]") == Key::RBracket);
	CHECK(lookup_key("-") == Key::Minus);
	CHECK(lookup_key("=") == Key::Equals);
	CHECK(lookup_key("`") == Key::Grave);
	CHECK(lookup_key("space") == Key::Space);

	// Text-editing keys (0x40 - 0x4F).

	CHECK(lookup_key("tab") == Key::Tab);
	CHECK(lookup_key("enter") == Key::Enter);
	CHECK(lookup_key("back") == Key::Backspace);
	CHECK(lookup_key("ins") == Key::Insert);
	CHECK(lookup_key("del") == Key::Delete);
	CHECK(lookup_key("home") == Key::Home);
	CHECK(lookup_key("end") == Key::End);
	CHECK(lookup_key("pgup") == Key::PageUp);
	CHECK(lookup_key("pgdn") == Key::PageDown);

	// Functional keys (0x50 - 0x6F).

	CHECK(lookup_key("f1") == Key::F1);
	CHECK(lookup_key("f2") == Key::F2);
	CHECK(lookup_key("f3") == Key::F3);
	CHECK(lookup_key("f4") == Key::F4);
	CHECK(lookup_key("f5") == Key::F5);
	CHECK(lookup_key("f6") == Key::F6);
	CHECK(lookup_key("f7") == Key::F7);
	CHECK(lookup_key("f8") == Key::F8);
	CHECK(lookup_key("f9") == Key::F9);
	CHECK(lookup_key("f10") == Key::F10);
	CHECK(lookup_key("f11") == Key::F11);
	CHECK(lookup_key("f12") == Key::F12);

	// Numpad keys (0x70 - 0x7F).

	CHECK(lookup_key("num0") == Key::Num0);
	CHECK(lookup_key("num1") == Key::Num1);
	CHECK(lookup_key("num2") == Key::Num2);
	CHECK(lookup_key("num3") == Key::Num3);
	CHECK(lookup_key("num4") == Key::Num4);
	CHECK(lookup_key("num5") == Key::Num5);
	CHECK(lookup_key("num6") == Key::Num6);
	CHECK(lookup_key("num7") == Key::Num7);
	CHECK(lookup_key("num8") == Key::Num8);
	CHECK(lookup_key("num9") == Key::Num9);
	CHECK(lookup_key("num.") == Key::Decimal);
	CHECK(lookup_key("num+") == Key::Add);
	CHECK(lookup_key("num-") == Key::Subtract);
	CHECK(lookup_key("num*") == Key::Multiply);
	CHECK(lookup_key("num/") == Key::Divide);
	CHECK(lookup_key("numenter") == Key::NumEnter);

	// Mouse keys (0x80 - 0x8F).

	CHECK(lookup_key("mouse1") == Key::Mouse1);
	CHECK(lookup_key("mouse2") == Key::Mouse2);
	CHECK(lookup_key("mouse3") == Key::Mouse3);
	CHECK(lookup_key("mouse4") == Key::Mouse4);
	CHECK(lookup_key("mouse5") == Key::Mouse5);
	CHECK(lookup_key("mwup") == Key::WheelUp);
	CHECK(lookup_key("mwdown") == Key::WheelDown);
	CHECK(lookup_key("mwleft") == Key::WheelLeft);
	CHECK(lookup_key("mwright") == Key::WheelRight);
}
