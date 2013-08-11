/// \file
/// \brief Input key declarations.

#ifndef __Y_KEY_H
#define __Y_KEY_H

#include <Yttrium/types.h>

namespace Yttrium
{

/// Underlying Key type.

typedef uint_fast8_t KeyType;

/// Input key.

enum class Key: KeyType
{
	// Null key (0x00):

	Null       = 0x00,

	// Keyboard keys (0x01 - 0x7F):

	// - Special keys (0x01 - 0x0F);

	LShift     = 0x01,
	RShift     = 0x02,
	LControl   = 0x03,
	RControl   = 0x04,
	LAlt       = 0x05,
	RAlt       = 0x06,
	Up         = 0x07,
	Down       = 0x08,
	Left       = 0x09,
	Right      = 0x0A,
	//         = 0x0B,
	//         = 0x0C,
	Print      = 0x0D,
	Pause      = 0x0E,
	Escape     = 0x0F,

	// - Text-producing keys (0x10 - 0x3F);

	_0         = 0x10,
	_1         = 0x11,
	_2         = 0x12,
	_3         = 0x13,
	_4         = 0x14,
	_5         = 0x15,
	_6         = 0x16,
	_7         = 0x17,
	_8         = 0x18,
	_9         = 0x19,
	A          = 0x1A,
	B          = 0x1B,
	C          = 0x1C,
	D          = 0x1D,
	E          = 0x1E,
	F          = 0x1F,
	G          = 0x20,
	H          = 0x21,
	I          = 0x22,
	J          = 0x23,
	K          = 0x24,
	L          = 0x25,
	M          = 0x26,
	N          = 0x27,
	O          = 0x28,
	P          = 0x29,
	Q          = 0x2A,
	R          = 0x2B,
	S          = 0x2C,
	T          = 0x2D,
	U          = 0x2E,
	V          = 0x2F,
	W          = 0x30,
	X          = 0x31,
	Y          = 0x32,
	Z          = 0x33,
	Comma      = 0x34,
	Period     = 0x35,
	Semicolon  = 0x36,
	Apostrophe = 0x37,
	Slash      = 0x38,
	Backslash  = 0x39,
	LBracket   = 0x3A,
	RBracket   = 0x3B,
	Minus      = 0x3C,
	Equals     = 0x3D,
	Grave      = 0x3E,
	Space      = 0x3F,

	// Text-editing keys (0x40 - 0x4F);

	Tab        = 0x40,
	Enter      = 0x41,
	Backspace  = 0x42,
	Insert     = 0x43,
	Delete     = 0x44,
	Home       = 0x45,
	End        = 0x46,
	PageUp     = 0x47,
	PageDown   = 0x48,
	//         = 0x49,
	//         = 0x4A,
	//         = 0x4B,
	//         = 0x4C,
	//         = 0x4D,
	//         = 0x4E,
	//         = 0x4F,

	// - Functional keys (0x50 - 0x6F);

	F1         = 0x50,
	F2         = 0x51,
	F3         = 0x52,
	F4         = 0x53,
	F5         = 0x54,
	F6         = 0x55,
	F7         = 0x56,
	F8         = 0x57,
	F9         = 0x58,
	F10        = 0x59,
	F11        = 0x5A,
	F12        = 0x5B,
	F13        = 0x5C,
	F14        = 0x5D,
	F15        = 0x5E,
	F16        = 0x5F,
	F17        = 0x60,
	F18        = 0x61,
	F19        = 0x62,
	F20        = 0x63,
	F21        = 0x64,
	F22        = 0x65,
	F23        = 0x66,
	F24        = 0x67,
	//         = 0x68,
	//         = 0x69,
	//         = 0x6A,
	//         = 0x6B,
	//         = 0x6C,
	//         = 0x6D,
	//         = 0x6E,
	//         = 0x6F,

	// - Numpad keys (0x70 - 0x7F).

	Num0       = 0x70,
	Num1       = 0x71,
	Num2       = 0x72,
	Num3       = 0x73,
	Num4       = 0x74,
	Num5       = 0x75,
	Num6       = 0x76,
	Num7       = 0x77,
	Num8       = 0x78,
	Num9       = 0x79,
	Decimal    = 0x7A,
	Add        = 0x7B,
	Subtract   = 0x7C,
	Multiply   = 0x7D,
	Divide     = 0x7E,
	NumEnter   = 0x7F,

	// Non-keyboard keys (0x80 - 0xFF):

	// - Mouse keys (0x80 - 0x8F);

	Mouse1     = 0x80,
	Mouse2     = 0x81,
	Mouse3     = 0x82,
	Mouse4     = 0x83,
	Mouse5     = 0x84,
	//         = 0x85,
	//         = 0x86,
	//         = 0x87,
	//         = 0x88,
	//         = 0x89,
	//         = 0x8A,
	//         = 0x8B,
	WheelUp    = 0x8C,
	WheelDown  = 0x8D,
	WheelLeft  = 0x8E,
	WheelRight = 0x8F,

	// - Unassigned keys (0x90 - 0xFF).

	__Count,
};

enum: KeyType
{
	KeyCount = Key::__Count, ///< Number of keys defined.
};

/// Input key state.

typedef uint_fast16_t KeyState;

} // namespace Yttrium

#endif // __Y_KEY_H
