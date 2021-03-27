// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/application/key.h>

#include <cstdint>

namespace Yt
{
	// This mapping is based on <uapi/linux/input-event-codes.h> from Linux source code.
	// Key codes from X11/XCB are off by 0x08; this offset is included in the mapping.
	constexpr Key map_linux_key_code(uint8_t code) noexcept
	{
		switch (code)
		{
		// KEY_RESERVED
		case 0x09: return Key::Escape;
		case 0x0a: return Key::_1;
		case 0x0b: return Key::_2;
		case 0x0c: return Key::_3;
		case 0x0d: return Key::_4;
		case 0x0e: return Key::_5;
		case 0x0f: return Key::_6;
		case 0x10: return Key::_7;
		case 0x11: return Key::_8;
		case 0x12: return Key::_9;
		case 0x13: return Key::_0;
		case 0x14: return Key::Minus;
		case 0x15: return Key::Equals;
		case 0x16: return Key::Backspace;
		case 0x17: return Key::Tab;
		case 0x18: return Key::Q;
		case 0x19: return Key::W;
		case 0x1a: return Key::E;
		case 0x1b: return Key::R;
		case 0x1c: return Key::T;
		case 0x1d: return Key::Y;
		case 0x1e: return Key::U;
		case 0x1f: return Key::I;
		case 0x20: return Key::O;
		case 0x21: return Key::P;
		case 0x22: return Key::LBracket;
		case 0x23: return Key::RBracket;
		case 0x24: return Key::Enter;
		case 0x25: return Key::LControl;
		case 0x26: return Key::A;
		case 0x27: return Key::S;
		case 0x28: return Key::D;
		case 0x29: return Key::F;
		case 0x2a: return Key::G;
		case 0x2b: return Key::H;
		case 0x2c: return Key::J;
		case 0x2d: return Key::K;
		case 0x2e: return Key::L;
		case 0x2f: return Key::Semicolon;
		case 0x30: return Key::Apostrophe;
		case 0x31: return Key::Grave;
		case 0x32: return Key::LShift;
		case 0x33: return Key::Backslash;
		case 0x34: return Key::Z;
		case 0x35: return Key::X;
		case 0x36: return Key::C;
		case 0x37: return Key::V;
		case 0x38: return Key::B;
		case 0x39: return Key::N;
		case 0x3a: return Key::M;
		case 0x3b: return Key::Comma;
		case 0x3c: return Key::Period;
		case 0x3d: return Key::Slash;
		case 0x3e: return Key::RShift;
		case 0x3f: return Key::Multiply;
		case 0x40: return Key::LAlt;
		case 0x41: return Key::Space;
		// KEY_CAPSLOCK
		case 0x43: return Key::F1;
		case 0x44: return Key::F2;
		case 0x45: return Key::F3;
		case 0x46: return Key::F4;
		case 0x47: return Key::F5;
		case 0x48: return Key::F6;
		case 0x49: return Key::F7;
		case 0x4a: return Key::F8;
		case 0x4b: return Key::F9;
		case 0x4c: return Key::F10;
		// KEY_NUMLOCK
		// KEY_SCROLLLOCK
		case 0x4f: return Key::Num7;
		case 0x50: return Key::Num8;
		case 0x51: return Key::Num9;
		case 0x52: return Key::Subtract;
		case 0x53: return Key::Num4;
		case 0x54: return Key::Num5;
		case 0x55: return Key::Num6;
		case 0x56: return Key::Add;
		case 0x57: return Key::Num1;
		case 0x58: return Key::Num2;
		case 0x59: return Key::Num3;
		case 0x5a: return Key::Num0;
		case 0x5b: return Key::Decimal;
		//
		// KEY_ZENKAKUHANKAKU
		case 0x5e: return Key::Backslash; // TODO: Fix duplicate mapping (this is KEY_102ND).
		case 0x5f: return Key::F11;
		case 0x60: return Key::F12;
		// KEY_RO
		// KEY_KATAKANA
		// KEY_HIRAGANA
		// KEY_HENKAN
		// KEY_KATAKANAHIRAGANA
		// KEY_MUHENKAN
		// KEY_KPJPCOMMA
		case 0x68: return Key::NumEnter;
		case 0x69: return Key::RControl;
		case 0x6a: return Key::Divide;
		// KEY_SYSRQ
		case 0x6c: return Key::RAlt;
		// KEY_LINEFEED
		case 0x6e: return Key::Home;
		case 0x6f: return Key::Up;
		case 0x70: return Key::PageUp;
		case 0x71: return Key::Left;
		case 0x72: return Key::Right;
		case 0x73: return Key::End;
		case 0x74: return Key::Down;
		case 0x75: return Key::PageDown;
		case 0x76: return Key::Insert;
		case 0x77: return Key::Delete;
		// KEY_MACRO
		// KEY_MUTE
		// KEY_VOLUMEDOWN
		// KEY_VOLUMEUP
		// KEY_POWER
		// KEY_KPEQUAL
		// KEY_KPPLUSMINUS
		case 0x7f: return Key::Pause;
		// KEY_SCALE
		// KEY_KPCOMMA
		// KEY_HANGEUL
		// KEY_HANJA
		// KEY_YEN
		// KEY_LEFTMETA (left Windows key)
		// KEY_RIGHTMETA (right Windows key)
		// KEY_COMPOSE (the key between right Windows key and right Control key)
		default: return Key::None;
		}
	}
}
