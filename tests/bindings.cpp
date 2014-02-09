#include "src/terminal/bindings/lookup.h"

#include "common.h"

#include "src/terminal/bindings/lookup.cpp"

using namespace Yttrium;

BOOST_AUTO_TEST_CASE(bindings_test)
{
	BOOST_CHECK(lookup_key(StaticString()) == Key::Null);
	BOOST_CHECK(lookup_key("invalid_key") == Key::Null);

	// Special keys (0x01 - 0x0F).

	BOOST_CHECK(lookup_key("lshift") == Key::LShift);
	BOOST_CHECK(lookup_key("rshift") == Key::RShift);
	BOOST_CHECK(lookup_key("lctrl") == Key::LControl);
	BOOST_CHECK(lookup_key("rctrl") == Key::RControl);
	BOOST_CHECK(lookup_key("lalt") == Key::LAlt);
	BOOST_CHECK(lookup_key("ralt") == Key::RAlt);
	BOOST_CHECK(lookup_key("up") == Key::Up);
	BOOST_CHECK(lookup_key("down") == Key::Down);
	BOOST_CHECK(lookup_key("left") == Key::Left);
	BOOST_CHECK(lookup_key("right") == Key::Right);
	BOOST_CHECK(lookup_key("print") == Key::Print);
	BOOST_CHECK(lookup_key("pause") == Key::Pause);
	BOOST_CHECK(lookup_key("esc") == Key::Escape);

	// Text-producing keys (0x10 - 0x3F).

	BOOST_CHECK(lookup_key("0") == Key::_0);
	BOOST_CHECK(lookup_key("1") == Key::_1);
	BOOST_CHECK(lookup_key("2") == Key::_2);
	BOOST_CHECK(lookup_key("3") == Key::_3);
	BOOST_CHECK(lookup_key("4") == Key::_4);
	BOOST_CHECK(lookup_key("5") == Key::_5);
	BOOST_CHECK(lookup_key("6") == Key::_6);
	BOOST_CHECK(lookup_key("7") == Key::_7);
	BOOST_CHECK(lookup_key("8") == Key::_8);
	BOOST_CHECK(lookup_key("9") == Key::_9);
	BOOST_CHECK(lookup_key("a") == Key::A);
	BOOST_CHECK(lookup_key("b") == Key::B);
	BOOST_CHECK(lookup_key("c") == Key::C);
	BOOST_CHECK(lookup_key("d") == Key::D);
	BOOST_CHECK(lookup_key("e") == Key::E);
	BOOST_CHECK(lookup_key("f") == Key::F);
	BOOST_CHECK(lookup_key("g") == Key::G);
	BOOST_CHECK(lookup_key("h") == Key::H);
	BOOST_CHECK(lookup_key("i") == Key::I);
	BOOST_CHECK(lookup_key("j") == Key::J);
	BOOST_CHECK(lookup_key("k") == Key::K);
	BOOST_CHECK(lookup_key("l") == Key::L);
	BOOST_CHECK(lookup_key("m") == Key::M);
	BOOST_CHECK(lookup_key("n") == Key::N);
	BOOST_CHECK(lookup_key("o") == Key::O);
	BOOST_CHECK(lookup_key("p") == Key::P);
	BOOST_CHECK(lookup_key("q") == Key::Q);
	BOOST_CHECK(lookup_key("r") == Key::R);
	BOOST_CHECK(lookup_key("s") == Key::S);
	BOOST_CHECK(lookup_key("t") == Key::T);
	BOOST_CHECK(lookup_key("u") == Key::U);
	BOOST_CHECK(lookup_key("v") == Key::V);
	BOOST_CHECK(lookup_key("w") == Key::W);
	BOOST_CHECK(lookup_key("x") == Key::X);
	BOOST_CHECK(lookup_key("y") == Key::Y);
	BOOST_CHECK(lookup_key("z") == Key::Z);
	BOOST_CHECK(lookup_key(",") == Key::Comma);
	BOOST_CHECK(lookup_key(".") == Key::Period);
	BOOST_CHECK(lookup_key("semicolon") == Key::Semicolon);
	BOOST_CHECK(lookup_key("'") == Key::Apostrophe);
	BOOST_CHECK(lookup_key("/") == Key::Slash);
	BOOST_CHECK(lookup_key("\\") == Key::Backslash);
	BOOST_CHECK(lookup_key("[") == Key::LBracket);
	BOOST_CHECK(lookup_key("]") == Key::RBracket);
	BOOST_CHECK(lookup_key("-") == Key::Minus);
	BOOST_CHECK(lookup_key("=") == Key::Equals);
	BOOST_CHECK(lookup_key("`") == Key::Grave);
	BOOST_CHECK(lookup_key("space") == Key::Space);

	// Text-editing keys (0x40 - 0x4F).

	BOOST_CHECK(lookup_key("tab") == Key::Tab);
	BOOST_CHECK(lookup_key("enter") == Key::Enter);
	BOOST_CHECK(lookup_key("back") == Key::Backspace);
	BOOST_CHECK(lookup_key("ins") == Key::Insert);
	BOOST_CHECK(lookup_key("del") == Key::Delete);
	BOOST_CHECK(lookup_key("home") == Key::Home);
	BOOST_CHECK(lookup_key("end") == Key::End);
	BOOST_CHECK(lookup_key("pgup") == Key::PageUp);
	BOOST_CHECK(lookup_key("pgdn") == Key::PageDown);

	// Functional keys (0x50 - 0x6F).

	BOOST_CHECK(lookup_key("f1") == Key::F1);
	BOOST_CHECK(lookup_key("f2") == Key::F2);
	BOOST_CHECK(lookup_key("f3") == Key::F3);
	BOOST_CHECK(lookup_key("f4") == Key::F4);
	BOOST_CHECK(lookup_key("f5") == Key::F5);
	BOOST_CHECK(lookup_key("f6") == Key::F6);
	BOOST_CHECK(lookup_key("f7") == Key::F7);
	BOOST_CHECK(lookup_key("f8") == Key::F8);
	BOOST_CHECK(lookup_key("f9") == Key::F9);
	BOOST_CHECK(lookup_key("f10") == Key::F10);
	BOOST_CHECK(lookup_key("f11") == Key::F11);
	BOOST_CHECK(lookup_key("f12") == Key::F12);

	// Numpad keys (0x70 - 0x7F).

	BOOST_CHECK(lookup_key("num0") == Key::Num0);
	BOOST_CHECK(lookup_key("num1") == Key::Num1);
	BOOST_CHECK(lookup_key("num2") == Key::Num2);
	BOOST_CHECK(lookup_key("num3") == Key::Num3);
	BOOST_CHECK(lookup_key("num4") == Key::Num4);
	BOOST_CHECK(lookup_key("num5") == Key::Num5);
	BOOST_CHECK(lookup_key("num6") == Key::Num6);
	BOOST_CHECK(lookup_key("num7") == Key::Num7);
	BOOST_CHECK(lookup_key("num8") == Key::Num8);
	BOOST_CHECK(lookup_key("num9") == Key::Num9);
	BOOST_CHECK(lookup_key("num.") == Key::Decimal);
	BOOST_CHECK(lookup_key("num+") == Key::Add);
	BOOST_CHECK(lookup_key("num-") == Key::Subtract);
	BOOST_CHECK(lookup_key("num*") == Key::Multiply);
	BOOST_CHECK(lookup_key("num/") == Key::Divide);
	BOOST_CHECK(lookup_key("numenter") == Key::NumEnter);

	// Mouse keys (0x80 - 0x8F).

	BOOST_CHECK(lookup_key("mouse1") == Key::Mouse1);
	BOOST_CHECK(lookup_key("mouse2") == Key::Mouse2);
	BOOST_CHECK(lookup_key("mouse3") == Key::Mouse3);
	BOOST_CHECK(lookup_key("mouse4") == Key::Mouse4);
	BOOST_CHECK(lookup_key("mouse5") == Key::Mouse5);
	BOOST_CHECK(lookup_key("mwup") == Key::WheelUp);
	BOOST_CHECK(lookup_key("mwdown") == Key::WheelDown);
	BOOST_CHECK(lookup_key("mwleft") == Key::WheelLeft);
	BOOST_CHECK(lookup_key("mwright") == Key::WheelRight);
}
