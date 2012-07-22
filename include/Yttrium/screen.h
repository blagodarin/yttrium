/// \file
/// \brief

#ifndef __Y_SCREEN_H
#define __Y_SCREEN_H

#include <Yttrium/allocator.h>

namespace Yttrium
{

class Window;

///

struct ScreenMode
{
	Dim          width;          ///<
	Dim          height;         ///<
	uint_fast8_t bits_per_pixel; ///<
	uint_fast8_t frequency;      ///<

	///

	ScreenMode()
		: width(0)
		, height(0)
		, bits_per_pixel(0)
		, frequency(0)
	{
	}
};

///

class Y_API Screen
{
	friend class Window;

public:

	enum ModeType
	{
		CurrentMode,
		DefaultMode,
	};

public:

	///

	Screen() noexcept
		: _private(nullptr)
	{
	}

	///

	Screen(Allocator *allocator) noexcept
		//: Screen() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(allocator);
	}

	///

	Screen(const Screen &screen) noexcept;

	///

	~Screen() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	ScreenMode mode(ModeType type = CurrentMode) noexcept;

	///

	bool open(Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	Screen &operator =(const Screen &screen) noexcept;

private:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SCREEN_H
