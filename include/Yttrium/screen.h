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

	Screen(Allocator *allocator = DefaultAllocator) noexcept;

	///

	Screen(const Screen &screen) noexcept;

	///

	~Screen() noexcept
	{
		close();
	}

public:

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	ScreenMode mode(ModeType type = CurrentMode) noexcept;

public:

	///

	Screen &operator =(const Screen &screen) noexcept;

private:

	void close() noexcept;

private:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SCREEN_H
